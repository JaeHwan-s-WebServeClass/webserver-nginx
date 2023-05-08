#include "Server.hpp"

//----- constructor ------------------------------------------------------------
Server::Server(std::vector<ServerConfig> &server_config)
    : server_config(server_config) {
  // std::cout << GRY << "Debug: Server: constructor\n" << DFT;
  std::vector<ServerConfig>::const_iterator it = this->server_config.begin();
  // TODO 같은 포트 여러개 들어올 때 예외처리
  // server_name 으로 처리해야할 듯
  for (; it != this->server_config.end(); it++) {
    ServerSocket tmp_socket(AF_INET, (*it).getListen());
    this->server_socket.push_back(tmp_socket);
  }
  if ((this->kq = kqueue()) == -1) {
    ft::errorHandler("Error: Server: constructor: kqueue()");
    throw Transaction::ErrorPageDefaultException();
  }
}

//---- error page -------------------------------------------------------------
void Server::loadErrorPage() {
  ServerConfig temp_conf = this->server_config[0];
  std::vector<std::string> conf_error_page = temp_conf.getErrorPage();

  std::vector<std::string>::const_iterator it = conf_error_page.begin();

  for (; it != conf_error_page.end(); ++it) {
    std::string key = *it;
    it++;
    std::string filename = *it;
    std::string rootdir = temp_conf.getRoot();
    FILE *fp = ft::safeFopen(('.' + rootdir + filename).c_str(), "r");
    fcntl(fp->_file, F_SETFL, O_NONBLOCK);
    this->setChangeList(this->change_list, fp->_file, EVFILT_READ,
                        EV_ADD | EV_ENABLE, 0, 0, fp);
    this->safeKevent(1, NULL);
    this->change_list.clear();

    char buf[BUFFER_SIZE];
    if (this->event_list[0].filter == EVFILT_READ) {
      size_t read_len =
          ft::safeFread(buf, sizeof(char), BUFFER_SIZE,
                        reinterpret_cast<FILE *>(this->event_list[0].udata));
      buf[read_len] = '\0';
    }
    this->error_page[key] = buf;
    ft::safeFclose(fp);
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

  for (; it != this->server_socket.end(); it++) {
    this->setChangeList(this->change_list, it->getServerSocket(), EVFILT_READ,
                        EV_ADD | EV_ENABLE, 0, 0, NULL);
  }

  int new_events;
  struct kevent *curr_event;
  while (1) {
    new_events = this->safeKevent(MAX_EVENT_SIZE, NULL);
    this->change_list.clear();
    for (int i = 0; i < new_events; i++) {
      curr_event = &(this->event_list[i]);

      if (curr_event->flags & EV_ERROR) {
        this->runErrorServer(curr_event);
      } else if ((this->clients.find(curr_event->ident) !=
                  this->clients.end()) &&
                 (curr_event->flags & EV_EOF)) {
        this->disconnectClient(curr_event->ident, this->clients);
      } else if (curr_event->filter == EVFILT_READ) {
        std::vector<ServerSocket>::const_iterator it =
            this->server_socket.begin();
        for (; it != this->server_socket.end(); it++) {
          if (static_cast<int>(curr_event->ident) == it->getServerSocket()) {
            break;
          }
        }
        try {
          if (it != this->server_socket.end()) {
            this->runReadEventServer(it);
          } else if (this->clients.find(curr_event->ident) !=
                     this->clients.end()) {
            this->runReadEventClient(curr_event);
          } else {
            this->runReadEventFile(curr_event);
          }
        } catch (std::exception &e) {
          if (curr_event->udata) {  // 파일일 때
            setErrorPage(e.what(),
                         reinterpret_cast<Transaction *&>(curr_event->udata));
            reinterpret_cast<Transaction *>(curr_event->udata)
                ->setFlag(RESPONSE_DONE);
            ft::safeFclose(const_cast<FILE *>(
                (reinterpret_cast<Transaction *>(curr_event->udata))
                    ->getFilePtr()));
          } else {  // 클라이언트일 때
            setErrorPage(e.what(), this->clients[curr_event->ident]);
            this->clients[curr_event->ident]->setFlag(RESPONSE_DONE);
          }
        }
      } else if (curr_event->filter == EVFILT_WRITE) {
        if (curr_event->udata) {
          this->runWriteEventFile(curr_event);
        } else {
          this->runWriteEventClient(curr_event);
        }
      }
    }
  }
}

void Server::runErrorServer(struct kevent *&curr_event) {
  // std::cout << GRY << "Debug: Server: runErrorServer\n" << DFT;
  // DEBUG
  std::cout << RED << "error: " << strerror(curr_event->data) << std::endl
            << DFT;
  std::vector<ServerSocket>::const_iterator it = this->server_socket.begin();
  for (; it != this->server_socket.end(); it++) {
    if (static_cast<int>(curr_event->ident) == it->getServerSocket()) {
      ft::errorHandler("Error: Server: runErrorServer: server socket error");
      throw Transaction::ErrorPageDefaultException();
    }
  }
  if (it == this->server_socket.end()) {
    if (curr_event->udata) {
      ft::errorHandler("Error: Server: runErrorServer: file error");
      throw Transaction::ErrorPageDefaultException();
    } else {
      this->disconnectClient(curr_event->ident, this->clients);
      ft::errorHandler("Error: Server: runErrorServer: client socket error");
      throw Transaction::ErrorPageDefaultException();
    }
  }
}

void Server::runReadEventServer(std::vector<ServerSocket>::const_iterator it) {
  // std::cout << GRY << "Debug: Server: runReadEventServer\n" << DFT;
  int client_socket = it->safeAccept();
  std::cout << GRN << "accept new client: " << client_socket << DFT
            << std::endl;
  fcntl(client_socket, F_SETFL, O_NONBLOCK);
  this->setChangeList(this->change_list, client_socket, EVFILT_READ,
                      EV_ADD | EV_ENABLE, 0, 0, NULL);
  this->setChangeList(this->change_list, client_socket, EVFILT_WRITE,
                      EV_ADD | EV_ENABLE, 0, 0, NULL);
  // TODO 같은 port 인 경우 처리해야 함 (server_name)
  std::vector<ServerConfig>::const_iterator it2 = this->server_config.begin();
  for (; it2 != this->server_config.end(); it2++) {
    if (it2->getListen() == it->getPort()) {
      this->clients[client_socket] = new Transaction(client_socket, *it2);
      break;
    }
  }
}

void Server::runReadEventClient(struct kevent *&curr_event) {
  // std::cout << GRY << "Debug: Server: runReadEventClient\n" << DFT;
  if (this->clients[curr_event->ident]->executeRead() == -1) {
    this->disconnectClient(curr_event->ident, this->clients);
  }
  if (this->clients[curr_event->ident]->getFlag() == REQUEST_DONE) {
    int file_fd = this->clients[curr_event->ident]->executeResource();
    if (file_fd == -1) {
      return;
    } else if (file_fd == -2) {
      this->clients[curr_event->ident]->executeMethod(0, 0);
      return;
    }
    fcntl(file_fd, F_SETFL, O_NONBLOCK);
    if (this->clients[curr_event->ident]->getFlag() == FILE_READ) {
      this->setChangeList(this->change_list, file_fd, EVFILT_READ,
                          EV_ADD | EV_ENABLE | EV_EOF, 0, 0,
                          this->clients[curr_event->ident]);
    } else if (this->clients[curr_event->ident]->getFlag() == FILE_WRITE) {
      this->setChangeList(this->change_list, file_fd, EVFILT_WRITE,
                          EV_ADD | EV_ENABLE | EV_EOF, 0, 0,
                          this->clients[curr_event->ident]);
    }
  }
}

void Server::runReadEventFile(struct kevent *&curr_event) {
  // std::cout << GRY << "Debug: Server: runReadEventFile\n" << DFT;
  // TODO file_fd를 vector로 관리할지는 추후 논의 필요.
  // 여러개의 client가 동시에 혹은 진행중인 사이클 내에서 같은 file에 접근할
  // 경우 어떤 문제가 생길까..?
  // fopen 쓰지 말고 싹 다 open으로 변경...? 아마 파일마스터 승혜가.... 우와...
  Transaction *curr_transaction =
      reinterpret_cast<Transaction *>(curr_event->udata);

  if (curr_transaction->getFlag() == FILE_READ) {
    curr_transaction->executeMethod(static_cast<int>(curr_event->data),
                                    curr_event->ident);
    if (curr_transaction->getFlag() == FILE_CGI) {
      this->setChangeList(this->change_list, curr_event->ident, EVFILT_READ,
                          EV_DELETE, 0, 0, NULL);
      int file_fd = curr_transaction->checkFile();

      fcntl(file_fd, F_SETFL, O_NONBLOCK);
      this->setChangeList(this->change_list, file_fd, EVFILT_WRITE,
                          EV_ADD | EV_ENABLE | EV_EOF, 0, 0, curr_transaction);
    }
  }
  if (curr_transaction->getFlag() == RESPONSE_DONE) {
    this->setChangeList(this->change_list, curr_event->ident, EVFILT_READ,
                        EV_DELETE, 0, 0, NULL);
  }
}

void Server::runWriteEventFile(struct kevent *&curr_event) {
  // std::cout << GRY << "Debug: Server: runWriteEventFile\n" << DFT;
  Transaction *curr_transaction =
      reinterpret_cast<Transaction *>(curr_event->udata);

  if (curr_transaction->getFlag() == FILE_WRITE) {
    curr_transaction->executeMethod(static_cast<int>(curr_event->data),
                                    curr_event->ident);
  }

  if (curr_transaction->getFlag() == RESPONSE_DONE) {
    this->setChangeList(this->change_list, curr_event->ident, EVFILT_WRITE,
                        EV_DELETE, 0, 0, NULL);
  }
}

void Server::runWriteEventClient(struct kevent *&curr_event) {
  // std::cout << GRY << "Debug: Server: runWriteEventClient\n" << DFT;
  std::map<int, Transaction *>::iterator it = clients.find(curr_event->ident);
  if (it != clients.end()) {
    // TODO 응답시간이 너무 길어질 때의 처리도 필요하다.
    // request의 body를 받는 부분에서 같이 처리해야함
    if (it->second->getFlag() == RESPONSE_DONE) {
      if (this->clients[curr_event->ident]->executeWrite() == -1) {
        this->disconnectClient(curr_event->ident, this->clients);
      }
    } else if (it->second->getFlag() == END) {
      this->disconnectClient(curr_event->ident, this->clients);
    }
    // TODO 어떤 조건이었지??? => 아마 keepalive를 위해 사용되는 조건들
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
  if (flags == EV_DELETE) {
    kevent(this->kq, &temp_event, 1, this->event_list, 0, NULL);
    return;
  }
  change_list.push_back(temp_event);
}

void Server::disconnectClient(int client_fd,
                              std::map<int, Transaction *> &clients) {
  // std::cout << GRY << "Debug: Server: disconnectClient\n" << DFT;
  this->setChangeList(this->change_list, client_fd, EVFILT_READ, EV_DELETE, 0,
                      0, NULL);
  delete clients[client_fd];
  ft::safeClose(client_fd);
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
    ft::errorHandler("Error: Server: safeKevent");
    throw Transaction::ErrorPageDefaultException();
  }
  return new_events;
}