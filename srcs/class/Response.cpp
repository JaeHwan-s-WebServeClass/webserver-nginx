#include "Response.hpp"

//---- constructor ----------------------------------------------
Response::Response()
    : http_version("HTTP/1.1"),
      status_code(""),
      status_msg(""),
      entity_done(false) {
  // std::cout << GRY << "Debug: Response::contructor\n" << DFT;
}

//---- setter ---------------------------------------------------
void Response::setHttpVersion(std::string http_version) {
  this->http_version = http_version;
}
void Response::setStatusCode(std::string status_code) {
  this->status_code = status_code;
}
void Response::setStatusMsg(std::string status_msg) {
  this->status_msg = status_msg;
}
void Response::setHeader(std::string key, std::string value) {
  this->header[key] = value;
}
void Response::setEntity(char* buf, size_t read_len) {
  if (this->entity.size() == 0) {
    this->entity.reserve(512);
  } else {
    for (size_t i = 0; i < read_len; ++i) this->entity.push_back(buf[i]);
  }
}
// struct MyClass {
//   int data;
//   MyClass() {std::cout << '*';}  // print an asterisk for each construction
// };
//
// int main () {
//   std::cout << "constructions (1): ";
//   // allocates and constructs five objects:
//   MyClass * p1 = new MyClass[5];
//   std::cout << '\n';

//   std::cout << "constructions (2): ";
//   // allocates and constructs five objects (nothrow):
//   MyClass * p2 = new (std::nothrow) MyClass[5];
//   std::cout << '\n';

//   delete[] p2;
//   delete[] p1;

//   return 0;
// }

void Response::setResponseMsg() {
  std::string response_head;

  response_head = this->http_version + " " + this->status_code + " " +
                  this->status_msg + "\r\n";
  for (std::map<std::string, std::string>::const_iterator it =
           this->header.begin();
       it != this->header.end(); ++it) {
    response_head += it->first + ": " + it->second + "\r\n";
  }
  this->response_msg = new char[response_head.size() + this->entity.size() + 6];

  char* pos = this->response_msg;

  std::copy(pos, (pos + response_head.size()), response_head.c_str());
  pos += response_head.size();
  std::copy(pos, (pos + 2), "\r\n");
  pos += 2;
  std::copy(pos, (pos + this->entity.size()), &(*this->entity.begin()));
  pos += this->entity.size();
  std::copy(pos, (pos + 4), "\r\n\r\n");

  this->entity_done = true;

  // DEBUG
  std::cout << "response msg: " << response_msg << std::endl;
  // std::cout << GRY << "Debug: Response::setResponseMsg\n" << DFT;
}

//---- getter ---------------------------------------------------
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