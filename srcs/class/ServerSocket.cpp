#include "ServerSocket.hpp"

//---- OCCF -------------------------------------------------------------------
ServerSocket::ServerSocket(sa_family_t address_family, int port) {
  std::memset(&(this->server_addr), 0, sizeof(server_addr));
  this->server_addr.sin_family = address_family;
  this->port = port;
  this->server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  this->server_addr.sin_port = htons(port);
  this->safeSocket(address_family, SOCK_STREAM, 0);
  this->safeBind();
  this->safeListen(BACK_LOG);
  this->setNonBlock(this->server_socket);

  // DEBUG: check server socket port number
  std::cout << "Construct ServerSocket: Port number: "
            << this->server_addr.sin_port << std::endl;
}
ServerSocket::ServerSocket(const ServerSocket &ref) { *this = ref; }
ServerSocket &ServerSocket::operator=(const ServerSocket &ref) {
  this->server_socket = ref.server_socket;
  this->server_addr = ref.server_addr;
  this->port = ref.port;
  return *this;
}
ServerSocket::~ServerSocket() {}

//---- getter -------------------------------------------------------------
const int &ServerSocket::getPort() const { return this->port; }
const int &ServerSocket::getServerSocket() const { return this->server_socket; }

//---- setter -------------------------------------------------------------
void ServerSocket::setNonBlock(int socket_fd) {
  // std::cout << GRY << "Debug: ServerSocket::setNonBlock\n" << DFT;
  if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1) {
    throw std::string("Error: ServerSocket: setNonBlock: fcntl()");
  }
}

//---- safe function ----------------------------------------------
void ServerSocket::safeSocket(int domain, int type, int protocol) {
  // std::cout << GRY << "Debug: ServerSocket::safeSocket\n" << DFT;
  if ((this->server_socket = socket(domain, type, protocol)) == -1) {
    throw std::string("Error: ServerSocket: safeSocket");
  }
}

void ServerSocket::safeBind(void) {
  // std::cout << GRY << "Debug: ServerSocket::safeBind\n" << DFT;
  if (bind(this->server_socket,
           reinterpret_cast<struct sockaddr *>(&this->server_addr),
           sizeof(this->server_addr)) == -1) {
    // DEBUG
    // std::cout << "socket fd : " << this->port << std::endl;
    throw std::string("Error: ServerSocket: safeBind");
  }
}

void ServerSocket::safeListen(int backlog) {
  // std::cout << GRY << "Debug: ServerSocket::safeListen\n" << DFT;
  if (listen(this->server_socket, backlog) == -1) {
    throw std::string("Error: ServerSocket: safeListen");
  }
}

int ServerSocket::safeAccept(void) const {
  // std::cout << GRY << "Debug: ServerSocket::safeAccept\n" << DFT;
  int client_socket;

  if ((client_socket = accept(this->server_socket, NULL, NULL)) == -1) {
    ft::printError("Error: ServerSocket: safeAccept");
    throw Transaction::ErrorPageDefaultException();
  }
  return client_socket;
}
