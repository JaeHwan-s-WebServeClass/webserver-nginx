#include "Request.hpp"

#include <cstring>

//---- constructor ------------------------------------------------------------
Request::Request(t_step &flag)
    : raw_head(""), flag(flag), chunk_size(0), hex_str("") {
  // std::cout << GRY << "Debug: Request::contructor\n" << DFT;
  entity.reserve(256);
}

//---- getter -----------------------------------------------------------------
const std::string &Request::getRawHead() const { return this->raw_head; }
const std::string &Request::getMethod() const { return this->method; }
const std::string &Request::getUrl() const { return this->url; };
const std::string &Request::getHttpVersion() const {
  return this->http_version;
}
const std::map<std::string, std::string> &Request::getHeader() const {
  return this->header;
}
const std::vector<char> &Request::getEntity() const { return this->entity; }
size_t Request::getEntitySize() const { return this->entity.size(); }
size_t Request::getContentLength() const {
  size_t content_length;
  std::stringstream ss;

  ss << this->header.find("Content-Length")->second;
  ss >> content_length;
  return content_length;
}

const std::vector<char> &Request::getEntityCgi() const {
  return this->entity_cgi;
}

//---- setter -----------------------------------------------------------------
void Request::setRawHead(std::string line) { this->raw_head += line; }
void Request::setFlag(t_step flag) { this->flag = flag; }
void Request::setEntity(std::vector<char> buf) { this->entity = buf; }
void Request::setEntityClear() { this->entity.clear(); }
void Request::setEntityCgi(const char *buf, size_t read_len) {
  for (size_t i = 0; i < read_len; ++i) {
    this->entity_cgi.push_back(buf[i]);
  }
}

//---- parser -----------------------------------------------------------------
void Request::parserHead() {
  // std::cout << GRY << "Debug: Request::parserHead\n" << DFT;
  std::vector<std::string> tmp_head;
  std::vector<std::string> tmp_start_line;

  tmp_head = ft::split(this->raw_head, '\n');
  tmp_start_line = ft::split(tmp_head[0], ' ');
  this->method = tmp_start_line[0];
  this->url = tmp_start_line[1];
  this->http_version = tmp_start_line[2];

  for (std::vector<std::string>::iterator it = tmp_head.begin() + 1;
       it != tmp_head.end(); ++it) {
    int pos = it->find(':');
    header[it->substr(0, pos)] = ft::trim(it->substr(pos + 1));
  }
}

void Request::addContentLengthEntity(char *buf, int read_len) {
  for (int i = 0; i < read_len; ++i) {
    this->entity.push_back(buf[i]);
  }
  if (this->getEntitySize() == this->getContentLength()) {
    this->setFlag(REQUEST_ENTITY);
  } else if (this->getEntitySize() > this->getContentLength()) {
    throw std::string("Error: Transaction: Request Entity Over Content-Length");
  }
  // FIXME 아래 코드대신, kevent 에서 timeout 처리하기
  // else if (this->request.getEntitySize < getContentLength())
}

void Request::addChunkedEntity(char *buf, size_t read_len) {
  size_t i = 0;

  while (i < read_len) {
    if (chunk_size == 0) {
      hex_str += buf[i];
      if (hex_str.back() == '\n') {
        hex_str.pop_back();
        hex_str.pop_back();
        chunk_size = ft::hexToInt(hex_str);
        std::cout << "chunk_size: " << chunk_size << "\n";
        if (chunk_size == 0) {
          this->setFlag(REQUEST_ENTITY);
          return;
        } else if (chunk_size < 0) {
          throw std::string("Error: Request:: Chunk size overflow");
        }
        chunk_size += 2;
        hex_str = "";
      }
    } else {
      entity.push_back(buf[i]);
      --chunk_size;
      if (chunk_size == 0) {
        entity.pop_back();
        entity.pop_back();
      }
    }
    ++i;
  }
}

//-----------------------------------------------------------------------------
std::ostream &operator<<(std::ostream &out, const Request &r) {
  out << ft::printHelper(
      "-------------------- start-line --------------------");
  out << BLU << "method: " << DFT << r.getMethod() << "\n";
  out << BLU << "url: " << DFT << r.getUrl() << "\n";
  out << BLU << "version: " << DFT << r.getHttpVersion() << "\n";
  out << ft::printHelper(
      "---------------------- header ----------------------");
  const std::map<std::string, std::string> &header = r.getHeader();
  for (std::map<std::string, std::string>::const_iterator it = header.begin();
       it != header.end(); ++it) {
    out << BLU << it->first << ": " << DFT << it->second << "\n";
  }
  out << ft::printHelper(
      "--------------------- entity -----------------------");
  const std::vector<char> &entity = r.getEntity();
  for (unsigned long i = 0; i < entity.size(); i++) {
    out << YLW << entity[i];
  }
  out << DFT << "\n";
  out << ft::printHelper(
      "----------------------------------------------------");
  return out;
}
