#pragma once

#include <arpa/inet.h>  // sockaddr_in
#include <fcntl.h>
#include <sys/socket.h>  // socket()

#include <exception>
#include <iostream>  //
#include <cstring>
#include "../include/define.hpp"

class ServerSocket {
 private:
  int server_socket;
  struct sockaddr_in server_addr;
  
  int port;

 private:
  /// @brief 소켓 생성하는 함수
  /// @param  domain PF_INET
  /// @param  type socket_stream
  /// @param  protocol 0
  /// @return
  void safeSocket(int, int, int);

  void safeBind(void);

  /// @brief
  /// @param backlog
  /// @return
  void safeListen(int);

 public:
  /// @brief ServerSocket constructor
  /// @param  address_family protocol AF_INET
  /// @param port 8080
  ServerSocket(sa_family_t, int);

  /// @brief  fd 를 NonBlock 방식으로 설정하는 함수
  /// @param  socket_fd
  /// @return
  void setNonBlock(int);

  int safeAccept(void) const;

  int getServerSocket(void) const;

  public:
    int getPort() const ;
};