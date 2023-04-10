// c header
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// kqueue header in C Library
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>

// c++ header
#include <iostream>
#include <map>
#include <vector>

#include "./class/Server.hpp"
#include "./class/ServerSocket.hpp"

//-------------------------------------------------------------------------------------//
int main(void) {
  try {
    // step 1 - server socket : socket, bind, listen, fcntl
    ServerSocket server_socket(AF_INET, 8080);

    // step 2 - init kqueue
    Server server(server_socket);

    // step 3 - main loop
    server.run();
    
  } catch (std::string msg) {
    std::cout << msg << std::endl;
  }
  return (0);
}
