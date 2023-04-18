#include "Response.hpp"

//---- constructor ----------------------------------------------
Response::Response()
    : http_version("HTTP/1.1"),
      status_code(""),
      status_msg(""),
      entity(""),
      response_msg(""),
      entity_done(false) {
  // std::cout << GRY << "Debug: Response::contructor\n" << DFT;
}

//---- setter ---------------------------------------------------
void Response::setHttpVersion(std::string http_version) {
  this->http_version = http_version;
  // std::cout << GRY << "Debug: Response::setHttpVersion\n" << DFT;
}

void Response::setStatusCode(std::string status_code) {
  this->status_code = status_code;
  // std::cout << GRY << "Debug: Response::setStatusCode\n" << DFT;
}

void Response::setStatusMsg(std::string status_msg) {
  this->status_msg = status_msg;
  // std::cout << GRY << "Debug: Response::setStatusMsg\n" << DFT;
}

void Response::setHeader(std::string key, std::string value) {
  this->header[key] = value;
  // std::cout << GRY << "Debug: Response::setHeader\n" << DFT;
}

void Response::setEntity(std::string content) {
  this->entity = content;
  // std::cout << GRY << "Debug: Response::setEntity\n" << DFT;
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
  this->entity_done = true;
  // std::cout << GRY << "Debug: Response::setResponseMsg\n" << DFT;
}

const bool Response::getEntityDone() {
  // std::cout << GRY << "Debug: Response::getEntityDone\n" << DFT;
  return this->entity_done;
}

//---- getter ---------------------------------------------------
std::string Response::getResponseMsg() {
  // std::cout << GRY << "Debug: Response::getResponseMsg\n" << DFT;
  return this->response_msg;
}
