#include "jssetting.h"
#include "jsread.h"

void js_set_clnt(int clnt_sock, int tcp_clnt_socks[], int epfd, int *clnt_cnt);
void js_set_hosting(int serv_sock, int stun_sock, int tcp_host_sock, int tcp_clnt_socks[], int *udp_host_sock, struct sockaddr_in *udp_host_addr, int clnt_cnt);
void js_accept_manage(int serv_sock, int stun_sock, int *tcp_host_sock, int tcp_clnt_socks[], int *udp_host_sock, struct sockaddr_in *udp_host_addr, int epfd, int *clnt_cnt);
void js_udp_holepunching(int stun_sock, int tcp_host_sock, int clnt_sock, int clnt_cnt_num, struct sockaddr_in udp_host_addr);


void js_accept_manage(int serv_sock, int stun_sock, int *tcp_host_sock, int tcp_clnt_socks[], int *udp_host_sock, struct sockaddr_in *udp_host_addr, int epfd, int *clnt_cnt)
{
	int clnt_sock;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;

	clnt_addr_size=sizeof(clnt_addr);
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
	if(clnt_sock==-1)
		error_handling("accept() error");

	printf("\n###\nNew Connecting Client\nIP: %s Port: %d\n###\n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

	if(*clnt_cnt<MAX_CLIENT)
	{
		js_set_clnt(clnt_sock, tcp_clnt_socks, epfd, clnt_cnt);

		if(*tcp_host_sock==-1)
		{
			*tcp_host_sock=clnt_sock;//호스트 탐색 함수 만들기
			js_set_hosting(serv_sock, stun_sock, *tcp_host_sock, tcp_clnt_socks, udp_host_sock, udp_host_addr, *clnt_cnt);
		}
		else
			js_udp_holepunching(stun_sock, *tcp_host_sock, clnt_sock, *clnt_cnt, *udp_host_addr);
	}
	else
	{
		puts("\n###Max Client###\n");
		close(clnt_sock);
	}

	return;
}

void js_udp_holepunching(int stun_sock, int tcp_host_sock, int clnt_sock, int clnt_cnt_num, struct sockaddr_in udp_host_addr)
{
	struct sockaddr_in clnt_addr;


	puts("\n1\n");
	js_tcp_swrite(clnt_sock, SERVER, CSTUN, &udp_host_addr, sizeof(struct sockaddr_in));
	puts("\n2\n");
	js_rudp_recvfrom(stun_sock, &clnt_addr);
	puts("\n3\n");
	js_tcp_swrite(tcp_host_sock, SERVER, HSTUN, &clnt_addr, sizeof(struct sockaddr_in));
	puts("\n4\n");
	
	printf("\n###\nHole Punching\nHost:\nIP: %s Port: %d\nClient:\nIP: %s Port: %d\n###\n", inet_ntoa(udp_host_addr.sin_addr), ntohs(udp_host_addr.sin_port), inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

	return;
}

void js_set_hosting(int serv_sock, int stun_sock, int tcp_host_sock, int tcp_clnt_socks[], int *udp_host_sock, struct sockaddr_in *udp_host_addr, int clnt_cnt)
{
	int i;
	js_tcp_packet(tcp_host_sock, SERVER, BEHOST);

	js_rudp_recvfrom(stun_sock, udp_host_addr);
	//udp_host_sock을 생성해 heartbeat 만들어주기

	printf("\n###\nStart Holepunching: %d\n###\n", clnt_cnt);
	for(i=0; i<clnt_cnt; i++)
		js_udp_holepunching(stun_sock, tcp_host_sock, tcp_clnt_socks[i], i, *udp_host_addr);

	return;
}

void js_set_clnt(int clnt_sock, int tcp_clnt_socks[], int epfd, int *clnt_cnt)
{
	struct epoll_event event;

	tcp_clnt_socks[*clnt_cnt]=clnt_sock;
	clnt_udp_order[*clnt_cnt]=0;

	setnonblockingmode(clnt_sock);
	event.events=EPOLLIN|EPOLLET;
	event.data.fd=clnt_sock;
	epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);

	//clnt_cnt를 클라이언트에 전송하기
	js_tcp_swrite(clnt_sock, SERVER, CLNTCNT, clnt_cnt, sizeof(*clnt_cnt));

	//
	(*clnt_cnt)++;
	printf("\n###\nNow clnt_cnt: %d\n###\n", *clnt_cnt);

	return;
}
