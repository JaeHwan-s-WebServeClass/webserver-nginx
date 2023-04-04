#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);

/**
 * struct sockaddr_in {
 * 		sa_family_t		sin_family;		// 주소체계 : UDP, TCP, ...
 * 		uint16_t		sin_port;		// 포트정보 
 * 		struct in_addr	sin_addr;		// IPv4 (32bit IPv4 주소)
 * 		char			sin_zero[8];	// 구조체를 16 bytes로 맞춰주기 위한 용도 (zero padding)
 * }; 
 */

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	char message[BUF_SIZE];
	int str_len, i;
	
	struct sockaddr_in serv_adr;
	struct sockaddr_in clnt_adr;
	socklen_t clnt_adr_sz;		// unsigned_int client address size
	
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	/* *
	 * socket(af, type, protocol(default : 0))
	 * af : 주소 패밀리 사양 ex) AF_INET(IPv4)
	 * 		AF_INET : Address Family
	 * 		PF_INET : Protocol Family
	 * 		두 가지는 본질적으로 같으나 상징적인 의미가 다르다. 
	 * 		Protocol Family를 사용한다는 것을 강조하고자 할 때, AF 대신 PF를, 사용하는 경우가 있음
	 * type : new socket의 형식 사양 
	 *		ex ) SOCK_STREAM(TCP를 의미), SOCK_DGRAM(UDP를 의미)
	 * protocol : 사용할 프로토콜 ex) IPPROTO_TCP(TCP)
	 * 		값이 0이라면 호출자가 프로토콜을 정하지 않고, OS가 적절한 거를 골라서 사용하라는 의미
	 * return : new socket에 대한 fd
	 * */
	
	// server socker 생성
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);   // create a new TCP socket for IPv4 communication

	if(serv_sock==-1)
		error_handling("socket() error");
	
	// server 구조성 초기화
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);	// INADDR_ANY: 0.0.0.0
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	// int 	bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
		// 1. sockfd: 소켓의 fd
		// 2. addr: 바인딩할 주소와 포트를 지정하는 sockaddr 구조체에 대한 포인터
		// 3. addrlen: sockaddr 구조체의 크기

	// 소켓에 주소체계 부여 : serv_sock + serv_adr 
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	
	// int 	listen(int sockfd, int backlog);
		// 1. sockfd: the socket file descriptor to be set to listening mode.
		// 2. backlog: the maximum length of the queue of pending connections.

	// request를 기다리는 대기상태
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	// backlog 매개변수는 대기 큐에 저장될 연결 요청의 최대 개수를 나타내며, 
	// 대기 큐에 연결 요청이 가득 차면 서버 소켓은 이후의 연결 요청을 거부합니다. 
	// 이때, 연결 요청이 거부되면 클라이언트는 ECONNREFUSED 오류 코드를 수신합니다.
	//----------------------------- server socket 준비 끝! -----------------------------

	clnt_adr_sz=sizeof(clnt_adr);

	for(i=0; i<5; i++)
	{
		// int	accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
			// 수신 대기열의 가장 앞에 있는 연결 요청을 가져온다.
			// addrlen이 포인터로 넘겨진다는 것 주의하기... to reflect the actual size of the 'addr'
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		if(clnt_sock==-1)
			error_handling("accept() error");
		else
			printf("Connected client %d \n", i+1);

		// client의 request msg 받음 => read
		while((str_len=read(clnt_sock, message, BUF_SIZE))!=0)
			write(clnt_sock, message, str_len);		// write로 response msg 작성

		// client (먼저) 끝
		close(clnt_sock);
	}

	// server 끝
	close(serv_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

/******************************************************************************
 	socket 함수 source code
 	참고한 깃허브 : https://github.com/torvalds/linux/blob/master/net/socket.c
-------------------------------------------------------------------------------

int __sys_socket(int family, int type, int protocol)
{
		struct socket *sock;
		int flags;

		sock = __sys_socket_create(family, type, protocol);
		if (IS_ERR(sock))
			return PTR_ERR(sock);

		flags = type & ~SOCK_TYPE_MASK;
		if (SOCK_NONBLOCK != O_NONBLOCK && (flags & SOCK_NONBLOCK))
			flags = (flags & ~SOCK_NONBLOCK) | O_NONBLOCK;

		return sock_map_fd(sock, flags & (O_CLOEXEC | O_NONBLOCK));
}

static int sock_map_fd(struct socket *sock, int flags)
{
		struct file *newfile;
		int fd = get_unused_fd_flags(flags);
		if (unlikely(fd < 0)) {
			sock_release(sock);
			return fd;
		}

		newfile = sock_alloc_file(sock, flags, NULL);
		if (!IS_ERR(newfile)) {
			fd_install(fd, newfile);
			return fd;
		}

		put_unused_fd(fd);
		return PTR_ERR(newfile);
}
******************************************************************************/