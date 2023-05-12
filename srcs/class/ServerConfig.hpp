#pragma once

#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "../include/define.hpp"
#include "../include/include.hpp"

class ServerConfig {
 public:
  typedef struct s_location {
    std::string root;
    std::vector<std::string> index;
    bool autoindex;
    int http_method;
    std::string cgi_exec;
    std::string cgi_path;
  } t_location;

 private:
  int listen;
  std::vector<std::string> server_name;
  std::vector<std::string> error_page;
  size_t client_max_body_size;
  size_t client_max_head_size;
  std::string root;
  int http_method;
  std::map<std::string, t_location> locations;
  std::string redirect;

 public:
  // ---- occf --------------------------------
  ServerConfig();
  ServerConfig(const ServerConfig &ref);
  ServerConfig &operator=(const ServerConfig &ref);
  ~ServerConfig();

  // ---- getter ------------------------------
  const int &getListen() const;
  const std::vector<std::string> &getServerName() const;
  const std::vector<std::string> &getErrorPage() const;
  size_t getClientMaxBodySize() const;
  size_t getClientMaxHeadSize() const;
  const std::string &getRoot() const;
  const std::map<std::string, t_location> &getLocation() const;
  const std::string &getRedirect() const;

  // ---- setter -------------------------------
  void setListen(int);
  void setServerName(const std::vector<std::string> &);
  void setErrorPage(const std::vector<std::string> &);
  void setClientMaxBodySize(int);
  void setClientMaxHeadSize(int);
  void setRoot(const std::string &);
  void setRedirect(const std::string &);
  void setDefault();
  void setLocationDefault(const std::string);
  void setDirective(std::string key, std::vector<std::string> value);
  void setLocation(const std::string, const std::string,
                   const std::vector<std::string> &);
};

std::ostream &operator<<(std::ostream &out, const std::vector<ServerConfig> &c);
std::ostream &operator<<(std::ostream &out, const ServerConfig::t_location &l);