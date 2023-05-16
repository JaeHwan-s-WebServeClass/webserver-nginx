#include "../include/include.hpp"

void ft::printError(std::string msg) {
  std::cout << RED << msg << DFT << std::endl;
}

std::string ft::printHelper(std::string msg) {
  std::string new_msg;

  new_msg = GRY + msg + DFT + "\n";
  return new_msg;
}

std::string ft::printHelper(std::vector<std::string> msg) {
  std::string new_msg;
  std::vector<std::string>::const_iterator it = msg.begin();

  for (; it != msg.end(); ++it) {
    new_msg += *it + " ";
  }
  new_msg += "\n";
  return new_msg;
}

void ft::printVector(const std::vector<std::string> &vec) {
  std::vector<std::string>::const_iterator it;

  for (it = vec.begin(); it != vec.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << "\n";
}
