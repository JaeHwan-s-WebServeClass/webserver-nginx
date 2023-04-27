#include "Transaction.hpp"

#include <exception>

//---- constructor -------------------------------------------------------------
Transaction::Transaction(int socket_fd, const ServerConfig &server_config)
    : socket_fd(socket_fd),
      flag(START),
      server_config(server_config),
      request(this->flag) {
  // std::cout << GRY << "Debug: Transaction::constructor\n" << DFT;
}

//---- getter ------------------------------------------------------------------
Response &Transaction::getResponse() { return this->response; }
Request &Transaction::getRequest() { return this->request; }
const t_step &Transaction::getFlag() const { return this->flag; }
FILE *Transaction::getFilePtr() const { return this->file_ptr; }

//---- setter ------------------------------------------------------------------
void Transaction::setFlag(t_step flag) { this->flag = flag; }

//---- checker -----------------------------------------------------------------
int Transaction::checkResource() {
  // std::cout << GRY << "Debug: Transaction::checkResource\n" << DFT;
  std::string request_location;
  std::string request_filename = "";
  std::string resource;
  // 요청이 디렉토리 일 경우
  if (this->request.getUrl().back() == '/') {
    // autoindex가 있는지 확인 후 해당 로직 처리

    // 요청이 파일 일 경우
  } else {
    std::size_t pos = this->request.getUrl().find_last_of("/");
    if (pos == std::string::npos) {
      this->response.setStatusCode("404");
      //  TODO 에러 파일 받아와서 등록하기
      std::cout << "checkResource :: return error page fd\n";
      // file_fd = std::fopen(, )._file;
      // return ("404.html");

    } else if (pos == 0) {  // localhost:8080/index.html 일 때.
      request_location = this->request.getUrl().substr(0, pos + 1);
      request_filename = this->request.getUrl().substr(pos + 1);

      std::cout << "case 1 - localhost:8080/index.html\n";
      std::cout << "request location: " << request_location << std::endl;
      std::cout << "request filename: " << request_filename << std::endl;
    } else {  // localhost:8080/example/index.html 일 때.
      request_location = this->request.getUrl().substr(0, pos);
      request_filename = this->request.getUrl().substr(pos);

      std::cout << "case 2 - localhost:8080/example/index.html\n";
      std::cout << "request location: " << request_location << std::endl;
      std::cout << "request filename: " << request_filename << std::endl;
    }
  }

  std::map<std::string, ServerConfig::t_location>::const_iterator it;
  if ((it = this->server_config.getLocation().find(request_location)) !=
      this->server_config.getLocation().end()) {
    this->location = it->second;
    std::string loc_root = this->location.root;
    resource = "." + server_config.getRoot() + loc_root + request_filename;
    std::cout << "resource : " << resource << std::endl;
    if (access(resource.c_str(), F_OK) == -1) {
      this->response.setStatusCode("500");
      //  TODO 에러 파일 받아와서 등록하기
      // cannot found request_filename
      std::cout << "Error: Transaction: checkResouce: access error\n";
    }
    this->file_ptr = std::fopen(resource.c_str(), "r+");
    this->setFlag(FILE_OPEN);
  } else {
    //  TODO 에러 파일 받아와서 등록하기
    this->response.setStatusCode("500");
    // Invalid directory: cannot found reqeust_location
    std::cout << "Error: Transaction: checkResouce: can not find in map\n";
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
  this->response.setStatusCode("501");
}

//---- executor ----------------------------------------------------------------
int Transaction::executeRead(void) {
  // std::cout << GRY << "Debug: Transaction::executeRead\n" << DFT;
  // std::cout << "step flag: " << this->flag << std::endl;

  // BUFFER_SIZE + 1을 할 것인가에 대한 논의
  char buf[BUFFER_SIZE + 1];
  int read_len = safeRead(this->socket_fd, buf, BUFFER_SIZE);
  int head_rest_len = 0;

  if (read_len == -1) {
    return -1;
  }
  // 1. head 파싱
  if (this->flag == START) {
    head_rest_len = this->executeReadHead(buf, read_len);
  }
  // 2. entity 파싱
  if (this->flag == REQUEST_HEAD && (this->request.getMethod() == "GET")) {
    if (this->flag < REQUEST_ENTITY) {
      this->executeReadEntity(buf, read_len, head_rest_len);
    }
  }

  if (this->flag == REQUEST_ENTITY) this->setFlag(REQUEST_DONE);
  // if ((this->request.getMethod() == "POST" && this->flag == REQUEST_ENTITY)
  //      || (this->request.getMethod() != "POST" && this->flag ==
  //      REQUEST_HEAD)) {
  //     this->setFlag(REQUEST_DONE);
  // }
  if (this->flag == REQUEST_DONE) this->request.toString();
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
  } else {
    throw std::string(
        "Error: Transaction: executeReadEntity: Invalid Request Header");
  }
  if (this->request.getEntitySize() > MAX_BODY_SIZE) {
    throw std::string(
        "Error: Transaction: executeReadEntity: Request Entity Over MAX BODY "
        "SIZE");
  }
}

