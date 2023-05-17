#include "Server.hpp"

extern int g_server_status;

//---- OCCF -------------------------------------------------------------------
Server::Server(std::vector<ServerConfig> &server_config)
    : server_config(server_config), disconnect(false) {
  std::vector<ServerConfig>::const_iterator it = this->server_config.begin();
  for (; it != this->server_config.end(); it++) {
    ServerSocket tmp_socket(AF_INET, (*it).getListen());
    this->server_socket.push_back(tmp_socket);
  }
  if ((this->kq = kqueue()) == -1) {
    g_server_status = UNHEALTHY;
    throw std::string("Error: Server: constructor: kqueue()");
  }
}
Server::Server(const Server &ref) : server_config(ref.server_config) {
  *this = ref;
}
Server &Server::operator=(const Server &ref) {
  this->clients = ref.clients;
  this->kq = ref.kq;
  for (int i = 0; i < MAX_EVENT_SIZE; i++) {
    this->event_list[i] = ref.event_list[i];
  }
  this->change_list = ref.change_list;
  this->server_socket = ref.server_socket;
  this->error_page = ref.error_page;
  return *this;
}
Server::~Server() {}

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
    int fd =
        ft::safeOpen('.' + rootdir + "/error_page" + filename, O_RDONLY, 0644);
    fcntl(fd, F_SETFL, O_NONBLOCK);
    this->setChangeList(this->change_list, fd, EVFILT_READ, EV_ADD | EV_ENABLE,
                        0, 0, NULL);
    this->safeKevent(1, NULL);
    this->change_list.clear();

    char buf[2048];
    if (this->event_list[0].filter == EVFILT_READ) {
      size_t read_len = ft::safeRead(fd, buf, 2048);
      buf[read_len] = '\0';
    }
    this->error_page[key] = buf;
    ft::safeClose(fd);
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

void Server::clearFileDescriptor(int client_fd) {
  int file_fd = this->clients[client_fd]->getFileDescriptor();

  if (ft::isFileDescriptorValid(file_fd)) {
    if (this->clients.find(file_fd) == this->clients.end()) {
      ft::safeClose(file_fd);
    }
    if (this->clients[client_fd]->getFlag() == FILE_WRITE) {
      this->setChangeList(this->change_list, file_fd, EVFILT_WRITE, EV_DELETE,
                          0, 0, NULL);
    } else {
      this->setChangeList(this->change_list, file_fd, EVFILT_READ, EV_DELETE, 0,
                          0, NULL);
    }
  }
}

//---- main loop
//--------------------------------------------------------------
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
    try {
      new_events = this->safeKevent(MAX_EVENT_SIZE, NULL);
    } catch (std::exception &e) {
      std::map<int, Transaction *>::iterator it = this->clients.begin();
      for (; it != this->clients.end(); ++it) {
        this->clearFileDescriptor(it->first);
        setErrorPage(e.what(), it->second);
        it->second->setFlag(RESPONSE_DONE);
      }
    }
    this->change_list.clear();
    for (int i = 0; i < new_events; i++) {
      curr_event = &(this->event_list[i]);

      try {
        if (g_server_status == UNHEALTHY) {
          throw ServerUnhealthyException();
        }
        if (curr_event->flags & EV_ERROR) {
          this->runErrorServer(curr_event);
        } else if ((this->clients.find(curr_event->ident) !=
                    this->clients.end()) &&
                   (curr_event->flags & (EV_EOF)) &&
                   curr_event->filter == EVFILT_READ) {
          this->disconnectClient(curr_event->ident);
        } else if (curr_event->filter == EVFILT_TIMER) {
          runTimerEventClient(curr_event);
        } else if (curr_event->filter == EVFILT_READ) {
          std::vector<ServerSocket>::const_iterator it =
              this->server_socket.begin();
          for (; it != this->server_socket.end(); it++) {
            if (static_cast<int>(curr_event->ident) == it->getServerSocket()) {
              break;
            }
          }
          if (it != this->server_socket.end()) {
            this->runReadEventServer(it);
          } else if (this->clients.find(curr_event->ident) !=
                     this->clients.end()) {
            this->runReadEventClient(curr_event);
          } else if (curr_event->udata) {
            this->runReadEventFile(curr_event);
          }
        } else if (curr_event->filter == EVFILT_WRITE) {
          if (curr_event->udata) {
            this->runWriteEventFile(curr_event);
          } else {
            this->runWriteEventClient(curr_event);
          }
        }
      } catch (ServerUnhealthyException &e) {
        std::map<int, Transaction *>::iterator it = this->clients.begin();
        for (; it != this->clients.end(); it++) {
          this->disconnectClient(it->first);
        }
        this->clients.clear();
        break;
      } catch (std::exception &e) {
        if (curr_event->udata) { // 파일일 때
          setErrorPage(e.what(),
                       reinterpret_cast<Transaction *&>(curr_event->udata));
          reinterpret_cast<Transaction *>(curr_event->udata)
              ->setFlag(RESPONSE_DONE);
          ft::safeClose(curr_event->ident);
          this->setChangeList(this->change_list, curr_event->ident,
                              curr_event->filter, EV_DELETE, 0, 0, NULL);
        } else { // 클라이언트일 때
          setErrorPage(e.what(), this->clients[curr_event->ident]);
          this->clients[curr_event->ident]->setFlag(RESPONSE_DONE);
        }
      }
      if (this->disconnect == true) {
        this->disconnect = false;
        break;
      }
    }
  }
}

