#include "Transaction.hpp"

#include <dirent.h>

//---- constructor -------------------------------------------------------------
Transaction::Transaction(int socket_fd, const ServerConfig &server_config)
    : socket_fd(socket_fd),
      flag(START),
      response(this->flag),
      request(this->flag),
      server_config(server_config) {
  // std::cout << GRY << "Debug: Transaction: constructor\n" << DFT;
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
// - request done 상황
// 1. 리소스가 파일인지 폴더인지 체크
// 1-1. autoindex 처리
// 3. method 유효성 확인
// 4. 파일 오픈 후 fd 반환
// - 파일을 이벤트 등록 전

int Transaction::executeResource() {
  // std::cout << GRY << "Debug: Transaction: executeResource\n" << DFT;
  // STEP 1 . request_URL을 path(location)와 filename(filename)으로 쪼개기
  this->checkResource();
  // STEP 2 . URL이 "/"로 끝났을 때 => 디렉토리
  if (this->request.getUrl().back() == '/') {
    return this->checkDirectory();
  }
  // STEP 3 . 처리할 수 있는 method인지 확인
  this->checkAllowedMethod();
  // STEP 4 . 각 상황에 따른 fd값 반환
  return this->checkFile();
}

void Transaction::checkResource() {
  // std::cout << GRY << "Debug: Transaction: checkResource\n" << DFT;
  std::string request_location;
  std::string request_filename = "";

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
  // STEP 2 . request_location과 conf_location을 비교해서 실제 local의 resource
  // 구하기
  std::map<std::string, ServerConfig::t_location>::const_iterator it;
  if ((it = this->server_config.getLocation().find(request_location)) !=
      this->server_config.getLocation().end()) {
    this->location = it->second;
    std::string loc_root = this->location.root;
    this->resource +=
        '.' + server_config.getRoot() + loc_root + request_filename;
  } else {
    throw ErrorPage500Exception();
  }
}

int Transaction::checkDirectory() {
  // std::cout << GRY << "Debug: Transaction: checkDirectory\n" << DFT;
  std::vector<std::string>::const_iterator it = this->location.index.begin();

  // index.html
  for (; it != this->location.index.end(); ++it) {
    std::string tmp = this->resource + *it;
    if (access(tmp.c_str(), F_OK) != -1) {
      // 찾았을 경우
      this->file_ptr = ft::safeFopen(tmp.c_str(), "r");
      this->setFlag(FILE_READ);
      return file_ptr->_file;
    }
  }
  if (this->location.autoindex) {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(this->resource.c_str())) != NULL) {
      std::string body = "<html><body>";
      while ((ent = readdir(dir)) != NULL) {
        body += "<a href=\"" + std::string(ent->d_name) + "\">" +
                std::string(ent->d_name) + "</a><br>";
      }
      body += "</body></html>";
      closedir(dir);
      this->response.setEntity(body.c_str(), body.size());
      this->response.setStatus("200");
      this->response.setHeader("Content-Type", "text/html");
      this->response.setResponseMsg();
      return -1;
    } else {                          //  디렉토리가 없는 경우
      throw ErrorPage404Exception();  // 403 Forbidden
    }
  } else {
    throw ErrorPage404Exception();  // 403 Forbidden
  }
  return -1;
}

int Transaction::checkFile() {
  // std::cout << GRY << "Debug: Transaction: checkFile\n" << DFT;

  if (this->request.getMethod() == "POST") {  // 파일 새로 생성 or pipe fd 반환
    if (this->flag == FILE_CGI) {
      this->file_ptr = ft::safeFopen(this->resource.c_str(), "w");
      this->setFlag(FILE_WRITE);
    } else if ((this->location.cgi_exec != "") &&
               ft::findSuffix(this->resource, ".py")) {
      this->setFlag(FILE_READ);
      return this->executeCGI();
    } else {
      this->file_ptr = ft::safeFopen(this->resource.c_str(), "w");
      this->setFlag(FILE_WRITE);
    }
  } else if ((this->request.getMethod() == "GET") &&
             (this->location.cgi_exec != "") &&
             ft::findSuffix(
                 this->resource,
                 ".py")) {  // 파일 일 경우 (이미 존재하는 파일을 조회하는 경우)
    if (access(this->resource.c_str(), F_OK) == -1) {
      throw ErrorPage404Exception();
    }
    this->setFlag(FILE_READ);
    return this->executeCGI();
  } else {
    if (access(this->resource.c_str(), F_OK) == -1) {
      throw ErrorPage404Exception();
    }
    this->file_ptr = ft::safeFopen(this->resource.c_str(), "r+");
    this->setFlag(FILE_READ);
  }
  return this->file_ptr->_file;
}

void Transaction::checkAllowedMethod() {
  // std::cout << GRY << "Debug: Transaction: checkAllowedMethod\n" << DFT;
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
  // std::cout << GRY << "Debug: Transaction: executeRead\n" << DFT;

  // BUFFER_SIZE + 1을 할 것인가에 대한 논의
  char buf[BUFFER_SIZE + 1];
  int read_len = ft::safeRecv(this->socket_fd, buf, BUFFER_SIZE);
  int head_rest_len = 0;

  if (read_len == -1) {
    return -1;
  }
  // head 파싱과 entity 파싱
  if (this->flag == START) {
    head_rest_len = this->executeReadHead(buf, read_len);
  }
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
  if (this->flag == REQUEST_DONE) {
    this->request.toString();
  }
  return 0;
}