int Transaction::executeWrite(void) {
  // std::cout << GRY << "Debug: Transacåtion::executeWrite\n";
  if (this->flag == REQUEST_ENTITY &&
      (safeWrite(this->socket_fd, this->response) == -1)) {
    return -1;
  }
  return 0;
}

int Transaction::executeMethod(void) {
  // std::cout << GRY << "Debug: Transaction::executeMethod\n" << DFT;
  if ((this->request.getMethod() == "POST" && this->flag < REQUEST_ENTITY) ||
      ((this->request.getMethod() != "POST") && this->flag < REQUEST_HEAD)) {
    return 0;
  }
  this->checkAllowedMethod();

  int status = std::atoi(this->response.getStatusCode().c_str());

  // resource를 open / resource가 없는 경우 or Method가 없는 경우
  if (!status) {
    if (this->request.getMethod() == "GET") {
      status = this->httpGet();
    } else if (this->request.getMethod() == "POST") {
      status = this->httpPost();
    } else if (this->request.getMethod() == "DELETE") {
      status = this->httpDelete();
    }
  }
  switch (status) {
    // setStatusCode 와 setStatusMsg 를 합치자?
    case 200:
      this->response.setStatusCode("200");
      this->response.setStatusMsg("(◟˙꒳​˙)◟ Good ◝(˙꒳​˙◝)");
      break;
    case 404:
      this->response.setStatusCode("404");
      this->response.setStatusMsg("Not Found :(");
      // entity 에 conf 파일 설정된 error_page 추가
      break;
    case 500:
      this->response.setStatusCode("500");
      this->response.setStatusMsg("Internal Server Error :l");
      break;
    case 501:
      this->response.setStatusCode("501");
      this->response.setStatusMsg("Not Implemented");
      break;
    default:
      std::cout << "(tmp msg) excuteTransaction: switch: default\n";
      break;
  }
  this->response.setResponseMsg();
  return 0;
}

//---- HTTP methods ------------------------------------------------------------
/*
1. read
2. response
3. close
*/

int Transaction::httpGet(void) {
  // std::cout << GRY << "Debug: Transaction::httpGet\n" << DFT;
  char buf[MAX_BODY_SIZE + 1];
  int read_len = safeRead(this->file_ptr->_file, buf, MAX_BODY_SIZE);

  if (read_len == 0) {
    this->response.setHeader("Content-Length", this->response.getEntitySize());
    fclose(this->file_ptr);
    flag = FILE_DONE;
  } else {
    this->response.setEntity(buf, read_len);
  }

  // content_length를 세기 위함

  return 200;
}

int Transaction::httpDelete(void) {
  // std::cout << GRY << "Debug: Transaction: httpDelete\n" << DFT;
  return 200;
}

int Transaction::httpPost(void) {
  // std::cout << GRY << "Debug: Transaction: httpPost\n" << DFT;
  return 200;
}

//---- safe functions ----------------------------------------------------------
int Transaction::safeRead(int fd, char *buf, int size) {
  // std::cout << GRY << "Debug: Transaction::safeRead\n" << DFT;
  int recv_len;

  signal(SIGPIPE, SIG_IGN);
  if ((recv_len = recv(fd, buf, size, 0)) == -1) {
    std::cerr << RED << "client read error!\n" << DFT;
  }
  signal(SIGPIPE, SIG_DFL);
  return recv_len;
}

int Transaction::safeWrite(int fd, Response &response) {
  // std::cout << GRY << "Debug: Transaction::safeWrite\n";
  int send_len;

  signal(SIGPIPE, SIG_IGN);
  if ((send_len = send(fd, response.getResponseMsg().c_str(),
                       response.getResponseMsg().size(), 0)) == -1) {
    std::cerr << RED << "client write error!\n" << DFT;
  }
  signal(SIGPIPE, SIG_DFL);
  return send_len;
}
