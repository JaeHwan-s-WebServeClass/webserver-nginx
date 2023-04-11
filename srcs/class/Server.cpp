#include "Server.hpp"
#include "ServerSocket.hpp"
#include "Request.hpp"

#include <sys/_types/_int16_t.h>
#include <sys/_types/_intptr_t.h>
#include <sys/event.h>
#include <sys/fcntl.h>

// Server 생성자에서 소캣 연결을 하는게 좋을지 아니면 따로 할지 고민중
Server::Server(ServerSocket &server_socket) {
  if ((this->kq = kqueue()) == -1) {
    throw std::string("socket() error\n" + std::string(strerror(errno)));
  }
  this->server_socket = &server_socket;
}

void Server::setChangeList(std::vector<struct kevent> &change_list,
                           uintptr_t ident, int16_t filter, uint16_t flags,
                           uint32_t fflags, intptr_t data, void *udata) {
  struct kevent temp_event;

  EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
  change_list.push_back(temp_event);
}

void Server::disconnectClient(int client_fd, std::map<int, Request *> &clients) {
  delete clients[client_fd];
  // system("leaks webserv | grep total");
  close(client_fd);
  clients.erase(client_fd);
  std::cout << RED << "client disconnected: " << client_fd << DFT << std::endl;
}

// ---- safe functions -----------------------------------------------------------------
int Server::safeKevent(int nevents, const timespec *timeout) {
    int new_events;
  
    if ((new_events = kevent(this->kq, &(this->change_list[0]),
                        this->change_list.size(), this->event_list, nevents, timeout))== -1) {
      throw std::string("kevent() error\n" + std::string(strerror(errno)));
    }
    return new_events;
}

int Server::safeRead(int fd, char *buf) {
  int   read_len;
    
  if ((read_len = read(fd, buf, BUFFER_SIZE)) == -1) {
    throw std::string("client read error!");
  }
  return read_len;
}

int Server::safeWrite(int fd) {
  int write_len;
  
  if ((write_len = write(fd, this->clients[fd]->getRawMsg().c_str(),
                         this->clients[fd]->getRawMsg().size())) == -1) {
    throw std::string("client write error!");
  }
  return write_len;
}

//---- main loop -----------------------------------------------------------------
void Server::run() {
  setChangeList(this->change_list, this->server_socket->getServerSocket(),
                EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
  
  int           new_events;
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
          int   client_socket;

          client_socket = this->server_socket->safeAccept();
          std::cout << GRN << "accept new client: " << client_socket << DFT << std::endl;
          fcntl(client_socket, F_SETFL, O_NONBLOCK);

          setChangeList(this->change_list, client_socket, EVFILT_READ,
                        EV_ADD | EV_ENABLE, 0, 0, NULL);
          setChangeList(this->change_list, client_socket, EVFILT_WRITE,
                        EV_ADD | EV_ENABLE, 0, 0, NULL);
          // value 를 초기화하는 과정
          this->clients[client_socket] = new Request();
        }
        // 2-2. 이벤트가 발생한 client가 이미 연결된 client인 경우 => read()
        else if (this->clients.find(curr_event->ident) != this->clients.end()) {
          char  buf[BUFFER_SIZE];
          int   read_len = safeRead(curr_event->ident, buf);

          if (read_len == 0) {
            this->disconnectClient(curr_event->ident, this->clients);
          } else {
            buf[read_len] = '\0';
            this->clients[curr_event->ident]->setRawMsg(buf);
            // this->clients[curr_event->ident]->parser();
            std::cout << GRY << "received data from " << curr_event->ident << ": " << DFT
                      << this->clients[curr_event->ident]->getRawMsg() << std::endl;
          }
        }
      }
      // 3. 감지된 이벤트가 write일 경우 => write()
      else if (curr_event->filter == EVFILT_WRITE) {
        std::map<int, Request*>::iterator it = clients.find(curr_event->ident);

        // 3-1. client에서 이벤트 발생
        if (it != clients.end()) {
          if (this->clients[curr_event->ident]->getRawMsg() != "") {
            //int write_len = safeWrite(curr_event->ident);
            int write_len = safeWrite(curr_event->ident);

            this->clients[curr_event->ident]->toString();
            
            if (write_len == -1)
            {
              this->disconnectClient(curr_event->ident, this->clients);
            }
            else
            {
              this->clients[curr_event->ident]->clearSetRawMsg();
            }
          }
        }
      }
    }
  }
}
