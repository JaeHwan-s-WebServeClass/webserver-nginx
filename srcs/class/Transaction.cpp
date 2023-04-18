#include "Transaction.hpp"
#include <exception>

//---- constructor --------------------------------------------
Transaction::Transaction(int socket_fd, std::string root_dir)
    : socket_fd(socket_fd), root_dir(root_dir) {
  // std::cout << GRY << "Debug: Transaction::constructor\n" << DFT;
}

//---- getter ---------------------------------------------
Response &Transaction::getResponse() { return this->response; }

Request &Transaction::getRequest() { return this->request; }

//---- execute --------------------------------------------
int Transaction::executeRead(void) {
  // if head일 때, else body일 때 

  // 1. head 파싱 -----------------------
  if (!this->request.getHeadDone()) {
    this->executeReadHead();
    return 0;
  }

  /**
  if (this->request.getHeadDone() && (this->request.getMethod() == "POST")) 
  {
    if (!this->request.getEntityDone())
    {
      std::map<std::string, std::string>::const_iterator it;
      if ((it = this->request.getHeader().find("Content-Length")) 
          != this->request.getHeader().end()) 
      {
        excuteReadContentLengthEntity()
      }
      else if (((it = this->request.getHeader().find("Transfer-Encoding"))
           != this->request.getHeader().end()) && (it->second == "Chunked")) 
      {
        excuteReadChunkedEntity()
      }
    }
  */

  // 2. entity 파싱 -----------------------
  if (this->request.getHeadDone() && (this->request.getMethod() == "POST")) {
    if (!this->request.getEntityDone()){
      int head_rest_len = this->read_head_len - (this->request.getRawHead().length());
      
      std::map<std::string, std::string>::const_iterator it;
      if ((it = this->request.getHeader().find("Content-Length")) 
          != this->request.getHeader().end()) {
        this->request.addContentLengthEntity(this->head_buf + this->request.getRawHead().length(), head_rest_len);
      }
      // else if (((it = this->request.getHeader().find("Transfer-Encoding"))
      //     != this->request.getHeader().end()) && (it->second == "Chunked")) {
      //   this->request.addChunkedEntity(entity_buf);
      // } else {
      //   throw std::string("error?");
      // }

      // TODO: 나중에 head_rest_len 이랑 read_len 이랑 더한 값과 Content_Length 값 비교하기
      int read_len = safeRead(this->socket_fd, entity_buf, MAX_BODY_SIZE);
      entity_buf[read_len] = '\0';

      // 2-1. content-length 파싱 ----------------------------------------------
      if ((it = this->request.getHeader().find("Content-Length")) 
          != this->request.getHeader().end()) {
        this->request.addContentLengthEntity(entity_buf, read_len);
      }
      // 2-2. chunked 파싱 -----------------------------------------------------
        // else if (((it = this->request.getHeader().find("Transfer-Encoding"))
        //     != this->request.getHeader().end()) && (it->second == "Chunked")) {
        //   this->request.addChunkedEntity(entity_buf);
      // } else {
      //   throw std::string("error?");
      // }
    } 
    this->request.setEntityDone(true);
  // std::cout << GRY << "Debug: Transaction::executeRead\n";
  }
  return 0;
}

void Transaction::executeReadHead() {
    this->read_head_len = safeRead(this->socket_fd, this->head_buf, MAX_HEAD_SIZE);
    this->head_buf[this->read_head_len] = '\0';
    std::istringstream  read_stream;
    read_stream.str(this->head_buf);

    std::string line;
    while (std::getline(read_stream, line, '\n')) {
      if (line.length() == 0 || line == "\r") {
        this->request.setRawHead(line + "\n");
        this->request.setHeadDone(true);
        this->request.parserHead();
        break;
      } else if (!this->request.getHeadDone()) {
        this->request.setRawHead(line + "\n");
        if (read_stream.eof()) {
          throw std::string("executeRead error : over max-header-size");
        }
      }
    }
}

// void Transaction::executeReadContentLengthEntity() {
//   int head_rest_len = this->read_head_len - (this->request.getRawHead().length());
//   this->request.addContentLengthEntity(this->head_buf + this->request.getRawHead().length(), head_rest_len);
//   int read_len = safeRead(this->socket_fd, entity_buf, MAX_BODY_SIZE);
//   entity_buf[read_len] = '\0';
//   this->request.addContentLengthEntity(entity_buf, read_len);
//   this->request.setEntityDone(true);
// }

int Transaction::executeWrite(void) {
  if (this->response.getEntityDone() &&
      (safeWrite(this->socket_fd, this->response) == -1)) {
    return -1;
  }
  // std::cout << GRY << "Debug: Transaction::executeWrite\n";
  return 0;
}

int Transaction::executeMethod(void) {
  if ((this->request.getMethod() == "POST" && !this->request.getEntityDone())
    || ((this->request.getMethod() != "POST") && this->request.getEntityDone())) {
    return 0;
  }

  //DEBUG
  this->request.toString();

  int status = this->httpCheckStartLine();
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
      this->response.setStatusMsg("Internal Server Error :");
      break;
    case 501:
      this->response.setStatusCode("501");
      this->response.setStatusMsg("Not Implemented");
      break;
    default:
      std::cout << "(tmp msg) excuteTransaction: swith: default\n";
      break;
  }
  this->response.setResponseMsg();
  // std::cout << GRY << "Debug: Transaction::executeMethod\n";
  return 0;
}

//---- check sl --------------------------------------------
int Transaction::httpCheckStartLine() {
  if (!(this->request.getMethod() == "GET" ||
        this->request.getMethod() == "POST" ||
        this->request.getMethod() == "DELETE")) {
    return (501);  // Not Implemented.
  }
  // std::cout << "rootdir: " << this->root_dir << "\n"
  // << "getUrl: " << this->request.getUrl() << std::endl;
  if (access((this->root_dir + this->request.getUrl()).c_str(), F_OK) == -1) {
    return (404);  // Not Found.
  }
  // std::cout << GRY << "Debug: Transaction::httpCheckStartLine\n";
  return (0);
}

//---- HTTP methods --------------------------------------------
int Transaction::httpGet(void) {
  std::cout << GRY << "Transaction: httpGet\n" << DFT;
  std::ifstream resource(root_dir + this->request.getUrl());

  if (!resource) {
    return 500;
  }
  std::string content((std::istreambuf_iterator<char>(resource)),
                      std::istreambuf_iterator<char>());

  std::stringstream content_length;
  content_length << content.length();

  this->response.setHeader("Content-Length", content_length.str());
  this->response.setEntity(content);
  // std::cout << GRY << "Debug: Transaction::httpGet\n";
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

// ---- safe functions -----------------------------------------
int Transaction::safeRead(int fd, char *buf, int size) {
  int read_len;

  if ((read_len = read(fd, buf, size)) == -1) {
    throw std::string("client read error!");
  }
  // std::cout << GRY << "Debug: Transaction::safeRead\n";
  return read_len;
}

int Transaction::safeWrite(int fd, Response &response) {
  int write_len;

  if ((write_len = write(fd, response.getResponseMsg().c_str(),
                         response.getResponseMsg().size())) == -1) {
    throw std::string("client write error!");
  }
  // std::cout << GRY << "Debug: Transaction::safeWrite\n";
  return write_len;
}
