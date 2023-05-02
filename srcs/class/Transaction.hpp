#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cstdio>
#include <exception>
#include <fstream>
#include <iostream>

#include "../include/define.hpp"
#include "../include/include.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "ServerConfig.hpp"

class Transaction {
 private:
  int socket_fd;  // client 랑 연결된 socket fd
  t_step flag;

  Response response;
  Request request;

  const ServerConfig &server_config;
  ServerConfig::t_location location;

  std::string resource;
  std::FILE *file_ptr;

  Transaction();

 public:
  // ---- constructor -------------------------
  /// @brief Transaction 생성자
  /// @param socket_fd
  /// @param server_config
  Transaction(int, const ServerConfig &);

  // ---- getter ------------------------------
  Response &getResponse();
  Request &getRequest();
  const ServerConfig &getServerConfig() const;
  const t_step &getFlag() const;
  const FILE *getFilePtr() const;

  // ---- setter ------------------------------
  void setFlag(t_step);

  // ---- checker -----------------------------
  int checkResource(void);
  void checkAllowedMethod(void);

  int CheckResourceFile(void);
  int CheckResourceDir(void);

  // ---- executor ----------------------------
  int executeRead(void);
  int executeReadHead(char *, int);
  void executeReadEntity(char *, int, int);
  int executeWrite(void);
  int executeMethod(int, int);

  // ---- http methods ------------------------
  void httpGet(int);
  void httpDelete(int);
  void httpPost(int);

  // ---- cgi ---------------------------------
  int executeCGI(void);

  // --- error class --------------------------
  class ErrorPage404Exception : public std::exception {
    virtual const char *what(void) const throw();
  };
  class ErrorPage500Exception : public std::exception {
    virtual const char *what(void) const throw();
  };
  class ErrorPage501Exception : public std::exception {
    virtual const char *what(void) const throw();
  };
  class ErrorPageDefaultException : public std::exception {
    virtual const char *what(void) const throw();
  };
};

#endif