void Server::runErrorServer(struct kevent *&curr_event) {
  // std::cout << GRY << "Debug: Server: runErrorServer\n" << DFT;
  // DEBUG
  // std::cout << RED << "error: " << strerror(curr_event->data)
  // << " errno: " << errno << std::endl << DFT;
  std::vector<ServerSocket>::const_iterator it = this->server_socket.begin();
  for (; it != this->server_socket.end(); it++) {
    if (static_cast<int>(curr_event->ident) == it->getServerSocket()) {
      ft::printError("Error: Server: runErrorServer: server socket error");
      throw Transaction::ErrorPage500Exception();
    }
  }
  if (it == this->server_socket.end()) {
    if (curr_event->udata) {
      ft::printError("Error: Server: runErrorServer: file error");
      throw Transaction::ErrorPage500Exception();
    } else if (this->clients.find(curr_event->ident) != this->clients.end()) {
      this->disconnectClient(curr_event->ident);
      ft::printError("Error: Server: runErrorServer: client socket error");
      throw Transaction::ErrorPage500Exception();
    }
  }
}

void Server::runTimerEventClient(struct kevent *&curr_event) {
  if (curr_event->ident == 0) { // cgi timer
    kill(reinterpret_cast<intptr_t>(curr_event->udata), SIGTERM);
  } else {
    Transaction *tmp_transaction =
        this->clients.find(curr_event->ident) == this->clients.end()
            ? 0
            : this->clients[curr_event->ident];

    if (tmp_transaction == curr_event->udata) {
      if (tmp_transaction->getFlag() < REQUEST_DONE) {
        curr_event->udata = 0;
        throw Transaction::ErrorPage408Exception();
      }
    }
  }
}

void Server::runReadEventServer(std::vector<ServerSocket>::const_iterator it) {
  // std::cout << GRY << "Debug: Server: runReadEventServer\n" << DFT;
  int client_socket;
  try {
    client_socket = it->safeAccept();
  } catch (std::exception &e) {
    return;
  }
  std::cout << GRN << "accept new client: " << client_socket << DFT
            << std::endl;
  fcntl(client_socket, F_SETFL, O_NONBLOCK);
  this->setChangeList(this->change_list, client_socket, EVFILT_READ,
                      EV_ADD | EV_ENABLE, 0, 0, NULL);
  this->setChangeList(this->change_list, client_socket, EVFILT_WRITE,
                      EV_ADD | EV_ENABLE, 0, 0, NULL);
  std::vector<ServerConfig>::const_iterator it2 = this->server_config.begin();
  for (; it2 != this->server_config.end(); it2++) {
    if (it2->getListen() == it->getPort()) {
      Transaction *new_transaction = new Transaction(client_socket, *it2);
      this->clients[client_socket] = new_transaction;
      this->setChangeList(this->change_list, client_socket, EVFILT_TIMER,
                          EV_ADD | EV_ENABLE | EV_ONESHOT, NOTE_SECONDS,
                          TIMEOUT, new_transaction);
      break;
    }
  }
  if (it2 == this->server_config.begin()) {
    this->clients[client_socket]->executeRedirect();
  }
}

