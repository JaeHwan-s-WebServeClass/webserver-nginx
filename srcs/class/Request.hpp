#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "../include/include.hpp"

#define POST 0
#define GET 1
#define DELETE 2

#define BUFFER_SIZE 1024

#define RED "\033[0;31m"
#define GRN "\033[0;32m"
#define YLW "\033[0;33m"
#define BLU "\033[0;36m"
#define GRY "\033[90m"
#define DFT "\033[0;37m"

// head : 시작줄, header
// body : entity, body

/*
Request msg 형식
    <메서드> <요청 URL> <버전>
        <헤더>

        <엔터티 본문>

ex)
    <메서드> <요청 URL> <버전> (white space 기준으로 구분)
    GET /test/hi-there.txt HTTP/1.1

    <헤더> (key: value 구조)
    Accept: text/
    Host: www.joes-hardware.com

    <본문>
    없음.
*/

// webserv에서 구현해야하는 method: GET, HOST, DELETE

class Request {
private:
  // 여러가지 요청이 동시에 들어오고, 동시에 처리해야 하는 상황이 있을까?
  // 하나씩 처리한다면 raw_msg 를 clear 하면서 진행하면 되지만, 병렬처리라면
  // 어떻게 해야하는가? -> 입력은 비동기로 받지만, 처리는 동기로 하기 떄문에
  // 병렬 처리할 일이 없음.
  std::string raw_head;

  std::string method;  // 파싱할 때 compare해서 define해둔 int 값 사용
  std::string url;
  std::string http_version;
  std::map<std::string, std::string> header;
  std::vector<char> entity;
  bool head_done;
  bool entity_done;

public:
  Request();

  // ---- setters -------------------------------------
  /// @brief set Is End Head
  /// @param type
  /// @return
  void setHeadDone(bool);
  
  /// @brief set Raw Head
  /// @param line
  /// @return
  void setRawHead(std::string);

  void setEntityDone(bool);

  void addContentLengthEntity(char *, int);
  void addChunkedEntity(char *);

  // ---- getters -------------------------------------
  const std::string &getRawHead() const;
  const bool &getHeadDone() const;
  const std::string &getMethod() const;
  const std::string &getUrl() const;
  const std::string &getHttpVersion() const;
  const std::map<std::string, std::string> &getHeader() const;
  const std::vector<char> &getEntity() const;
  const bool getEntityDone();

// ---- utils -----------------------------------------
  void clearSetRawMsg();
  void parserHead();
  void toString() const;
};

#endif
