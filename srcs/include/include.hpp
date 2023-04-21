#pragma once

#include <stdlib.h>

#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cctype>
namespace ft {

void errorHandler(std::string);
void parser(std::string);
std::vector<std::string> split(std::string, char);
int hexToInt(const std::string& hex_str);
std::string trim(std::string str);

struct ServerConfig {
  int listen;
  std::vector<std::string> server_name;
  std::vector<std::string> error_page;
  int client_max_body_size;
  std::vector<std::string> location;
};
}  // namespace ft