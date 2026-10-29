
#include "stdafx.h"

#include "PacketManager.h"
#include "PacketBase.h"
#include "GameMaths.h"
#include "cocos2d.h"
#include "ThreadSocket.h"
#include "json/json.h"
#include <google/protobuf/message.h>
#include "LoginPacket.h"

#ifndef _UTILITY_USE_

#include "MessageBoxPage.h"
#include "script_support/CCScriptSupport.h"
#include "CCLuaEngine.h"

#include "GamePlatform.h"
#include "waitingManager.h"

#else


unsigned char* getFileData(const char* pszFileName, const char* pszMode, unsigned long* pSize)
{
	unsigned char* pBuffer = NULL;

	*pSize = 0;
	do 
	{
		// read the file from hardware
		FILE *fp = fopen(pszFileName, pszMode);
		CC_BREAK_IF(!fp);

		fseek(fp,0,SEEK_END);
		*pSize = ftell(fp);
		fseek(fp,0,SEEK_SET);
		pBuffer = new unsigned char[*pSize];
		*pSize = fread(pBuffer,sizeof(unsigned char), *pSize,fp);
		fclose(fp);
	} while (0);

	return pBuffer;
}

#endif

USING_NS_CC;

PacketManager::PacketManager(void)
{
	mNeedReConnect = false;
}


PacketManager::~PacketManager(void)
{
}

void PacketManager::init(const std::string& configFile)
{
	std::string ip = "127.0.0.1";
	int port = 9999;
	if(configFile!="")
	{

		Json::Reader jreader;
		Json::Value root;
		unsigned long filesize;

#ifndef _UTILITY_USE_
		char* pBuffer = (char*)getFileData(cocos2d::CCFileUtils::sharedFileUtils()->fullPathForFilename(configFile.c_str()).c_str(),"rt",&filesize);
#else
		char* pBuffer = (char*)getFileData((const char*)configFile.c_str(),"rt",&filesize);
#endif
		if(!pBuffer)
		{
			char msg[256];
			sprintf(msg,"Failed open net config file: %s !!",configFile.c_str());
			cocos2d::CCMessageBox(msg,"ERROR!");
		}
		else
		{
			jreader.parse(pBuffer,root,false);
			CC_SAFE_DELETE_ARRAY(pBuffer);

			if(root["version"].asInt()<=1)
			{
				ip = root["ip"].asString();
				port = root["port"].asInt();
			}
		}
		CC_SAFE_DELETE_ARRAY(pBuffer);
	}
	ThreadSocket::Get()->disconnect();
	ThreadSocket::Get()->connect(ip.c_str(),port);
}

void PacketManager::init( const std::string& ip, int port )
{
	ThreadSocket::Get()->disconnect();
	ThreadSocket::Get()->connect(ip.c_str(),port);
}

void PacketManager::update( float dt )
{
	//_checkReceivePacket();
	ThreadSocket::Get()->update();
}

void PacketManager::disconnect()
{
	ThreadSocket::Get()->disconnect();
}

void PacketManager::sendPakcet( int opcode, ::google::protobuf::Message* msg, bool needWaiting/*=true*/)
{
#ifndef _UTILITY_USE_
	if(mNeedReConnect)
	{
		mNeedReConnect = false;
		LoginPacket::Get()->forceSentPacket();
	}
#endif
	std::string debugstr = msg->DebugString();
	std::string outStr;
	GameMaths::replaceStringWithCharacter(debugstr,'\n',' ',outStr);
	CCLOG("send packet! opcode:%d message:%s",opcode,outStr.c_str());
	PacketBase* pack = createPacket(opcode);
	int size;
	void* buffer = pack->PackPacket(size,msg);
	ThreadSocket::PacketData data;
	data.buffer=buffer;
	data.length = size;
	ThreadSocket::Get()->sendPacket(data,opcode);
	delete pack;

#ifndef _UTILITY_USE_
	if(needWaiting)
		waitingManager::Get()->startWaiting(opcode+1);
#endif
}

