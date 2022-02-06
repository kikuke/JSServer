#include <errno.h>
#include "jsmanager.h"

void js_server(int serv_sock, int stun_sock, int epfd, struct epoll_event *ep_events);

int main(void)
{
	int serv_sock, stun_sock;
	int epfd;
	struct epoll_event *ep_events;

	js_set_sock(&serv_sock, &stun_sock);
	js_set_epoll(serv_sock, &epfd, ep_events);

	js_server(serv_sock, stun_sock, epfd, ep_events);

	return 0;
}


void js_server(int serv_sock, int stun_sock, int epfd, struct epoll_event *ep_events)
{
	struct epoll_event event;
	int event_cnt, i;

	int tcp_host_sock = -1;
	int udp_host_sock = -1;
	struct sockaddr_in udp_host_addr;

	int clnt_cnt = 0;
	int tcp_clnt_socks[MAX_CLIENT];
	char buf[TCP_BUF_SIZE];
	int str_len, len;

	puts("\n#####\nServer Start...\n#####\n");

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
				js_accept_manage(serv_sock, stun_sock, &tcp_host_sock, tcp_clnt_socks, &udp_host_sock, &udp_host_addr, epfd, &clnt_cnt);
			else
			{
				str_len=0;

				while(1)
				{
					len = read(ep_events[i].data.fd, &buf[str_len], TCP_BUF_SIZE - str_len);
					
					if(len==0)
					{
						if(tcp_host_sock==ep_events[i].data.fd)
						{//호스트, 클라이언트가 나갈경우 구현하기
						}
					}
					else if(len<0)
						if(errno==EAGAIN)
						{
							js_tcp_read(str_len, buf);

							break;
						}

					str_len+=len;
				}
			}
		}
	}

	close(serv_sock); close(epfd);
	return;
}
