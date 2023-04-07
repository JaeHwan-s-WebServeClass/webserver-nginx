#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <stdio.h>
#define BUF_SIZE 100
void error_handling(char *buf);

// 멀티 뭐시기: 하나의 서버에서 동시에 여러 개의 클라이언트의 요청을 처리하기 위해 고안된 방법
// 멀티 프로세스: fork() 사용
	// 여러 개의 프로그램을 띄워놓고 각 프로그램에서 처리하게끔 하는 것
	// 프로세스가 여러 개가 되면 수행 완료 시점에 차이가 발생하고 좀비 프로세스가 발생하지 않도록 종료에 신경을 써야한다.
	// 많은 연산과 많은 자원을 필요로한다.
// 멀티 플렉싱: select() 사용
	// 하나의 프로세스를 둘 이상의 영역에서 공유하는 것
	// 하나의 프로세스로 여러 개의 요청을 처리하는 것인데, select()를 이용하여 여러 개의 클라이언트를 구분한다.
// 멀티 쓰레딩: 
	// 하나의 프로세스에서 여러 개의 쓰레드를 생성하여 요청을 처리하는 방식
	// 근본적으로 하나의 프로세스이기 때문에 멀티 프로세싱에 비해 자원 소모가 덜하다.

// int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
	// nfds:
	// readfds:
	// writefds:
	// exceptfds:
	// timeout:

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	struct timeval timeout;
	fd_set reads, cpy_reads;

	socklen_t adr_sz;
	int fd_max, str_len, fd_num, i;
	char buf[BUF_SIZE];
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");

	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);
	fd_max=serv_sock;

	while(1)
	{
		cpy_reads=reads;
		timeout.tv_sec=5;
		timeout.tv_usec=5000;

		if((fd_num=select(fd_max+1, &cpy_reads, 0, 0, &timeout))==-1)
			break;
		
		if(fd_num==0)
			continue;

		for(i=0; i<fd_max+1; i++)
		{
			if(FD_ISSET(i, &cpy_reads))
			{
				// server socket 이벤트가 발생한 경우 => client에서 connection 요청이 옴
				if(i==serv_sock)     // connection request!
				{
					adr_sz=sizeof(clnt_adr);
					clnt_sock=
						accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
					FD_SET(clnt_sock, &reads);
					if(fd_max<clnt_sock)
						fd_max=clnt_sock;
					printf("connected client: %d \n", clnt_sock);
				}
				// client socket 이벤트가 발생한 경우 => client에서 request msg 보냄
				else    // read message!
				{
					str_len=read(i, buf, BUF_SIZE);
					if(str_len==0)    // close request!
					{
						FD_CLR(i, &reads);	// FD_CLR: 관찰대상에서 제외한다.
						close(i);
						printf("closed client: %d \n", i);
					}
					else
					{
						write(i, buf, str_len);    // echo!
					}
				}
			}
		}
	}
	close(serv_sock);
	return 0;
}

void error_handling(char *buf)
{
	fputs(buf, stderr);
	fputc('\n', stderr);
	exit(1);
}