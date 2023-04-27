#include "Response.hpp"

//---- constructor ------------------------------------------------------------
Response::Response(t_step &flag)
    : flag(flag), http_version("HTTP/1.1"), status_code(""), status_msg("") {
  this->entity.reserve(512);
  // std::cout << GRY << "Debug: Response::contructor\n" << DFT;
}

Response::~Response() { delete[] this->response_msg; }

//---- getter -----------------------------------------------------------------
const std::string Response::getResponseMsg() const {
  return this->response_msg;
}
const std::string Response::getHttpVersion() const {
  return this->http_version;
}
const std::string Response::getStatusCode() const { return this->status_code; }
const std::string Response::getStatusMsg() const { return this->status_msg; }
const std::map<std::string, std::string> Response::getHeader() const {
  return this->header;
}
const std::vector<char> Response::getEntity() const { return this->entity; }

const std::string Response::getEntitySize() const {
  std::ostringstream ss;

  ss << this->entity.size();
  return ss.str();
}

//---- setter -----------------------------------------------------------------
void Response::setHttpVersion(std::string http_version) {
  this->http_version = http_version;
}

void Response::setStatus(std::string status_code) {
  if (status_code == "200") {
    this->status_msg = "(◟˙꒳​˙)◟ Good ◝(˙꒳​˙◝)";
  } else if (status_code == "404") {
    this->status_msg = "Not Found :(";
  } else if (status_code == "500") {
    this->status_msg = "Internal Server Error :l";
  } else if (status_code == "501") {
    this->status_msg = "Not Implemented";
  }
}

void Response::setHeader(std::string key, std::string value) {
  this->header[key] = value;
}
void Response::setEntity(char *buf, size_t read_len) {
  for (size_t i = 0; i < read_len; ++i) {
    this->entity.push_back(buf[i]);
  }
}

void Response::setResponseMsg() {
  // std::cout << GRY << "Debug: Response::setResponseMsg\n" << DFT;
  std::string response_head;

  response_head = this->http_version + " " + this->status_code + " " +
                  this->status_msg + "\r\n";
  for (std::map<std::string, std::string>::const_iterator it =
           this->header.begin();
       it != this->header.end(); ++it) {
    response_head += it->first + ": " + it->second + "\r\n";
  }
  this->response_msg = new char[response_head.size() + this->entity.size() + 6];

  char *pos = this->response_msg;

  // FIXME c_str 리턴값이 const 인데 copy 로 할당하려고 하는 코드 수정 필요
  std::memcpy(pos, response_head.c_str(), response_head.size());
  pos += response_head.size();
  std::memcpy(pos, "\r\n", 2);
  pos += 2;
  std::memcpy(pos, &(this->entity[0]), this->entity.size());
  pos += this->entity.size();
  std::memcpy(pos, "\r\n\r\n", 4);
  // this->entity_done = true;
  // if (this->flag == FILE_DONE)
  this->flag = RESPONSE_DONE;

  // DEBUG
  std::cout << "response msg: " << response_msg << std::endl;
}
