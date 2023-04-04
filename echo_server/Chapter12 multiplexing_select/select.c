#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 30

// select()의 역할
	// 1. 수신한 데이터를 지니고 있는 소켓이 존재하는가?
	// 2. 블로킹되지 않고 데이터의 전송이 가능한 소켓은 무엇인가?
	// 3. 예외상황이 발생한 소켓은 무엇인가?

int main(int argc, char *argv[])
{
	fd_set reads, temps;
	int result, str_len;
	char buf[BUF_SIZE];
	struct timeval timeout;

	FD_ZERO(&reads);	// 전부 다 0로 초기화해주는...
	FD_SET(0, &reads);	// 0 is standard input(console), 검색의 대상을 지정할 수 있다.

	/*
	timeout.tv_sec=5;
	timeout.tv_usec=5000;
	*/

	while(1)
	{
		temps=reads;
		timeout.tv_sec=5;
		timeout.tv_usec=0;
		
		// int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
			// -1: 오류발생 시의 반환
			// 0: 타임아웃에 의한 반환
			// 0보다 큰 값: 이벤트가 발생할 경우의 반환
		result=select(1, &temps, 0, 0, &timeout);
		
		if(result==-1)		// 오류발생
		{
			puts("select() error!");
			break;
		}
		else if(result==0)	// timeout
		{
			puts("Time-out!");
		}
		else 				// 이벤트 발생
		{
			if(FD_ISSET(0, &temps)) // FD_ISSET: 소켓의 상태를 반환하며, 읽기가 가능하면 1을 반환한다.
			{
				str_len=read(0, buf, BUF_SIZE);
				buf[str_len]=0;
				printf("message from console: %s", buf);
			}
		}
	}
	return 0;
}