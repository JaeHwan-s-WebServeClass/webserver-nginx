#include "Transaction.hpp"

extern int g_server_status;

//---- OCCF -------------------------------------------------------------------
Transaction::Transaction(int socket_fd, const ServerConfig &server_config)
    : socket_fd(socket_fd),
      file_fd(-1),
      flag(START),
      response(this->flag),
      request(this->flag),
      server_config(server_config),
      cgi_pid(DEFALUT_CGIPID) {
  // std::cout << GRY << "Debug: Transaction: constructor\n" << DFT;
}
Transaction::Transaction(const Transaction &ref)
    : response(this->flag),
      request(this->flag),
      server_config(ref.server_config) {
  *this = ref;
}
Transaction &Transaction::operator=(const Transaction &ref) {
  this->socket_fd = ref.socket_fd;
  this->file_fd = ref.file_fd;
  this->flag = ref.flag;
  this->response = ref.response;
  this->request = ref.request;
  this->location = ref.location;
  this->resource = ref.resource;
  this->cgi_pid = ref.cgi_pid;
  return *this;
}
Transaction::~Transaction() {}

//---- getter ------------------------------------------------------------------
Response &Transaction::getResponse() { return this->response; }
Request &Transaction::getRequest() { return this->request; }
const ServerConfig &Transaction::getServerConfig() const {
  return this->server_config;
}
const int &Transaction::getPid() const { return this->cgi_pid; };
const t_step &Transaction::getFlag() const { return this->flag; }
const int &Transaction::getFileDescriptor() const { return this->file_fd; }

//---- setter ------------------------------------------------------------------
void Transaction::setFlag(t_step flag) { this->flag = flag; }
void Transaction::setResource() {
  // std::cout << GRY << "Debug: Transaction: setResource\n" << DFT;
  std::string request_location;
  std::string request_filename = "";

  std::size_t pos = this->request.getUrl().find_last_of("/");
  if (pos == std::string::npos) {
    throw ErrorPage404Exception();
  } else if (pos == 0) {
    request_location = this->request.getUrl().substr(0, pos + 1);
    request_filename = this->request.getUrl().substr(pos + 1);
  } else {
    request_location = this->request.getUrl().substr(0, pos);
    request_filename = this->request.getUrl().substr(pos + 1);
  }

  if ((request_filename == ".") || (request_filename == "..")) {
    throw ErrorPage403Exception();
  }
  // STEP 2 . request_loc과 conf_loc을 비교해서 실제 local의 resource 구하기
  std::map<std::string, ServerConfig::t_location>::const_iterator it;
  if ((it = this->server_config.getLocation().find(request_location)) !=
      this->server_config.getLocation().end()) {
    this->location = it->second;
    std::string loc_root = this->location.root;
    if ((loc_root.back() != '/')) {
      loc_root += "/";
    }
    this->resource +=
        "." + server_config.getRoot() + loc_root + request_filename;
  } else {
    throw ErrorPage404Exception();
  }
}

//---- checker -----------------------------------------------------------------
void Transaction::checkAllowedMethod() {
  // std::cout << GRY << "Debug: Transaction: checkAllowedMethod\n" << DFT;
  if ((this->request.getMethod() == "GET" &&
       this->location.http_method & GET) ||
      (this->request.getMethod() == "POST" &&
       this->location.http_method & POST) ||
      (this->request.getMethod() == "DELETE" &&
       this->location.http_method & DELETE) ||
      (this->request.getMethod() == "PUT" &&
       this->location.http_method & PUT)) {
    return;
  }
  if ((this->request.getMethod() == "GET") ||
      (this->request.getMethod() == "POST") ||
      (this->request.getMethod() == "DELETE") ||
      (this->request.getMethod() == "PUT")) {
    throw ErrorPage405Exception();
  }
  throw ErrorPage501Exception();
}

void Transaction::checkServerName() {
  // std::cout << GRY << "Debug: Transaction: checkServerName\n" << DFT;
  std::string request_host =
      ft::split(this->request.getHeader().find("Host")->second, ':')[0];
  if (request_host != "localhost") {
    std::vector<std::string>::const_iterator it =
        this->server_config.getServerName().begin();
    for (; it != this->server_config.getServerName().end(); it++) {
      if (request_host == *it) {
        return;
      }
    }
    throw ErrorPage404Exception();
  }
}

