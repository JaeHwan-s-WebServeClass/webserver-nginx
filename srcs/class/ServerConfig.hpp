#pragma once

#include <cstdint>
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <map>

#include "../include/include.hpp"

class ServerConfig {
private:
	typedef struct s_location {
		std::string 				root;
		bool						autoindex;
		std::vector<std::string> 	index;
		bool						GET;
		bool						POST;
		bool						DELETE;
	}	t_location;

private:
  int                       listen;
  std::vector<std::string>  server_name;
  std::vector<std::string>  error_page;
  int                       client_max_body_size;
  int 						client_max_head_size;
  std::string				root;
  std::map<std::string, t_location>	locations;

public:
	ServerConfig();
	ServerConfig(const char *);

	// ---- getters ------------------------------
	int getListen() const;
	const std::vector<std::string>& getServerName() const;
	const std::vector<std::string>& getErrorPage() const;
	int getClientMaxBodySize() const;
	int getClientMaxHeadSize() const;
	const std::string& getRoot() const;
	const std::map<std::string, t_location>& getLocation() const;

	// ---- setters ------------------------------
	void setListen(int);
	void setServerName(const std::vector<std::string>&);
	void setErrorPage(const std::vector<std::string>&);
	void setClientMaxBodySize(int);
	void setClientMaxHeadSize(int);
	void setRoot(const std::string&);
	
	// ---- Default -------------------------------
	void setDefault();
	void setLocationDefault(const std::string);

	// ---- set after parse ------------------------
	void setDirective(std::string key, std::vector<std::string> value);
	void setLocation(const std::string, const std::string, const std::vector<std::string>&);
	
	// ---- utils --------------------------------
	void	printLocation(const t_location &location) {
		std::cout << "    autoindex: " << location.autoindex << std::endl ;
		std::cout << "    root: " << location.root << std::endl ;
		std::cout << "    index: ";
		ft::printVector(location.index);
		std::cout << "    GET: " << location.GET << std::endl ;
		std::cout << "    POST: " << location.POST << std::endl ;
		std::cout << "    DELETE: " << location.DELETE << std::endl ;
	}

	void	printConfig(const std::vector<ServerConfig> &config) {
		std::vector<ServerConfig>::const_iterator it1 = config.begin();
		// int i = 1;

		// for (; it1 != config.end(); it1++) {
			std::cout << "-------- server config --------\n";
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
		std::cout << "----------------------------------\n";
	}
};
