#include "Transaction.hpp"

#include <exception>


//---- constructor --------------------------------------------
Transaction::Transaction(int socket_fd, const ServerConfig & server_config)
    : socket_fd(socket_fd), server_config(server_config), request(this->flag) {
  // std::cout << GRY << "Debug: Transaction::constructor\n" << DFT;
}

//---- getter ---------------------------------------------
Response &Transaction::getResponse() { return this->response; }
Request &Transaction::getRequest() { return this->request; }

//---- setter ---------------------------------------------


//---- execute --------------------------------------------
int Transaction::executeRead(void) {
  // BUFFER_SIZE + 1을 할 것인가에 대한 논의
  char buf[BUFFER_SIZE + 1];
  int read_len = safeRead(this->socket_fd, buf, BUFFER_SIZE);
  int head_rest_len = 0;

  if (read_len == -1) {
    return -1;
  }
  // 1. head 파싱 -----------------------
  if (this->flag == START) {
    head_rest_len = this->executeReadHead(buf, read_len);
  }
  // 2. entity 파싱 -----------------------
  if (this->flag == REQUEST_HEAD && (this->request.getMethod() == "GET")) {
    if (this->flag < REQUEST_ENTITY) {
      this->executeReadEntity(buf, read_len, head_rest_len);
    }
    // std::cout << GRY << "Debug: Transaction::executeRead\n";
  }

  if ((this->request.getMethod() == "POST" && this->flag == REQUEST_ENTITY) 
       || (this->request.getMethod() != "POST" && this->flag == REQUEST_HEAD)) {
      this->setFlag(REQUEST_DONE);
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
      this->flag == REQUEST_HEAD;

      // DEBUG
      std::cout << GRY << "-------------------- raw head ----------------------"
                << DFT << std::endl;
      std::cout << GRY << this->request.getRawHead() << DFT << std::endl;
      std::cout << GRY << "----------------------------------------------------"
                << DFT << std::endl;

      this->request.parserHead();
      this->request.setRawHead(line + "\n");
      break;
    } else if (this->flag == START) {
      this->request.setRawHead(line + "\n");
      if (read_stream.eof()) {
        throw std::string("executeRead error : over max-header-size");
      }
    }
  }
  if (this->request.getRawHead().length() > MAX_HEAD_SIZE) {
    throw std::string("Error: Transaction: Request Head Over MAX HEAD SIZE");
  }
  return (read_len - this->request.getRawHead().length());
}

void Transaction::executeReadEntity(char *buf, int read_len,
                                    int head_rest_len) {
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
    throw std::string("Error: Transaction: Invalid Request Header");
  }
  // std::cout << "content-length: " << this->request.getContentLength()
  //           << std::endl;
  // std::cout << "entity-size: " << this->request.getEntitySize() << std::endl;

  if (this->request.getEntitySize() > MAX_BODY_SIZE) {
    throw std::string("Error: Transaction: Request Entity Over MAX BODY SIZE");
  }
}

int Transaction::executeWrite(void) {
  if (this->flag == REQUEST_ENTITY &&
      (safeWrite(this->socket_fd, this->response) == -1)) {
    return -1;
  }
  // std::cout << GRY << "Debug: Transaction::executeWrite\n";
  return 0;
}

int Transaction::executeMethod(void) {


  if ((this->request.getMethod() == "POST" && this->flag < REQUEST_ENTITY) ||
      ((this->request.getMethod() != "POST") && this->flag < REQUEST_HEAD)) {
    return 0;
  }

  // DEBUG
  this->request.toString();
  // allow method check
  if (!(this->request.getMethod() == "GET" ||
      this->request.getMethod() == "POST" ||
      this->request.getMethod() == "DELETE")) {
    return (501);  // Not Implemented.
  }

  // defineUrl();
  int status = std::atoi(this->response.getStatusCode().c_str()); // startLine 해석 (resource 정의)

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
  // std::cout << GRY << "Debug: Transaction::executeMethod\n";
  return 0;
}

// function 1 : return file_fd
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
    this->setFlag(FILE_OPEN);
	} else {
		//  TODO 에러 파일 받아와서 등록하기
		std::cout << "checkResouce :: return error page fd\n"; // Invalid directory :cannot found reqeust_location
	}
    return (file_fd);
}

//---- HTTP methods --------------------------------------------
int Transaction::httpGet(void) {
  std::cout << GRY << "Transaction: httpGet\n" << DFT;
  std::ifstream resource(this->server_config.getRoot() + this->request.getUrl()); // server_config? rootdir?

// buf[BUFFER_SIZE] 함수 지역변수
// int file_all_read_flag 트랜잭션 멤버 변수?

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
  int recv_len;

  signal(SIGPIPE, SIG_IGN);
  if ((recv_len = recv(fd, buf, size, 0)) == -1) {
    std::cerr << RED << "client read error!\n" << DFT;
  }
  signal(SIGPIPE, SIG_DFL);
  // std::cout << GRY << "Debug: Transaction::safeRead\n";
  return recv_len;
}

int Transaction::safeWrite(int fd, Response &response) {
  int send_len;

  signal(SIGPIPE, SIG_IGN);
  if ((send_len = send(fd, response.getResponseMsg().c_str(),
                       response.getResponseMsg().size(), 0)) == -1) {
    std::cerr << RED << "client write error!\n" << DFT;
  }
  signal(SIGPIPE, SIG_DFL);
  // std::cout << GRY << "Debug: Transaction::safeWrite\n";
  return send_len;
}
