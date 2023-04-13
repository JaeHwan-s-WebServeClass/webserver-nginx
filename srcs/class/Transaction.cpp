#include "Transaction.hpp"
#include <exception>

//---- constructor --------------------------------------------
Transaction::Transaction(int socket_fd, std::string root_dir)
    : socket_fd(socket_fd), root_dir(root_dir) {
  std::cout << "transaction constructed\n";
}

//---- getter ---------------------------------------------
Response &Transaction::getResponse() { return this->response; }

Request &Transaction::getRequest() { return this->request; }

//---- check & execute ---------------------------------------
void Transaction::executeTransaction() {
  int status = this->httpCheckStartLine();
  if (!status) {
//    status = method GET/POST/DELETE
    // switch(this->request.getMethod()) {
    //   case GET :
    //   case POST :
    //   case DELETE :
    // }
    if (this->request.getMethod() == "GET") {
      status = this->httpGet();
    } else if (this->request.getMethod() == "POST") {
      status = this->httpPost();
    } else if (this->request.getMethod() == "DELETE") {
      status = this->httpDelete();
    }
  }

  // switch (color)
  //   {
  //       case COLOR_GREEN:
  //           std::cout << "Green";
  //           break;
  //       case COLOR_BLUE:
  //           std::cout << "Blue";
  //           break;
  //       default:
  //           std::cout << "Unknown";
  //           break;
  //   }
  switch(result)
    200:
    404:
    500:
    501:
}

int  Transaction::httpCheckStartLine() {
  if (!(this->request.getMethod() == "GET" || this->request.getMethod() == "POST" ||
        this->request.getMethod() == "DELETE")) {
    return (501); // Not Implemented.
  }
// method, resource, GET DELETE POST 까지 코드 진행하며 상태코드를 int 로 반환 후 switch case 로 한번에 처리
  if (access((this->root_dir + this->request.getUrl()).c_str(), F_OK) == -1) {
    return (404); // Not Found.
  }
  return (0);
}

  // if (request->getMethod() == "GET") {
  //   // if (access("file.txt", F_OK) != -1) {
  //   //         std::cout << "file.txt exists!\n";
  //   //     } else {
  //   //         std::cout << "file.txt does not exist.\n";
  //   // }
  // } else if (request->getMethod() == "POST") {
  //   //access
  // } else if (request->getMethod() == "DELETE") {
  //   //access
  // }

  // 있으면 ok를 주고 entity도 넘겨줄 수 있어야한다.
  // 파일 내부에 있는 내용을 소켓에 써줘야하기 때문에 fopen를 사용해야할까?

  // 체크할 때 파일을 열어서 체크해야하나?
  // fopen
  // 파일명으로 확인하기?
  // 파일 내용을 바디에 담아서 전달해줘야하나?
  // 읽어들여서 보내야하는지
  // 파일 자체를 보내줘야 하는지
}

//---- HTTP methods --------------------------------------------
int Transaction::httpGet(Request *request) {
  // 리소스가 오면 그게 있는지 검사해서
  std::ifstream resource(root_dir + request->getUrl());

  // 파일 열기에 실패했을 때 처리하는 코드
  if (resource.bad()) {
    std::cerr << "Error opening file.\n";
  } else {
    // 있으면 ok를 주고 entity도 넘겨줄 수 있어야한다.
  }

  // 파일 내부에 있는 내용을 소켓에 써줘야하기 때문에 fopen를 사용해야할까?

  // 체크할 때 파일을 열어서 체크해야하나?
  // fopen
  // 파일명으로 확인하기?
  // 파일 내용을 바디에 담아서 전달해줘야하나?
  // 읽어들여서 보내야하는지
  // 파일 자체를 보내줘야 하는지
}

int Transaction::httpDelete(Request *request) {}

int Transaction::httpPost(Request *request) {}
