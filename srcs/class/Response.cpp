#include "Response.hpp"

//---- constructor ----------------------------------------------
Response::Response()
    : http_version("HTTP/1.1"),
      status_code(""),
      status_msg(""),
      entity(""),
      response_msg(""),
      entity_done(false) {
  std::cout << GRY << "Debug: Response::contructor\n" << DFT;
}

//---- setter ---------------------------------------------------
void Response::setHttpVersion(std::string http_version) { this->http_version = http_version; }
void Response::setStatusCode(std::string status_code) { this->status_code = status_code; }
void Response::setStatusMsg(std::string status_msg) { this->status_msg = status_msg; }
void Response::setHeader(std::string key, std::string value) { this->header[key] = value; }
void Response::setEntity(std::string content) { this->entity = content; }

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
  std::cout << GRY << "Debug: Response::setResponseMsg\n" << DFT;
}

//---- getter ---------------------------------------------------
// const bool Response::getEntityDone() const { return this->entity_done; }
const std::string Response::getResponseMsg() const { return this->response_msg; }
const std::string Response::getHttpVersion() const { return this->http_version; }
const std::string Response::getStatusCode() const { return this->status_code; }
const std::string Response::getStatusMsg() const { return this->status_msg; }
const std::map<std::string, std::string> Response::getHeader() const { return this->header; }
const std::string Response::getEntity() const { return this->entity; }