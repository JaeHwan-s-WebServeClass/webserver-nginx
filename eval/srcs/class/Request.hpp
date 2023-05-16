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
  std::vector<char> entity_cgi;

  Request();

 public:
  // ---- occf --------------------------------
  Request(t_step &);
  Request(const Request &ref);
  Request &operator=(const Request &ref);
  ~Request();

  // ---- getter ------------------------------
  const std::string &getRawHead() const;
  const std::string &getMethod() const;
  const std::string &getUrl() const;
  const std::string &getHttpVersion() const;
  const std::map<std::string, std::string> &getHeader() const;
  const std::vector<char> &getEntity() const;
  size_t getEntitySize() const;
  size_t getContentLength() const;
  const std::vector<char> &getEntityCgi() const;

  // ---- setter ------------------------------
  void setRawHead(std::string);
  void setFlag(t_step);
  void setEntity(std::vector<char>);
  void setEntityClear();
  void setEntityCgi(const char *buf, size_t read_len);

  // ---- parser ------------------------------
  void parserHead();
  void addContentLengthEntity(char *, int);
  /// @brief
  /// @param buf
  /// @param read_len
  void addChunkedEntity(char *, size_t);
};

std::ostream &operator<<(std::ostream &out, const Request &r);

#endif
