/*
DELETE 메소드는 서버에서 클라이언트가 지정한 리소스를 삭제하는 메소드입니다.
이 메소드를 구현하기 위해서는 파일을 삭제하는 기능이 필요합니다.
C++에서는 <cstdio> 라이브러리의 remove 함수를 이용하여 파일을 삭제할 수
있습니다. httpDelete 함수는 지정된 경로의 파일을 삭제하는 기능을 가지고
있습니다. 경로는 Transaction 객체의 멤버 변수인 request와 server를 통해 얻을 수
있습니다. server는 현재 서버 객체를 가리키고 있으며, 서버 객체의 루트 디렉토리와
request 객체의 URL을 조합하여 파일의 전체 경로를 구합니다. 그리고 remove 함수를
이용하여 파일을 삭제합니다.
*/

/*
void Transaction::httpDelete(void) {
  std::string file_path = this->server->getRoot() + this->request.getUrl();
  if (std::remove(file_path.c_str()) == 0) {  // 파일 삭제 성공
    this->response.setStatus("204");
  } else {  // 파일 삭제 실패
    this->response.setStatus("500");
  }
}
*/

/* 경로는 마찬가지로 Transaction 객체의 멤버 변수인 request와 server를 통해 얻을
수 있습니다. server는 현재 서버 객체를 가리키고 있으며, 서버 객체의 루트
디렉토리와 request 객체의 URL을 조합하여 파일의 전체 경로를 구합니다. 그리고
std::ofstream 객체를 생성하여 파일을 열고, 요청 메세지 바디의 데이터를 이용하여
파일을 업데이트합니다. 마지막으로 std::ofstream 객체를 닫고, 응답 메세지의 상태
코드를 설정합니다.
*/

/*
void Transaction::httpPost(void) {
  std::string file_path = this->server->getRoot() + this->request.getUrl();
  std::ofstream ofs(file_path.c_str(), std::ios::out | std::ios::trunc);
  if (!ofs.is_open()) {  // 파일 생성 실패
    this->response.setStatus("500");
  } else {  // 파일 생성 성공
    ofs << this->request.getBody();
    ofs.close();
    this->response.setStatus("201");
  }
}
*/