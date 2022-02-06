#include <errno.h>
#include "jsmanager.h"

void js_server(int stun_sock, int epfd, struct epoll_event *ep_events);

int main(void)
{
	int stun_sock;
	int epfd;
	struct epoll_event *ep_events;

	js_set_sock(&stun_sock);
	js_set_epoll(stun_sock, &epfd, ep_events);

	js_server(stun_sock, epfd, ep_events);

	return 0;
}


void js_server(int stun_sock, int epfd, struct epoll_event *ep_events)
{
	struct epoll_event event;
	int event_cnt, i;

	struct sockaddr_in clnt_addr;
	int clnt_sz;

	int iResult;
	char stun_buf[UDP_BUF_SIZE];

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
			clnt_sz = sizeof(clnt_addr);
			iResult = recvfrom(ep_events[i].data.fd, stun_buf, UDP_BUF_SIZE, 0, (struct sockaddr*)&clnt_addr, &clnt_sz);
			
			if(iResult > 0){
				int id = stun_buf[0];
				int j;

				for(j=0; j<map_cnt; j++)
					if(map_index[j]==id)
					{
						SendResponse(stun_sock, map_addr[j], clnt_addr);
						SendResponse(stun_sock, clnt_addr, map_addr[j]);
					
						printf("\n###\nLinked ID: %d\nEndpoint1 - IP: %s Port: %d\nEndpoint2 - IP: %s Port: %d\n###\n", id, inet_ntoa(map_addr[j].sin_addr), ntohs(map_addr[j].sin_port), inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

						j=-1;
						break;
					}
				
				if(j!=-1)
				{
					map_index[map_cnt]=id;
					map_addr[map_cnt]=clnt_addr;
					map_cnt++;

					printf("\n###\nRegistered ID: %d\nIP: %s Port: %d\n###\n", id, inet_ntoa(map_addr[map_cnt-1].sin_addr), ntohs(map_addr[map_cnt-1].sin_port));
				}
			}
		}
	}

	close(stun_sock); close(epfd);
	return;
}
