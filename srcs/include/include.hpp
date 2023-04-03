#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <map>

void	errorHandler(std::string);
void	parser(std::string);
std::vector<std::string> split(std::string, char);

struct ServerConfig {
	
	int							listen;
	std::vector<std::string>	server_name;
	std::vector<std::string>	error_page;
	int							client_max_body_size;
	std::vector<std::string>	location;

};