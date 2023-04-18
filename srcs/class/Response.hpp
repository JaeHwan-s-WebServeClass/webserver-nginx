
#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <unistd.h>

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "../include/include.hpp"

#define POST 0
#define GET 1
#define DELETE 2

#define RED "\033[0;31m"
#define GRN "\033[0;32m"
#define YLW "\033[0;33m"
#define BLU "\033[0;36m"
#define GRY "\033[90m"
#define DFT "\033[0;37m"

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

    // void getHttpVersion();
    // void getStatusCode();
    // void getStatusMsg();
    // void getHeader();
    // void getEntity();

  void setResponseMsg();
  std::string getResponseMsg();
  const bool  getEntityDone();
};

#endif
