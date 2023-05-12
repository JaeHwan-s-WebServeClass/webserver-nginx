#include "include/webserv.hpp"

int g_server_status = HEALTHY;

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
    // DEBUG
    // std::cout << config << std::endl;
    Server server(config);
    server.loadErrorPage();
    server.run();
  } catch (std::exception &e) {
    std::cout << RED << "main exception: " << e.what() << DFT << std::endl;
  } catch (std::string &s) {
    std::cout << RED << "main: " << s << DFT << std::endl;
  }
  return (0);
}
