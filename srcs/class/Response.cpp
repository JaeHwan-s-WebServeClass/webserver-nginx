#include "Response.hpp"

//---- OCCF -------------------------------------------------------------------
Response::Response(t_step &flag)
    : flag(flag),
      response_msg(0),
      http_version("HTTP/1.1"),
      status_code(""),
      status_msg("") {
  this->entity.reserve(512);
}
Response::Response(const Response &ref) : flag(ref.flag) { *this = ref; }
Response &Response::operator=(const Response &ref) {
  this->response_msg = ref.response_msg;
  this->http_version = ref.http_version;
  this->status_code = ref.status_code;
  this->status_msg = ref.status_msg;
  this->header = ref.header;
  this->entity = ref.entity;
  this->response_msg = ref.response_msg;
  return *this;
}
Response::~Response() { delete[] this->response_msg; }

//---- getter -----------------------------------------------------------------
const char *Response::getResponseMsg() const { return this->response_msg; }
const std::string &Response::getHttpVersion() const {
  return this->http_version;
}
const std::string &Response::getStatusCode() const { return this->status_code; }
const std::string &Response::getStatusMsg() const { return this->status_msg; }
const std::map<std::string, std::string> &Response::getHeader() const {
  return this->header;
}
const std::vector<char> &Response::getEntity() const { return this->entity; }

std::string Response::getEntitySize() const {
  std::ostringstream ss;

  ss << this->entity.size();
  return ss.str();
}
size_t Response::getResponseMsgSize() const { return this->response_msg_size; }

//---- setter -----------------------------------------------------------------
void Response::setHttpVersion(std::string http_version) {
  this->http_version = http_version;
}

void Response::setStatus(std::string status_code) {
  if (status_code == "200") {
    this->status_code = "200";
    this->status_msg = "(◟˙꒳​˙)◟ Good ◝(˙꒳​˙◝)";
  } else if (status_code == "201") {
    this->status_code = "201";
    this->status_msg = "Created";
  } else if (status_code == "301") {
    this->status_code = "301";
    this->status_msg = "Moved Permanently";
  } else if (status_code == "403") {
    this->status_code = "403";
    this->status_msg = "Forbidden";
  } else if (status_code == "404") {
    this->status_code = "404";
    this->status_msg = "Not Found :(";
  } else if (status_code == "405") {
    this->status_code = "405";
    this->status_msg = "Method Not Allowed";
  } else if (status_code == "408") {
    this->status_code = "408";
    this->status_msg = "Request Timeout";
  } else if (status_code == "409") {
    this->status_code = "409";
    this->status_msg = "Conflict";
  } else if (status_code == "500") {
    this->status_code = "500";
    this->status_msg = "Internal Server Error :l";
  } else if (status_code == "501") {
    this->status_code = "501";
    this->status_msg = "Not Implemented :O";
  } else {
    this->status_code = "418";
    this->status_msg = "I'm a teapot";
  }
}

void Response::setHeader(std::string key, std::string value) {
  this->header[key] = value;
}
void Response::setEntity(const char *buf, size_t read_len) {
  for (size_t i = 0; i < read_len; ++i) {
    this->entity.push_back(buf[i]);
  }
}

void Response::setResponseMsg() {
  // std::cout << GRY << "Debug: Response::setResponseMsg\n" << DFT;
  std::string response_head;

  response_head = this->http_version + " " + this->status_code + " " +
                  this->status_msg + "\r\n";

  this->setHeader("Content-Length", ft::intToStr(this->entity.size()));
  for (std::map<std::string, std::string>::const_iterator it =
           this->header.begin();
       it != this->header.end(); ++it) {
    response_head += it->first + ": " + it->second + "\r\n";
  }
  this->response_msg_size = response_head.size() + this->entity.size() + 6;
  this->response_msg = new char[this->response_msg_size];

  char *pos = this->response_msg;

  std::memcpy(pos, response_head.c_str(), response_head.size());
  pos += response_head.size();
  std::memcpy(pos, "\r\n", 2);
  pos += 2;
  std::memcpy(pos, &(this->entity[0]), this->entity.size());
  pos += this->entity.size();
  std::memcpy(pos, "\r\n\r\n", 4);
  this->flag = RESPONSE_DONE;

  // DEBUG
  // std::cout << "response msg: " << response_msg << std::endl;
}

void Response::setErrorMsg(std::string status_code,
                           const std::string &error_msg) {
  this->setStatus(status_code);
  this->setEntity(error_msg.c_str(), error_msg.size());
  this->setHeader("Content-Type", "text/html");
  this->setHeader("Connection", "close");
  this->setResponseMsg();
}