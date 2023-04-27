#pragma once

#include <arpa/inet.h> // sockaddr_in
#include <fcntl.h>
#include <sys/socket.h> // socket()

#include <cstring>
#include <exception>
#include <iostream>

#include "../include/define.hpp"

class ServerSocket {
private:
  int server_socket;
  struct sockaddr_in server_addr;
  int port;

public:
  // ---- constructor -------------------------
  /// @brief ServerSocket constructor
  /// @param  address_family protocol AF_INET
  /// @param port 8080
  ServerSocket(sa_family_t, int);

  // ---- getter ------------------------------
  int getServerSocket(void) const;
  int getPort() const;

  // ---- setter ------------------------------
  /// @brief  fd 를 NonBlock 방식으로 설정하는 함수
  /// @param  socket_fd
  /// @return
  void setNonBlock(int);

  // ---- safe-functions ----------------------
  int safeAccept(void) const;

private:
  // ---- safe-functions ----------------------
  /// @brief 소켓 생성하는 함수
  /// @param  domain PF_INET
  /// @param  type socket_stream
  /// @param  protocol 0
  /// @return
  void safeSocket(int, int, int);

  /// @brief
  /// @param backlog
  /// @return
  void safeListen(int);

  void safeBind(void);
};