#ifndef JSCONTROL
#define JSCONTROL//디버깅용

#include <stdio.h>
#include <stdlib.h>

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

#endif
