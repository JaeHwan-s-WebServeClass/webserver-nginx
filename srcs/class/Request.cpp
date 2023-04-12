#include "Request.hpp"

Request::Request() : raw_head(""), is_end_head(0) {}

//---- getter/setter --------------------------------------------
void Request::setRawMsg(const char* read_msg) {
  std::istringstream buf;
  buf.str(read_msg);

  std::string line;
  while (std::getline(buf, line, '\n')) {
    if (line.length() == 0 || line == "\r") {
      this->is_end_head = true;
      this->parserHead();
    } else if (!this->is_end_head) {
      this->raw_head += line;
      if (!buf.eof()) {
        this->raw_head += '\n';
      }
    } else {
      this->entity.push_back(line);
    }
  }
}

const std::string& Request::getRawHead() const { return this->raw_head; }
const bool& Request::getIsEndHead() const { return this->is_end_head; }
const std::string& Request::getMethod() const { return this->method; }
const std::string& Request::getUrl() const { return this->url; };
const std::string& Request::getHttpVersion() const {
  return this->http_version;
}
const std::map<std::string, std::string>& Request::getHeader() const {
  return this->header;
}
const std::vector<std::string>& Request::getEntity() const {
  return this->entity;
}

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
  std::cout << GRY << "----------------------------------------------------"
            << DFT << std::endl;
  //  for (int i = 0; i < entity.size(); i++) {
  // 	std::cout << YLW << entity[i] << std::endl;
  //  }
  // std::cout << DFT << std::endl;
}

void Request::parserHead() {
  std::vector<std::string> tmp_head;
  std::vector<std::string> tmp_start_line;

  // 개행기준으로 split하고
  tmp_head = ft::split(this->raw_head, '\n');

  // 시작줄은 space 기준으로 split
  tmp_start_line = ft::split(tmp_head[0], ' ');
  this->method = tmp_start_line[0];
  this->url = tmp_start_line[1];
  this->http_version = tmp_start_line[2];

  // header를 처리하는 부분
  // colon(:)을 찾아서 그 전까지는 key에 넣고 뒤의 부분은 value에 넣도록 합시다
  for (std::vector<std::string>::iterator it = tmp_head.begin() + 1;
       it != tmp_head.end(); ++it) {
    int pos = it->find(':');
    header[it->substr(0, pos)] = it->substr(pos + 1);
  }
}