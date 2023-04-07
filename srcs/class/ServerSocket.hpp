#pragma once

#include <arpa/inet.h>  // sockaddr_in
#include <fcntl.h>
#include <sys/socket.h>  // socket()

#include <exception>
#include <iostream>  //

class ServerSocket {
 private:
  int server_socket;
  struct sockaddr_in server_addr;

 public:
  /// @brief ServerSocket constructor
  /// @param protocol AF_INET
  /// @param port 8080
  ServerSocket(sa_family_t address_family, int port);

  /// @brief  fd 를 NonBlock 방식으로 설정하는 함수
  /// @param  socket_fd
  /// @return
  void setNonBlock(int);

 private:
  /// @brief 소켓 생성하는 함수
  /// @param  PF_INET
  /// @param  socket_stream
  /// @param  0
  /// @return socket fd
  void safeSocket(int, int, int);

  /// @brief
  /// @param
  /// @param
  /// @param
  /// @return
  void safeBind(void);

  /// @brief
  /// @param backlog
  /// @return
  void safeListen(int backlog);

  /// @brief
  /// @param
  /// @param
  /// @param
  /// @return
  int safeAccept(void);
};