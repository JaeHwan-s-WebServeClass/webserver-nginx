#include "Transaction.hpp"
#include <exception>

//---- constructor --------------------------------------------
Transaction::Transaction(int socket_fd, std::string root_dir)
    : socket_fd(socket_fd), root_dir(root_dir) {
  std::cout << GRY << "Transaction: constructor\n" << DFT;
}

//---- getter ---------------------------------------------
Response &Transaction::getResponse() { return this->response; }

Request &Transaction::getRequest() { return this->request; }

//---- check & execute ---------------------------------------
void Transaction::executeTransaction(const char *buf) {
  std::cout << GRY << "Transaction: executeTransaction\n" <<  DFT;
  this->request.setRawMsg(buf);
  if (!this->request.isDone()) {
    return;
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
  switch(status){
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
}

int  Transaction::httpCheckStartLine() {
  if (!(this->request.getMethod() == "GET" || this->request.getMethod() == "POST" ||
        this->request.getMethod() == "DELETE")) {
    return (501); // Not Implemented.
  }
// method, resource, GET DELETE POST 까지 코드 진행하며 상태코드를 int 로 반환 후 switch case 로 한번에 처리
  std::cout << "rootdir: " << this->root_dir << " " << "getUrl: " << this->request.getUrl() << std::endl;
  if (access((this->root_dir + this->request.getUrl()).c_str(), F_OK) == -1) {
    return (404); // Not Found.
  }
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
// std::string을 사용하여 파일을 한 번에 읽으면, 파일 크기와 같은 크기의 메모리를 사용합니다. 파일이 매우 큰 경우, 이는 메모리 부족으로 이어질 수 있습니다.
// 2. 대용량 파일 처리 문제
// 대용량 파일 처리에는 더 좋은 방법이 있습니다. 위의 코드는 파일 내용을 한 번에 읽어들이기 때문에, 매우 큰 파일을 처리할 때는 속도가 느려질 수 있습니다.
// 3. 문자 인코딩 문제
// std::string은 바이트 단위로 문자열을 처리하기 때문에, 파일의 문자 인코딩이 유니코드가 아닌 경우, 문자열이 올바르게 처리되지 않을 수 있습니다. 이 경우, 유니코드를 지원하는 std::wstring을 사용해야 합니다.
  std::string   content((std::istreambuf_iterator<char>(resource)),
                         std::istreambuf_iterator<char>());
  
  std::stringstream content_length;
  content_length << content.length();
 
  this->response.setHeader("Content-Length", content_length.str());
  this->response.setEntity(content);
  return 200;
}

int Transaction::httpDelete(void) {
  std::cout <<  GRY << "Transaction: httpDelete\n" << DFT;
  return 200;
}

int Transaction::httpPost(void) {
  std::cout <<  GRY << "Transaction: httpPost\n" << DFT;
  return 200;
}
