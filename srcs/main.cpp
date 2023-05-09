#include "include/webserv.hpp"

int main(int argc, char **argv) {
  try {
    std::vector<ServerConfig> config;
    if (argc == 1) {
      config = parseConfig("./conf/default.conf");
    } else if (argc == 2) {
      config = parseConfig(argv[1]);
    } else {
      throw std::string("usage: ./webserv [config file]");
    }
    // std::cout << "--- 1 -----------------------------------" << std::endl;
    // system("leaks webserv | grep leaked");
    // DEBUG
    // std::cout << config << std::endl;
    // config.begin()->printConfig(config);
    Server server(config);
    // std::cout << "--- 2 -----------------------------------" << std::endl;
    // system("leaks webserv | grep leaked");
    server.loadErrorPage();
    // std::cout << "--- 3 -----------------------------------" << std::endl;
    // system("leaks webserv | grep leaked");
    server.run();
  } catch (std::exception e) {
    // std::cout << "--- 5-1 ---------------------------------" << std::endl;
    // system("leaks webserv | grep leaked");
    std::cout << RED << "main: " << e.what() << DFT << std::endl;
  }
  // std::cout << "--- 5-2 ---------------------------------" << std::endl;
  // system("leaks webserv | grep leaked");
  return (0);
}
