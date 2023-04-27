#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <unistd.h>

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "../include/define.hpp"
#include "../include/include.hpp"

class Response {
private:
  t_step &flag;
  char *response_msg;
  std::string http_version;
  std::string status_code;
  std::string status_msg;
  std::map<std::string, std::string> header;
  std::vector<char> entity;
  // bool entity_done;
  int event_fl;

public:
  // ---- constructor -------------------------
  Response(t_step &);
  ~Response();

  // ---- getter ------------------------------
  const std::string getHttpVersion() const;
  const std::string getStatusCode() const;
  const std::string getStatusMsg() const;
  const std::map<std::string, std::string> getHeader() const;
  const std::vector<char> getEntity() const;
  const std::string getResponseMsg() const;
  const std::string getEntitySize() const;

  // ---- setter ------------------------------
  void setHttpVersion(std::string);
  void setStatus(std::string);
  void setHeader(std::string, std::string);
  void setEntity(char *, size_t);
  void setResponseMsg();
};

#endif

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