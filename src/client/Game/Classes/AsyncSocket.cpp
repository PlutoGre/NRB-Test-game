
#include "stdafx.h"

#include "SocketBase.h"
#include "AsyncSocket.h"
#include <stdio.h>
#ifdef PROJECTIOSLOCAL
#include "ConnectionSettings.h"
#include "RemoteSocket.h"
#endif
//#include "Concurrency.h"
//#include <time.h>


AsyncSocket::AsyncSocket()
	: IoSocket()
{
}

AsyncSocket::~AsyncSocket()
{
	onClose();
}

bool AsyncSocket::onCreate(const char *ipaddr /* = 0 */, int port /* = 0 */)
{
	bool ret = IoSocket::onCreate(ipaddr, port);
	if(ret)
	{
		if(GameCommon::setnonblocking(mSocket, 1) < 0)
		{
			ret = false;
			onClose();
		}

// 		if(GameCommon::setkeepalive(mSocket) < 0)
// 		{
// 			ret = false;
// 			onClose();
// 		}
	}
	return ret;
}

bool AsyncSocket::onConnect(const char * ipaddr, int port, unsigned long timeout_ms)
{
#ifdef PROJECTIOSLOCAL
	if (OPRemoteEnabled()) return OPConnectRemoteSocket(mSocket, timeout_ms);
#endif
	if(mSocket == INVALID_SOCKET)
	{
		fprintf(stderr,"AsyncSocket::connect::(mSocket == INVALID_SOCKET)\n");
		return false;
	}

	bool ret = false;
	GameCommon::tcpconnect(mSocket, ipaddr, port);
	do
	{
		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(mSocket, &fdset);

		struct timeval tv;
		tv.tv_sec = timeout_ms / 1000;
		tv.tv_usec = (timeout_ms % 1000) * 1000;

		if (select(mSocket+1, 0, &fdset, 0, &tv) != 1)
		{
			fprintf(stderr,"AsyncSocket::connect(%s:%d) timeout.\n", ipaddr, port);
			break;
		}
			
		//ret = (geterror(mSocket)==0);
		int errorno = GameCommon::geterror(mSocket);
		if(errorno != 0)
		{
			fprintf(stderr,"AsyncSocket::connect(%s:%d) error = %d.\n", ipaddr, port, errorno);
			break;
		}

		ret = true;
	}while(0);

	return ret;
}

int AsyncSocket::onReceive(void* lpBuf, int nBufLen, unsigned long timeout_ms)
{
	if (lpBuf == NULL)
	{
		fprintf(stderr,"AsyncSocket::Receive::(lpBuf == NULL)\n");
		return -1;
	}

	if(mSocket == INVALID_SOCKET)
	{
		//fprintf(stderr,"AsyncSocket::Receive::(mSocket == INVALID_SOCKET)\n");
		return -1;
	}

	struct timeval tv;
	tv.tv_sec = timeout_ms / 1000;
	tv.tv_usec = (timeout_ms % 1000) * 1000;

	fd_set fdread; 
	FD_ZERO(&fdread); 
	FD_SET(mSocket, &fdread);    

	select(mSocket+1, &fdread, 0, 0, &tv); 

	int len = -1;
	if(FD_ISSET(mSocket, &fdread)) 
	{ 
		len = recv(mSocket, static_cast<char*>(lpBuf), nBufLen, 0);
//		bool needrec = true;
// 		while(needrec)
// 		{
// 			char* recv_buf[1024];
// 			int recv_len = recv(mSocket, recv_buf, 1024, 0);
// 			
// 			memcpy( static_cast<char*>(lpBuf) + len, recv_buf, recv_len);
// 			len += recv_len;
// 
// 			if(recv_len < 0)
// 				break;
// 			else if(recv_len == 0)
// 				break;
// 			else if(recv_len < nBufLen)
// 			{
// 				memcpy( static_cast<char*>(lpBuf) + len, recv_buf, recv_len);
// 				len += recv_len;
// 				needrec = false;
// 			}
// 			else if(recv_len == nBufLen)
// 			{
// 
// 			}
// 		}
	}

	return len;
}

int AsyncSocket::onForceReceive(void* lpBuf, int nBufLen, unsigned long timeout_ms)
{
	GameCommon::setnonblocking(mSocket, 0);
	int ret = onReceive(lpBuf, nBufLen, timeout_ms);
	GameCommon::setnonblocking(mSocket, 1);

	return ret;
}

int AsyncSocket::onSend(const void* lpBuf, int nBufLen, unsigned long timeout_ms)
{
	if (lpBuf == NULL)
	{
		fprintf(stderr,"AsyncSocket::Send::(lpBuf == NULL)\n");
		return -1;
	}

	if(mSocket == INVALID_SOCKET)
	{
		fprintf(stderr,"AsyncSocket::Send::(mSocket == INVALID_SOCKET)\n");
		return -1;
	}

	struct timeval tv;
	tv.tv_sec = timeout_ms / 1000;
	tv.tv_usec = (timeout_ms % 1000) * 1000; 

	fd_set fdwrite; 
	FD_ZERO(&fdwrite); 
	FD_SET(mSocket, &fdwrite);

	//handle exception
	fd_set fdexcept; 
	FD_ZERO(&fdexcept); 
	FD_SET(mSocket, &fdexcept); 

	select(mSocket+1, 0, &fdwrite, &fdexcept, &tv); 

	int len = 0;
	if(FD_ISSET(mSocket, &fdwrite))
	{
// 		for(int i = 0; i < 10; ++i) 
// 		{
// 			int sendLen = send(mSocket, (char*)(lpBuf)+len, (nBufLen-len), 0);
// 			if(sendLen < 0)
// 			{
// 				//error
// 				return -1;
// 			}
// 			len += sendLen;
// 			if(len >= nBufLen)
// 			{
// 				break;
// 			}
// 			else
// 			{
// 				sleeping(50);
// 			}
// 		}
		len = send(mSocket, (char*)(lpBuf), nBufLen, 0);
		if(len != nBufLen)
		{
			return -1;
		}
	}
	else if(FD_ISSET(mSocket, &fdexcept))
	{
		//socket has error! need reconnect
		return -2;
	}

	
	return len;
}

bool AsyncSocket::onConnect(const char * ipaddr, int port)
{
	return AsyncSocket::onConnect(ipaddr, port, ASYNC_SOCKET_TIMEOUT_MS);
}

int AsyncSocket::onReceive(void* lpBuf, int nBufLen)
{
	return AsyncSocket::onReceive(lpBuf, nBufLen, ASYNC_SOCKET_TIMEOUT_MS);
}

int AsyncSocket::onForceReceive(void* lpBuf, int nBufLen)
{
	return AsyncSocket::onForceReceive(lpBuf, nBufLen, ASYNC_SOCKET_TIMEOUT_MS);
}

int AsyncSocket::onSend(const void* lpBuf, int nBufLen)
{
	return AsyncSocket::onSend(lpBuf, nBufLen, ASYNC_SOCKET_TIMEOUT_MS);
}
