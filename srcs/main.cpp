#include "class/ServerConfig.hpp"
#include "include/webserv.hpp"

// ---- parsing ---------------------------------------------------
int isLocation(std::string& line, ServerConfig &tmp_conf, std::string * location_key) {
    std::vector<std::string> location = ft::split(line, '\t');
	if (location.front() != "location") {
		return 0;
	}
	
	std::vector<std::string> key = ft::split(location.back(), ' ');
    if (key.back() != "{") {
        return 0;
    }
    *location_key = key.front();
  	return 1;
}

void goParsing(ServerConfig &tmp_conf, std::string& line, std::string& location_key, bool server_fl, bool location_fl) {
  std::string key;
  std::vector<std::string> value;
  std::vector<std::string> vec = ft::split(line, '\t');
  key = vec.front();
  value = ft::split(vec.back(), ' ');

  if (server_fl == true && location_fl == true) {
    tmp_conf.setLocation(location_key, key, value);
  } else if (server_fl == true && location_fl == false) {
    tmp_conf.setDirective(key, value);
  } else {
    throw std::string("Error: goParsing: Invalid Config File");
  }
}

std::vector<ServerConfig> parseConfig(char *config_file) {
  std::vector<ServerConfig> config_data;
  std::ifstream file_stream(config_file);
  std::string line;
  std::string location_key;
  bool server_fl;
  bool location_fl;

  if (!file_stream.is_open()) {
      throw std::string("Error: parseConf: Invalid Config File");
  }

  ServerConfig  tmp_conf;

  while (std::getline(file_stream, line)) {
      line = ft::trim(line);
      if (line[0] == '#' || line[0] == '\0') {
          continue;
      } else if (line == "server {") {
          server_fl = true;
      } else if (isLocation(line, tmp_conf, &location_key)) {
          tmp_conf.setLocationDefault(location_key);
          location_fl = true;
      } else if (server_fl == true && location_fl == true && line == "}") {
          location_fl = false;
      } else if (server_fl == true && location_fl == false && line == "}") {
          server_fl = false;
          config_data.push_back(tmp_conf);
          tmp_conf.setDefault();
      } else {
        if (line.back() != ';') {
          throw std::string("Error: parseConf: Missing Semicolon");
        }
        line = ft::trim(line, ';');
        goParsing(tmp_conf, line, location_key, server_fl, location_fl);
      }
  }
  file_stream.close();
  
  return config_data;
}

//----------------------------------------------------------------------------//
int main(int argc, char **argv) {
  try {
    // step 1 - config file parsing and server setting
    std::vector<ServerConfig> config = parseConfig(argv[1]);
    
    // DEBUG
    int tmp_cnt = 2;
    for (int i = 0; i < tmp_cnt; i++) {
      config[i].printConfig(config);
    }
    std::cout << "---------------------------------------\n";
    // step 2 - server socket : socket, bind, listen, fcntl
    ServerSocket server_socket1(AF_INET, 8080);
    ServerSocket server_socket2(AF_INET, 4242);
    std::vector<ServerSocket *> socket_vec;
    socket_vec.push_back(&server_socket1);
    socket_vec.push_back(&server_socket2);

    // step 3 - init kqueue
    Server server(socket_vec);

    // step 4 - main loop
    server.run();

  } catch (std::string msg) {
    std::cout << RED << msg << DFT << std::endl;
  }
  return (0);
}
