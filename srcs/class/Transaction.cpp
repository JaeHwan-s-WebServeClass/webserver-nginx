#include "Transaction.hpp"

#include <sys/_types/_size_t.h>

#include <cstdio>
#include <exception>

//---- constructor -------------------------------------------------------------
Transaction::Transaction(int socket_fd, const ServerConfig &server_config)
    : socket_fd(socket_fd),
      flag(START),
      response(this->flag),
      request(this->flag),
      server_config(server_config) {
  // std::cout << GRY << "Debug: Transaction::constructor\n" << DFT;
}

//---- getter ------------------------------------------------------------------
Response &Transaction::getResponse() { return this->response; }
Request &Transaction::getRequest() { return this->request; }
const ServerConfig &Transaction::getServerConfig() const {
  return this->server_config;
}
const t_step &Transaction::getFlag() const { return this->flag; }
const FILE *Transaction::getFilePtr() const { return this->file_ptr; }

//---- setter ------------------------------------------------------------------
void Transaction::setFlag(t_step flag) { this->flag = flag; }

//---- checker -----------------------------------------------------------------
int Transaction::checkResource() {
  // std::cout << GRY << "Debug: Transaction::checkResource\n" << DFT;
  std::string request_location;
  std::string request_filename = "";
  // 요청이 디렉토리 일 경우
  if (this->request.getUrl().back() == '/') {
    // TODO autoindex or index 가 있는지 확인 후 해당 로직 처리
    throw std::string(
        "checkResource :: request URL is directory. check autoindex\n");
    // file_fd를 반환하지 않을 수도....? 그러면 FILE_OPEN flag를 켤 수 없다.
    // autoindex 부분 로직 논의 후 flag 세팅!
  } else {  // 요청이 파일 일 경우
    std::size_t pos = this->request.getUrl().find_last_of("/");
    if (pos == std::string::npos) {
      throw ErrorPage404Exception();
    } else if (pos == 0) {  // localhost:8080/index.html 일 때.
      request_location = this->request.getUrl().substr(0, pos + 1);
      request_filename = this->request.getUrl().substr(pos + 1);
    } else {  // localhost:8080/example/index.html 일 때.
      request_location = this->request.getUrl().substr(0, pos);
      request_filename = this->request.getUrl().substr(pos);
    }
  }

  std::map<std::string, ServerConfig::t_location>::const_iterator it;
  if ((it = this->server_config.getLocation().find(request_location)) !=
      this->server_config.getLocation().end()) {
    this->location = it->second;
    std::string loc_root = this->location.root;
    std::string resource = "";
    resource += '.' + server_config.getRoot() + loc_root + request_filename;
    // DEBUG
    std::cout << "resource : " << resource << std::endl;
    if (access(resource.c_str(), F_OK) == -1) {
      throw ErrorPage500Exception();
    }
    this->file_ptr = ft::safeFopen(resource.c_str(), "r+");
    this->setFlag(FILE_OPEN);
  } else {
    throw ErrorPage500Exception();
  }
  return (file_ptr->_file);
}

//  1. uri 를 자르기
//  2. request_location 찾기
//  3. request_filename 찾기
//  4. request_location 으로 locale_root 찾기
//  5. doc_root 와 loc_root 와 file_name 을 합쳐서 local 파일 생성하기
//  6. 파일을 열고 fd 반환하기
//  7. 필요한 부분 = request_location

void Transaction::checkAllowedMethod() {
  if (((this->request.getMethod() == "GET") &&
       (this->location.http_method & GET)) ||
      ((this->request.getMethod() == "POST") &&
       (this->location.http_method & POST)) ||
      ((this->request.getMethod() == "DELETE") &&
       (this->location.http_method & DELETE))) {
    return;
  }
  throw ErrorPage501Exception();
}

//---- executor ----------------------------------------------------------------
int Transaction::executeRead(void) {
  // std::cout << GRY << "Debug: Transaction::executeRead\n" << DFT;
  // std::cout << "step flag: " << this->flag << std::endl;

  // BUFFER_SIZE + 1을 할 것인가에 대한 논의
  char buf[BUFFER_SIZE + 1];
  int read_len = ft::safeRecv(this->socket_fd, buf, BUFFER_SIZE);
  int head_rest_len = 0;

  if (read_len == -1) {
    return -1;
  }
  // 1. head 파싱
  if (this->flag == START) {
    head_rest_len = this->executeReadHead(buf, read_len);
  }
  // 2. entity 파싱
  if (this->flag == REQUEST_HEAD && (this->request.getMethod() == "POST")) {
    if (this->flag < REQUEST_ENTITY) {
      this->executeReadEntity(buf, read_len, head_rest_len);
    }
  }

  // FIXME 아래 주석이 힌트. entity 완료되면 REQUEST_DONE 으로 설정해뒀음.
  // if (this->flag == REQUEST_ENTITY) { // 임시
  //   this->setFlag(REQUEST_DONE);
  // }
  if ((this->request.getMethod() == "POST" && this->flag == REQUEST_ENTITY) ||
      (this->request.getMethod() != "POST" && this->flag == REQUEST_HEAD)) {
    this->setFlag(REQUEST_DONE);
  }

  // DEBUG MSG : REQUEST
  if (this->flag == REQUEST_DONE) {  // 임시
    this->request.toString();
  }
  return 0;
}

