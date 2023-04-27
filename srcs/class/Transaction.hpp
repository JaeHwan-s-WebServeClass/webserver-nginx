#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cstdio>
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

  FILE *file_ptr;

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
  const t_step &getFlag() const;
  FILE *getFilePtr() const;

  // ---- setter ------------------------------
  void setFlag(t_step);

  // ---- checker -----------------------------
  int checkResource(void);
  void checkAllowedMethod(void);

  // ---- executor ----------------------------
  int executeRead(void);
  int executeReadHead(char *, int);
  void executeReadEntity(char *, int, int);
  int executeWrite(void);
  int executeMethod(void);

  // ---- http methods ------------------------
  int httpGet(void);
  int httpDelete(void);
  int httpPost(void);

  // ---- safe-functions ----------------------
  /// @brief
  /// @param fd
  /// @param buf
  /// @return read size
  int safeRecv(int, char *, int);

  /// @brief
  /// @param fd
  /// @param response
  /// @return write size
  int safeSend(int, Response &);

  /// @brief
  /// @param fd
  /// @param buf
  /// @param size
  /// @return read/write size
  int safeRead(int, char *, int);
  int safeWrite(int, char *, int);
};

#endif
