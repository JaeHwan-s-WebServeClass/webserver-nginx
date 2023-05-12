#include "../class/ServerConfig.hpp"
#include "../class/Transaction.hpp"
#include "../include/include.hpp"

int isLocation(std::string &line, std::string *location_key) {
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

void goParsing(ServerConfig &tmp_conf, std::string &line,
               std::string &location_key, bool server_fl, bool location_fl) {
  std::string key;
  std::vector<std::string> value;
  std::vector<std::string> vec = ft::split(line, '\t');
  key = vec.front();
  value = ft::split(vec.back(), ' ');

  if (server_fl == true && location_fl == true) {
    // clear?
    tmp_conf.setLocation(location_key, key, value);
  } else if (server_fl == true && location_fl == false) {
    tmp_conf.setDirective(key, value);
  } else {
    throw std::string("Error: goParsing: Invalid Config File");
  }
}

std::vector<ServerConfig> parseConfig(const char *config_file) {
  std::vector<ServerConfig> config_data;
  std::ifstream file_stream(config_file);
  std::string line;
  std::string location_key;
  bool server_fl = false;
  bool location_fl = false;

  if (!file_stream.is_open()) {
    throw std::string("Error: parseConf: Invalid Config File");
  }

  ServerConfig *tmp_conf;

  while (std::getline(file_stream, line)) {
    line = ft::trim(line);
    if (line[0] == '#' || line[0] == '\0') {
      continue;
    } else if (line == "server {") {
      server_fl = true;
      tmp_conf = new ServerConfig();
      tmp_conf->setDefault();
    } else if (isLocation(line, &location_key)) {
      tmp_conf->setLocationDefault(location_key);
      location_fl = true;
    } else if (server_fl == true && location_fl == true && line == "}") {
      location_fl = false;
    } else if (server_fl == true && location_fl == false && line == "}") {
      server_fl = false;
      config_data.push_back(*tmp_conf);
      delete tmp_conf;
      // tmp_conf->setDefault();
    } else {
      if (line.back() != ';') {
        throw std::string("Error: parseConf: Missing Semicolon");
      }
      line = ft::trim(line, ';');
      goParsing(*tmp_conf, line, location_key, server_fl, location_fl);
    }
  }
  file_stream.close();
  return config_data;
}