void PacketManager::sendPakcet( int opcode, char* buff, int length, bool needWaiting)
{
	ThreadSocket::PacketData data;
	if(length == 0)length = strlen(buff);
	std::string str(buff,length);
	void*buffGen = PacketBase::PackPacket(opcode,data.length,str);
	data.buffer = new char[data.length+1];
	memcpy(data.buffer,buffGen,data.length);
	((char*)data.buffer)[data.length]=0;
	CCLOG("send packet! opcode:%d message is in lua",opcode);
	ThreadSocket::Get()->sendPacket(data,opcode);
	delete buffGen;
#ifndef _UTILITY_USE_
	if(needWaiting)
		waitingManager::Get()->startWaiting(opcode+1);
#endif
}

void PacketManager::_onReceivedPacket( void* buffer, int len )
{
	if (!buffer || len <= 0) return;
	// TCP and the WebSocket adapter may split a header at any byte. Keep one
	// incomplete packet, with room for the next bounded socket read.
	if (len > DEFAULT_BUFFER_LENGTH || mReceiveBuffer.size() + len > 2 * DEFAULT_BUFFER_LENGTH)
	{
		CCLOG("PacketManager::_onReceivedPacket: receive buffer limit exceeded");
		mReceiveBuffer.clear();
		return;
	}
	mReceiveBuffer.append(static_cast<const char*>(buffer), len);
	while (mReceiveBuffer.size() >= PacketHead)
	{
		const char* header = mReceiveBuffer.data();
		int size, opcode;
		memcpy(&opcode, header + 2, 4);
		memcpy(&size, header + 6, 4);
		size = ReverseAuto<int>(size);
		opcode = ReverseAuto<int>(opcode);
		if (header[0] != 0x43 || header[1] != 0x46 || size < PacketHead || size > DEFAULT_BUFFER_LENGTH)
		{
			CCLOG("PacketManager::_onReceivedPacket: invalid packet header");
			mReceiveBuffer.clear();
			return;
		}
		if (mReceiveBuffer.size() < static_cast<size_t>(size)) return;
		// Remove before invoking handlers: a handler may disconnect and reset state.
		std::string packet = mReceiveBuffer.substr(0, size);
		mReceiveBuffer.erase(0, size);
		char* rec = &packet[0];
		PacketBase* pack = createPacket(opcode);
		if (pack)
		{
			if (pack->UnpackPacket(rec + PacketHead, size - PacketHead))
				_boardcastPacketToHandler(pack->getOpcode(), pack->getMessage(), pack->getInfoString());
			else
			{
				char out[128];
				sprintf(out, "Network error!\nFailed to create packet! \nopcode:%d", opcode);
				CCMessageBox(out, "error");
			}
			delete pack;
		}
		else
			_boardcastPacketToHandler(opcode, NULL, PacketBase::UnpackPacket(opcode, rec + PacketHead, size - PacketHead));
	}
}

void PacketManager::_checkReceivePacket()
{
//	char rec[DEFAULT_BUFFER_LENGTH];
//	int len = mySocket.onReceive(rec,DEFAULT_BUFFER_LENGTH);
//	_onReceivedPacket(rec,len);
}

PacketBase* PacketManager::createPacket( int opcode )
{
	PACKET_FACTORY_MAP::iterator it = mFactories.find(opcode);
	if(it==mFactories.end())
	{
		CCLOG("Can't find Packet Factory Name !");
		return 0;
	}
	return it->second->createPacket();
}

bool PacketManager::_regisiterPacketFactory( int opcode, const std::string& packetName, PacketFactoryBase* fac)
{
	CCAssert(mFactories.find(opcode)==mFactories.end(),"Packet Factory Name REDEFINED!!!");
	mFactories.insert(std::make_pair(opcode,fac));
	mNameToOpcode.insert(std::make_pair(packetName,opcode));
	return true;
}

bool PacketManager::regisiterPacketHandler( int opcode,PacketHandler* handler)
{
	if(handler == 0)
		return false;

	PACKET_HANDLER_MAP::iterator it = mHandlers.find(opcode);
	if(it == mHandlers.end())
	{
		std::set<PacketHandler*> sec;
		sec.insert(handler);
		mHandlers.insert(std::make_pair(opcode,sec));
	}
	else 
	{
		std::set<PacketHandler*>& handlerset = it->second;
		if(handlerset.find(handler)==handlerset.end())
			handlerset.insert(handler);
	}
	return true;
}


