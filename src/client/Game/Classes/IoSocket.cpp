
#include "stdafx.h"

#include "SocketBase.h"
#include "IoSocket.h"
#include <stdio.h>
#ifdef PROJECTIOSLOCAL
#include "ConnectionSettings.h"
#include "RemoteSocket.h"
#endif


IoSocket::IoSocket()
{
	mSocket = INVALID_SOCKET;

#ifndef WIN32
	signal(SIGPIPE, SIG_IGN);
#endif//WINDOWS	
}

IoSocket::~IoSocket()
{
	onClose();
}

bool IoSocket::onCreate(const char *ipaddr /* = 0 */, int port /* = 0 */)
{
	if (mSocket != INVALID_SOCKET)
	{
		fprintf(stderr,"IoSocket::Create::(mSocket != INVALID_SOCKET)\n");
		return false;
	}

#ifdef PROJECTIOSLOCAL
	mSocket = OPRemoteEnabled() ? OPCreateRemoteSocket() : GameCommon::tcpsocket(ipaddr, port, 1);
#else
	mSocket = GameCommon::tcpsocket(ipaddr, port, 1);
#endif
	return (mSocket!=INVALID_SOCKET);
}

bool IoSocket::onAttach(SOCKET hSocket)
{
	if(hSocket == INVALID_SOCKET)
	{
		fprintf(stderr,"IoSocket::Attach::(hSocket == INVALID_SOCKET)\n");
		return false;
	}

	if(mSocket != INVALID_SOCKET)
	{
		fprintf(stderr,"IoSocket::Attach::(mSocket != INVALID_SOCKET)\n");
	}

	mSocket = hSocket;
	return true;
}

SOCKET IoSocket::onDetach()
{
	SOCKET tmp = mSocket;
	mSocket = INVALID_SOCKET;

	return tmp;
}

bool IoSocket::onShutdown(int nHow)
{
	if(mSocket == INVALID_SOCKET)
	{
		fprintf(stderr,"IoSocket::ShutDown::(mSocket == INVALID_SOCKET)\n");
		return false;
	}

	if(shutdown(mSocket, nHow) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void IoSocket::onClose()
{
	if(mSocket == INVALID_SOCKET)
	{
		return;
	}

#ifdef PROJECTIOSLOCAL
	OPCloseRemoteSocket(mSocket);
#endif
	closesocket(mSocket);
	mSocket = INVALID_SOCKET;
}

void IoSocket::sleep(unsigned long ms)
{
	struct timeval tv;
	tv.tv_sec = ms/1000;
	tv.tv_usec = (ms%1000)*1000;
	select( 0, 0, 0, 0, &tv );
}
