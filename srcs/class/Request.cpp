#include "Request.hpp"

Request::Request() : raw_head(""), is_end_head(0), done(false), entity("") {
  std::cout << GRY << "Debug: Request::contructor\n" << DFT;
}

//---- getter/setter --------------------------------------------
// -------------------------- 수정할 부분 -----------------------
void Request::setRawMsg(const char* read_msg) {
  std::istringstream buf;
  buf.str(read_msg);

  std::string line;
// -------------------------- head 파싱 -----------------------
  while (std::getline(buf, line, '\n')) {
  // char buf[BUFFER_SIZE];
  // int read_len = safeRead(this->socket_fd, buf);
    if (line.length() == 0 || line == "\r") {
      this->is_end_head = true;
      this->parserHead();
    } else if (!this->is_end_head) {
      this->raw_head += line;
      if (!buf.eof()) {
        this->raw_head += '\n';
      }
    }
// -------------------------- entity 파싱 -----------------------
  }
// -------------------------- done -----------------------
  this->done = true;
  std::cout << GRY << "Debug: Request::setRawMsg\n" << DFT;
}
// ------------------------------------------------------------------------

// void Request::setEntity(std::string line) {
//   // content length가 있을 때
//   if (header.find("Content-Length") != header.end()) {
//     if (entity.size() == header["Content_Length"]) {
//       done = true;
//     }
//     if (header["Content-Length"] >= (entity.length() + line.length()))
//       this->entity += line;
//     else if (header["Content-Length"] < (entity.length() + line.length()))
//     {
//       int i = 0;
//       while(header["Content-Length"] > entity.length()) {
//         entity += line[i++];
//       }
//     }
//   }
//   // content length가 없을 때
//   else {
//     // transfer encoding이 있을 때
//       // chunk: body를 chunk 단위로 쪼개서 보내게 되며, 다 끝나면 0으로
//       채워진 chunk가 오게 된다.
//       // chunk size는 chunk의 시작부분에 있는 16진수의 값으로 size가
//       지정된다. (p.438)
//     if (header.find("Transfer-Encoding") != header.end()) {
//     // transfer encoding이 없을 때
//     } else {

//     }

//     this->entity += line;
//     if (EOF 만났으면)
//     {
//       done = true;
//     }
//   }

// }

const std::string& Request::getRawHead() const { return this->raw_head; }
const bool& Request::getIsEndHead() const { return this->is_end_head; }
const std::string& Request::getMethod() const { return this->method; }
const std::string& Request::getUrl() const { return this->url; };
const std::string& Request::getHttpVersion() const {
  return this->http_version;
}
const std::map<std::string, std::string>& Request::getHeader() const {
  return this->header;
}
const std::string& Request::getEntity() const { return this->entity; }

void Request::clearSetRawMsg() { this->raw_head.clear(); }

void Request::toString() const {
  std::cout << GRY << "-------------------- start-line --------------------"
            << DFT << std::endl;
  std::cout << BLU << "method: " << DFT << this->method << std::endl;
  std::cout << BLU << "url: " << DFT << this->url << std::endl;
  std::cout << BLU << "version: " << DFT << this->http_version << std::endl;
  std::cout << GRY << "---------------------- header ----------------------"
            << DFT << std::endl;
  for (std::map<std::string, std::string>::const_iterator it = header.begin();
       it != header.end(); ++it) {
    std::cout << BLU << it->first << ": " << DFT << it->second << std::endl;
  }
  std::cout << GRY << "--------------------- entity -----------------------"
            << DFT << std::endl;
  //  for (int i = 0; i < entity.size(); i++) {
  // 	std::cout << YLW << entity[i] << std::endl;
  //  }
  std::cout << YLW << entity << DFT << std::endl;
  std::cout << GRY << "Debug: Request::toString\n" << DFT;
}

void Request::parserHead() {
  std::vector<std::string> tmp_head;
  std::vector<std::string> tmp_start_line;

  // 개행기준으로 split하고
  tmp_head = ft::split(this->raw_head, '\n');

  // 시작줄은 space 기준으로 split
  tmp_start_line = ft::split(tmp_head[0], ' ');
  this->method = tmp_start_line[0];
  this->url = tmp_start_line[1];
  this->http_version = tmp_start_line[2];

  // header를 처리하는 부분
  // colon(:)을 찾아서 그 전까지는 key에 넣고 뒤의 부분은 value에 넣도록 합시다
  for (std::vector<std::string>::iterator it = tmp_head.begin() + 1;
       it != tmp_head.end(); ++it) {
    int pos = it->find(':');
    header[it->substr(0, pos)] = it->substr(pos + 1);
  }
  std::cout << GRY << "Debug: Request::setRawMsg\n" << DFT;
}

const bool Request::isDone() { return this->done; }