void Server::runReadEventClient(struct kevent *&curr_event) {
  // std::cout << GRY << "Debug: Server: runReadEventClient\n" << DFT;
  Transaction *tmp_transaction = this->clients[curr_event->ident];
  tmp_transaction->executeRead();

  if (tmp_transaction->getFlag() == REQUEST_DONE) {
    int file_fd = tmp_transaction->executeResource();
    if ((file_fd == DIRECTORY) || (file_fd == EMPTY_FILE)) { // directory
      return;
    } else if (file_fd == NONE_FD) { // delete, put
      tmp_transaction->executeMethod(0, 0);
      return;
    }
    fcntl(file_fd, F_SETFL, O_NONBLOCK);
    if (tmp_transaction->getFlag() == FILE_READ) {
      this->setChangeList(this->change_list, file_fd, EVFILT_READ,
                          EV_ADD | EV_ENABLE | EV_EOF, 0, 0, tmp_transaction);
      if (tmp_transaction->getPid() != DEFALUT_CGIPID) {
        this->setChangeList(
            this->change_list, 0, EVFILT_TIMER, EV_ADD | EV_ENABLE | EV_ONESHOT,
            NOTE_SECONDS, TIMEOUT,
            reinterpret_cast<void *>(tmp_transaction->getPid()));
      }
    } else if (tmp_transaction->getFlag() == FILE_WRITE) {
      this->setChangeList(this->change_list, file_fd, EVFILT_WRITE,
                          EV_ADD | EV_ENABLE | EV_EOF, 0, 0, tmp_transaction);
    }
  }
}

void Server::runReadEventFile(struct kevent *&curr_event) {
  // std::cout << GRY << "Debug: Server: runReadEventFile\n" << DFT;
  Transaction *curr_transaction =
      reinterpret_cast<Transaction *>(curr_event->udata);

  if (curr_transaction->getFlag() == FILE_READ) {
    curr_transaction->executeMethod(static_cast<int>(curr_event->data),
                                    curr_event->ident);
    if (curr_transaction->getFlag() == FILE_CGI) {
      this->setChangeList(this->change_list, curr_event->ident, EVFILT_READ,
                          EV_DELETE, 0, 0, NULL);
      int file_fd = curr_transaction->executeResourceFile();

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
  std::map<int, Transaction *>::const_iterator it =
      clients.find(curr_event->ident);
  if (it != clients.end()) {
    if (it->second->getFlag() == RESPONSE_DONE) {
      if (this->clients[curr_event->ident]->executeWrite() == -1) {
        this->disconnectClient(curr_event->ident);
      }
    } else if (it->second->getFlag() == END) {
      this->disconnectClient(curr_event->ident);
    }
  }
}

//----- utils ----------------------------------------------------------------
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

void Server::disconnectClient(int client_fd) {
  // std::cout << GRY << "Debug: Server: disconnectClient\n" << DFT;
  this->clearFileDescriptor(client_fd);

  const std::map<std::string, std::string> &check_header =
      this->clients[client_fd]->getRequest().getHeader();
  std::map<std::string, std::string>::const_iterator check =
      check_header.find("Connection");
  if ((check != check_header.end()) && (check->second == "keep-alive")) {
    std::vector<ServerConfig>::const_iterator it = this->server_config.begin();
    for (; it != this->server_config.end(); ++it) {
      if (&(*it) == &this->clients[client_fd]->getServerConfig()) {
        break;
      }
    }
    delete this->clients[client_fd];
    this->clients[client_fd] = new Transaction(client_fd, *it);
  } else {
    this->setChangeList(this->change_list, client_fd, EVFILT_READ, EV_DELETE, 0,
                        0, NULL);
    this->setChangeList(this->change_list, client_fd, EVFILT_WRITE, EV_DELETE,
                        0, 0, NULL);
    delete this->clients[client_fd];
    ft::safeClose(client_fd);
    this->clients.erase(client_fd);
    this->disconnect = true;
    std::cout << RED << "client disconnected: " << client_fd << DFT
              << std::endl;
  }
}

//----- safe_method -----------------------------------------------------------
int Server::safeKevent(int nevents, const timespec *timeout) {
  // std::cout << GRY << "Debug: Server: safeKevent\n" << DFT;
  int new_events =
      kevent(this->kq, &(this->change_list[0]), this->change_list.size(),
             this->event_list, nevents, timeout);

  if (new_events == -1) {
    ft::printError("Error: Server: safeKevent");
    throw Transaction::ErrorPage500Exception();
  } else if (new_events == 0) {
    // std::cout << "new_event is 0\n";
  }
  return new_events;
}

const char *Server::ServerUnhealthyException::what() const throw() {
  return "500";
}