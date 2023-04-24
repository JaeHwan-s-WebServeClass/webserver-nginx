#pragma once

#include <stdlib.h>

#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cctype>
#include <sstream>

namespace ft {

void errorHandler(std::string);
void parser(std::string);

std::vector<std::string> split(std::string, char);
std::vector<std::string> split(std::string, std::string);

int hexToInt(const std::string& hex_str);

std::string trim(std::string str);
std::string trim(std::string str, char);

void	printVector(const std::vector<std::string> &vec);

}  // namespace ft
