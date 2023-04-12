#include "Transaction.hpp"

//---- constructor --------------------------------------------
Transaction::Transaction(int socket_fd, std::string root_dir)
    : socket_fd(socket_fd), root_dir(root_dir) {
  std::cout << "transaction constructed\n";
}

//---- getter ---------------------------------------------
Response &Transaction::getResponse() { return this->response; }

Request &Transaction::getRequest() { return this->request; }

//---- methods --------------------------------------------
void Transaction::httpGet(Request *request) {
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

void Transaction::httpDelete(Request *request) {}

void Transaction::httpPost(Request *request) {}