int Transaction::executeReadHead(char *buf, int read_len) {
  // std::cout << GRY << "Debug: Transaction: executeReadHead\n" << DFT;
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
        "Error: Transaction: executeReadHead: Request Head Over MAX HEAD "
        "SIZE");
  }
  return (read_len - this->request.getRawHead().length());
}

void Transaction::executeReadEntity(char *buf, int read_len,
                                    int head_rest_len) {
  // std::cout << GRY << "Debug: Transaction: executeReadEntity\n" << DFT;
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
  // std::cout << GRY << "Debug: Transacation: executeWrite\n";
  if ((ft::safeSend(this->socket_fd, this->response) == -1)) {
    return -1;
  }
  this->setFlag(END);
  return 0;
}

int Transaction::executeMethod(int data_size, int fd) {
  // std::cout << GRY << "Debug: Transaction: executeMethod\n" << DFT;
  if (!std::atoi(this->response.getStatusCode().c_str())) {
    if (this->request.getMethod() == "GET") {
      this->httpGet(data_size, fd);
    } else if (this->request.getMethod() == "POST") {
      this->httpPost(fd);
    } else if (this->request.getMethod() == "DELETE") {
      this->httpDelete();
    }
  }
  if (this->flag == FILE_DONE) {
    this->response.setResponseMsg();
  }
  return 0;
}

//---- HTTP methods ------------------------------------------------------------
void Transaction::httpGet(int data_size, int fd) {
  // std::cout << GRY << "Debug: Transaction: httpGet\n" << DFT;
  if (ft::findSuffix(this->resource, ".py")) {
    char buf[BUFFER_SIZE];
    int read_len;
    read_len = ft::safeRead(fd, buf, BUFFER_SIZE);
    this->response.setEntity(buf, read_len);
    buf[read_len] = '\0';
    // DEBUG
    // std::cout << buf << std::endl;
    this->setFlag(FILE_DONE);
    close(fd);
  } else {
    char buf[MAX_BODY_SIZE + 1];
    size_t read_len =
        ft::safeFread(buf, sizeof(char), F_STREAM_SIZE, this->file_ptr);

    std::cout << "data_size: " << data_size << std::endl;
    this->response.setEntity(buf, read_len);
    if (static_cast<int>(read_len) >= data_size) {
      std::fclose(this->file_ptr);
      this->setFlag(FILE_DONE);
      this->response.setStatus("200");
    }
  }
}

void Transaction::httpDelete() {
  // std::cout << GRY << "Debug: Transaction: httpDelete\n" << DFT;
  if (std::remove(this->resource.c_str()) == 0) {  // 파일 삭제 성공
    this->setFlag(FILE_DONE);
    this->response.setStatus("200");
    this->response.setHeader("Content-Type", "text/plain");
    this->response.setEntity("200 OK", 6);
  } else {  // 파일 삭제 실패
    throw ErrorPage500Exception();
  }
}

void Transaction::httpPost(int fd) {
  // std::cout << GRY << "Debug: Transaction: httpPost\n" << DFT;
  if (this->flag == FILE_READ) {  // cgi pipe read
    char buf[BUFFER_SIZE];
    int read_len;
    read_len = ft::safeRead(fd, buf, BUFFER_SIZE);
    // this->response.setEntity(buf, read_len);
    buf[read_len] = '\0';
    // DEBUG
    // this->setFlag(FILE_DONE);
    this->request.setEntity(buf, read_len);

    this->setFlag(FILE_CGI);
    close(fd);
  } else if (this->flag == FILE_WRITE) {  // upload file
    ft::safeFwrite(&this->request.getEntity()[0], sizeof(char),
                   this->request.getEntitySize(), this->file_ptr);
    ft::safeFclose(this->file_ptr);
    this->setFlag(FILE_DONE);
    this->response.setStatus("201");
    this->response.setHeader("Content-Type", "text/plain");
    this->response.setEntity("201 Created", 12);
  }
}

//---- cgi ---------------------------------------------------------------------
int Transaction::executeCGI(void) {
  // std::cout << GRY << "Debug: Transaction: executeCGI\n" << DFT;
  int fd[2];
  int status;
  std::string cgi_path;

  ft::safePipe(fd);  // pipe 는 반이중(Half-duplex) 통신
  if (this->request.getMethod() == "POST") {
    cgi_path = ("." + this->server_config.getRoot() + this->location.root +
                this->location.cgi_path);
  } else if (this->request.getMethod() == "GET") {
    cgi_path = this->resource;
  }
  char const *const args[] = {
      (this->location.cgi_exec).c_str(), cgi_path.c_str(),
      ft::vecToCharArr(this->request.getEntity()), NULL};
  pid_t cgi_pid = ft::safeFork();
  if (cgi_pid == 0) {  // 자식은 쓰고 -> read closed
    close(fd[0]);
    dup2(fd[1], 1);
    if (execve((this->location.cgi_exec).c_str(), (char **)args, NULL) == -1) {
      exit(1);
    }
  } else {  // 부모는 읽고 -> write closed
    close(fd[1]);
    // FIXME: nonblocking waitpid
    waitpid(cgi_pid, &status, 0);
    if (!WIFEXITED(status) && WEXITSTATUS(status)) {
      // std::cout << RED << "child error" << std::endl << DFT;
      throw ErrorPage500Exception();
    }
  }
  return fd[0];
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
const char *Transaction::ErrorPageDefaultException::what() const throw() {
  return "default";
}
