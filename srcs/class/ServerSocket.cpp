#include "ServerSocket.hpp"

//---- constructor -------------------------------------------------------------
ServerSocket::ServerSocket(sa_family_t address_family, int port) {
  // std::cout << GRY << "Debug: ServerSocket::ServerSocket\n" << DFT;
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

  // DEBUG: check server socket port number
  std::cout << "Construct ServerSocket: Port number: "
            << this->server_addr.sin_port << std::endl;
}

//---- getter -------------------------------------------------------------
int ServerSocket::getPort() const { return this->port; }
int ServerSocket::getServerSocket() const { return this->server_socket; }

//---- setter -------------------------------------------------------------
void ServerSocket::setNonBlock(int socket_fd) {
  // std::cout << GRY << "Debug: ServerSocket::setNonBlock\n" << DFT;
  if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1) {
    throw std::string("Error: ServerSocket: setNonBlock: fcntl error\n" +
                      std::string(strerror(errno)));
  }
}

//---- safe function ----------------------------------------------
void ServerSocket::safeSocket(int domain, int type, int protocol) {
  // std::cout << GRY << "Debug: ServerSocket::safeSocket\n" << DFT;
  if ((this->server_socket = socket(domain, type, protocol)) == -1) {
    throw std::string("Error: ServerSocket: safeSocket: socket() error\n" +
                      std::string(strerror(errno)));
  }
}

void ServerSocket::safeBind(void) {
  // std::cout << GRY << "Debug: ServerSocket::safeBind\n" << DFT;
  if (bind(this->server_socket,
           reinterpret_cast<struct sockaddr *>(&this->server_addr),
           sizeof(this->server_addr)) == -1) {
    // DEBUG
    std::cout << "socket fd : " << this->port << std::endl;

    throw std::string("Error: ServerSocket: safeBind: bind() error\n" +
                      std::string(strerror(errno)));
  }
}

void ServerSocket::safeListen(int backlog) {
  // std::cout << GRY << "Debug: ServerSocket::safeListen\n" << DFT;
  if (listen(this->server_socket, backlog) == -1) {
    throw std::string("Error: ServerSocket: safeListen: listen() error\n" +
                      std::string(strerror(errno)));
  }
}

int ServerSocket::safeAccept(void) const {
  // std::cout << GRY << "Debug: ServerSocket::safeAccept\n" << DFT;
  int client_socket;

  if ((client_socket = accept(this->server_socket, NULL, NULL)) == -1) {
    throw std::string("Error: ServerSocket: sageAccept: accept() error\n" +
                      std::string(strerror(errno)));
  }
  return client_socket;
}
