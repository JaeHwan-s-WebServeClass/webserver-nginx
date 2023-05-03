#include "Server.hpp"

//----- constructor ------------------------------------------------------------
Server::Server(std::vector<ServerConfig> &server_config)
    : server_config(server_config) {
  // std::cout << GRY << "Debug: Server: constructor\n" << DFT;
  std::vector<ServerConfig>::const_iterator it = this->server_config.begin();
  // TODO 같은 포트 여러개 들어올 때 예외처리
  for (; it != this->server_config.end(); it++) {
    ServerSocket tmp_socket(AF_INET, (*it).getListen());
    this->server_socket.push_back(tmp_socket);
  }
  if ((this->kq = kqueue()) == -1) {
    throw std::string("Error: Server: constructor\n" +
                      std::string(strerror(errno)));
  }
}

//---- error page -------------------------------------------------------------
void Server::loadErrorPage() {
  // key 값이 config, value 가 페이지 내용
  ServerConfig temp_conf = this->server_config[0];
  std::vector<std::string> conf_error_page = temp_conf.getErrorPage();

  std::vector<std::string>::const_iterator it = conf_error_page.begin();

  for (; it != conf_error_page.end(); ++it) {
    //  error_page map 초기화
    //  open && event 등록
    std::string key = *it;
    it++;
    std::string filename = *it;
    std::string rootdir = temp_conf.getRoot();
    FILE *fp = ft::safeFopen(('.' + rootdir + filename).c_str(), "r");
    fcntl(fp->_file, F_SETFL, O_NONBLOCK);
    setChangeList(this->change_list, fp->_file, EVFILT_READ, EV_ADD | EV_ENABLE,
                  0, 0, fp);
    //  이벤트 탐지해서 읽고 map 에 넘겨주기
    this->safeKevent(1, NULL);
    this->change_list.clear();

    char buf[9999];
    if (this->event_list[0].filter == EVFILT_READ) {
      size_t read_len =
          ft::safeFread(buf, sizeof(char), 9999,
                        reinterpret_cast<FILE *>(this->event_list[0].udata));
      buf[read_len] = '\0';
    }
    this->error_page[key] = buf;
    setChangeList(this->change_list, fp->_file, EVFILT_READ, EV_DELETE, 0, 0,
                  NULL);
    this->change_list.clear();
    std::fclose(fp);
  }
  this->change_list.clear();
  // DEBUG
  // std::map<std::string, std::string>::iterator mapit =
  // this->error_page.begin(); for (; mapit != this->error_page.end(); ++mapit)
  // {
  //   std::cout << " [ " << mapit->first << " ] \n"
  //             << mapit->second << std::endl
  //             << std::endl;
  // }
}

void Server::setErrorPage(std::string error_code, Transaction *&transaction) {
  transaction->getResponse().setErrorMsg(error_code,
                                         this->error_page[error_code]);
}

