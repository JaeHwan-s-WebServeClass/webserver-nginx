#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <unistd.h>

#include <fstream>
#include <iostream>
#include <algorithm>

#include "../include/include.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Transaction {
 private:
  // root_dir 위치는 실행파일 기준으로
  int socket_fd;  // client 랑 연결된 socket fd
  std::string root_dir;
  //  std::string another_path = "";
  // 요청 시작줄의 경로에 하위경로를 붙여서 파일이 있는지 확인하고...
  Transaction();

  Response response;
  Request request;
  
  int read_head_len;
  char head_buf[MAX_HEAD_SIZE];
  char entity_buf[MAX_BODY_SIZE];

 public:
  /// @brief Transaction 생성자
  /// @param socket_fd
  /// @param root_dir
  Transaction(int, std::string);

  Response &getResponse();
  Request &getRequest();

  int httpCheckStartLine(void);

  int executeRead(void);
      void  executeReadHead(void);
      int   executeReadContentLengthEntity(void);
      int   executeReadChunkedEntity(void);
  int executeWrite(void);
  int executeMethod(void);

  int httpGet(void);
  int httpDelete(void);
  int httpPost(void);

  /// @brief
  /// @param fd
  /// @param buf
  /// @return read size
  int safeRead(int, char *, int);

  /// @brief
  /// @param fd
  /// @param response
  /// @return write size
  int safeWrite(int, Response &);
};

#endif
