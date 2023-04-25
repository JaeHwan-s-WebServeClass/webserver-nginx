
#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <unistd.h>

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "../include/include.hpp"
#include "../include/define.hpp"

/* Response 리턴할 형식
    <버전> <상태 코드> <상태 메시지>
    <헤더>

    <엔터티 본문>

    ex)
    HTTP/1.1 200 OK
    Date: Mon, 23 May 2005 22:38:34 GMT
    (CRLF * 2)
    본문
*/

class Response {
 private:
  //  const std::string response_msg = "";
  std::string response_msg;

  std::string http_version;
  std::string status_code;
  std::string status_msg;
  std::map<std::string, std::string> header;
  std::string entity;
  bool entity_done;

  int event_fl;

 public:
  Response();

  void setHttpVersion(std::string);
  void setStatusCode(std::string);
  void setStatusMsg(std::string);
  void setHeader(std::string, std::string);
  void setEntity(std::string);
  void setResponseMsg();

  const std::string getHttpVersion() const;
  const std::string getStatusCode() const;
  const std::string getStatusMsg() const;
  const std::map<std::string, std::string> getHeader() const;
  const std::string getEntity() const;
  const std::string getResponseMsg() const;
  //const bool getEntityDone() const;

};

#endif
