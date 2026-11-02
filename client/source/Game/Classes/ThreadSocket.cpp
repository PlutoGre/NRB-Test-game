
#include "stdafx.h"

#include "ThreadSocket.h"
#include "PacketManager.h"
#include "cocos2d.h"

class SocketConnectTask : public SocketTask
{
public:
	std::string mIP;
	int mPort;

	virtual SOCKETTYPE getType(){return ST_CONNECT;}
	SocketConnectTask(const std::string & ipaddress, int port)
		:mIP(ipaddress),mPort(port)
	{}
	virtual int run()
	{
		AsyncSocket& socket = ThreadSocket::Get()->lockSocket();
		bool connected = false;
		do
		{
#ifdef WIN32
			WSADATA wsaData;   
			if (WSAStartup(MAKEWORD(2,1), &wsaData))
			{ 
				printf("Windows socket initialize error!\n"); 
				WSACleanup(); 
				ThreadSocket::Get()->setState(ThreadSocket::SS_CONNECT_FAILED);
				break; 
			}
#endif
			if(! socket.onCreate())
			{
				printf("create SyncSocket error.\n");
				ThreadSocket::Get()->setState(ThreadSocket::SS_CONNECT_FAILED);
				break;
			}
			if(! socket.onConnect(mIP.c_str(), mPort))
			{
				printf("connect server error.\n");
				ThreadSocket::Get()->setState(ThreadSocket::SS_CONNECT_FAILED);
				socket.onClose();
				break;
			}

			connected = true;
		}while(0);
		// Preserve the failure state so the UI can offer another connection attempt.
		if (connected) ThreadSocket::Get()->setState(ThreadSocket::SS_WAIT);
		ThreadSocket::Get()->releaseSocket();
		return 0;
	} 
};


class SocketSendTask : public SocketTask
{
private:
	ThreadSocket::PacketData mData;
public:
	int opcode;
	virtual SOCKETTYPE getType(){return ST_SEND;}
	SocketSendTask(ThreadSocket::PacketData data, int Opcode)
	{
		mData = data;
		opcode = Opcode;
	}
	virtual int run()
	{
		AsyncSocket& socket = ThreadSocket::Get()->lockSocket();
		do
		{
			if(mData.buffer == 0)
				break;
			if(mData.length<=0 )
			{
				delete[] (char*)mData.buffer;
				break;
			}
			bool sendOK = (socket.onSend(mData.buffer,mData.length) > 0);
			//             int sendCount = 0;
			// 			while(!sendOK && sendCount<10)
			//             {
			//                 sendOK = (socket.onSend(mData.buffer,mData.length) >= 0);
			//                 //socket.reconnect();
			//                 sendCount++;
			//             }

			delete[] (char*) mData.buffer;
			if(sendOK)
			{
				ThreadSocket::Get()->setState(ThreadSocket::SS_WAIT);
				ThreadSocket::Get()->releaseSocket();
				return 0;
			}

		}while(0);


		ThreadSocket::Get()->setState(ThreadSocket::SS_SEND_FAILED);
		ThreadSocket::Get()->releaseSocket();
		return 0;
	} 
};

class SocketReceiveTask : public SocketTask
{
private:
	ThreadSocket::PacketData mData;
public:
	virtual SOCKETTYPE getType(){return ST_RECEIVED;}
	SocketReceiveTask()
	{
		mData.buffer = new char[PacketManager::DEFAULT_BUFFER_LENGTH];
		mData.length = -1;
	}
	~SocketReceiveTask()
	{
		if(mData.buffer)
			delete [](char*)mData.buffer;
	}
	virtual int run()
	{
		AsyncSocket& socket = ThreadSocket::Get()->lockSocket();
		do
		{
			mData.length = socket.onReceive(mData.buffer,PacketManager::DEFAULT_BUFFER_LENGTH,0);
			if (mData.length == 0)
			{
				socket.onClose();
				ThreadSocket::Get()->setState(ThreadSocket::SS_CONNECTION_LOST);
				ThreadSocket::Get()->releaseSocket();
				return 0;
			}
			if(mData.length > 0)
			{
				ThreadSocket::Get()->setState(ThreadSocket::SS_RECEIVE_DONE);
				ThreadSocket::Get()->releaseSocket();
				return 0;
			}
		}while(0);

		ThreadSocket::Get()->setState(ThreadSocket::SS_WAIT);
		ThreadSocket::Get()->releaseSocket();
		return 0;
	} 

	ThreadSocket::PacketData getData(){return mData;}
};


ThreadSocket::ThreadSocket(void)
	:mState(SS_UNINITIALIZED)
	,mCurrentTask(0)
	,mForceShutDone(false)
{
}


ThreadSocket::~ThreadSocket(void)
{
}

