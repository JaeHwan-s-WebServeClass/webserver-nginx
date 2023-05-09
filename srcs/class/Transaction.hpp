#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <dirent.h>
#include <fcntl.h>
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
  int fd;

  t_step flag;

  Response response;
  Request request;

  const ServerConfig &server_config;
  ServerConfig::t_location location;

  std::string resource;
  std::FILE *file_ptr;

  int cgi_pid;

  Transaction();

 public:
  // ---- occf --------------------------------
  /// @brief Transaction 생성자
  /// @param socket_fd
  /// @param server_config
  Transaction(int, const ServerConfig &);
  Transaction(const Transaction &ref);
  Transaction &operator=(const Transaction &ref);
  ~Transaction();

  // ---- getter ------------------------------
  Response &getResponse();
  Request &getRequest();
  const ServerConfig &getServerConfig() const;
  const t_step &getFlag() const;
  const FILE *getFilePtr() const;

  // ---- setter ------------------------------
  void setFlag(t_step);

  // ---- checker -----------------------------
  void checkResource(void);  // set this->location, this->resoure
  // 1. setResource
  int checkDirectory(
      void);  // URI가 directory일 때 처리 (index 처리 or 없으면 autoindex)
  // 1. getIndex 2. openIndex 3. handleUriDirectory
  void checkAllowedMethod(
      void);  // 유효한 method인지 체크 (requset의 method와 conf의 method 비교)

  int checkFile(void);  // file descriptor를 반환해야 하는 경우 처리
  // this->file_ptr setting, setFlag

  // ---- executor ----------------------------
  int executeRead(void);
  int executeReadHead(char *, int);
  void executeReadEntity(char *, int, int);
  int executeWrite(void);
  int executeMethod(int, int);
  int executeResource(void);

  // ---- http methods ------------------------
  void httpGet(int, int);
  void httpDelete(void);
  void httpPost(int, int);
  void httpPut(int, int);

  // ---- cgi ---------------------------------
  int executeCGI(void);

  // --- error class --------------------------
  class ErrorPage403Exception : public std::exception {
    virtual const char *what(void) const throw();
  };
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
