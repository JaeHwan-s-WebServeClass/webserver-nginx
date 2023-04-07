#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/event.h>

#include <exception>
#include <iostream>
#include <map>
#include <vector>

#include "ServerSocket.hpp"

class Server {
 private:
  std::map<int, std::string> clients;
  int kq;
  struct kevent event_list[8];
  std::vector<struct kevent> change_list;

  ServerSocket  *server_socket;

public :
  Server();
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
  /// @param kq
  /// @param changelist
  /// @param nchanges
  /// @param eventlist
  /// @param nevents
  /// @param timeout
  /// @return
  int newEvents(int kq, const struct kevent *changelist, int nchanges,
                struct kevent *eventlist, int nevents, const timespec *timeout);

  void  run();

};

#endif