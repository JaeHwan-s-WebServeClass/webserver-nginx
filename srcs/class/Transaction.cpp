#include "Transaction.hpp"

#include <exception>

// Todo
// 1. content_length 변수 추가
// 2. MAX_(HEAD,BODY)_SIZE 조건문

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
  // BUFFER_SIZE + 1을 할 것인가에 대한 논의
  char buf[BUFFER_SIZE + 1];
  int read_len = safeRead(this->socket_fd, buf, BUFFER_SIZE);
  int head_rest_len = 0;

  // if head일 때, else body일 때

  // 1. head 파싱 -----------------------
  if (!this->request.getHeadDone()) {
    head_rest_len = this->executeReadHead(buf, read_len);

    // 세팅된 헤드가 max head size 보다 클 경우, error throw
    // if (this->request.getRawHead().length() > MAX_HEAD_SIZE)
    //   throw std::string("error : executeRead : over max head size\n");
  }
  

  // 2. entity 파싱 -----------------------
  if (this->request.getHeadDone() && (this->request.getMethod() == "GET")) {
    std::cout << RED << "(41)here is entity parsing\n" << DFT;
    if (!this->request.getEntityDone()) {
      std::map<std::string, std::string>::const_iterator it =
          this->request.getHeader().find("Content-Length");
      std::map<std::string, std::string>::const_iterator it2 =
          this->request.getHeader().find("Transfer-Encoding");

      // content length 일 때
      //!= this->request.getHeader().end()
      if (it->first == "Content-Length") {
        // DEBUG
        std::cout << RED << "(49)here is Content-Length \n" << DFT;
        // content_length 맴버 변수에 대충 저장 -> request 클래스에 변수 추가 및
        // headParse에서 처리
        // this->content_length = std::atoi(it->second.c_str());

        // 47 - 51 : entity 채우는 부분
        if (head_rest_len) {
          this->request.addContentLengthEntity(
              buf + this->request.getRawHead().length(), head_rest_len);
        } else {
          this->request.addContentLengthEntity(buf, read_len);
        }

        // 55-59 : 읽은 entity가 조건에 맞는지 : content-length && max-Body-Size
        // content_length == getEntitySize 일때 setEntityDone
        // if (this->request.getEntitySize() == this->content_length) {
        //   this->request.setEntityDone(true);
        // } else if (this->request.getEntitySize() > this->content_length) {
        // 			// error_handle;
        // }
        // else if (this->request.getEntitySize() > MAX_BODY_SIZE)
        // error_handle;

      } else if ((it2->first == "Transfer-Encoding") &&
                 (it2->second == "Chunked\r")) {
        // DEBUG
        std::cout << RED << "(76)here is chunked \n" << DFT;
        if (head_rest_len) {
          this->request.addChunkedEntity(
              buf + this->request.getRawHead().length(), head_rest_len);
        } else {
          this->request.addChunkedEntity(buf, read_len);
        }
      }
      // else {
      //   throw std::string("error?");
      // }
    }
    // this->request.setEntityDone(true);
    // std::cout << GRY << "Debug: Transaction::executeRead\n";
  }
  return 0;
}

int Transaction::executeReadHead(char *buf, int read_len) {
  // 들어오는 데이터가 텍스트가 아닐수도있어 필요가 없?
  // 헤드는 무조건 텍스트로 들어오지 않을까요?
  // 터미널에서 보내는게 아니어서 상관없을거같아요..
  buf[read_len] = '\0';
  std::istringstream read_stream;
  read_stream.str(buf);

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
  // 읽은 head가 max-head-size에 맞는지 확인

  return (read_len - this->request.getRawHead().length());
}

// void Transaction::executeReadContentLengthEntity() {
//   int head_rest_len = this->read_head_len -
//   (this->request.getRawHead().length());
//   this->request.addContentLengthEntity(this->head_buf +
//   this->request.getRawHead().length(), head_rest_len); int read_len =
//   safeRead(this->socket_fd, entity_buf, MAX_BODY_SIZE); entity_buf[read_len]
//   = '\0'; this->request.addContentLengthEntity(entity_buf, read_len);
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
  if ((this->request.getMethod() == "POST" && !this->request.getEntityDone()) ||
      ((this->request.getMethod() != "POST") && !this->request.getHeadDone())) {
    return 0;
  }

  // DEBUG
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
