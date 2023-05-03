#include "ServerConfig.hpp"

//---- constructor -------------------------------------------------------------
ServerConfig::ServerConfig() { this->setDefault(); }

//---- getter ------------------------------------------------------------------
const int &ServerConfig::getListen() const { return this->listen; }
const std::vector<std::string> &ServerConfig::getServerName() const {
  return this->server_name;
}
const std::vector<std::string> &ServerConfig::getErrorPage() const {
  return this->error_page;
}
int ServerConfig::getClientMaxBodySize() const {
  return this->client_max_body_size;
}
int ServerConfig::getClientMaxHeadSize() const {
  return this->client_max_head_size;
}
const std::string &ServerConfig::getRoot() const { return this->root; }
const std::map<std::string, ServerConfig::t_location> &
ServerConfig::getLocation() const {
  return this->locations;
}

//---- setter ------------------------------------------------------------------
void ServerConfig::setListen(int value) { listen = value; }
void ServerConfig::setServerName(const std::vector<std::string> &value) {
  server_name = value;
}
void ServerConfig::setErrorPage(const std::vector<std::string> &value) {
  error_page = value;
}
void ServerConfig::setClientMaxBodySize(int value) {
  client_max_body_size = value;
}
void ServerConfig::setClientMaxHeadSize(int value) {
  client_max_head_size = value;
}
void ServerConfig::setRoot(const std::string &value) { root = value; }

void ServerConfig::setDirective(std::string key,
                                std::vector<std::string> value) {
  if (key == "listen") {
    this->setListen(std::atoi(value.front().c_str()));
  } else if (key == "server_name") {
    this->server_name.clear();
    this->setServerName(value);
  } else if (key == "error_page") {
    this->error_page.clear();
    this->setErrorPage(value);
  } else if (key == "client_max_body_size") {
    this->setClientMaxBodySize(std::atoi(value.front().c_str()));
  } else if (key == "client_max_head_size") {
    this->setClientMaxHeadSize(std::atoi(value.front().c_str()));
  } else if (key == "root") {
    this->setRoot(value.front());
  } else if (key == "allow") {
    std::vector<std::string>::const_iterator it = value.begin();
    for (; it != value.end(); it++) {
      if (*it == "GET") {
        this->http_method |= GET;
      } else if (*it == "POST") {
        this->http_method |= POST;
      } else if (*it == "DELETE") {
        this->http_method |= DELETE;
      }
    }
  } else if (key == "deny") {
    std::vector<std::string>::const_iterator it = value.begin();
    for (; it != value.end(); it++) {
      if (*it == "GET") {
        this->http_method &= (0b110);
      } else if (*it == "POST") {
        this->http_method &= (0b101);
      } else if (*it == "DELETE") {
        this->http_method &= (0b011);
      }
    }
  } else {
    throw std::string("Error: setDirective: Invalid Element");
  }
}

void ServerConfig::setLocation(const std::string map_key, const std::string key,
                               const std::vector<std::string> &value) {
  if (key == "root") {
    this->locations[map_key].root = value.front();
  } else if (key == "autoindex") {
    if (value.front() == "on") {
      this->locations[map_key].autoindex = true;
    } else if (value.front() == "off") {
      this->locations[map_key].autoindex = false;
    }
  } else if (key == "index") {
    this->locations[map_key].index.clear();
    this->locations[map_key].index = value;
  } else if (key == "cgi") {
    this->locations[map_key].cgi = value.front();
  } else if (key == "allow") {
    std::vector<std::string>::const_iterator it = value.begin();
    for (; it != value.end(); it++) {
      if (*it == "GET") {
        this->locations[map_key].http_method |= GET;
      } else if (*it == "POST") {
        this->locations[map_key].http_method |= POST;
      } else if (*it == "DELETE") {
        this->locations[map_key].http_method |= DELETE;
      }
    }
  } else if (key == "deny") {
    std::vector<std::string>::const_iterator it = value.begin();
    for (; it != value.end(); it++) {
      if (*it == "GET") {
        this->locations[map_key].http_method &= (0b110);
      } else if (*it == "POST") {
        this->locations[map_key].http_method &= (0b101);
      } else if (*it == "DELETE") {
        this->locations[map_key].http_method &= (0b011);
      }
    }
  } else {
    throw std::string("Error: setLocation: Invalid Element");
  }
}

void ServerConfig::setLocationDefault(const std::string key) {
  this->locations[key].root = "/";
  this->locations[key].autoindex = false;
  this->locations[key].index.push_back("index.html");
  this->locations[key].cgi = "";
  this->locations[key].http_method = (0b111);
}

void ServerConfig::setDefault() {
  this->listen = 80;
  this->server_name.push_back("localhost");
  this->error_page.push_back("404");
  this->error_page.push_back("/404.html");
  this->error_page.push_back("500");
  this->error_page.push_back("/500.html");
  this->error_page.push_back("501");
  this->error_page.push_back("/501.html");
  this->client_max_body_size = 1024;
  this->client_max_head_size = 1024;
  this->http_method = (0b111);
  root = "/rootdir";
}

//----- utils ------------------------------------------------------------------
void ServerConfig::printLocation(const t_location &location) {
  std::cout << "     autoindex:  " << location.autoindex << std::endl;
  std::cout << "     root:  " << location.root << std::endl;
  std::cout << "     index:  ";
  ft::printVector(location.index);
  std::cout << "     http_method:  " << location.http_method << GRY
            << " (GET = 1, POST = 2, DELETE = 4)" << DFT << std::endl;
  std::cout << "     cgi:  " << location.cgi << std::endl;
}

void ServerConfig::printConfig(std::vector<ServerConfig> config) {
  int i = 1;
  std::vector<ServerConfig>::const_iterator it1 = config.begin();

  for (; it1 != config.end(); it1++) {
    std::cout << "------------ server config: " << i << " ------------\n";
    std::cout << "- listen:  " << it1->getListen() << std::endl;
    std::cout << "- server_name:  ";
    ft::printVector(it1->getServerName());
    std::cout << "- error_page:  ";
    ft::printVector(it1->getErrorPage());
    std::cout << "- client_max_head_size:  " << it1->getClientMaxHeadSize()
              << std::endl;
    std::cout << "- client_max_body_size:  " << it1->getClientMaxBodySize()
              << std::endl;
    std::cout << "- root:  " << it1->getRoot() << std::endl;
    std::cout << "- location:  " << std::endl;

    std::map<std::string, t_location>::const_iterator it2 =
        it1->locations.begin();
    for (; it2 != it1->locations.end(); ++it2) {
      std::cout << "   - path: " << it2->first << std::endl;
      printLocation(it2->second);
    }
    std::cout << "\n";
    i++;
  }
  std::cout << "------------------------------------------\n";
}