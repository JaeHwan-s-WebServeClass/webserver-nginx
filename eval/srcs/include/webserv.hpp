#pragma once

// c header
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// kqueue header in C Library
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>

// c++ header
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "../class/Request.hpp"
#include "../class/Server.hpp"
#include "../class/ServerConfig.hpp"
#include "../class/ServerSocket.hpp"
#include "./include.hpp"

// ---- config parser ----------------------------
std::vector<ServerConfig> parseConfig(const char *config_file);
void goParsing(ServerConfig &tmp_conf, std::string &line,
               std::string &location_key, bool server_fl, bool location_fl);