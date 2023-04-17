#include "Server.hpp"

// Server 생성자에서 소캣 연결을 하는게 좋을지 아니면 따로 할지 고민중
Server::Server(ServerSocket &server_socket) {
  if ((this->kq = kqueue()) == -1) {
    throw std::string("socket() error\n" + std::string(strerror(errno)));
  }
  this->server_socket = &server_socket;
  std::cout << GRY << "Debug: Server\n" << DFT;
}

void Server::setChangeList(std::vector<struct kevent> &change_list,
                           uintptr_t ident, int16_t filter, uint16_t flags,
                           uint32_t fflags, intptr_t data, void *udata) {
  struct kevent temp_event;

  EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
  change_list.push_back(temp_event);
  std::cout << GRY << "Debug: Server::setChangeList\n" << DFT;
}

void Server::disconnectClient(int client_fd,
                              std::map<int, Transaction *> &clients) {
  delete clients[client_fd];
  // system("leaks webserv | grep total");
  close(client_fd);
  clients.erase(client_fd);
  std::cout << RED << "client disconnected: " << client_fd << DFT << std::endl;
  std::cout << GRY << "Debug: Server::disconnectClient\n" << DFT;
}

// ---- safe functions
int Server::safeKevent(int nevents, const timespec *timeout) {
  int new_events;

  if ((new_events =
           kevent(this->kq, &(this->change_list[0]), this->change_list.size(),
                  this->event_list, nevents, timeout)) == -1) {
    throw std::string("kevent() error\n" + std::string(strerror(errno)));
  }
  std::cout << GRY << "Debug: Server::safeKevent\n" << DFT;

  return new_events;
}

//---- main loop
void Server::run() {
  setChangeList(this->change_list, this->server_socket->getServerSocket(),
                EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

  int new_events;
  struct kevent *curr_event;

  while (42) {
    new_events = safeKevent(8, NULL);

    this->change_list.clear();
    for (int i = 0; i < new_events; i++) {
      curr_event = &(this->event_list[i]);

      /* 이벤트를 감지해서 filter에 맞게 조건문으로 분기 */

      // 1. 들어온 신호가 error일 경우
      if (curr_event->flags & EV_ERROR) {
        if (curr_event->ident == this->server_socket->getServerSocket()) {
          throw std::string("server socket error");
        } else {
          throw std::string("client socket error");
          this->disconnectClient(curr_event->ident, this->clients);
        }
      }
      // 2. 감지된 이벤트가 read일 경우
      else if (curr_event->filter == EVFILT_READ) {
        // 2-1. server 에게 connect 요청이 온 경우 => accept()
        if (curr_event->ident == this->server_socket->getServerSocket()) {
          int client_socket;

          client_socket = this->server_socket->safeAccept();
          std::cout << GRN << "accept new client: " << client_socket << DFT
                    << std::endl;
          fcntl(client_socket, F_SETFL, O_NONBLOCK);

          setChangeList(this->change_list, client_socket, EVFILT_READ,
                        EV_ADD | EV_ENABLE, 0, 0, NULL);
          setChangeList(this->change_list, client_socket, EVFILT_WRITE,
                        EV_ADD | EV_ENABLE, 0, 0, NULL);
          // value 를 초기화하는 과정
          this->clients[client_socket] =
              new Transaction(client_socket, "./rootdir/test");
        }
        // 2-2. 이벤트가 발생한 client가 이미 연결된 client인 경우 => read()
        else if (this->clients.find(curr_event->ident) != this->clients.end()) {
          if (this->clients[curr_event->ident]->executeRead() == -1) {
            this->disconnectClient(curr_event->ident, this->clients);
          } else {
            // executeMethod() 안에서 executeRead 완료했는지 체크하고 있음
            this->clients[curr_event->ident]->executeMethod();
          }
        }
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
  std::cout << GRY << "Debug: Server::run\n" << DFT;
}
