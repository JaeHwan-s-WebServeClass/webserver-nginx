#include "Server.hpp"
#include "Transaction.hpp"

//----- constructor ------------------------------------------------------------
Server::Server(std::vector<ServerConfig> &server_config)
    : server_config(server_config) {
  // std::cout << GRY << "Debug: Server\n" << DFT;
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

//---- main loop --------------------------------------------------------------
void Server::run() {
  // std::cout << GRY << "Debug: Server::run\n" << DFT;
  std::vector<ServerSocket>::const_iterator it = this->server_socket.begin();
  for (; it != this->server_socket.end(); it++) {
    //  FIXME: udata 추가하기
    setChangeList(this->change_list, it->getServerSocket(), EVFILT_READ,
                  EV_ADD | EV_ENABLE, 0, 0, NULL);
  }

  int new_events;
  struct kevent *curr_event;

  while (1) {
    new_events = this->safeKevent(8, NULL);
    this->change_list.clear();
    for (int i = 0; i < new_events; i++) {
      curr_event = &(this->event_list[i]);

      if (curr_event->flags & EV_ERROR) {
        this->runErrorServer(curr_event);
      } else if (curr_event->filter == EVFILT_READ) {
        if (curr_event->flags & EV_EOF) { // 소켓 연결이 끊어진 걸 감지하는 조건문.
          std::cout << "EV_EOF1\n";
        }
        std::cout << "DEBUG" << std::endl;
        int client_socket = 0;
        std::vector<ServerSocket>::const_iterator it =
            this->server_socket.begin();
        for (; it != this->server_socket.end(); it++) {
          if (static_cast<int>(curr_event->ident) == it->getServerSocket()) {
            break;
          }
        }
        if (it != this->server_socket.end()) {
          this->runReadEventServer(client_socket, it);
        } else if (this->clients.find(curr_event->ident) !=
                   this->clients.end()) {
          this->runReadEventClient(curr_event);
        } else {
          this->runReadEventFile(curr_event);
          if (curr_event->flags & EV_EOF) {
            std::cout << "EV_EOF2\n";
          }
        }
      } else if (curr_event->filter == EVFILT_WRITE) {
        this->runWriteEventClient(curr_event);
      }
    }
  }
}

void Server::runErrorServer(struct kevent *&curr_event) {
  std::vector<ServerSocket>::const_iterator it = this->server_socket.begin();
  // std::cout << GRY << "Debug: Server::runErrorServer\n" << DFT;
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
  // std::cout << GRY << "Debug: Server::runReadEventServer\n" << DFT;
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
          new Transaction(client_socket, *it2); // flag = START
      break;
    }
  }
}

void Server::runReadEventClient(struct kevent *&curr_event) {
  // std::cout << GRY << "Debug: Server::runReadEventClient\n" << DFT;
  // executeRead() : request msg를 read & parsing
  int read_len = this->clients[curr_event->ident]->executeRead();
  if (read_len == -1) {
    this->disconnectClient(curr_event->ident, this->clients);
  }
  // 파일은 한번만 호출되야 한다
  // checkAllowMethod() : method의 유효성 검사
  // checkResource() : file open & return file fd
  if (this->clients[curr_event->ident]->getFlag() == REQUEST_DONE) {
    this->clients[curr_event->ident]->checkAllowedMethod();
    // 위 함수 에서 501 셋팅 시 아래 함수 x?
    int file_fd = this->clients[curr_event->ident]->checkResource();
    fcntl(file_fd, F_SETFL, O_NONBLOCK);
    setChangeList(this->change_list, file_fd, EVFILT_READ,
                  EV_ADD | EV_ENABLE | EV_EOF, 0, 0,
                  this->clients[curr_event->ident]);
    // setChangeList(this->change_list, file_fd, EV_EOF, EV_ADD | EV_ENABLE,
    //               0, 0, this->clients[curr_event->ident]);
  }
}

void Server::runReadEventFile(struct kevent *&curr_event) {
  std::cout << GRY << "Debug: Server::runReadEventFile\n" << DFT;
  // TODO file_fd를 vector로 관리할지는 추후 논의 필요.
  // udata에 있는 Transaction의 method를 호출한다.
  // executeMethod() : FILE_DONE -> RESPONSE_DONE (file을 읽고, method
  // 호출해서, response msg 완성)
  Transaction *curr_transaction =
      reinterpret_cast<Transaction *>(curr_event->udata);

  if (curr_transaction->getFlag() == FILE_OPEN) {
    curr_transaction->executeMethod();
  }
  if (curr_transaction->getFlag() == FILE_DONE) {
    std::cout << GRY << "Debug: Server::runReadEventFile : FILE_DONE\n" << DFT;

    //  TODO delete 할 때 udata 를 NULL 로 둬도 될까...?
    setChangeList(this->change_list, curr_event->ident, EVFILT_READ, EV_DELETE,
                  0, 0, NULL);
  }
}

void Server::runWriteEventClient(struct kevent *&curr_event) {
  std::map<int, Transaction *>::iterator it = clients.find(curr_event->ident);
  // std::cout << GRY << "Debug: Server::runWriteEventClient\n" << DFT;
  // 3-1. client에서 이벤트 발생
  //  response 를 완료한 client 인 경우
  if (it != clients.end()) {
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
  } // TODO write 이벤트가 파일 일 때 else{}
}

//----- utils -----------------------------------------------------------------
void Server::setChangeList(std::vector<struct kevent> &change_list,
                           uintptr_t ident, int16_t filter, uint16_t flags,
                           uint32_t fflags, intptr_t data, void *udata) {
  // std::cout << GRY << "Debug: Server::setChangeList\n" << DFT;
  struct kevent temp_event;

  EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
  change_list.push_back(temp_event);
}

void Server::disconnectClient(int client_fd,
                              std::map<int, Transaction *> &clients) {
  // std::cout << GRY << "Debug: Server::disconnectClient\n" << DFT;
  delete clients[client_fd];
  close(client_fd);
  clients.erase(client_fd);
  std::cout << RED << "client disconnected: " << client_fd << DFT << std::endl;
}

//----- safe_method ------------------------------------------------------------
int Server::safeKevent(int nevents, const timespec *timeout) {
  // std::cout << GRY << "Debug: Server::safeKevent\n" << DFT;
  int new_events;

  if ((new_events =
           kevent(this->kq, &(this->change_list[0]), this->change_list.size(),
                  this->event_list, nevents, timeout)) == -1) {
    throw std::string("Error: Server: safeKevent\n" +
                      std::string(strerror(errno)));
  }
  return new_events;
}