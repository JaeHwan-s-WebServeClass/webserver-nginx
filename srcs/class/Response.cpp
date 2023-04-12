#include "Response.hpp"

//---- constructor ----------------------------------------------
Response::Response() : response_msg("") {
  setHttpVersion("null");
  setStatusCode("null");
  setStatusMsg("null");
  setHeader("null", "null");
  setEntity("null");
  setResponseMsg();
}

//---- setter ---------------------------------------------------
void Response::setHttpVersion(std::string) { this->http_version = "HTTP/1.1"; }

void Response::setStatusCode(std::string) { this->status_code = "200"; }

void Response::setStatusMsg(std::string) { this->status_msg = "OK"; }

void Response::setHeader(std::string, std::string) {
  this->header["Date"] = "Mon, 23 May 2005 22:38:34 GMT";
}

void Response::setEntity(std::string) {
  this->entity =
      "<!DOCTYPE html>\n<html>\n<head>\n<meta "
      "charset=\"UTF-8\">\n<title>Hello, "
      "World!</title>\n</head>\n<body>\n<h1>Hello, "
      "World!</h1>\n</body>\n</html>";
}

void Response::setResponseMsg() {
  this->response_msg += this->http_version + " " + this->status_code + " " +
                        this->status_msg + "\r\n" +
                        "Date: " + this->header["Date"] + "\r\n\r\n" +
                        this->entity + "\r\n";
}

int Response::safeWrite(int fd) {
  int write_len;

  if ((write_len = write(fd, this->getResponseMsg().c_str(),
                         this->getResponseMsg().size())) == -1) {
    throw std::string("response write error!");
  }
  return write_len;
}

//---- getter ---------------------------------------------------
std::string Response::getResponseMsg() { return this->response_msg; }
