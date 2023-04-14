#include "Response.hpp"

//---- constructor ----------------------------------------------
Response::Response() : http_version("HTTP/1.1") ,status_code(""), status_msg(""), entity(""), response_msg(""), done(false) {

}

//---- setter ---------------------------------------------------
void Response::setHttpVersion(std::string http_version) { 
  this->http_version = http_version;
  // this->http_version = "HTTP/1.1"; 
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

void Response::setEntity(std::string content) {
  this->entity = content;
}

void Response::setResponseMsg() {
  this->response_msg += this->http_version + " " + this->status_code + " " +
                        this->status_msg + "\r\n";
  for (std::map<std::string, std::string>::const_iterator it = this->header.begin(); \
        it != this->header.end(); ++it) {
        this->response_msg += it->first + ": " + it->second + "\r\n";
  }
  this->response_msg += "\r\n" + this->entity + "\r\n\r\n";
  std::cout << "response msg: " << this->response_msg << std::endl;
  this->done = true;
}

const bool  Response::isDone() {
  return this->done;
}

//---- getter ---------------------------------------------------
std::string Response::getResponseMsg() { return this->response_msg; }
