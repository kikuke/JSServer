#ifndef JSSETTING
#define JSSETTING

#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include "jsnet.h"
#include "jscontrol.h"

void setnonblockingmode(int fd);

void js_set_epoll(int stun_sock, int* epfd, struct epoll_event *ep_events)
{
	struct epoll_event event;
	
	puts("\n#####\nSet Epoll...\n#####\n");

	*epfd=epoll_create(EPOLL_SIZE);
	ep_events=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

	setnonblockingmode(stun_sock);
	event.events=EPOLLIN;
	event.data.fd=stun_sock;
	epoll_ctl(*epfd, EPOLL_CTL_ADD, stun_sock, &event);
}

void js_set_sock(int* stun_sock)
{
	int flag;
	struct sockaddr_in serv_addr;

	puts("\n#####\nSet Socket...\n#####\n");

	*stun_sock=socket(PF_INET, SOCK_DGRAM, 0);
	if(*stun_sock==-1)
		error_handling("socket() error");
	flag=1;
	setsockopt(*stun_sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(SERV_PORT);

	if(bind(*stun_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
		error_handling("bind() error");

	return;
}

void setnonblockingmode(int fd)
{
	int flag=fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flag|O_NONBLOCK);
}

#endif
