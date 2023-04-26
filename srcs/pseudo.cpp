#include <cstdio>   // FILE
#include "./class/Transaction.hpp"

// function 1
int Transaction::checkResource() {
	int file_fd;

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
			std::cout << "checkResouce :: return error page fd\n";
			//file_fd = std::fopen(, )._file;
			//return ("404.html");
		}
		request_location = this->request.getUrl().substr(0, pos);
		request_filename = this->request.getUrl().substr(pos);
	}
	std::map<std::string, ServerConfig::t_location>::const_iterator config_location;
	if ((config_location = this->server_config.getLocation().find(request_location)) != this->server_config.getLocation().end()) {
		std::string loc_root = config_location->second.root;
		resource = server_config.getRoot() + loc_root + request_filename;
		if (access(resource.c_str(), F_OK) == -1) {
		//  TODO 에러 파일 받아와서 등록하기
			std::cout << "checkResouce :: return error page fd\n";  // cannot found request_filename
		}
		file_fd = std::fopen(resource.c_str(), "r+")->_file;
	} else {
		//  TODO 에러 파일 받아와서 등록하기
		std::cout << "checkResouce :: return error page fd\n"; // Invalid directory :cannot found reqeust_location
	}
    return (file_fd);
}

// function 2
int Transaction::executeMethod(void) {
  if ((this->request.getMethod() == "POST" && this->flag < REQUEST_ENTITY) ||
      ((this->request.getMethod() != "POST") && this->flag < REQUEST_HEAD)) {
    return 0;
  }

	// url은 이미 확인한 상황?
	// 각 method의 allow와 deny를 확인하기 전에,
	// root의 method를 적용하느냐 or location의 method를 적용하느냐는 location의 key 값에 의해 결정된다.
	// ex) server에서는 default로 GET만을 허용하고 location에서 allow/deny를 따진다?

  if (!(this->request.getMethod() == "GET" ||
      this->request.getMethod() == "POST" ||
      this->request.getMethod() == "DELETE")) {
    return (501);  // Not Implemented.
  }

	int status = checkMethod();

  status = std::atoi(this->response.getStatusCode().c_str()); // startLine 해석 (resource 정의)
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
      std::cout << "(tmp msg) excuteTransaction: swith: default\n";
      break;
  }
  this->response.setResponseMsg();
  std::cout << GRY << "Debug: Transaction::executeMethod\n";
  return 0;
}


int Transaction::checkMethod() {

	if (this->location.root == "")

	//this->request.getMethod() , this->location->http_method

	// 1, location 으로 비트 만들기
	this->request.getMethod();

}
/*

kevent 에 등록되어 있다.

EVENT_READ
	server_socket : accept
	client_socket : buffer read => request setting
	file_fd : file read => execute

파일을 언제 읽고 어떻게 써야 할까??


server.cpp 에서 2개의 함수를 호출하자

1. fd 를 반환하는 함수
2. execute 를 할 함수


client 가 요청을 보냈다 => ServerSocket client_socket accept => executeRead() : request msg 완성

request msg 다  읽었는지 검사
if ((this->request.getMethod() == "POST" && !this->request.getEntityDone()) ||
      ((this->request.getMethod() != "POST") && !this->request.getHeadDone())) {
    return 0;
  }

  GET => url 의 파일을 open & read => response 에 담아야 한다.

  POST => request entity 에 담긴 내용을 파일 생성 후 담아야 한다. (파일 open & write)

  DELETE => request 확인 후 해당 파일을 삭제해야 한다.

*/






