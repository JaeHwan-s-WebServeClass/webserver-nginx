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

    // DEBUG
    config.begin()->printConfig(config);

    Server server(config);
    server.loadErrorPage();
    server.run();

  } catch (std::string msg) {
    std::cout << RED << msg << DFT << std::endl;
  } catch (std::exception e) {
    std::cout << RED << "main: " << e.what() << DFT << std::endl;
  }
  return (0);
}
