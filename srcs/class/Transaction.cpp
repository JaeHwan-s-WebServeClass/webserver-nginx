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
/*
      1-1. head parsing이 끝나지 않은 경우
           ㄴ 더 읽어들여야한다.
      1-2. head parsing이 끝난 경우
           ㄴ head가 끝났다면 2(body에 대한 parsing)로 넘어간다
          2-1. content-length의 존재하는 경우
               ㄴ content-length의 value를 받아와서 활용한다.
          2-2. content-length의 존재하지 않는 경우
               ㄴ chunk로 처리 => gnl식 처리
    */

    /* pseudo code
    setRawMsgHead() {
      if (head의 content-length 옵션) {
        content-length의 길이만큼 Request의 entity 를 채워준다.
      }
      else if (청크일 경우) {
      }
      else  { Error! }
    }
    */

int Transaction::executeRead(void) {
  char buf[BUFFER_SIZE];
  int read_len = safeRead(this->socket_fd, buf);

  if (read_len == 0) {
    return -1;
  }
  buf[read_len] = '\0';
  
  // head 파싱 -----------------------
  if (!this->request.getHeadDone()) {
    std::istringstream  read_msg;
    read_msg.str(buf);

    std::string line;
    while (std::getline(read_msg, line, '\n')) {
      if (line.length() == 0 || line == "\r") {
        this->request.setHeadDone(true);
        this->request.parserHead();
        read_msg.str().copy(buf, read_msg.gcount());
        break;
      } else if (!this->request.getHeadDone()) {
        // 헤드를 한 번에 읽어오지 못하는 경우(config 의 max header size 보다 큰 데이터가 들어올 때)
        // 에러 상황으로 간주하는 코드로 구조를 바꿈. 원래는 큰 헤더가 들어왔을 때 반복문 돌며 쪼개서 다 받음.
        this->request.setRawHead(line + "\n");
        if (read_msg.eof()) {
          throw ("executeRead error : over max-header-size");
        }
      }
    }
  }

  // entity 파싱 -----------------------
  if (this->request.getHeadDone() && (this->request.getMethod() == "POST")) {
    if (!this->request.getDone()){
      std::map<std::string, std::string>::const_iterator it;
      
      if ((it = this->request.getHeader().find("Content-Length")) 
          != this->request.getHeader().end()) {
        

      }
      else if (((it = this->request.getHeader().find("Transfer-Encoding"))
          != this->request.getHeader().end()) && (it->second == "Chunked")) {
      }
    } else {
      this->request.setDone(true);
    }
  return 0;
  // std::cout << GRY << "Debug: Transaction::executeRead\n";
  }
}

int Transaction::executeWrite(void) {
  if (this->response.getDone() &&
      (safeWrite(this->socket_fd, this->response) == -1)) {
    return -1;
  }
  // std::cout << GRY << "Debug: Transaction::executeWrite\n";

  return 0;
}

int Transaction::executeMethod(void) {
  if (!this->request.getDone()) {
    return 0;
  }
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
  // 1. 메모리 사용량
  // std::string을 사용하여 파일을 한 번에 읽으면, 파일 크기와 같은 크기의
  // 메모리를 사용합니다. 파일이 매우 큰 경우, 이는 메모리 부족으로 이어질 수
  // 있습니다.
  // 2. 대용량 파일 처리 문제
  // 대용량 파일 처리에는 더 좋은 방법이 있습니다. 위의 코드는 파일 내용을 한
  // 번에 읽어들이기 때문에, 매우 큰 파일을 처리할 때는 속도가 느려질 수
  // 있습니다.
  // 3. 문자 인코딩 문제
  // std::string은 바이트 단위로 문자열을 처리하기 때문에, 파일의 문자 인코딩이
  // 유니코드가 아닌 경우, 문자열이 올바르게 처리되지 않을 수 있습니다. 이 경우,
  // 유니코드를 지원하는 std::wstring을 사용해야 합니다.
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
int Transaction::safeRead(int fd, char *buf) {
  int read_len;

  if ((read_len = read(fd, buf, BUFFER_SIZE)) == -1) {
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