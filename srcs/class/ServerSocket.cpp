#include "ServerSocket.hpp"

//---- constructor ----------------------------------------------
ServerSocket::ServerSocket(sa_family_t address_family, int port) {
  std::memset(&(this->server_addr), 0, sizeof(server_addr));
  this->server_addr.sin_family = address_family;
  // htonl(INADDR_ANY) 는 주소를 지정해 주는 것으로 inet_addr( "내 시스템의 IP
  // ")로도 지정할 수 있습니다. 그러나 프로그램이 실행되는 시스템 마다 IP 가
  // 다를 것이므로 주소 지정을 고정 IP로 하지 않고 htonl( INADDR_ANY) 를
  // 사용하는 것이 편리합니다.
  this->port = port;

  this->server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  this->server_addr.sin_port = htons(port);
  this->safeSocket(address_family, SOCK_STREAM, 0);
  this->safeBind();
  this->safeListen(2);  // backlog : 대기열의 길이
  this->setNonBlock(this->server_socket);

  std::cout << "Construct ServerSocket: Port number: "
            << this->server_addr.sin_port << std::endl;
  // std::cout << GRY << "Debug: ServerSocket::ServerSocket\n" << DFT;
}

int ServerSocket::getPort() const { return this->port; }

//---- safe function ----------------------------------------------
void ServerSocket::safeSocket(int domain, int type, int protocol) {
  if ((this->server_socket = socket(domain, type, protocol)) == -1) {
    throw std::string("socket() error\n" + std::string(strerror(errno)));
  }
  // std::cout << GRY << "Debug: ServerSocket::safeSocket\n" << DFT;
}

void ServerSocket::safeBind(void) {
  if (bind(this->server_socket,
           reinterpret_cast<struct sockaddr *>(&this->server_addr),
           sizeof(this->server_addr)) == -1) {
    std::cout << "socket fd : " << this->port << std::endl;
    throw std::string("bind() error\n" + std::string(strerror(errno)));
  }
  // std::cout << GRY << "Debug: ServerSocket::safeBind\n" << DFT;
}

void ServerSocket::safeListen(int backlog) {
  if (listen(this->server_socket, backlog) == -1) {
    throw std::string("listen() error\n" + std::string(strerror(errno)));
  }
  // std::cout << GRY << "Debug: ServerSocket::safeListen\n" << DFT;
}

void ServerSocket::setNonBlock(int socket_fd) {
  if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1) {
    throw std::string("fcntl() error\n" + std::string(strerror(errno)));
  }
  // std::cout << GRY << "Debug: ServerSocket::setNonBlock\n" << DFT;
}

int ServerSocket::safeAccept(void) const {
  int client_socket;

  if ((client_socket = accept(this->server_socket, NULL, NULL)) == -1) {
    throw std::string("accept() error\n" + std::string(strerror(errno)));
  }
  // std::cout << GRY << "Debug: ServerSocket::safeAccept\n" << DFT;

  return client_socket;
}

int ServerSocket::getServerSocket() const { return this->server_socket; }
