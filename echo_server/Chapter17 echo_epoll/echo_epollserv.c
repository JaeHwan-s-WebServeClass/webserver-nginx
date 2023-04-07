#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUF_SIZE 100
#define EPOLL_SIZE 50
void error_handling(char *buf);

// select()의 단점: 매번 호출할 때마다 OS에 정보를 전달해야한다.
// select 함수의 단점을 극복하기 위해서는 OS 레벨에서 멀티플렉싱 기능을 지원해야한다.
	// OS에게 관찰대상에 대한 정보를 한 번만 알려주고,
	// 관찰대상의 범위 또는 내용에 변경이 있을 때 변경사항만 알려주는 것.
// 단, 호환성을 중시하고자 하는 경우에는 select()를 사용하는 것이 도움이 된다.

// 1. epoll_create:	
	// epoll fd 저장소 (epoll instance) 생성
	// int	epoll_create(int size);
// 2. epoll_ctl: 
	// 저장소에 fd 등록 및 삭제
	// int 	epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
// 3. epoll_wait: 
	// select 함수와 마찬가지로 fd의 변화를 대기한다.
	// int	epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
	// timeout에 -1 전달 시 이벤트가 발생할 때까지 무한대기
/* 
struct epoll_event {
    uint32_t		events;		// Events to be monitored
    epoll_data_t	data; 		// User data variable
};
*/

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t adr_sz;
	int str_len, i;
	char buf[BUF_SIZE];

	struct epoll_event *ep_events;
	struct epoll_event event;		// event의 유형 등록에 사용
	int epfd, event_cnt;

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

	// epoll instance 생성
	epfd=epoll_create(EPOLL_SIZE);	
	ep_events=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

	// event의 유형 등록
	event.events=EPOLLIN;	// EPOLLIN: 수신할 데이터가 존재하는 상황
	event.data.fd=serv_sock;	

	// server socket 을 epoll instance에 등록
	epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

	while(1)
	{
		event_cnt=epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
		if(event_cnt==-1)
		{
			puts("epoll_wait() error");
			break;
		}

		for(i=0; i<event_cnt; i++)
		{
			if(ep_events[i].data.fd==serv_sock)
			{
				adr_sz=sizeof(clnt_adr);
				clnt_sock=
					accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
				event.events=EPOLLIN;
				event.data.fd=clnt_sock;
				// select에서는 fd_max를 증가시켜줬지만 epoll에서는 clnt_sock를 epoll instance에 추가해준다.
				epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
				printf("connected client: %d \n", clnt_sock);
			}
			else
			{
					str_len=read(ep_events[i].data.fd, buf, BUF_SIZE);
					if(str_len==0)    // close request!
					{
						epoll_ctl(
							epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
						close(ep_events[i].data.fd);
						printf("closed client: %d \n", ep_events[i].data.fd);
					}
					else
					{
						write(ep_events[i].data.fd, buf, str_len);    // echo!
					}
	
			}
		}
		// for문이 아니라 비동기적으로 실행시킬 수 있는 방법이 있는지 모색해볼 필요가 있다.
	}
	close(serv_sock);
	close(epfd);
	return 0;
}

void error_handling(char *buf)
{
	fputs(buf, stderr);
	fputc('\n', stderr);
	exit(1);
}

//
/*
struct pollfd {
	int		fd;         // file descriptor to monitor
    short	events;    	// events to monitor for
    short 	revents;  	// events that occurred
};
*/

// poll():
	// socket에서 동시에 여러개의 I/O를 대기할 경우에 
	// 특정한 fd에 blocking되지 않고 I/O를 할 수 있는 상태인 지를 모니터링하여 I/O 가능한 상태의 fd인지를 검사하는 함수

// int	poll(struct pollfd *fds, nfds_t nfds, int timeout);
	// 1. fds: a pointer to an array of struct pollfd objects
	// 2. nfds: the number of file descriptors in the fds array.
	// 3. timeout: