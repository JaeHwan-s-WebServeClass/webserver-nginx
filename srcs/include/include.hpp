#pragma once

#include <sys/socket.h>

#include <cctype>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "./define.hpp"

class Response;

namespace ft {
// ---- parser --------------------------------------------
void parser(std::string);

// ---- utils ---------------------------------------------
void errorHandler(std::string);

int hexToInt(const std::string &);
std::string intToStr(int);

std::vector<std::string> split(std::string, char);
std::vector<std::string> split(std::string, std::string);

std::string trim(std::string);
std::string trim(std::string, char);

void printVector(const std::vector<std::string> &);

// ---- safe-functions ------------------------------------
/// @brief
/// @param fd
/// @param buf
/// @return read size
int safeRecv(int, char *, int);

/// @brief
/// @param fd
/// @param response
/// @return write size
int safeSend(int, Response &);

/// @brief
/// @param buf
/// @param size
/// @param count
/// @param file_ptr
/// @return read/write size
size_t safeFread(char *, int, int, FILE *);
size_t safeFwrite(const char *, int, int, FILE *);
std::FILE *safeFopen(const char *, const char *);

/// @brief
/// @param nevents
/// @param timeout
/// @return
int safeKevent(int, const timespec *);
}  // namespace ft
