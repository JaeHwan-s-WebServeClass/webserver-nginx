
#include <iostream>
#include <sstream>
#include <vector>

#include "../include/include.hpp"

std::vector<std::string> ft::split(std::string input, char dlim)
{
	std::vector<std::string>    result;
	std::stringstream           ss;
	std::string                 str_buf;

	ss.str(input);			
	while (getline(ss, str_buf, dlim))
		result.push_back(str_buf);
	return result;
}