void ThreadSocket::update()
{
	if (getState() == SS_CONNECTION_LOST)
	{
		disconnect();
		// Let the existing failure UI decide when to reconnect. Do not replay sends.
		PacketManager::Get()->_boardcastSendFailed(0);
		return;
	}
	if(getState() == SS_CONNECT_FAILED)
	{
		std::string ip;
		int port;
		if(mCurrentTask)
		{
			SocketConnectTask* conn = dynamic_cast<SocketConnectTask*>(mCurrentTask);
			if(conn)
			{
				ip = conn->mIP;
				port = conn->mPort;
			}
			delete mCurrentTask;//mThread.shutdown();
			mCurrentTask = 0;
		}
		setState(SS_UNINITIALIZED);
		PacketManager::Get()->_boardcastConnectionFailed(ip,port);
	}
	if(getState() == SS_UNINITIALIZED)
	{
		if(mCurrentTask)
		{
			delete mCurrentTask;//mThread.shutdown();
			mCurrentTask = 0;
		}
		std::list<SocketTask*>::iterator it = mTaskList.begin();
		for(;it!=mTaskList.end();++it)
		{
			if((*it)->getType() ==SocketTask::ST_CONNECT)
			{
				SocketConnectTask* conn = dynamic_cast<SocketConnectTask*>(*it);
				if(conn)
				{
					mCurrentTask = conn;
					setState(SS_CONNECTING);	
					mThread.execute(mCurrentTask);//mCurrentTask->run();
					//mCurrentTask->run();
					mTaskList.erase(it);
					return;
				}
			}
		}
	}
	if(getState() == SS_SEND_FAILED)
	{
		int opcode = 0;
		if(mCurrentTask)
		{
			SocketSendTask* conn = dynamic_cast<SocketSendTask*>(mCurrentTask);
			if(conn)
			{
				opcode = conn->opcode;
			}
			delete mCurrentTask;//mThread.shutdown();
			mCurrentTask = 0;
		}
		setState(SS_WAIT);
		PacketManager::Get()->_boardcastSendFailed(opcode);
	}
	if(getState() == SS_RECEIVE_DONE)
	{
		if(mCurrentTask && mCurrentTask->getType() == SocketTask::ST_RECEIVED)//last task finished
		{

			SocketReceiveTask* reTask = dynamic_cast<SocketReceiveTask*>(mCurrentTask);
			if(reTask)
			{
				PacketManager::Get()->_onReceivedPacket(reTask->getData().buffer,reTask->getData().length);
			}
			setState(SS_WAIT);
			delete mCurrentTask;//mThread.shutdown();
			mCurrentTask = 0;
		}
	}
	if(getState() == SS_WAIT)
	{
		if(mCurrentTask)//last task finished
		{
			delete mCurrentTask;//mThread.shutdown();
			mCurrentTask = 0;
		}
		//should after delete current task
		if(mForceShutDone)
		{
			AsyncSocket& socket = lockSocket();
			socket.onClose();
#ifdef WIN32
			WSACleanup();
#endif
			releaseSocket();
			setState(SS_UNINITIALIZED);
			mForceShutDone = false;
		}

		if(mTaskList.empty())
		{
			static bool check = true;
			if(check)
			{
				SocketReceiveTask* reTask = new SocketReceiveTask;
				mTaskList.push_back(reTask);

			}
		}
		else
		{
			mCurrentTask = mTaskList.front();
			mTaskList.pop_front();

			while (!mTaskList.empty() && mCurrentTask->getType() == SocketTask::ST_CONNECT)
			{
				mCurrentTask = mTaskList.front();
				mTaskList.pop_front();
			}

			if (mCurrentTask->getType() == SocketTask::ST_SEND)
				setState(SS_SENDING);
			if (mCurrentTask->getType() == SocketTask::ST_RECEIVED)
				setState(SS_RECEIVING);

			mThread.execute(mCurrentTask);//mCurrentTask->run();
			//mCurrentTask->run();

		}
	}
}

void ThreadSocket::connect( const std::string& ip ,int port )
{
	mIp = ip;
	mPort = port;
	mForceShutDone = false;
	SocketConnectTask* task = new SocketConnectTask(ip,port);
	mTaskList.push_back(task);

	CCLOG("connect start!");
}
void ThreadSocket::reconnect()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (getState() != SS_CONNECT_FAILED && getState() != SS_UNINITIALIZED && getState() != SS_SEND_FAILED)
	{
		return;
	}
#endif

	disconnect();

	mForceShutDone = false;
	SocketConnectTask* task = new SocketConnectTask(mIp,mPort);
	mTaskList.push_back(task);

	CCLOG("REconnect start!");
}

void ThreadSocket::sendPacket( PacketData data, int opcode )
{
	SocketSendTask* task = new SocketSendTask(data, opcode);
	mTaskList.push_back(task);
	SocketReceiveTask* reTask = new SocketReceiveTask;
	mTaskList.push_back(reTask);
}

void ThreadSocket::disconnect()
{
	PacketManager::Get()->resetReceiveBuffer();
	if (getState() == SS_UNINITIALIZED)
		return;

	AsyncSocket& socket = lockSocket();

	if(mCurrentTask)
	{
		delete mCurrentTask;//mThread.shutdown();
		mCurrentTask = 0;
	}

	mTaskList.clear();

	socket.onClose();
#ifdef WIN32
	WSACleanup();
#endif


	releaseSocket();
	setState(SS_UNINITIALIZED);

	CCLOG("Disconnected!");
	//mForceShutDone = true;
}
