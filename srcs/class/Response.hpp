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
  size_t response_msg_size;

  Response();

 public:
  // ---- occf --------------------------------
  Response(t_step &);
  Response(const Response &ref);
  Response &operator=(const Response &ref);
  ~Response();

  // ---- getter ------------------------------
  const std::string &getHttpVersion() const;
  const std::string &getStatusCode() const;
  const std::string &getStatusMsg() const;
  const std::map<std::string, std::string> &getHeader() const;
  const std::vector<char> &getEntity() const;
  const char *getResponseMsg() const;
  std::string getEntitySize() const;
  size_t getResponseMsgSize() const;

  // ---- setter ------------------------------
  void setHttpVersion(std::string);
  void setStatus(std::string);
  void setHeader(std::string, std::string);
  void setEntity(const char *, size_t);
  void setResponseMsg();
  void setErrorMsg(std::string, const std::string &);
};

#endif
