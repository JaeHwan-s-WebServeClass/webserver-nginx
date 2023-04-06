#pragma once

// c header
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>
# include <errno.h>
# include <arpa/inet.h>
# include <sys/socket.h>

// kqueue header in C Library
# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
// #include <netinet/in.h>

// c++ header
# include <iostream>
# include <map>
# include <vector>

// function kevent()
/*
int kevent (
    int 					kq,				// kqueue의 fd
    const struct kevent		*changelist,	// 등록하고자 하는 이벤트
    int 					nchanges,		// changelist의 길이
    struct kevent 			*eventlist,		// 발생한 이벤트들
    int 					nevents,		// eventlist의 길이
    const struct timespec 	*timeout	  	// timeout
);
*/

// struct kevent
/*
struct kevent {
    uintptr_t   ident;	  	// 이벤트에 대한 식별자, fd 번호
    int16_t	 	filter;	 	// 이벤트에 대한 식별자, 이벤트의 종류에 해당
    uint16_t	flags;	  	// event를 적용시키거나, event가 return 됐을 때 flag
    uint32_t	fflags;	 	// filter에 대한 flag
    intptr_t	data;	   	// filter에 대한 data
    void		*udata;	 	// user data
};
*/

// struct sockaddr_in
/**
 * struct sockaddr_in {
 * 		sa_family_t		sin_family;		// 주소체계 : UDP, TCP, ...
 * 		uint16_t		sin_port;		// 포트정보 
 * 		struct in_addr	sin_addr;		// IPv4 (32bit IPv4 주소)
 * 		char			sin_zero[8];	// 구조체를 16 bytes로 맞춰주기 위한 용도 (zero padding)
 * }; 
 */

// struct in_addr
/**
struct in_addr {
	uint32_t s_addr;
};
*/

// fcntl(server_socket, F_SETFL, O_NONBLOCK);
/*
#define F_SETFL         4   // set file status flags
*/

/*
#define EV_SET(kevp, a, b, c, d, e, f) do {     \
	struct kevent *__kevp__ = (kevp);       \
	__kevp__->ident = (a);                  \
	__kevp__->filter = (b);                 \
	__kevp__->flags = (c);                  \
	__kevp__->fflags = (d);                 \
	__kevp__->data = (e);                   \
	__kevp__->udata = (f);                  \
} while(0)
*/
