#include "Transaction.hpp"

// 1. head / cl => MAX size랑 실제로 read한 size 비교문 
// 2. set head done => head 함수로 올리기 => entity 조건문 수정 필요

// Todo
// 1. content_length 변수 추가 
// 2. getEntitySize 함수 추가 

//---- execute --------------------------------------------
int Transaction::executeRead(void) {
  char  buf[BUFFER_SIZE];
  int read_len = safeRead(this->socket_fd, buf, BUFFER_SIZE);
  int head_rest_len = 0;

  // 1. head 파싱 -----------------------
  if (!this->request.getHeadDone()) {
    buf[read_len] = '\0';
    std::istringstream  read_stream;
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
	
     head_rest_len = read_len - this->request.getRawHead().length();
  }
  // 2. entity 파싱 -----------------------
  //if (this->request.getHeadDone() && (this->request.getMethod() == "GET")) {
  if (this->request.getMethod() == "GET") {
    if (!this->request.getEntityDone()){
      std::map<std::string, std::string>::const_iterator it;
      
	  // content length 일 때
      if ((it = this->request.getHeader().find("Content-Length")) 
          != this->request.getHeader().end()) {
		// content_length 맴버 변수에 대충 저장 -> request 클래스에 변수 추가 및 headParse에서 처리
        // this->content_length = std::atoi(it->second.c_str());

        // 47 - 51 : entity 채우는 부분	
        if (head_rest_len) {
          this->request.addContentLengthEntity(buf + this->request.getRawHead().length(), head_rest_len);
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

      } else if (((it = this->request.getHeader().find("Transfer-Encoding"))
          != this->request.getHeader().end()) && (it->second == "Chunked")) {
        if (this->request.getHeadDone()) {
          this->request.addChunkedEntity(buf);
        } else {
          this->request.addChunkedEntity(buf + this->request.getRawHead().length());
          this->request.setHeadDone(true);
        }
        // if (청크 사이즈 == 0)
          // this->request.setEntityDone(true);
      } else {
        throw std::string("error?");
      }
    }
    //this->request.setEntityDone(true);
  }
  return 0;
}