//---- executor : read --------------------------------------------------------
void Transaction::executeRead(void) {
  // std::cout << GRY << "Debug: Transaction: executeRead\n" << DFT;
  char buf[BUFFER_SIZE + 1];
  int read_len = ft::safeRecv(this->socket_fd, buf, BUFFER_SIZE);
  int head_rest_len = 0;

  if (this->flag == START) {
    head_rest_len = this->executeReadHead(buf, read_len);
    this->checkServerName();
  }
  if (this->flag == REQUEST_HEAD && (this->request.getMethod() == "POST" ||
                                     this->request.getMethod() == "PUT")) {
    if (this->flag < REQUEST_ENTITY) {
      this->executeReadEntity(buf, read_len, head_rest_len);
    }
  }
  if (((this->request.getMethod() == "POST" ||
        this->request.getMethod() == "PUT") &&
       this->flag == REQUEST_ENTITY) ||
      ((this->request.getMethod() != "POST" &&
        this->request.getMethod() != "PUT") &&
       this->flag == REQUEST_HEAD)) {
    this->setFlag(REQUEST_DONE);
  }
  // DEBUG
  // if (this->flag == REQUEST_DONE) {
  // std::cout << this->request << std::endl;
  // }
}

int Transaction::executeReadHead(char *buf, int read_len) {
  // std::cout << GRY << "Debug: Transaction: executeReadHead\n" << DFT;
  buf[read_len] = '\0';
  std::istringstream read_stream;
  read_stream.str(buf);

  std::string line;
  while (std::getline(read_stream, line, '\n')) {
    if (line.length() == 0 || line == "\r") {
      this->flag = REQUEST_HEAD;
      // DEBUG: checking raw head
      // std::cout << ft::printHelper("------------- raw head -------------");
      // std::cout << GRY << this->request.getRawHead() << DFT;
      // std::cout << ft::printHelper("------------------------------------");
      this->request.parserHead();
      this->request.setRawHead(line + "\n");
      break;
    } else if (this->flag == START) {
      this->request.setRawHead(line + "\n");
      if (read_stream.eof()) {
        ft::printError(
            "Error: Transaction: executeReadHead: over max header size");
        throw Transaction::ErrorPageDefaultException();
      }
    }
  }
  if (this->request.getRawHead().length() >
      this->server_config.getClientMaxHeadSize()) {
    ft::printError("Error: Transaction: executeReadHead: over max header size");
    throw Transaction::ErrorPageDefaultException();
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
  } else {
    ft::printError(
        "Error: Transaction: executeReadEntity: invalid request header");
    throw Transaction::ErrorPageDefaultException();
  }
  if (this->request.getEntitySize() >
      this->server_config.getClientMaxBodySize()) {
    ft::printError("Error: Transaction: executeReadEntity: over max body size");
    throw Transaction::ErrorPageDefaultException();
  }
}
//---- executor : resource ----------------------------------------------------
int Transaction::executeResource() {
  // std::cout << GRY << "Debug: Transaction: executeResource\n" << DFT;
  this->setResource();
  this->checkAllowedMethod();
  if (ft::isDirectory(this->resource)) {
    return this->executeResourceDirectory();
  }
  return this->executeResourceFile();
}

int Transaction::executeResourceDirectory() {
  // std::cout << GRY << "Debug: Transaction: executeResourceDir\n" << DFT;
  if (this->request.getUrl().back() != '/') {
    throw ErrorPage404Exception();
  }
  if (this->request.getMethod() != "GET") {
    this->response.setHeader("Allow", "GET");
    throw ErrorPage405Exception();
  }

  std::vector<std::string>::const_iterator it = this->location.index.begin();
  // index 파일을 찾아서 있으면 보여줌
  for (; it != this->location.index.end(); ++it) {
    std::string tmp = this->resource + *it;
    if (access(tmp.c_str(), F_OK) != -1) {
      this->resource = tmp;
      if ((this->location.cgi_exec != "") &&
          ft::findSuffix(this->resource, ".py")) {
        this->setFlag(FILE_READ);
        return this->executeCGI();
      } else {
        this->file_fd = ft::safeOpen(this->resource, O_RDONLY, 0644);
        this->setFlag(FILE_READ);
        return this->file_fd;
      }
    }
  }
  // index 파일을 못찾았으면 autoindex 처리
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
      return DIRECTORY;
    } else {  //  디렉토리가 없는 경우
      throw ErrorPage403Exception();
    }
  } else {
    throw ErrorPage403Exception();
  }
  return DIRECTORY;
}

