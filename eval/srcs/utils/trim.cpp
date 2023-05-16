#include "../include/include.hpp"

std::string ft::trim(std::string str) {
  std::string whitespace = " \t\r\n\f\v";  // 공백 문자 집합

  str.erase(0, str.find_first_not_of(whitespace));
  if (str != "") {
    while (std::isspace(str.back()))
      str.erase(str.find_last_not_of(whitespace) + 1, str.length() - 1);
  }
  return str;
}

std::string ft::trim(std::string str, char dlim) {
  std::string whitespace = " \t\r\n\f\v";

  whitespace += dlim;
  str.erase(0, str.find_first_not_of(whitespace));
  if (str != "") {
    while (str.back() == dlim || std::isspace(str.back()))
      str.erase(str.find_last_not_of(whitespace) + 1, str.length() - 1);
  }
  return str;
}
