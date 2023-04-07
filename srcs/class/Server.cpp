#include "Server.hpp"

// Server 생성자에서 소캣 연결을 하는게 좋을지 아니면 따로 할지 고민중
Server::Server(void) {
  if ((this->kq = kqueue()) == -1) {
    throw std::string("socket() error\n" + std::string(strerror(errno)));
  }
}

void Server::setChangeList(std::vector<struct kevent> &change_list,
                           uintptr_t ident, int16_t filter, uint16_t flags,
                           uint32_t fflags, intptr_t data, void *udata) {
  struct kevent temp_event;

  EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
  change_list.push_back(temp_event);
}

int Server::newEvents(int kq, const struct kevent *changelist, int nchanges,
                      struct kevent *eventlist, int nevents,
                      const timespec *timeout) {}

void Server::run() {
  setChangeList(this->change_list, this->server_socket->getServerSocket(),
                EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
  int new_events;
  while (42) {
    new_events = kevent(this->kq, &(this->change_list[0]),
                        this->change_list.size(), this->event_list, 8, NULL);
    if (new_events == -1) {
                            throw std::string("kevent() error\n" + std::string(strerror(errno));
    }
    this->change_list.clear();
    }
}
