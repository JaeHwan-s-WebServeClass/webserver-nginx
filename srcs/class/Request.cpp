#include "Request.hpp"

Request::Request() {
	this->raw_msg = "";
}

//---- getter/setter --------------------------------------------
void Request::setRawMsg(const std::string &update) {
	this->raw_msg += update;
}

std::string Request::getRawMsg() {
	return this->raw_msg;
}

void Request::clearSetRawMsg() {
	this->raw_msg.clear();
}