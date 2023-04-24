#pragma once

// c header
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

// kqueue header in C Library
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>

// c++ header
#include <iostream>
#include <map>
#include <vector>

#include "../class/Request.hpp"
#include "../class/Server.hpp"
#include "../class/ServerSocket.hpp"
#include "../class/ServerConfig.hpp"
#include "./include.hpp"

std::vector<ServerConfig> parseConf(char *config_file);
void setDirective(ServerConfig &tmp_conf, std::string key, std::vector<std::string> value);
void setLocation(ServerConfig &tmp_conf, std::string key, std::vector<std::string> value);

// ---- config parser ----------------------------
std::vector<ServerConfig> parseConfig(char *config_file);
void goParsing(ServerConfig &tmp_conf, std::string& line, std::string& location_key, bool server_fl, bool location_fl);

	
// ---- is func ---------------------------------
std::string isLocation(std::string& line, ServerConfig &tmp_conf);