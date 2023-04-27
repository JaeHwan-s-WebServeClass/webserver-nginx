#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/_types/_int16_t.h>
#include <sys/_types/_intptr_t.h>
#include <sys/_types/_size_t.h>
#include <sys/event.h>
#include <sys/fcntl.h>
#include <unistd.h>

#include <exception>
#include <iostream>
#include <map>
#include <vector>

#include "../include/define.hpp"
#include "Response.hpp"
#include "ServerConfig.hpp"
#include "ServerSocket.hpp"
#include "Transaction.hpp"

class Server {
 private:
  std::map<int, Transaction *> clients;
  int kq;
  struct kevent event_list[8];
  std::vector<struct kevent> change_list;
  const std::vector<ServerConfig> &server_config;
  std::vector<ServerSocket> server_socket;

 public:
  // ---- constructor -------------------------
  /// @param server_socket
  // Server(ServerSocket &);
  Server(std::vector<ServerConfig> &);

  /// @brief
  /// @param change_list
  /// @param ident
  /// @param filter
  /// @param flags
  /// @param fflags
  /// @param data
  /// @param udata
  void setChangeList(std::vector<struct kevent> &, uintptr_t, int16_t, uint16_t,
                     uint32_t, intptr_t, void *);

  void run(void);

  /// @brief
  /// @param client_fd
  /// @param clients
  /// @return
  void disconnectClient(int, std::map<int, Transaction *> &);

  // ---- safe-functions ----------------------
  /// @brief
  /// @param nevents
  /// @param timeout
  /// @return
  int safeKevent(int, const timespec *);
};

#endif