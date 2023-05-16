#include "ServerConfig.hpp"

//---- OCCF -------------------------------------------------------------------
ServerConfig::ServerConfig() { this->setDefault(); }
ServerConfig::ServerConfig(const ServerConfig &ref) { *this = ref; }
ServerConfig &ServerConfig::operator=(const ServerConfig &ref) {
  this->listen = ref.listen;
  this->server_name = ref.server_name;
  this->error_page = ref.error_page;
  this->client_max_body_size = ref.client_max_body_size;
  this->client_max_head_size = ref.client_max_head_size;
  this->root = ref.root;
  this->http_method = ref.http_method;
  this->locations = ref.locations;
  this->redirect = ref.redirect;
  return *this;
}
ServerConfig::~ServerConfig() {}

//---- getter ------------------------------------------------------------------
const int &ServerConfig::getListen() const { return this->listen; }
const std::vector<std::string> &ServerConfig::getServerName() const {
  return this->server_name;
}
const std::vector<std::string> &ServerConfig::getErrorPage() const {
  return this->error_page;
}
size_t ServerConfig::getClientMaxBodySize() const {
  return this->client_max_body_size;
}
size_t ServerConfig::getClientMaxHeadSize() const {
  return this->client_max_head_size;
}
const std::string &ServerConfig::getRoot() const { return this->root; }
const std::map<std::string, ServerConfig::t_location> &
ServerConfig::getLocation() const {
  return this->locations;
}
const std::string &ServerConfig::getRedirect() const { return this->redirect; }

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

void ServerConfig::setRedirect(const std::string &value) {
  this->redirect = value;
}

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
      } else if (*it == "PUT") {
        this->http_method |= PUT;
      }
    }
  } else if (key == "deny") {
    std::vector<std::string>::const_iterator it = value.begin();
    for (; it != value.end(); it++) {
      if (*it == "GET") {
        this->http_method &= (0b1110);
      } else if (*it == "POST") {
        this->http_method &= (0b1101);
      } else if (*it == "DELETE") {
        this->http_method &= (0b1011);
      } else if (*it == "PUT") {
        this->http_method &= (0b0111);
      }
    }
  } else if (key == "redirect") {
    this->redirect = value[0];
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
  } else if (key == "cgi_exec") {
    this->locations[map_key].cgi_exec = value.front();
  } else if (key == "cgi_path") {
    this->locations[map_key].cgi_path = value.front();
  } else if (key == "allow") {
    std::vector<std::string>::const_iterator it = value.begin();
    for (; it != value.end(); it++) {
      if (*it == "GET") {
        this->locations[map_key].http_method |= GET;
      } else if (*it == "POST") {
        this->locations[map_key].http_method |= POST;
      } else if (*it == "DELETE") {
        this->locations[map_key].http_method |= DELETE;
      } else if (*it == "PUT") {
        this->locations[map_key].http_method |= PUT;
      }
    }
  } else if (key == "deny") {
    std::vector<std::string>::const_iterator it = value.begin();
    for (; it != value.end(); it++) {
      if (*it == "GET") {
        this->locations[map_key].http_method &= (0b1110);
      } else if (*it == "POST") {
        this->locations[map_key].http_method &= (0b1101);
      } else if (*it == "DELETE") {
        this->locations[map_key].http_method &= (0b1011);
      } else if (*it == "PUT") {
        this->locations[map_key].http_method &= (0b0111);
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
  this->locations[key].cgi_exec = "";
  this->locations[key].cgi_path = "";
  this->locations[key].http_method = (0b111);
}

void ServerConfig::setDefault() {
  this->listen = 80;
  // this->server_name.push_back("localhost");
  // this->error_page.push_back("403");
  // this->error_page.push_back("/403.html");
  // this->error_page.push_back("404");
  // this->error_page.push_back("/404.html");
  // this->error_page.push_back("418");
  // this->error_page.push_back("/418.html");
  // this->error_page.push_back("500");
  // this->error_page.push_back("/500.html");
  // this->error_page.push_back("501");
  // this->error_page.push_back("/501.html");
  this->client_max_body_size = 1024;
  this->client_max_head_size = 1024;
  this->http_method = (0b1111);
  root = "/rootdir";
  this->redirect = "http://google.com";
}

//------------------------------------------------------------------------------
std::ostream &operator<<(std::ostream &out,
                         const std::vector<ServerConfig> &c) {
  int i = 1;
  std::vector<ServerConfig>::const_iterator it1 = c.begin();

  for (; it1 != c.end(); it1++) {
    out << "------------ server config: " << i << " ------------\n";
    out << "- listen:  " << it1->getListen() << "\n";
    out << "- server_name:  ";
    ft::printVector(it1->getServerName());
    out << "- error_page:  ";
    ft::printVector(it1->getErrorPage());
    out << "- client_max_head_size:  " << it1->getClientMaxHeadSize() << "\n";
    out << "- client_max_body_size:  " << it1->getClientMaxBodySize() << "\n";
    out << "- root:  " << it1->getRoot() << "\n";
    out << "- redirect:  " << it1->getRedirect() << "\n";
    out << "- location:  "
        << "\n";
    std::map<std::string, ServerConfig::t_location> locations =
        it1->getLocation();
    std::map<std::string, ServerConfig::t_location>::const_iterator it2 =
        locations.begin();
    for (; it2 != locations.end(); ++it2) {
      out << "   - path: " << it2->first << "\n";
      out << it2->second;
    }
    out << "\n";
    i++;
  }
  out << "------------------------------------------";
  return out;
}

std::ostream &operator<<(std::ostream &out, const ServerConfig::t_location &l) {
  out << "     autoindex:  " << l.autoindex << "\n";
  out << "     root:  " << l.root << "\n";
  out << "     index:  " << ft::printHelper(l.index);
  out << "     http_method:  " << l.http_method << GRY
      << " (GET = 1, POST = 2, DELETE = 4, PUT = 8)" << DFT << "\n";
  out << "     cgi_exec:  " << l.cgi_exec << "\n";
  out << "     cgi_path:  " << l.cgi_path << "\n";
  return out;
}
