#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/_types/_size_t.h>
#include <sys/event.h>
#include <unistd.h>

#include <exception>
#include <iostream>
#include <map>
#include <vector>

#include "ServerSocket.hpp"

class Server {
 private:
  std::map<int, std::string>    clients;
  int                           kq;
  struct kevent                 event_list[8];
  std::vector<struct kevent>    change_list;

  ServerSocket                  *server_socket;

public :
  Server(ServerSocket &server_socket);
  
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
  void disconnectClient(int, std::map<int, std::string> &);

  /// @brief
  /// @param nevents
  /// @param timeout
  /// @return
  int safeKevent(int, const timespec *);

  /// @brief
  /// @param fd
  /// @param buf
  /// @return read size
  int safeRead(int, char *);

  /// @brief
  /// @param fd
  /// @return write size
  int safeWrite(int);

};

#endif