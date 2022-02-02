#ifndef JSNET
#define JSNET

#define SERV_PORT 2323
#define SERV_LISTEN 5

#define TCP_BUF_SIZE 1000
#define UDP_BUF_SIZE 1000
#define MAX_UDP_ORDER 1000

#define MAX_CLIENT 8
#define EPOLL_SIZE 50

//메시지 처리 매크로
//클라이언트 프로그램도 동일해야 하는 매크로.
//msg_from
#define SERVER 0
#define HOST 1
#define CLIENT 2

//msg_type
#define OK 0
#define MESSAGE 1
#define CLNTCNT 2
#define BEHOST 3
#define RCVADR 4
#define HSTUN 5
#define CSTUN 6

int clnt_udp_order[MAX_CLIENT];

typedef struct {
	int msg_from;
	int msg_type;
} js_tcp_struct;

typedef struct {
	int msg_clntcnt;
	int msg_order;
	int msg_from;
	int msg_type;
} js_udp_struct;//체크섬 공부해서 무결성 보장하기/도착을 보장하는 함수 만들기

#endif
