#include "jssetting.h"
#include "jsread.h"

void SendResponse(int stun_sock, struct sockaddr_in addr, struct sockaddr_in receiver)
{
	int size = sizeof(addr);

	sendto(stun_sock, &addr, size, 0, (struct sockaddr*)&receiver, sizeof(receiver));
}
