#include "Request.hpp"
#include <cstring>

//---- constructor ---------------------------------------
Request::Request() : raw_head(""), head_done(0), entity_done(false) {
  entity.reserve(256);
  // std::cout << GRY << "Debug: Request::contructor\n" << DFT;
}

//---- setter --------------------------------------------
void Request::setRawHead(std::string line) { this->raw_head += line; }
void Request::setHeadDone(bool type) { this->head_done = type; } // status? type?
void Request::setEntityDone(bool type ) { this->entity_done = type; }

void Request::addContentLengthEntity(char * buf, int read_len) {
  //if (entity.size() < read_length) {
    //std::cout << "addContentLengthEntity buf: " << buf << std::endl;
    for (int i = 0; i < read_len; ++i) {
      this->entity.push_back(buf[i]);
    }
  //}
}

// RFC7230 - 4.1.3.  Decoding Chunked

//   A process for decoding the chunked transfer coding can be represented
//   in pseudo-code as:

//     length := 0
//     read chunk-size, chunk-ext (if any), and CRLF
//     while (chunk-size > 0) {
//        read chunk-data and CRLF
//        append chunk-data to decoded-body
//        length := length + chunk-size
//        read chunk-size, chunk-ext (if any), and CRLF
//     }
//     read trailer field
//     while (trailer field is not empty) {
//        if (trailer field is allowed to be sent in a trailer) {
//            append trailer field to existing header fields
//        }
//        read trailer-field
//     }
//     Content-Length := length
//     Remove "chunked" from Transfer-Encoding
//     Remove Trailer from existing header fields

void Request::addChunkedEntity(char * buf, int read_len) {
// read buf ->(parsing).\r\n -> request.entity(vec)
  int i = 0;
  std::string hex_str;
  int chunk_size;
  bool mode = CHUNK_SIZE; // size or entity

  while(i < read_len) {
    if (mode == CHUNK_SIZE) {
      // buf를 /r/n 을 딜리미터로 파싱해서, hex_str 을 구해야함   
      if (buf[i] != '\r' && buf[i + 1] && buf[i + 1] != '\n') {
        hex_str += buf[i];
      } else if (buf[i] == '\r' && buf[i + 1] && buf[i + 1] == '\n') {
        chunk_size = ft::hexToInt(hex_str); // FIXME overflow 문제 처리
        mode = CHUNK_ENTITY;
      }
    } else {
      while (chunk_size) {
      // buf[index] ++ 하면서 chunk_size 만큼 vector 로 옮겨주기?
      }
      mode = CHUNK_SIZE; 
      chunk_size = 0;
      --chunk_size;
    }
    ++i;
  }
  // 청크사이즈만큼 다 읽은 후 바로 다음이 0\r\n 일 때 끝
}
//---- getter --------------------------------------------
const bool Request::getEntityDone() { return this->entity_done; }
const std::string& Request::getRawHead() const { return this->raw_head; }
const bool& Request::getHeadDone() const { return this->head_done; }
const std::string& Request::getMethod() const { return this->method; }
const std::string& Request::getUrl() const { return this->url; };
const std::string& Request::getHttpVersion() const { return this->http_version; }
const std::map<std::string, std::string>& Request::getHeader() const { return this->header; }
const std::vector<char> & Request::getEntity() const { return this->entity; }
const size_t Request::getEntitySize() const { return this->entity.size(); }

//---- utils --------------------------------------------
void Request::clearSetRawMsg() { this->raw_head.clear(); }

void Request::toString() const {
  std::cout << GRY << "-------------------- start-line --------------------"
            << DFT << std::endl;
  std::cout << BLU << "method: " << DFT << this->method << std::endl;
  std::cout << BLU << "url: " << DFT << this->url << std::endl;
  std::cout << BLU << "version: " << DFT << this->http_version << std::endl;
  std::cout << GRY << "---------------------- header ----------------------"
            << DFT << std::endl;
  for (std::map<std::string, std::string>::const_iterator it = header.begin();
       it != header.end(); ++it) {
    std::cout << BLU << it->first << ": " << DFT << it->second << std::endl;
  }
  std::cout << GRY << "--------------------- entity -----------------------"
            << DFT << std::endl;
   for (int i = 0; i < entity.size(); i++) {
  	std::cout << YLW << entity[i];
   }
  std::cout << DFT << std::endl;
  std::cout << GRY << "----------------------------------------------------"
            << DFT << std::endl;
  // std::cout << YLW << entity << DFT << std::endl;
  // std::cout << GRY << "Debug: Request::toString\n" << DFT;
}

void Request::parserHead() {
  std::vector<std::string> tmp_head;
  std::vector<std::string> tmp_start_line;

  // 개행기준으로 split후 space 기준으로 start line split
  tmp_head = ft::split(this->raw_head, '\n');
  tmp_start_line = ft::split(tmp_head[0], ' ');
  this->method = tmp_start_line[0];
  this->url = tmp_start_line[1];
  this->http_version = tmp_start_line[2];

  // header를 처리
  for (std::vector<std::string>::iterator it = tmp_head.begin() + 1;
       it != tmp_head.end(); ++it) {
    int pos = it->find(':');
    header[it->substr(0, pos)] = it->substr(pos + 1);
  }
  // std::cout << GRY << "Debug: Request::setRawMsg\n" << DFT;
}