//---- main loop --------------------------------------------------------------
void Server::run() {
  // std::cout << GRY << "Debug: Server: run\n" << DFT;
  std::vector<ServerSocket>::const_iterator it = this->server_socket.begin();

  for (; it != this->server_socket.end(); it++) {  // kevent 클리어
    setChangeList(this->change_list, it->getServerSocket(), EVFILT_READ,
                  EV_ADD | EV_ENABLE, 0, 0, NULL);
  }

  int new_events;
  struct kevent *curr_event;
  // error page
  while (1) {
    new_events = this->safeKevent(MAX_EVENT_SIZE, NULL);
    this->change_list.clear();
    for (int i = 0; i < new_events; i++) {
      curr_event = &(this->event_list[i]);

      if (curr_event->flags & EV_ERROR) {
        this->runErrorServer(curr_event);
      }
      // TODO : client가 일방적으로 연결을 끊으면 EV_EOF flag가 켜짐
      else if ((this->clients.find(curr_event->ident) != this->clients.end()) &&
               (curr_event->flags & EV_EOF)) {
        this->disconnectClient(curr_event->ident, this->clients);
      } else if (curr_event->filter == EVFILT_READ) {
        int client_socket = 0;
        std::vector<ServerSocket>::const_iterator it =
            this->server_socket.begin();
        for (; it != this->server_socket.end(); it++) {
          if (static_cast<int>(curr_event->ident) == it->getServerSocket()) {
            break;
          }
        }
        try {
          if (it != this->server_socket.end()) {
            this->runReadEventServer(client_socket, it);
          } else if (this->clients.find(curr_event->ident) !=
                     this->clients.end()) {
            this->runReadEventClient(curr_event);
          } else {
            this->runReadEventFile(curr_event);
            // TODO 우리가 처음 등록한 error page 인 경우 server 에 있는
            // 버퍼에 담도록 설정해줘야 함.
          }
        } catch (std::exception &e) {
          // std::cout << e.what();
          // TODO error page 를 request 에 담기위함.
          // Server::error_handler(e, curr_event);  만들어서 에러처리 로직을
          // 분리
          // * 트랜잭션 과정에서 발생하는 에러처리
          // * 에러를 처리하는 과정에서 발생하는 에러처리
          //  에러 시작줄세팅 HTTP 1.1 / 404 / Not Found
          //  에러 헤더 세팅
          //  에러 내용 세팅 => map에 들어있어서 error[404]
          // HTTP/1.1 404 Not Found\r\n
          // Content-Type: text/html\r\n
          // Content-Length:
          // \r\n
          if (curr_event->udata) {  // 파일일 때
            setErrorPage(e.what(),
                         reinterpret_cast<Transaction *&>(curr_event->udata));
            reinterpret_cast<Transaction *>(curr_event->udata)
                ->setFlag(RESPONSE_DONE);
            std::fclose(const_cast<FILE *>(
                (reinterpret_cast<Transaction *>(curr_event->udata))
                    ->getFilePtr()));
          } else {  // 클라이언트일 때
            setErrorPage(e.what(), this->clients[curr_event->ident]);
            this->clients[curr_event->ident]->setFlag(RESPONSE_DONE);
          }
        }
        // std::cerr << e.what() << std::endl;
        // error page를 위한 response msg를 setting해주고
        // RESPONSE_DONE flag를 켜줌

        // exception 상속받아서 404, 500, 501 등 처리하고,
        // 미리 정해두지 않은 에러의 경우 default error string 적용해주기
      } else if (curr_event->filter == EVFILT_WRITE) {
        this->runWriteEventClient(curr_event);
      }
    }
  }
}

void Server::runErrorServer(struct kevent *&curr_event) {
  std::vector<ServerSocket>::const_iterator it = this->server_socket.begin();
  // std::cout << GRY << "Debug: Server: runErrorServer\n" << DFT;
  for (; it != this->server_socket.end(); it++) {
    if (static_cast<int>(curr_event->ident) == it->getServerSocket()) {
      throw std::string("Error: Server: run: server socket error");
    }
  }
  if (it == this->server_socket.end()) {
    throw std::string("Error: Server: run: client socket error");
    this->disconnectClient(curr_event->ident, this->clients);
  }
}

void Server::runReadEventServer(int client_socket,
                                std::vector<ServerSocket>::const_iterator it) {
  // std::cout << GRY << "Debug: Server: runReadEventServer\n" << DFT;
  client_socket = it->safeAccept();
  std::cout << GRN << "accept new client: " << client_socket << DFT
            << std::endl;
  fcntl(client_socket, F_SETFL, O_NONBLOCK);
  setChangeList(this->change_list, client_socket, EVFILT_READ,
                EV_ADD | EV_ENABLE, 0, 0, NULL);
  setChangeList(this->change_list, client_socket, EVFILT_WRITE,
                EV_ADD | EV_ENABLE, 0, 0, NULL);
  // value 를 초기화하는 과정
  // TODO 같은 port 인 경우 처리해야 함
  std::vector<ServerConfig>::const_iterator it2 = this->server_config.begin();
  for (; it2 != this->server_config.end(); it2++) {
    if (it2->getListen() == it->getPort()) {
      this->clients[client_socket] =
          new Transaction(client_socket, *it2);  // flag = START
      break;
    }
  }
}

void Server::runReadEventClient(struct kevent *&curr_event) {
  // std::cout << GRY << "Debug: Server: runReadEventClient\n" << DFT;
  // executeRead() : request msg를 read & parsing
  int read_len = this->clients[curr_event->ident]->executeRead();
  if (read_len == -1) {
    this->disconnectClient(curr_event->ident, this->clients);
  }
  // 파일은 한번만 호출되야 한다
  // checkAllowMethod() : method의 유효성 검사
  // checkResource() : file open & return file fd
  if (this->clients[curr_event->ident]->getFlag() == REQUEST_DONE) {
    int file_fd = this->clients[curr_event->ident]->checkResource();
    if (file_fd == -1) {
      return;
    }
    // std::cout << "file_fd : " << file_fd << std::endl;
    // checkResource 내부에서 호출
    // this->clients[curr_event->ident]->checkAllowedMethod();
    fcntl(file_fd, F_SETFL, O_NONBLOCK);
    if (this->clients[curr_event->ident]->getFlag() == FILE_READ) {
      setChangeList(this->change_list, file_fd, EVFILT_READ,
                    EV_ADD | EV_ENABLE | EV_EOF, 0, 0,
                    this->clients[curr_event->ident]);
    } else if (this->clients[curr_event->ident]->getFlag() == FILE_WRITE) {
      setChangeList(this->change_list, file_fd, EVFILT_WRITE,
                    EV_ADD | EV_ENABLE | EV_EOF, 0, 0,
                    this->clients[curr_event->ident]);
    }
    // setChangeList(this->change_list, file_fd, EV_EOF, EV_ADD | EV_ENABLE,
    //               0, 0, this->clients[curr_event->ident]);
  }
}

