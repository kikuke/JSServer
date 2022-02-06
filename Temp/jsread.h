#ifndef JSREAD
#define JSREAD

#include "jswrite.h"

//릴레이서버 전환 함수 만들기
int js_udp_decode(char msg_buf[])//오류검사 관련 코드 구현하기
{
	//수정필요
	switch(msg_buf[3]) {
	case SERVER:
		switch(msg_buf[4]) {
		case MESSAGE:
			//함수호출
			break;
		case OK:
			puts("\n###\nGet OK - Server\n###\n");
			break;
		default:
			return -1;
			break;
		}
		break;
	case HOST:
		switch(msg_buf[4]) {
		case MESSAGE:
			//함수호출
			break;
		case OK:
			puts("\n###\nGet OK - Host\n###\n");
			break;
		default:
			return -1;
			break;
		}
		break;
	case CLIENT:
		switch(msg_buf[4]) {
		case MESSAGE:
			//함수호출
			break;
		case OK:
			puts("\n###\nGet OK - Client\n###\n");
			break;
		default:
			return -1;
			break;
		}
		break;
	default:
		return -1;
		break;
	}

	return 0;
}

void js_rudp_recvfrom(int sock, struct sockaddr_in *clnt_addr)
{
	int i;
	char udp_buf[UDP_BUF_SIZE];
	socklen_t adr_sz = sizeof(*clnt_addr);

	for(i=0; i<5; i++)//시도 횟수
	{
		recvfrom(sock, udp_buf, UDP_BUF_SIZE, 0, (struct sockaddr*)clnt_addr, &adr_sz);
		printf("\n###\nReceived UDP ADDR: %s\n###\n", inet_ntoa((*clnt_addr).sin_addr));

		printf("\n###\nUDP order\nServer: %d Client: %d\n###\n", clnt_udp_order[udp_buf[1]], udp_buf[2]);
		if(udp_buf[2]<clnt_udp_order[udp_buf[1]])
		{
			js_udp_ssendto(sock, clnt_addr, SERVER, OK, &udp_buf[2], sizeof(char));

			puts("\n###\nAlready recived\n###\n");
		}
		else if(udp_buf[2]==clnt_udp_order[udp_buf[1]])
		{
			if(js_udp_decode(udp_buf)!=-1)//오류 검사. 나중에 강화하기
			{
				puts("\n###\nReceived UDP Packet!\n###\n");

				js_udp_ssendto(sock, clnt_addr, SERVER, OK, &udp_buf[2], sizeof(char));

				clnt_udp_order[udp_buf[1]]++;
				if(clnt_udp_order[udp_buf[1]]>MAX_UDP_ORDER)
					clnt_udp_order[udp_buf[1]]=0;

				break;
			}
			else
				puts("\n###\nWrong Packet!\n###\n");
		}
		else
			puts("\n###\nWrong Order!\n###\n");
	}

	return;
}

int js_tcp_decode(char msg_buf[])//처리에 필요한 구조체 만들기?
{
	switch(msg_buf[1]) {
	case SERVER:
		switch(msg_buf[2]) {
		case MESSAGE:
			//함수호출
			break;
		default:
			return -1;
			break;
		}
		break;
	case HOST:
		switch(msg_buf[2]) {
		case MESSAGE:
			//함수호출
			break;
		default:
			return -1;
			break;
		}
		break;
	case CLIENT:
		switch(msg_buf[2]) {
		case MESSAGE:
			//함수호출
			break;
		default:
			return -1;
			break;
		}
		break;
	default:
		return -1;
		break;
	}

	return 0;
}

void js_tcp_read(int len, char msg_buf[])
{
	int i;

	for(i=0; i<len; i+=msg_buf[i])
		js_tcp_decode(&msg_buf[i]);

	return;
}

#endif
