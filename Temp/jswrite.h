#ifndef JSWRITE
#define JSWRITE

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "jsnet.h"

#define RUDP_USLEEP 300000

//나중에 비트단위로 쪼개서 보내기
ssize_t js_udp_encode(char udp_buf[], const void *msg_buf, size_t n, js_udp_struct udp_struct)
{
	udp_buf[0]=n+6;
	udp_buf[1]=udp_struct.msg_clntcnt;
	udp_buf[2]=udp_struct.msg_order;
	udp_buf[3]=udp_struct.msg_from;
	udp_buf[4]=udp_struct.msg_type;

	memcpy(&udp_buf[5], msg_buf, n);
	if(n>UDP_BUF_SIZE-6)
		return -1;

	udp_buf[n+5]='\0';

	return udp_buf[0];
}

ssize_t js_udp_sendto(int sock, struct sockaddr_in *clnt_addr, const void *msg_buf, size_t n, js_udp_struct udp_struct)
{
	char udp_buf[UDP_BUF_SIZE];

	if(js_udp_encode(udp_buf, msg_buf, n, udp_struct)==-1)
		return -1;

	printf("\n###\nUDP Send\nClnt: %d, Addr: %s\nFrom: %d, Type: %d\n###\n", udp_buf[1], inet_ntoa((*clnt_addr).sin_addr), udp_buf[3], udp_buf[4]);

	return sendto(sock, udp_buf, udp_buf[0], 0, (struct sockaddr*)clnt_addr, sizeof(*clnt_addr));
}

ssize_t js_udp_ssendto(int sock, struct sockaddr_in *clnt_addr, int msg_from, int msg_type, const void *msg_buf, size_t n)
{
	js_udp_struct udp_struct;

	udp_struct.msg_from = msg_from;
	udp_struct.msg_type = msg_type;

	return js_udp_sendto(sock, clnt_addr, msg_buf, n, udp_struct);
}

ssize_t js_udp_sendpacket(int sock, struct sockaddr_in *clnt_addr, int msg_from, int msg_type)
{
	return js_udp_ssendto(sock, clnt_addr, msg_from, msg_type, "0", 1);
}

ssize_t js_rudp_sendto(int sock, struct sockaddr_in *clnt_addr, const void *msg_buf, size_t n, js_udp_struct udp_struct)
{
	int i;
	char udp_buf[UDP_BUF_SIZE];
	char buf[UDP_BUF_SIZE];

	if(js_udp_encode(udp_buf, msg_buf, n, udp_struct)==-1)
	{
		puts("\n###\nSend Failed!\n###\n");
		return -1;
	}

	for(i=0; i<5; i++)
	{
		sendto(sock, udp_buf, udp_buf[0], 0, (struct sockaddr*)clnt_addr, sizeof(*clnt_addr));

		usleep(RUDP_USLEEP);

		if(recv(sock, buf, UDP_BUF_SIZE, MSG_DONTWAIT)>0)
		{
			printf("\n###\nRecive Data:\nClntcnt: %d, Order: %d\nFrom: %d, Type: %d\n###\n", buf[1], buf[2], buf[3], buf[4]);

			//if((buf[4]==OK)&&(buf[5]==udp_buf[2])&&(buf[1]==udp_buf[1]))//클라는 전송시 반드시 clntcnt를 추가.
			if((buf[4]==OK)&&(buf[5]==udp_buf[2]))
			{
				clnt_udp_order[udp_buf[1]]++;
				if(clnt_udp_order[udp_buf[1]]>MAX_UDP_ORDER)
					clnt_udp_order[udp_buf[1]]=0;

				break;
			}
		}
		puts("\n###\nRetry Send...!\n###\n");
		if(i==4)
			puts("\n###\nRUDP Failed\n###\n");
	}

	return 0;
}

ssize_t js_rudp_ssendto(int sock, struct sockaddr_in *clnt_addr, int msg_clntcnt, int msg_order, int msg_from, int msg_type, const void *msg_buf, size_t n)
{
	js_udp_struct udp_struct;

	udp_struct.msg_clntcnt = msg_clntcnt;
	udp_struct.msg_order = msg_order;
	udp_struct.msg_from = msg_from;
	udp_struct.msg_type = msg_type;

	return js_rudp_sendto(sock, clnt_addr, msg_buf, n, udp_struct);
}

ssize_t js_rudp_sendpacket(int sock, struct sockaddr_in *clnt_addr, int msg_clntcnt, int msg_order, int msg_from, int msg_type)
{
	return js_rudp_ssendto(sock, clnt_addr, msg_clntcnt, msg_order, msg_from, msg_type, "0", 1);
}

//나중에 비트단위로 쪼개서 보내기
ssize_t js_tcp_encode(char tcp_buf[], const void *msg_buf, size_t n, js_tcp_struct tcp_struct)
{
	tcp_buf[0]=n+4;//메시지의 길이. 헤더 개수 + 맨 뒤의 null
	tcp_buf[1]=tcp_struct.msg_from;
	tcp_buf[2]=tcp_struct.msg_type;

	memcpy(&tcp_buf[3], msg_buf, n);
	if(n>TCP_BUF_SIZE-4)
		return -1;

	tcp_buf[n+3]='\0';

	return tcp_buf[0];
}

ssize_t js_tcp_write(int sock, const void *msg_buf, size_t n, js_tcp_struct tcp_struct)
{
	char tcp_buf[TCP_BUF_SIZE];

	if(js_tcp_encode(tcp_buf, msg_buf, n, tcp_struct)==-1)
		return -1;

	printf("\n###\nTCP Send!\nLen: %d From: %d Type: %d\n###\n", tcp_buf[0], tcp_buf[1], tcp_buf[2]);
	return write(sock, tcp_buf, tcp_buf[0]);//n+멤버개수+2
}

ssize_t js_tcp_swrite(int sock, int msg_from, int msg_type, const void *msg_buf, size_t n)
{
	js_tcp_struct tcp_struct;

	tcp_struct.msg_from = msg_from;
	tcp_struct.msg_type = msg_type;

	return js_tcp_write(sock, msg_buf, n, tcp_struct);
}

ssize_t js_tcp_packet(int sock, int msg_from, int msg_type)
{
	return js_tcp_swrite(sock, msg_from, msg_type, "0", 1);
}

#endif
