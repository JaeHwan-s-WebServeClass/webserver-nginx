#include "cpp_echo_server.hpp"

//-------------------------------------------------------------------------------------//

void exit_with_perror(const std::string &msg) {
  std::cerr << msg << std::endl;
  exit(EXIT_FAILURE);
}

void change_events(std::vector<struct kevent> &change_list, uintptr_t ident,
                   int16_t filter, uint16_t flags, uint32_t fflags,
                   intptr_t data, void *udata) {
  struct kevent temp_event;

  EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
  change_list.push_back(temp_event);
}

void disconnect_client(int client_fd, std::map<int, std::string> &clients) {
  std::cout << "client disconnected: " << client_fd << std::endl;
  close(client_fd);
  clients.erase(client_fd);
}

//-------------------------------------------------------------------------------------//

int main(void) {
  // init server socket and listen
  int server_socket;
  struct sockaddr_in server_addr;

  if ((server_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    exit_with_perror("socket() error\n" + std::string(strerror(errno)));
  }
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  // set 0.0.0.0 ( #define INADDR_ANY (u_int32_t)0x00000000 )
  server_addr.sin_addr.s_addr =
      htonl(INADDR_ANY);  // typedef __uint32_t	in_addr_t;	/* base type for
                          // internet address */
  server_addr.sin_port = htons(8080);  // typedef __uint16_t	in_port_t;

  // 소켓을 연결
  if (bind(server_socket, reinterpret_cast<struct sockaddr *>(&server_addr),
           sizeof(server_addr)) == -1) {
    exit_with_perror("bind error\n" + std::string(strerror(errno)));
  }
  if (listen(server_socket, 2) == -1) {
    exit_with_perror("listen error\n" + std::string(strerror(errno)));
  }
  fcntl(server_socket, F_SETFL, O_NONBLOCK);

  /* init kqueue */
  int kq;
  if ((kq = kqueue()) == -1) {
    exit_with_perror("kqueue error\n" + std::string(strerror(errno)));
  }

  /* init client map */
  std::map<int, std::string> clients;

  /* kevent list 등록? */
  // 이벤트 모음집 (우리가 쓸거 전부 넣어놓기)
  std::vector<struct kevent> change_list;
  struct kevent event_list[8];  // kevent 배열

  /* add events for server socket */
  change_events(change_list, server_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0,
                0, NULL);
  std::cout << "server started!!!" << std::endl;

  /* main loop */
  int new_events;
  struct kevent *curr_event;

  while (1) {
    // apply changes and return new events (pending events)
    // change_list 에 있는 이벤트를 kq 에 등록함.
	// kq 에 등록된 이벤트는 삭제하지 않는 이상 안 없어진다.
    new_events =
        kevent(kq, &change_list[0], change_list.size(), event_list, 8, NULL);
    if (new_events == -1) {
      exit_with_perror("kevent() error\n" + std::string(strerror(errno)));
    }
    // clear change_list for new changes
    // change_list 에 같은 이벤트가 쌓이면 에러 상황이 생길 수 있기 때문에
    // 비어준다.
    change_list.clear();
    // kevent 함수를 통해 감지한 이벤트들을 처리
    for (int i = 0; i < new_events; i++) {
      curr_event = &event_list[i];

      // check error event return
      if (curr_event->flags & EV_ERROR) {
        if (curr_event->ident == server_socket) {
          exit_with_perror("server socket error");
        } else {
          std::cerr << "client socket error" << std::endl;
          disconnect_client(curr_event->ident, clients);
        }
      } else if (curr_event->filter == EVFILT_READ) {
        if (curr_event->ident ==
            server_socket) {  // server 한테 connect 요청이 온 경우. why? 61줄에
                              // server_socket 으로 ident 를 초기화한다.
          /* accept new client */
          int client_socket;
          if ((client_socket = accept(server_socket, NULL, NULL)) == -1) {
            exit_with_perror("accept() error\n" + std::string(strerror(errno)));
          }
          std::cout << "accept new client: " << client_socket << std::endl;
          fcntl(client_socket, F_SETFL, O_NONBLOCK);

          /* add event for client_socket */
          change_events(change_list, client_socket, EVFILT_READ,
                        EV_ADD | EV_ENABLE, 0, 0, NULL);
          change_events(change_list, client_socket, EVFILT_WRITE,
                        EV_ADD | EV_ENABLE, 0, 0, NULL);
          clients[client_socket] = "";
        } else if (clients.find(curr_event->ident) !=
                   clients.end()) {  // 이벤트가 발생한 client가 연결된 client
                                     // 목록에 있는 경우
          /* read data from client */
          char buf[100];
          int read_len = read(curr_event->ident, buf, sizeof(buf));

          if (read_len <= 0) {
            if (read_len < 0) {
              std::cerr << "client read error!" << std::endl;
            }
            disconnect_client(curr_event->ident, clients);
          } else {
            buf[read_len] = '\0';
            clients[curr_event->ident] += buf;
            std::cout << "received data from " << curr_event->ident << ": "
                      << clients[curr_event->ident] << std::endl;
          }
        }
      } else if (curr_event->filter == EVFILT_WRITE) {
        /* send data to client */
        std::map<int, std::string>::iterator it =
            clients.find(curr_event->ident);
        if (it != clients.end()) {
          if (clients[curr_event->ident] != "") {  // line 114 참고
            int write_len;
            if ((write_len = write(curr_event->ident,
                                   clients[curr_event->ident].c_str(),
                                   clients[curr_event->ident].size())) == -1) {
              std::cerr << "client write error!" << std::endl;
              disconnect_client(curr_event->ident, clients);
            } else {
              clients[curr_event->ident].clear();
            }
          }
        }
      }
    }
  }
  return (0);
}

/*
int	kevent(int kq, const struct kevent	*changelist, int nchanges,
                        struct	kevent *eventlist, int nevents,
                        const struct timespec *timeout);

changelist 인수는 <sys/event.h>에 정의된 대로 이벤트 구조체 배열에 대한
포인터입니다. changelist에 포함된 모든 변경 사항은 대기열에서 보류 중인 이벤트를
읽기 전에 적용됩니다. nchanges 인수는 변경 목록의 크기를 지정합니다.

eventlist 인수는 이벤트 구조체 배열에 대한 포인터입니다.
nevents 인수는 이벤트 목록의 크기를 결정합니다.
nevents가 0이면 select(2)와 달리 시간 제한이 지정되어 있어도 kevent()가 즉시
반환됩니다.

struct kevent {
        uintptr_t	ident;	identifier for this event
        short     filter;	filter for event
        u_short   flags;	action flags for kqueue
        u_int     fflags;	filter flag value
        int64_t   data;		filter data value
        void      *udata;	opaque user data identifier
        uint64_t  ext[4];	extensions
     }
*/