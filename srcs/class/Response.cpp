#include "Response.hpp"

//---- constructor ----------------------------------------------
Response::Response()
    : http_version("HTTP/1.1"),
      status_code(""),
      status_msg(""),
      entity(""),
      response_msg(""),
      done(false) {
  std::cout << GRY << "Response::contructor\n" << DFT;
}

//---- setter ---------------------------------------------------
void Response::setHttpVersion(std::string http_version) {
  this->http_version = http_version;
  // this->http_version = "HTTP/1.1";
  std::cout << GRY << "Response::setHttpVersion\n" << DFT;
}

void Response::setStatusCode(std::string status_code) {
  this->status_code = status_code;
  std::cout << GRY << "Response::setStatusCode\n" << DFT;
}

void Response::setStatusMsg(std::string status_msg) {
  this->status_msg = status_msg;
  std::cout << GRY << "Response::setStatusMsg\n" << DFT;
}

void Response::setHeader(std::string key, std::string value) {
  this->header[key] = value;
  std::cout << GRY << "Response::setHeader\n" << DFT;
}

void Response::setEntity(std::string content) {
  this->entity = content;
  std::cout << GRY << "Response::setEntity\n" << DFT;
}

void Response::setResponseMsg() {
  this->response_msg += this->http_version + " " + this->status_code + " " +
                        this->status_msg + "\r\n";
  for (std::map<std::string, std::string>::const_iterator it =
           this->header.begin();
       it != this->header.end(); ++it) {
    this->response_msg += it->first + ": " + it->second + "\r\n";
  }
  this->response_msg += "\r\n" + this->entity + "\r\n\r\n";
  std::cout << "response msg: " << this->response_msg << std::endl;
  this->done = true;
  std::cout << GRY << "Response::setResponseMsg\n" << DFT;
}

const bool Response::isDone() {
  std::cout << GRY << "Response::isDone\n" << DFT;
  return this->done;
}

//---- getter ---------------------------------------------------
std::string Response::getResponseMsg() {
  std::cout << GRY << "Response::getResponseMsg\n" << DFT;
  return this->response_msg;
}
