#include "../include/include.hpp"

std::vector<std::string> ft::split(std::string input, char dlim) {
  std::vector<std::string> result;
  std::stringstream ss;
  std::string str_buf;

  ss.str(input);
  while (getline(ss, str_buf, dlim)) {
    result.push_back(str_buf);
  }
  return result;
}

std::vector<std::string> ft::split(std::string input, std::string dlim) {
  std::vector<std::string> result;
  std::stringstream ss(input);
  std::string str_buf;
  size_t dlim_len = dlim.length();

  while (1) {
    size_t pos = input.find(dlim);
    if (pos == std::string::npos) {
      result.push_back(input);
      break;
    } else if (pos == 0) {
      input = input.substr(dlim_len);
      continue;
    } else {
      result.push_back(input.substr(0, pos));
      input = input.substr(pos + dlim_len);
    }
  }
  return result;
}
