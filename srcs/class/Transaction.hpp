#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <unistd.h>

#include <fstream>
#include <iostream>
#include <algorithm>

#include "../include/include.hpp"
#include "Request.hpp"
#include "Response.hpp"

#define POST 0
#define GET 1
#define DELETE 2

#define READ 0
#define WRITE 1

#define RED "\033[0;31m"
#define GRN "\033[0;32m"
#define YLW "\033[0;33m"
#define BLU "\033[0;36m"
#define GRY "\033[90m"
#define DFT "\033[0;37m"

#define BUFFER_SIZE 1024
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

  int httpCheckStartLine(void);

  // int executeTransaction(int);
  int executeRead(void);
  int executeWrite(void);
  int executeMethod(void);
  // int requestTransaction(void);
  // int methodTransaction(void);

  // /// @brief
  // /// @param status
  // /// @return write_status
  // int responseTransaction(int);

  int httpGet(void);
  int httpDelete(void);
  int httpPost(void);

  /// @brief
  /// @param fd
  /// @param buf
  /// @return read size
  int safeRead(int, char *);

  /// @brief
  /// @param fd
  /// @param response
  /// @return write size
  int safeWrite(int, Response &);
};

#endif

// method 각각으로 넘어가기 전에 공통적으로 검사해볼만한 것
// 1. method (GET, DELETE, POST 중에 존재하는지 즉, allow method)
// 2. resource가 존재하는지

// isopen으로 열렸는지 안열렸는지 확인할 수 있다.
// 1. 파일이 존재하지 않는 경우 => access(file, F_OK) == -1 이면 존재 안함
// (존재하면 0 리턴)
// 2. 파일은 존재하지만 (손상되어) 열 수 없는/읽을 수 없는 경우 => access(file,
// R_OK || X_OK || W_OK)
// ---------------------------------------
// 404 Not Found
// 500 Internal Server Error

// F_OK: 파일이 존재하는지 여부를 확인합니다.
// R_OK: 파일에 대한 읽기 권한이 있는지 여부를 확인합니다.
// W_OK: 파일에 대한 쓰기 권한이 있는지 여부를 확인합니다.
// X_OK: 파일에 대한 실행 권한이 있는지 여부를 확인합니다.
//
// if (access("file.txt", F_OK) != -1) {
//         std::cout << "file.txt exists!\n";
//     } else {
//         std::cout << "file.txt does not exist.\n";
//     }

//     if (access("file.txt", R_OK) != -1) {
//         std::cout << "file.txt is readable!\n";
//     } else {
//         std::cout << "file.txt is not readable.\n";
//     }

//     if (access("file.txt", W_OK) != -1) {
//         std::cout << "file.txt is writable!\n";
//     } else {
//         std::cout << "file.txt is not writable.\n";
//     }

//     if (access("file.txt", X_OK) != -1) {
//         std::cout << "file.txt is executable!\n";
//     } else {
//         std::cout << "file.txt is not executable.\n";
//     }
