#include "include/webserv.hpp"

int main(int argc, char **argv) {
  (void)argc; // temporary
  try {
    // step 1 - config file parsing
    std::vector<ServerConfig> config = parseConfig(argv[1]);

    // step 2 - setting server & create socket & init kqueue
    Server server(config);

    // step 3 - caching error page
    server.loadErrorPage();

    // step 4 - main loop
    server.run();

  } catch (std::string msg) {
    std::cout << RED << msg << DFT << std::endl;
  }
  return (0);
}

// DEBUG: checking result of parseConfig()
// int tmp_cnt = 2;
// for (int i = 0; i < tmp_cnt; i++) {
//   config[i].printConfig(config);
// }
// std::cout << "---------------------------------------\n";