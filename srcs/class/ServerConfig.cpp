#include "ServerConfig.hpp"

// ---- constructors ----------------------------------------------
ServerConfig::ServerConfig() {
	this->setDefault();
}

ServerConfig::ServerConfig(const char *path) {
	//
}

// ---- getters ---------------------------------------------------
int ServerConfig::getListen() const { return this->listen; }
const std::vector<std::string>& ServerConfig::getServerName() const { return this->server_name; }
const std::vector<std::string>& ServerConfig::getErrorPage() const { return this->error_page; }
int ServerConfig::getClientMaxBodySize() const { return this->client_max_body_size; }
int ServerConfig::getClientMaxHeadSize() const { return this->client_max_head_size; }
const std::string& ServerConfig::getRoot() const { return this->root; }
const bool ServerConfig::getGET() const { this->GET; }
const bool ServerConfig::getPOST() const { this->POST; }
const bool ServerConfig::getDELETE() const { this->DELETE; }
const std::map<std::string, ServerConfig::t_location>& ServerConfig::getLocation() const { return this->locations; }

// ---- setters ---------------------------------------------------
void ServerConfig::setListen(int value) { listen = value; }
void ServerConfig::setServerName(const std::vector<std::string>& value) { server_name = value; }
void ServerConfig::setErrorPage(const std::vector<std::string>& value) { error_page = value; }
void ServerConfig::setClientMaxBodySize(int value) { client_max_body_size = value; }
void ServerConfig::setClientMaxHeadSize(int value) { client_max_head_size = value; }
void ServerConfig::setRoot(const std::string& value) { root = value; }

void ServerConfig::setDirective(std::string key, std::vector<std::string> value) {
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
		for(; it != value.end(); it++) {
			if (*it == "GET") {
				this->GET = true;
			} else if (*it == "POST") {
				this->POST = true;
			} else if (*it == "DELETE") {
				this->DELETE = true;
			}
	    }
    } else if (key == "deny") {
        std::vector<std::string>::const_iterator it = value.begin();
		for(; it != value.end(); it++) {
			if (*it == "GET") {
				this->GET = false;
			} else if (*it == "POST") {
				this->POST = false;
			} else if (*it == "DELETE") {
				this->DELETE = false;
			} 
		}
	} else {
    throw std::string("Error: setDirective: Invalid Element");
  }
}

void ServerConfig::setLocation(const std::string map_key, const std::string key, const std::vector<std::string>& value) { 
	if (key == "root") {
          this->locations[map_key].root = value.front();
	} else if (key == "autoindex") {
 		if (value.front() == "on") {
			this->locations[map_key].autoindex = true;
		} else if(value.front() == "off") {
			this->locations[map_key].autoindex = false;
		}
	} else if (key == "index") {
		this->locations[map_key].index.clear();
		this->locations[map_key].index = value;
	} else if (key == "allow") {
		std::vector<std::string>::const_iterator it = value.begin();
		for(; it != value.end(); it++) {
			if (*it == "GET") {
				this->locations[map_key].GET = true;
			} else if (*it == "POST") {
				this->locations[map_key].POST = true;
			} else if (*it == "DELETE") {
				this->locations[map_key].DELETE = true;
			}
	    }
    } else if (key == "deny") {
        std::vector<std::string>::const_iterator it = value.begin();
		for(; it != value.end(); it++) {
			if (*it == "GET") {
				this->locations[map_key].GET = false;
			} else if (*it == "POST") {
				this->locations[map_key].POST = false;
			} else if (*it == "DELETE") {
				this->locations[map_key].DELETE = false;
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
	this->locations[key].GET = true;
	this->locations[key].POST = true;
	this->locations[key].DELETE = true;
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
	this->GET = true;
	this->POST = true;
	this->DELETE = true;
	root = "/rootdir";
}

// ---- utils ----------------------------------------------------
void	ServerConfig::printLocation(const t_location &location) {
	std::cout << "    autoindex: " << location.autoindex << std::endl ;
	std::cout << "    root: " << location.root << std::endl ;
	std::cout << "    index: ";
	ft::printVector(location.index);
	std::cout << "    GET: " << location.GET << std::endl ;
	std::cout << "    POST: " << location.POST << std::endl ;
	std::cout << "    DELETE: " << location.DELETE << std::endl ;
}

void	ServerConfig::printConfig(const std::vector<ServerConfig> &config) {
	std::vector<ServerConfig>::const_iterator it1 = config.begin();
	// int i = 1;

	// for (; it1 != config.end(); it1++) {
		std::cout << "------------ server config ------------\n";
		std::cout << "listen: " << this->getListen() << std::endl ;
		std::cout << "server_name: ";
		ft::printVector(this->getServerName());
		std::cout << "error_page: ";
		ft::printVector(this->getErrorPage());
		std::cout << "client_max_head_size: " << this->getClientMaxHeadSize() << std::endl ;
		std::cout << "client_max_body_size: " << this->getClientMaxBodySize() << std::endl ;
		std::cout << "root: " << this->getRoot() << std::endl ;
		std::cout << "location: " << std::endl ;
		std::map<std::string, t_location>::const_iterator it2 = this->getLocation().begin();
		for (; it2 != this->getLocation().end(); it2++) {
			printLocation(it2->second);
		}
		std::cout << "\n";
		// i++;
	// }
	// std::cout << "----------------------------------\n";
}