void PacketManager::_boardcastPacketToHandler( int id, const ::google::protobuf::Message* msg, const std::string& msgStr)
{
	if(msg == 0 && msgStr.empty())
		return ;

	PACKET_HANDLER_MAP::iterator it = mHandlers.find(id);
	if(it != mHandlers.end())
	{
		std::set<PacketHandler*> handlerset;
		handlerset.insert(it->second.begin(),it->second.end());

		std::set<PacketHandler*>& handlerset_ref = it->second;
		std::set<PacketHandler*>::iterator hanIt = handlerset.begin();
		for( ; hanIt!= handlerset.end(); ++hanIt)
		{
			if(handlerset_ref.find(*hanIt)!=handlerset_ref.end())
			{
// 				if(msg!=0)
// 					(*hanIt)->onReceivePacket(id,msg);
// 				else if((*hanIt)->isScriptHandle() && !msgStr.empty())
// 					(*hanIt)->onReceivePacket(id,msgStr);
				if( (*hanIt)->getHandleType() == PacketHandler::Default_Handler )
				{
					if(msg!=0)
						(*hanIt)->onReceivePacket(id,msg);
				}
				else if(!msgStr.empty() && (*hanIt)->getHandleType() == PacketHandler::Scripty_Handler)
					(*hanIt)->onReceivePacket(id,msgStr);
				else
					(*hanIt)->onReceivePacket(id,msg);
			}
		}
	}
}

void PacketManager::_boardcastConnectionFailed(std::string ip, int port)
{
	PACKET_HANDLER_MAP::iterator it = mHandlers.begin();
	for(;it != mHandlers.end();++it)
	{
		std::set<PacketHandler*> handlerset;
		handlerset.insert(it->second.begin(),it->second.end());

		std::set<PacketHandler*>& handlerset_ref = it->second;
		std::set<PacketHandler*>::iterator hanIt = handlerset.begin();
		for( ; hanIt!= handlerset.end(); ++hanIt)
		{
			(*hanIt)->onConnectFailed(ip,port);
		}
	}
	CCLOG("ConnectionFailed! ip:%s port:%d",ip.c_str(),port);
}

void PacketManager::_boardcastSendFailed( int opcode)
{
	PACKET_HANDLER_MAP::iterator it = mHandlers.begin();
	for(;it != mHandlers.end();++it)
	{
		std::set<PacketHandler*> handlerset;
		handlerset.insert(it->second.begin(),it->second.end());

		std::set<PacketHandler*>& handlerset_ref = it->second;
		std::set<PacketHandler*>::iterator hanIt = handlerset.begin();
		for( ; hanIt!= handlerset.end(); ++hanIt)
		{
			(*hanIt)->onSendPacketFailed(opcode);
		}
	}
	CCLOG("SendFailed! opcode:%d",opcode);
#ifndef _UTILITY_USE_

	MSG_BOX_LAN("@SendPacketFailed");
#endif
}

void PacketManager::_boardcastReceiveTimeout( int opcode)
{
	CCLOG("ReceiveTimeout! opcode:%d",opcode);
	PACKET_HANDLER_MAP::iterator it = mHandlers.begin();
	for(;it != mHandlers.end();++it)
	{
		std::set<PacketHandler*> handlerset;
		handlerset.insert(it->second.begin(),it->second.end());

		std::set<PacketHandler*>& handlerset_ref = it->second;
		std::set<PacketHandler*>::iterator hanIt = handlerset.begin();
		for( ; hanIt!= handlerset.end(); ++hanIt)
		{
			(*hanIt)->onTimeout(opcode);
		}
	}
}

void PacketManager::_boardcastPacketError( int opcode)
{
	PACKET_HANDLER_MAP::iterator it = mHandlers.begin();
	for(;it != mHandlers.end();++it)
	{
		std::set<PacketHandler*> handlerset;
		handlerset.insert(it->second.begin(),it->second.end());

		std::set<PacketHandler*>& handlerset_ref = it->second;
		std::set<PacketHandler*>::iterator hanIt = handlerset.begin();
		for( ; hanIt!= handlerset.end(); ++hanIt)
		{
			(*hanIt)->onPacketError(opcode);
		}
	}

#ifndef _UTILITY_USE_

	MSG_BOX_LAN("@ReceivedTimeout");
#endif
}

