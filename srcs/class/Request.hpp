#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "../include/define.hpp"
#include "../include/include.hpp"

class Request {
private:
  std::string raw_head;
  std::string method;
  std::string url;
  std::string http_version;
  std::map<std::string, std::string> header;
  std::vector<char> entity;
  t_step &flag;

  int chunk_size;
  std::string hex_str;

public:
  // ---- constructor -------------------------
  Request(t_step &);

  // ---- getter ------------------------------
  const std::string &getRawHead() const;
  const std::string &getMethod() const;
  const std::string &getUrl() const;
  const std::string &getHttpVersion() const;
  const std::map<std::string, std::string> &getHeader() const;
  const std::vector<char> &getEntity() const;
  size_t getEntitySize() const;
  size_t getContentLength() const;

  // ---- setter ------------------------------
  void setRawHead(std::string);
  void setFlag(t_step);

  // ---- parser ------------------------------
  void parserHead();
  void addContentLengthEntity(char *, int);
  /// @brief
  /// @param buf
  /// @param read_len
  void addChunkedEntity(char *, size_t);

  // ---- utils -------------------------------
  void clearSetRawMsg();
  void toString() const;
};

#endif
