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

#define RED "\033[0;31m"
#define GRN "\033[0;32m"
#define YLW "\033[0;33m"
#define BLU "\033[0;36m"
#define GRY "\033[90m"
#define DFT "\033[0;37m"

#define BUFFER_SIZE 1024

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
  /// @param kq
  /// @param changelist
  /// @param nchanges
  /// @param eventlist
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