void PacketManager::removePacketHandler(int opcode, PacketHandler* messagehandler)
{
	if(messagehandler == 0)
		return;

	PACKET_HANDLER_MAP::iterator it = mHandlers.find(opcode);
	if(it != mHandlers.end())
	{//modify by dylan at 20130717 HZW-1764
		std::set<PacketHandler*>& handlerset = it->second;
		if(handlerset.find(messagehandler) != handlerset.end())
			handlerset.erase(messagehandler);
	}

}
void PacketManager::removePacketHandler(PacketHandler* messagehandler)
{
	if(messagehandler == 0)
		return;

	PACKET_HANDLER_MAP::iterator it = mHandlers.begin();
	for(;it!=mHandlers.end();++it)
	{
		std::set<PacketHandler*>& handlerset = it->second;
		if(handlerset.find(messagehandler)!= handlerset.end())
		{
			handlerset.erase(messagehandler);
		}
	}
}

int PacketManager::nameToOpcode( const std::string& name )
{
	int ret = -1;
	NAME_TO_OPCODE_MAP::iterator it = mNameToOpcode.find(name);
	if(it!=mNameToOpcode.end())
		ret = it->second;
	return ret;
}

bool PacketManager::_buildDefaultMessage( int opcode, ::google::protobuf::Message* msg)
{
	mDefaultMessageMap.insert(std::make_pair(opcode,msg));
	return true;
}

::google::protobuf::Message* PacketManager::_getDefaultMessage( int opcode )
{
	DEFAULT_MESSAGE_MAP::iterator it = mDefaultMessageMap.find(opcode);
	if(it!=mDefaultMessageMap.end())
	{
		return it->second;
	}
	return 0;
}

PacketManager* PacketManager::getInstance()
{
	return PacketManager::Get();
}

#ifndef _UTILITY_USE_
#define RUN_SCRIPT_FUN(funname) \
	if(mScriptFunHandler) \
{ \
	cocos2d::CCLuaEngine* pEngine = cocos2d::CCLuaEngine::defaultEngine(); \
	pEngine->executeEvent(mScriptFunHandler,funname,this,"PacketScriptHandler"); \
}
#else
#define RUN_SCRIPT_FUN(funname) 
#endif
PacketScriptHandler::PacketScriptHandler( int opcode, int nHandler ) : mRecOpcode(opcode)
	, mScriptFunHandler(nHandler)
{
	PacketManager::Get()->regisiterPacketHandler(mRecOpcode,this);
}

PacketScriptHandler::~PacketScriptHandler()
{
	PacketManager::Get()->removePacketHandler(mRecOpcode,this);

	if (mScriptFunHandler)
	{
		CCScriptEngineManager::sharedManager()->getScriptEngine()->removeScriptHandler(mScriptFunHandler);
		mScriptFunHandler = 0;
	}
}

void PacketScriptHandler::onReceivePacket( const int opcode, const ::google::protobuf::Message* packet )
{
	std::string str;
	packet->SerializeToString(&str);

	mPktBuffer = str;
	mRecOpcode = opcode;

	RUN_SCRIPT_FUN("luaReceivePacket");
}

void PacketScriptHandler::onReceivePacket( const int opcode, const std::string& str )
{
	mPktBuffer = str;
	mRecOpcode = opcode;

	RUN_SCRIPT_FUN("luaReceivePacket");
}

void PacketScriptHandler::onSendPacketFailed( const int opcode )
{
	RUN_SCRIPT_FUN("luaSendPacketFailed");
}

void PacketScriptHandler::onConnectFailed( std::string ip, int port )
{
	RUN_SCRIPT_FUN("luaConnectFailed");
}

void PacketScriptHandler::onTimeout( const int opcode )
{
	RUN_SCRIPT_FUN("luaTimeout");
}

void PacketScriptHandler::onPacketError( const int opcode )
{
	RUN_SCRIPT_FUN("luaPacketError");
}
