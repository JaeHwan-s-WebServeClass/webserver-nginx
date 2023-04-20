#include "Request.hpp"

#include <cstring>

//---- constructor ---------------------------------------
Request::Request()
    : raw_head(""),
      head_done(0),
      entity_done(false),
      chunk_size(0),
      hex_str("") {
  entity.reserve(256);
  // std::cout << GRY << "Debug: Request::contructor\n" << DFT;
}

//---- setter --------------------------------------------
void Request::setRawHead(std::string line) { this->raw_head += line; }
void Request::setHeadDone(bool type) {
  this->head_done = type;
}  // status? type?
void Request::setEntityDone(bool type) { this->entity_done = type; }

void Request::addContentLengthEntity(char* buf, int read_len) {
  // if (entity.size() < read_length) {
  // std::cout << "addContentLengthEntity buf: " << buf << std::endl;
  for (int i = 0; i < read_len; ++i) {
    this->entity.push_back(buf[i]);
  }
  //}
}

void Request::addChunkedEntity(char* buf, size_t read_len) {
  size_t i = 0;

  while (i < read_len) {
    if (chunk_size == 0) {
      hex_str += buf[i];
      if (hex_str.back() == '\n') {
        hex_str.pop_back();
        hex_str.pop_back();
        chunk_size = ft::hexToInt(hex_str);
        std::cout << "chunk_size: " << chunk_size << "\n";
        if (chunk_size == 0) {
          this->setEntityDone(true);
          return;
        } else if (chunk_size < 0) {
          throw std::string("Error: Request:: Chunk size overflow");
        }
        chunk_size += 2;
        hex_str = "";
      }
    } else {
      entity.push_back(buf[i]);
      --chunk_size;
      if (chunk_size == 0) {
        entity.pop_back();
        entity.pop_back();
      }
    }
    ++i;
  }
}
//---- getter --------------------------------------------
const bool Request::getEntityDone() { return this->entity_done; }
const std::string& Request::getRawHead() const { return this->raw_head; }
const bool& Request::getHeadDone() const { return this->head_done; }
const std::string& Request::getMethod() const { return this->method; }
const std::string& Request::getUrl() const { return this->url; };
const std::string& Request::getHttpVersion() const {
  return this->http_version;
}
const std::map<std::string, std::string>& Request::getHeader() const {
  return this->header;
}
const std::vector<char>& Request::getEntity() const { return this->entity; }
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
    header[it->substr(0, pos)] = ft::trim(it->substr(pos + 1));
  }
  // std::cout << GRY << "Debug: Request::setRawMsg\n" << DFT;
}
