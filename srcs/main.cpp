#include "include/webserv.hpp"

int main(int argc, char **argv) {
  (void)argc;  // temporary
  try {
    // step 1 - config file parsing
    std::vector<ServerConfig> config = parseConfig(argv[1]);
    // DEBUG
    // config.begin()->printConfig(config);

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
