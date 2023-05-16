#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/_types/_int16_t.h>
#include <sys/_types/_intptr_t.h>
#include <sys/_types/_size_t.h>
#include <sys/event.h>
#include <sys/fcntl.h>
#include <unistd.h>

#include <csignal>
#include <exception>
#include <iostream>
#include <map>
#include <vector>

#include "../include/define.hpp"
#include "../include/include.hpp"
#include "Response.hpp"
#include "ServerConfig.hpp"
#include "ServerSocket.hpp"
#include "Transaction.hpp"

class Server {
private:
  std::map<int, Transaction *> clients;
  int kq;
  struct kevent event_list[MAX_EVENT_SIZE];
  std::vector<struct kevent> change_list;
  const std::vector<ServerConfig> &server_config;
  std::vector<ServerSocket> server_socket;

  // error_status, 시작줄 + 헤더 + 엔티
  std::map<std::string, std::string> error_page;

  bool disconnect;

  Server();

public:
  // ---- occf --------------------------------
  /// @param server_socket
  Server(std::vector<ServerConfig> &);
  Server(const Server &ref);
  Server &operator=(const Server &ref);
  ~Server();

  // ---- error page --------------------------
  void loadErrorPage();
  void setErrorPage(std::string, Transaction *&);

  // ---- main loop ---------------------------
  void run(void);
  void runErrorServer(struct kevent *&);
  void runTimerEventClient(struct kevent *&);
  void runReadEventServer(std::vector<ServerSocket>::const_iterator);
  void runReadEventClient(struct kevent *&);
  void runReadEventFile(struct kevent *&);
  void runWriteEventFile(struct kevent *&curr_event);
  void runWriteEventClient(struct kevent *&);

  // ---- utils -------------------------------
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

  /// @brief
  /// @param client_fd
  /// @return
  void disconnectClient(int);

  void clearFileDescriptor(int client_fd);

  // ---- safe_method -------------------------
  int safeKevent(int nevents, const timespec *timeout);

  // ---- exception ---------------------------
  class ServerUnhealthyException : public std::exception {
    virtual const char *what(void) const throw();
  };
};

#endif