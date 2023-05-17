#include "../class/Response.hpp"
#include "../class/Server.hpp"
#include "../class/Transaction.hpp"
#include "../include/include.hpp"

extern int g_server_status;

//----- process ----------------------------------------------------------------
pid_t ft::safeFork() {
  // std::cout << GRY << "Debug: safeFork\n" << DFT;
  pid_t pid;

  pid = fork();
  if (pid == -1) {
    std::cout << RED << "Error: ft: fork() error\n" << DFT;
    g_server_status = UNHEALTHY;
    throw Transaction::ErrorPage500Exception();
  }
  return pid;
}

void ft::safePipe(int *fd) {
  // std::cout << GRY << "Debug: safePipe\n" << DFT;
  if (pipe(fd) == -1) {
    std::cout << RED << "Error: ft: pipe() error\n" << DFT;
    g_server_status = UNHEALTHY;
    throw Transaction::ErrorPage500Exception();
  }
}

void ft::safeDup2(int fd, int mode) {
  // std::cout << GRY << "Debug: safeDup2\n" << DFT;
  if (dup2(fd, mode) == -1) {
    exit(1);
  }
}

void ft::safeExecve(const char *file, char *const *argv, char *const *envp) {
  // std::cout << GRY << "Debug: safeDup2\n" << DFT;
  if (execve(file, argv, envp) == -1) {
    exit(1);
  }
}

//----- send/recv -------------------------------------------------------------
int ft::safeSend(int fd, Response &response) {
  // std::cout << GRY << "Debug: safeSend\n" << DFT;
  signal(SIGPIPE, SIG_IGN);
  int send_len =
      send(fd, response.getResponseMsg(), response.getResponseMsgSize(), 0);

  if ((send_len == -1) || (send_len == 0)) {
    std::cout << RED << "Error: ft: send() error\n" << DFT;
    g_server_status = UNHEALTHY;
    signal(SIGPIPE, SIG_DFL);
    throw Server::ServerUnhealthyException();
  }
  signal(SIGPIPE, SIG_DFL);
  return send_len;
}

int ft::safeRecv(int fd, char *buf, int size) {
  // std::cout << GRY << "Debug: safeRecv\n" << DFT;
  signal(SIGPIPE, SIG_IGN);
  int recv_len = recv(fd, buf, size, 0);

  if ((recv_len == -1) || (recv_len == 0)) {
    std::cout << RED << "Error: ft: recv() error\n" << DFT;
    g_server_status = UNHEALTHY;
    signal(SIGPIPE, SIG_DFL);
    throw Server::ServerUnhealthyException();
  }
  signal(SIGPIPE, SIG_DFL);
  return recv_len;
}

//----- read/write ------------------------------------------------------------
ssize_t ft::safeRead(int fd, char *buf, int size) {
  // std::cout << GRY << "Debug: safeRead\n" << DFT;
  signal(SIGPIPE, SIG_IGN);
  ssize_t read_len = read(fd, buf, size);
  if (read_len == -1) {
    std::cout << RED << "Error: ft: read() error\n" << DFT;
    g_server_status = UNHEALTHY;
    signal(SIGPIPE, SIG_DFL);
    throw Server::ServerUnhealthyException();
  }
  signal(SIGPIPE, SIG_DFL);
  return read_len;
}

ssize_t ft::safeWrite(int fd, char *buf, int size) {
  // std::cout << GRY << "Debug: safeWrite\n" << DFT;
  signal(SIGPIPE, SIG_IGN);
  ssize_t write_len = write(fd, buf, size);
  if (write_len == -1) {
    std::cout << RED << "Error: ft: write() error\n" << DFT;
    g_server_status = UNHEALTHY;
    signal(SIGPIPE, SIG_DFL);
    throw Server::ServerUnhealthyException();
  }
  signal(SIGPIPE, SIG_DFL);
  return write_len;
}

//----- open/close  ----------------------------------------------------------
size_t ft::safeOpen(std::string resource, int flag, mode_t mode) {
  // std::cout << GRY << "Debug: safeOpen\n" << DFT;
  int fd;

  if ((fd = open(resource.c_str(), flag, mode)) == -1) {
    std::cout << RED << "Error: ft: open() error\n" << DFT;
    g_server_status = UNHEALTHY;
    throw Server::ServerUnhealthyException();
  }
  return fd;
}

int ft::safeClose(int fd) {
  // std::cout << GRY << "Debug: safeClose\n" << DFT;
  if (ft::isFileDescriptorValid(fd)) {
    if (close(fd) == -1) {
      std::cout << RED << "Error: ft: close() error\n" << DFT;
      g_server_status = UNHEALTHY;
      throw Server::ServerUnhealthyException();
    }
  }
  return 0;
}