int Transaction::executeResourceFile() {
  // std::cout << GRY << "Debug: Transaction: executeResourceFile\n" << DFT;
  if ((this->request.getMethod() != "POST" &&
       this->request.getMethod() != "PUT") &&
      (access(this->resource.c_str(), F_OK) == -1)) {
    throw ErrorPage404Exception();
  }
  if (this->request.getMethod() == "DELETE") {
    return NONE_FD;
  }
  // 1. cgi 처리 해야하는 상황인데, 아직 처리 안된 상태
  if ((this->location.cgi_exec != "") && (this->request.getMethod() != "PUT") &&
      ft::findSuffix(this->resource, ".py") && this->flag != FILE_CGI) {
    this->setFlag(FILE_READ);
    return this->executeCGI();
  } else if (this->request.getMethod() == "POST") {  // 2. 평범한 post
    this->file_fd =
        ft::safeOpen(this->resource, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    this->setFlag(FILE_WRITE);
  } else if (this->request.getMethod() == "PUT") {
    if (access(this->resource.c_str(), F_OK) == 0) {
      return NONE_FD;
    } else {
      this->file_fd =
          ft::safeOpen(this->resource, O_CREAT | O_TRUNC | O_WRONLY, 0644);
      this->setFlag(FILE_WRITE);
    }
  } else {  // 3. 평범한 get
    if (ft::isFileEmpty(this->resource.c_str())) {
      this->response.setStatus("200");
      this->response.setResponseMsg();
      return EMPTY_FILE;
    }
    this->file_fd = ft::safeOpen(this->resource, O_RDWR, 0644);
    this->setFlag(FILE_READ);
  }
  return (this->file_fd);
}

//---- executor : write -------------------------------------------------------
int Transaction::executeWrite(void) {
  // std::cout << GRY << "Debug: Transacation: executeWrite\n";
  if ((ft::safeSend(this->socket_fd, this->response) == -1)) {
    return -1;
  }
  this->setFlag(END);
  return 0;
}

//---- executor : method ------------------------------------------------------
int Transaction::executeMethod(int data_size, int fd) {
  // std::cout << GRY << "Debug: Transaction: executeMethod\n" << DFT;
  if (!std::atoi(this->response.getStatusCode().c_str())) {
    if (this->request.getMethod() == "GET") {
      this->httpGet(data_size, fd);
    } else if (this->request.getMethod() == "POST") {
      this->httpPost(data_size, fd);
    } else if (this->request.getMethod() == "DELETE") {
      this->httpDelete();
    } else if (this->request.getMethod() == "PUT") {
      this->httpPut(data_size, fd);
    }
  }
  if (this->flag == FILE_DONE) {
    this->response.setResponseMsg();
  }
  return 0;
}

//---- HTTP methods -----------------------------------------------------------
void Transaction::httpGet(int data_size, int fd) {
  // std::cout << GRY << "Debug: Transaction: httpGet\n" << DFT;
  // 1. cgi get
  if (ft::findSuffix(this->resource, ".py")) {
    char buf[BUFFER_SIZE + 1];
    int read_len;
    read_len = ft::safeRead(fd, buf, BUFFER_SIZE);
    this->response.setEntity(buf, read_len);
    buf[read_len] = '\0';
    if (read_len == data_size) {
      this->setFlag(FILE_DONE);
      this->response.setStatus("200");
      this->response.setHeader("Content-Type", "text/html");
      ft::safeClose(fd);
    }
  } else {  // 2. 그냥 get
    char buf[this->server_config.getClientMaxBodySize() + 1];
    size_t read_len = ft::safeRead(this->file_fd, buf,
                                   this->server_config.getClientMaxBodySize());
    buf[read_len] = '\0';
    this->response.setEntity(buf, read_len);
    if (static_cast<int>(read_len) >= data_size) {
      ft::safeClose(this->file_fd);
      this->setFlag(FILE_DONE);
      this->response.setStatus("200");
      this->response.setHeader("Content-Type", "text/html");
    }
  }
}

void Transaction::httpDelete() {
  // std::cout < GRY << "Debug: Transaction: httpDelete\n" << DFT;
  if (ft::isDirectory(this->resource.c_str())) {
    throw ErrorPage403Exception();
  }
  if (std::remove(this->resource.c_str()) == 0) {  // 파일 삭제 성공
    this->setFlag(FILE_DONE);
    this->response.setStatus("200");
    this->response.setHeader("Content-Type", "text/plain");
    this->response.setEntity("200 OK", 6);
  } else {  // 파일 삭제 실패
    throw ErrorPage500Exception();
  }
}

void Transaction::httpPost(int data_size, int fd) {
  // std::cout << GRY << "Debug: Transaction: httpPost\n" << DFT;
  if (this->flag == FILE_READ) {  // cgi pipe read
    int wait_pid, status;
    wait_pid = waitpid(this->cgi_pid, &status, WNOHANG);
    if (wait_pid && (wait_pid == -1 || WIFSIGNALED(status) ||
                     (WIFEXITED(status) && WEXITSTATUS(status)))) {
      ft::safeClose(fd);
      throw ErrorPage500Exception();
    } else if (wait_pid == this->cgi_pid ||
               this->request.getEntityCgi().size()) {
      char buf[BUFFER_SIZE];
      int read_len;
      read_len = ft::safeRead(fd, buf, BUFFER_SIZE);
      this->request.setEntityCgi(buf, read_len);
      if (read_len == data_size) {
        this->request.setEntityClear();
        this->request.setEntity(this->request.getEntityCgi());
        this->setFlag(FILE_CGI);
        ft::safeClose(fd);
      }
    }
  } else if (this->flag == FILE_WRITE) {  // upload file
    ft::safeWrite(this->file_fd,
                  const_cast<char *>(&this->request.getEntity()[0]),
                  this->request.getEntitySize());
    ft::safeClose(fd);
    this->setFlag(FILE_DONE);
    this->response.setStatus("201");
    this->response.setHeader("Content-Type", "text/plain");
    this->response.setEntity("201 Created", 11);
  }
}

void Transaction::httpPut(int data_size, int fd) {
  // std::cout << GRY << "Debug: Transaction: httpPut\n" << DFT;
  if (fd == 0 && data_size == 0) {
    this->response.setHeader("Content-Type", "text/plain");
    throw ErrorPage409Exception();
  }
  ft::safeWrite(fd, const_cast<char *>(&this->request.getEntity()[0]),
                this->request.getEntitySize());
  ft::safeClose(fd);
  this->setFlag(FILE_DONE);
  this->response.setStatus("201");
  this->response.setHeader("Content-Type", "text/plain");
  this->response.setEntity("201 Created", 11);
}

//---- cgi ---------------------------------------------------------------------
int Transaction::executeCGI(void) {
  // std::cout << GRY << "Debug: Transaction: executeCGI\n" << DFT;
  int fd[2];
  std::string cgi_path;

  ft::safePipe(fd);
  if (this->request.getMethod() == "POST" ||
      this->request.getMethod() == "PUT") {
    cgi_path = ("." + this->server_config.getRoot() + this->location.root +
                this->location.cgi_path);
  } else if (this->request.getMethod() == "GET") {
    cgi_path = this->resource;
  }
  const char *tmp = ft::vecToCharArr(this->request.getEntity());
  char const *const args[] = {(this->location.cgi_exec).c_str(),
                              cgi_path.c_str(), tmp, NULL};

  this->cgi_pid = ft::safeFork();
  if (this->cgi_pid == 0) {
    ft::safeClose(fd[0]);
    ft::safeDup2(fd[1], STDOUT_FILENO);
    ft::safeClose(fd[1]);
    ft::safeExecve((this->location.cgi_exec).c_str(), (char **)args, NULL);
  } else {
    ft::safeClose(fd[1]);
  }
  delete[] tmp;
  return fd[0];
}

//---- redirection ------------------------------------------------------------
void Transaction::executeRedirect() {
  std::string entity = "<html><head><script>window.location.href = \'" +
                       this->server_config.getRedirect() + "\'</script></html>";
  this->response.setStatus("301");
  this->response.setHeader("Content-Type", "text/html");
  this->response.setEntity(entity.c_str(), entity.size());
  this->response.setResponseMsg();
}

//---- error class ------------------------------------------------------------
const char *Transaction::ErrorPage403Exception::what() const throw() {
  return "403";
}
const char *Transaction::ErrorPage404Exception::what() const throw() {
  return "404";
}
const char *Transaction::ErrorPage405Exception::what() const throw() {
  return "405";
}
const char *Transaction::ErrorPage408Exception::what() const throw() {
  return "408";
}
const char *Transaction::ErrorPage409Exception::what() const throw() {
  return "409";
}
const char *Transaction::ErrorPage500Exception::what() const throw() {
  return "500";
}
const char *Transaction::ErrorPage501Exception::what() const throw() {
  return "501";
}
const char *Transaction::ErrorPageDefaultException::what() const throw() {
  return "418";
}
