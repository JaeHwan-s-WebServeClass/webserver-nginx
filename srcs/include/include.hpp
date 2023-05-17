#pragma once

#include <dirent.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

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
void printError(std::string);
std::string printHelper(std::string msg);
std::string printHelper(std::vector<std::string> msg);
void printVector(const std::vector<std::string> &);

bool isFileEmpty(const char *file_name);
bool isFileDescriptorEmpty(int fd);
bool isFileDescriptorValid(int fd);
bool isDirectory(std::string file_path);

int hexToInt(const std::string &);
std::string intToStr(int);
const char *vecToCharArr(const std::vector<char> &vec);

std::vector<std::string> split(std::string, char);
std::vector<std::string> split(std::string, std::string);

std::string trim(std::string);
std::string trim(std::string, char);

bool findSuffix(std::string filename, std::string suffix);

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

ssize_t safeRead(int, char *, int);
ssize_t safeWrite(int, char *, int);

/// @brief
/// @param buf
/// @param size
/// @param count
/// @param file_ptr
/// @return read/write size
size_t safeFread(char *, int, int, FILE *);
size_t safeFwrite(const char *, int, int, FILE *);
size_t safeOpen(std::string, int, mode_t);
int safeClose(int);

// std::FILE *safeFopen(const char *, const char *);
// int safeFclose(FILE *);

void safePipe(int *);
pid_t safeFork(void);
void safeDup2(int, int);
void safeExecve(const char *, char *const *, char *const *);
}  // namespace ft