void Server::runReadEventFile(struct kevent *&curr_event) {
  // std::cout << GRY << "Debug: Server: runReadEventFile\n" << DFT;
  // TODO file_fd를 vector로 관리할지는 추후 논의 필요.
  // udata에 있는 Transaction의 method를 호출한다.
  // executeMethod() : FILE_DONE -> RESPONSE_DONE (file을 읽고, method
  // 호출해서, response msg 완성)
  Transaction *curr_transaction =
      reinterpret_cast<Transaction *>(curr_event->udata);

  if (curr_transaction->getFlag() == FILE_READ) {
    curr_transaction->executeMethod(static_cast<int>(curr_event->data),
                                    curr_event->ident);
  }
  if (curr_transaction->getFlag() == FILE_DONE) {
    // std::cout << GRY << "Debug: Server: runReadEventFile\n" << DFT;
    //  TODO delete 할 때 udata 를 NULL 로 둬도 될까...?
    setChangeList(this->change_list, curr_event->ident, EVFILT_READ, EV_DELETE,
                  0, 0, NULL);
  }
}

void Server::runWriteEventClient(struct kevent *&curr_event) {
  std::map<int, Transaction *>::iterator it = clients.find(curr_event->ident);
  // std::cout << GRY << "Debug: Server: runWriteEventClient\n" << DFT;
  // 3-1. client에서 이벤트 발생
  //  response 를 완료한 client 인 경우
  if (curr_event->udata &&
      reinterpret_cast<Transaction *>(curr_event->udata)->getFlag() ==
          FILE_WRITE) {
    reinterpret_cast<Transaction *>(curr_event->udata)
        ->executeMethod(static_cast<int>(curr_event->data), curr_event->ident);
  } else if (it != clients.end()) {
    // 응답시간이 너무 길어질 때의 처리도 필요하다.
    if (it->second->getFlag() == RESPONSE_DONE) {
      if (this->clients[curr_event->ident]->executeWrite() == -1) {
        this->disconnectClient(curr_event->ident, this->clients);
      }
    } else if (it->second->getFlag() == END) {
      this->disconnectClient(curr_event->ident, this->clients);
    }
    // 어떤 조건이었지??? => 아마 keepalive를 위해 사용되는 조건들
    // if (write() == -1) {
    //   this->disconnectClient(curr_event->ident, this->clients);
    // } else {
    //   this->clients[curr_event->ident]->getRequest().clearSetRawMsg();
    // }
  }
}

//----- utils -----------------------------------------------------------------
void Server::setChangeList(std::vector<struct kevent> &change_list,
                           uintptr_t ident, int16_t filter, uint16_t flags,
                           uint32_t fflags, intptr_t data, void *udata) {
  // std::cout << GRY << "Debug: Server: setChangeList\n" << DFT;
  struct kevent temp_event;

  EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
  change_list.push_back(temp_event);
}

void Server::disconnectClient(int client_fd,
                              std::map<int, Transaction *> &clients) {
  // std::cout << GRY << "Debug: Server: disconnectClient\n" << DFT;
  delete clients[client_fd];
  close(client_fd);
  clients.erase(client_fd);
  std::cout << RED << "client disconnected: " << client_fd << DFT << std::endl;
}

//----- safe_method ------------------------------------------------------------
int Server::safeKevent(int nevents, const timespec *timeout) {
  // std::cout << GRY << "Debug: Server: safeKevent\n" << DFT;
  int new_events;

  if ((new_events =
           kevent(this->kq, &(this->change_list[0]), this->change_list.size(),
                  this->event_list, nevents, timeout)) == -1) {
    throw std::string("Error: Server: safeKevent\n" +
                      std::string(strerror(errno)));
  }
  return new_events;
}
