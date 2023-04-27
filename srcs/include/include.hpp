#pragma once

#include <stdlib.h>

#include <cctype>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace ft {
// ---- parser --------------------------------------------
void parser(std::string);

// ---- utils ---------------------------------------------
void errorHandler(std::string);

std::vector<std::string> split(std::string, char);
std::vector<std::string> split(std::string, std::string);

int hexToInt(const std::string& hex_str);

std::string trim(std::string str);
std::string trim(std::string str, char);

void printVector(const std::vector<std::string>& vec);

}  // namespace ft
