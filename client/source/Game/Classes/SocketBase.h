#ifndef _SOCKET_BASE_H__
#define _SOCKET_BASE_H__


#ifndef WIN32

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define SOCKET			int
#define closesocket		close
#define INVALID_SOCKET	(-1)

#else

#include <WinSock2.h>

#endif//WINDOWS

namespace GameCommon
{
	SOCKET udpsocket(const char *ipaddr, int port, int reuseaddr = 0);
	int udpsendto(SOCKET sock, const char *ipaddr, int port, const char *buf, int len, int flag);
	int udprecvfrom(SOCKET sock, char *ipaddr, unsigned int *port, char *buf, int len, int flag);

	SOCKET tcpsocket(const char *ipaddr, int port, int reuseaddr = 0);
	int tcpconnect(SOCKET sock, const char *ipaddr, int port);
	SOCKET tcpaccept(SOCKET sock, char *ipaddr, int *port);

	int setnonblocking(SOCKET sock, int nonblock = 1);
	int setreuseaddr(SOCKET sock, int opt);
	int setkeepalive(SOCKET sock, int keepalive = 1, int keepidle = 30, int keepinterval = 1, int keepcount = 5);

	int getpeeraddr(SOCKET sock, char* ip, int* port);
	int getlocaladdr(SOCKET sock, char* ip, int* port);

	int geterror(SOCKET sock);
}
#endif//_SOCKET_BASE_H__
