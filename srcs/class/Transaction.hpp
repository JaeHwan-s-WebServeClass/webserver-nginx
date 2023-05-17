#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <dirent.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
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
  int file_fd;

  t_step flag;

  Response response;
  Request request;

  const ServerConfig &server_config;
  ServerConfig::t_location location;

  std::string resource;

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
  const int &getFileDescriptor() const;
  const int &getPid() const;

  // ---- setter ------------------------------
  void setFlag(t_step);
  void setResource(void);

  // ---- checker -----------------------------
  void checkAllowedMethod(void);
  void checkServerName();

  // ---- executor ----------------------------
  void executeRead(void);
  int executeReadHead(char *, int);
  void executeReadEntity(char *, int, int);

  int executeResource(void);
  int executeResourceDirectory(void);
  int executeResourceFile(void);

  int executeWrite(void);

  int executeMethod(int, int);

  // ---- http methods ------------------------
  void httpGet(int, int);
  void httpDelete(void);
  void httpPost(int, int);
  void httpPut(int, int);

  // ---- cgi ---------------------------------
  int executeCGI(void);

  // ---- redirection ---------------------------------
  void executeRedirect();

  // --- error class --------------------------
  class ErrorPage403Exception : public std::exception {
    virtual const char *what(void) const throw();
  };
  class ErrorPage404Exception : public std::exception {
    virtual const char *what(void) const throw();
  };
  class ErrorPage405Exception : public std::exception {
    virtual const char *what(void) const throw();
  };
  class ErrorPage408Exception : public std::exception {
    virtual const char *what(void) const throw();
  };
  class ErrorPage409Exception : public std::exception {
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
