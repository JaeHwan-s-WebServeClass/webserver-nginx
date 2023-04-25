#include "Server.hpp"


// ---- constructors

Server::Server(std::vector<ServerConfig> & server_config): server_config(server_config) {
    std::vector<ServerConfig>::const_iterator it = this->server_config.begin();
    // TODO 같은 포트 여러개 들어올 때 예외처리
    for(; it != this->server_config.end(); it++) {
      ServerSocket tmp_socket(AF_INET, (*it).getListen());
      this->server_socket.push_back(tmp_socket);
    }
  if ((this->kq = kqueue()) == -1) {
    throw std::string("Error: Server: constructor\n" + std::string(strerror(errno)));
  }
  // std::cout << GRY << "Debug: Server\n" << DFT;
}

// ---- utils
void Server::setChangeList(std::vector<struct kevent> &change_list,
                           uintptr_t ident, int16_t filter, uint16_t flags,
                           uint32_t fflags, intptr_t data, void *udata) {
  struct kevent temp_event;

  EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
  change_list.push_back(temp_event);
  // std::cout << GRY << "Debug: Server::setChangeList\n" << DFT;
}

void Server::disconnectClient(int client_fd,
                              std::map<int, Transaction *> &clients) {
  delete clients[client_fd];
  // system("leaks webserv | grep total");
  close(client_fd);
  clients.erase(client_fd);
  std::cout << RED << "client disconnected: " << client_fd << DFT << std::endl;
  // std::cout << GRY << "Debug: Server::disconnectClient\n" << DFT;
}

// ---- safe functions
int Server::safeKevent(int nevents, const timespec *timeout) {
  int new_events;

  if ((new_events =
           kevent(this->kq, &(this->change_list[0]), this->change_list.size(),
                  this->event_list, nevents, timeout)) == -1) {
    throw std::string("Error: Server: safeKevent\n" + std::string(strerror(errno)));
  }
  // std::cout << GRY << "Debug: Server::safeKevent\n" << DFT;

  return new_events;
}

//---- main loop
void Server::run() {
  std::vector<ServerSocket>::const_iterator it = this->server_socket.begin();
  for (; it != this->server_socket.end(); it++) {
    //  FIXME: udata 추가하기
    setChangeList(this->change_list, it->getServerSocket(), EVFILT_READ,
                  EV_ADD | EV_ENABLE, 0, 0, NULL);
  }

// const int x = 10; // 변수를 선언하고 초기화합니다.
// const int& ref1 = x; // 참조를 선언하고 초기화합니다.
// int& ref2 = const_cast<int&>(ref1); // const-ness를 제거합니다.

  int new_events;
  struct kevent *curr_event;

  while (1) {
    new_events = safeKevent(8, NULL);
    this->change_list.clear();
    for (int i = 0; i < new_events; i++) {
      curr_event = &(this->event_list[i]);

      /* 이벤트를 감지해서 filter에 맞게 조건문으로 분기 */

      // 1. 들어온 신호가 error일 경우
      if (curr_event->flags & EV_ERROR) {
        std::vector<ServerSocket>::const_iterator it =
            this->server_socket.begin();
        for (; it != this->server_socket.end(); it++) {
          if (curr_event->ident == it->getServerSocket()) {
            throw std::string("Error: Server: run: server socket error");
          }
        }
        if (it == this->server_socket.end()) {
          throw std::string("Error: Server: run: client socket error");
          this->disconnectClient(curr_event->ident, this->clients);
        }
      }
      // 2. 감지된 이벤트가 read일 경우
      else if (curr_event->filter == EVFILT_READ) {
        // 2-1. server 에게 connect 요청이 온 경우 => accept()
        int client_socket;
        std::vector<ServerSocket>::const_iterator it =
            this->server_socket.begin();
        for (; it != this->server_socket.end(); it++) {
          if (curr_event->ident == it->getServerSocket()) {
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
                 new Transaction(client_socket, *it2);
                break;
              }
            }
            break;
          }
        }
        if (this->clients.find(curr_event->ident) != this->clients.end()) {
          if (this->clients[curr_event->ident]->executeRead() == -1) {
            this->disconnectClient(curr_event->ident, this->clients);
          } else {
            // executeMethod() 안에서 executeRead 완료했는지 체크하고 있음
            this->clients[curr_event->ident]->executeMethod();
            // file_fd = this->clients[curr_event->ident]->executeMethod();
            // setChangeList(file_fd)??
          }
        }
        // 2-2. 이벤트가 발생한 client가 이미 연결된 client인 경우 => read()
      }
      // 3. 감지된 이벤트가 write일 경우 => write()
      else if (curr_event->filter == EVFILT_WRITE) {
        std::map<int, Transaction *>::iterator it =
            clients.find(curr_event->ident);

        // 3-1. client에서 이벤트 발생
        if (it != clients.end()) {
          // 응답시간이 너무 길어질 때의 처리도 필요하다.
          if (this->clients[curr_event->ident]->executeWrite() == -1) {
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
    }
  }
  // std::cout << GRY << "Debug: Server::run\n" << DFT;
}