int Transaction::executeReadHead(char *buf, int read_len) {
  // std::cout << GRY << "Debug: Transaction::executeReadHead\n" << DFT;
  // 들어오는 데이터가 텍스트가 아닐수도있어 필요가 없?
  // 헤드는 무조건 텍스트로 들어오지 않을까요?
  // 터미널에서 보내는게 아니어서 상관없을거같아요..
  buf[read_len] = '\0';
  std::istringstream read_stream;
  read_stream.str(buf);

  std::string line;
  while (std::getline(read_stream, line, '\n')) {
    if (line.length() == 0 || line == "\r") {
      this->flag = REQUEST_HEAD;

      // DEBUG: checking raw head
      // std::cout << GRY << "------------------ raw head ------------------"
      //           << DFT << std::endl;
      // std::cout << GRY << this->request.getRawHead() << DFT << std::endl;
      // std::cout << GRY << "----------------------------------------------"
      //           << DFT << std::endl;

      this->request.parserHead();
      this->request.setRawHead(line + "\n");
      break;
    } else if (this->flag == START) {
      this->request.setRawHead(line + "\n");
      if (read_stream.eof()) {
        throw std::string(
            "Error: Transaction: executeReadHead: Over MAX HEADER SIZE");
      }
    }
  }
  if (this->request.getRawHead().length() > MAX_HEAD_SIZE) {
    throw std::string(
        "Error: Transaction: executeReadHead: Request Head Over MAX HEAD SIZE");
  }
  return (read_len - this->request.getRawHead().length());
}

void Transaction::executeReadEntity(char *buf, int read_len,
                                    int head_rest_len) {
  // std::cout << GRY << "Debug: Transaction::executeReadEntity\n" << DFT;
  std::map<std::string, std::string>::const_iterator it;

  if ((it = this->request.getHeader().find("Content-Length")) !=
      this->request.getHeader().end()) {
    if (head_rest_len) {
      this->request.addContentLengthEntity(
          buf + this->request.getRawHead().length(), head_rest_len);
    } else {
      this->request.addContentLengthEntity(buf, read_len);
    }
  } else if (((it = this->request.getHeader().find("Transfer-Encoding")) !=
              this->request.getHeader().end()) &&
             (it->second == "Chunked")) {
    if (head_rest_len) {
      this->request.addChunkedEntity(buf + this->request.getRawHead().length(),
                                     head_rest_len);
    } else {
      this->request.addChunkedEntity(buf, read_len);
    }
  }
  // else {
  //   throw std::string(
  //       "Error: Transaction: executeReadEntity: Invalid Request Header");
  // }
  if (this->request.getEntitySize() > MAX_BODY_SIZE) {
    throw std::string(
        "Error: Transaction: executeReadEntity: Request Entity Over MAX BODY "
        "SIZE");
  }
}

int Transaction::executeWrite(void) {
  // std::cout << GRY << "Debug: Transacåtion::executeWrite\n";
  if ((ft::safeSend(this->socket_fd, this->response) == -1)) {
    return -1;
  }
  this->setFlag(END);
  return 0;
}

int Transaction::executeMethod(int data_size) {
  // std::cout << GRY << "Debug: Transaction::executeMethod\n" << DFT;

  // method 처리
  if (!std::atoi(this->response.getStatusCode().c_str())) {
    if (this->request.getMethod() == "GET") {
      this->httpGet(data_size);
    } else if (this->request.getMethod() == "POST") {
      this->httpPost();
    } else if (this->request.getMethod() == "DELETE") {
      this->httpDelete();
    }
  }
  // 다 됐으면 ====> FILE_DONE

  if (this->flag == FILE_DONE) {
    this->response.setResponseMsg();
  }

  return 0;
}

//---- HTTP methods ------------------------------------------------------------
void Transaction::httpGet(int data_size) {
  // std::cout << GRY << "Debug: Transaction: httpGet\n" << DFT;
  char buf[MAX_BODY_SIZE + 1];
  size_t read_len =
      ft::safeFread(buf, sizeof(char), F_STREAM_SIZE, this->file_ptr);

  std::cout << "data_size: " << data_size << std::endl;
  this->response.setEntity(buf, read_len);
  if (static_cast<int>(read_len) >= data_size) {
    this->response.setHeader("Content-Length", this->response.getEntitySize());
    std::fclose(this->file_ptr);
    this->setFlag(FILE_DONE);
    this->response.setStatus("200");
  }
}

void Transaction::httpDelete(void) {
  // std::cout << GRY << "Debug: Transaction: httpDelete\n" << DFT;
}

void Transaction::httpPost(void) {
  // std::cout << GRY << "Debug: Transaction: httpPost\n" << DFT;
}

//---- error class -------------------------------------------------------------
const char *Transaction::ErrorPage404Exception::what() const throw() {
  return "404";
}
const char *Transaction::ErrorPage500Exception::what() const throw() {
  return "500";
}
const char *Transaction::ErrorPage501Exception::what() const throw() {
  return "501";
}
