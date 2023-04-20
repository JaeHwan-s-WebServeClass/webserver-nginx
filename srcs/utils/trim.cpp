#include "../include/include.hpp"

std::string ft::trim(std::string str) {
  std::string whitespace = " \t\r\n\f\v";  // 공백 문자 집합
  std::string::size_type pos = str.find_first_not_of(whitespace);

  str.erase(0, str.find_first_not_of(whitespace));
  while (isspace(str[str.length() - 1]))
    str.erase(str.find_last_not_of(whitespace) + 1, str[str.length() - 1]);
  return str;
}

// int main(void) {
//   std::string str = " \r";
//   std::cout << ft::trim(str) << std::endl;
//   return (0);
// }

// while (pos != std::string::npos) {
//   str.erase(0, pos);
//   pos = str.find_first_not_of(whitespace);
// }
// pos = str.find_last_not_of(whitespace);
// if (pos != std::string::npos) {
//   str.erase(pos + 1);
// }

// #include <iostream>
// #include <string>

// int main() {
//     std::string str = "  Hello,    World!  ";
//     std::string whitespace = " \t\r\n\f\v"; // 공백 문자 집합
//     std::string::size_type pos = str.find_first_not_of(whitespace);

//     while (pos != std::string::npos) {
//         str.erase(0, pos);
//         pos = str.find_first_not_of(whitespace);
//     }

//     pos = str.find_last_not_of(whitespace);

//     if (pos != std::string::npos) {
//         str.erase(pos + 1);
//     }

//     std::cout << "Result: " << str << std::endl; // 출력: Result:

//     return 0;
// }