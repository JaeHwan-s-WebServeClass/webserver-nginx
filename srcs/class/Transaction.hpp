#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <fstream>
#include <iostream>

#include "../include/include.hpp"
#include "Request.hpp"
#include "Response.hpp"

#define POST 0
#define GET 1
#define DELETE 2

#define RED "\033[0;31m"
#define GRN "\033[0;32m"
#define YLW "\033[0;33m"
#define BLU "\033[0;36m"
#define GRY "\033[90m"
#define DFT "\033[0;37m"

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

 public:
  /// @brief Transaction 생성자
  /// @param socket_fd
  /// @param root_dir
  Transaction(int, std::string);

  Response &getResponse();
  Request &getRequest();

  void httpGet(Request *request);
  void httpDelete(Request *request);
  void httpPost(Request *request);
};

#endif
