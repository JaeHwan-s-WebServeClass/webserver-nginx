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
public:
	typedef struct s_location {
		std::string 				root;
		std::vector<std::string> 	index;
		bool						autoindex;
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
	bool						GET;
	bool						POST;
	bool						DELETE;
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
	const bool getGET() const;
	const bool getPOST() const;
	const bool getDELETE() const;
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
	void	printLocation(const t_location &location);
	void	printConfig(const std::vector<ServerConfig> &config);
};
