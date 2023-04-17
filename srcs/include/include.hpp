#pragma once

#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <exception>
namespace ft {

void errorHandler(std::string);
void parser(std::string);
std::vector<std::string> split(std::string, char);
int hexToInt(const std::string& hex_str);
struct ServerConfig {
  int listen;
  std::vector<std::string> server_name;
  std::vector<std::string> error_page;
  int client_max_body_size;
  std::vector<std::string> location;
};
}  // namespace ft