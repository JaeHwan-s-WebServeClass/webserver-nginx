#include "class/ServerConfig.hpp"
#include "include/webserv.hpp"

int main(int argc, char **argv) {
  try {
    // step 1 - config file parsing
    std::vector<ServerConfig> config = parseConfig(argv[1]);
    // DEBUG ----------------------------------------------
    // int tmp_cnt = 2;
    // for (int i = 0; i < tmp_cnt; i++) {
    //   config[i].printConfig(config);
    // }
    // std::cout << "---------------------------------------\n";

    // step 2 - setting server
    std::vector<ServerSocket> socket_vec;
    std::vector<ServerConfig>::const_iterator it = config.begin();
    for(; it != config.end(); it++) {
      // step 3 - server socket : socket, bind, listen, fcntl
      ServerSocket tmp_socket(AF_INET, (*it).getListen());
      socket_vec.push_back(tmp_socket);
    }

    // step 4 - init kqueue
    Server server(socket_vec);

    // step 5 - main loop
    server.run();

  } catch (std::string msg) {
    std::cout << RED << msg << DFT << std::endl;
  }
  return (0);
}
