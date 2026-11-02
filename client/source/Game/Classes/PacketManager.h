#pragma once
#ifndef __PacketHandler_H_
#define __PacketHandler_H_

namespace google{namespace protobuf{class Message;}}

#include "Singleton.h"
#include "AsyncSocket.h"
#include "cocos2d.h"

#include <map>
#include <set>
#include <string>

class PacketBase;
class PacketFactoryBase;

class PacketHandler
{
public:
	enum Handler_Type
	{
		Default_Handler,
		Scripty_Handler,
		Double_Handler,
	};
	virtual void onReceivePacket(const int opcode, const ::google::protobuf::Message* packet){};
	virtual void onReceivePacket(const int opcode, const std::string& str){};
	virtual void onSendPacketFailed(const int opcode) = 0;
	virtual void onConnectFailed(std::string ip, int port) = 0;
	virtual void onTimeout(const int opcode) = 0;
	virtual void onPacketError(const int opcode) = 0;
	virtual Handler_Type getHandleType(void) {return Default_Handler;};
};

class PacketScriptHandler : public cocos2d::CCObject, public PacketHandler 
{
public:
	PacketScriptHandler(int opcode, int nHandler);
	virtual ~PacketScriptHandler();
	virtual void onReceivePacket(const int opcode, const ::google::protobuf::Message* packet);;
	virtual void onReceivePacket(const int opcode, const std::string& str);;

	virtual void onSendPacketFailed(const int opcode);;
	virtual void onConnectFailed(std::string ip, int port);;
	virtual void onTimeout(const int opcode);;
	virtual void onPacketError(const int opcode);;
	virtual Handler_Type getHandleType(void) {return Scripty_Handler;};

	//for lua
	int getRecPacketBufferLength(){return mPktBuffer.length();}
	std::string  getRecPacketBuffer(){return mPktBuffer;}

private:

	int mRecOpcode;
	std::string mPktBuffer;

	int mScriptFunHandler;
};

class PacketManager : public Singleton<PacketManager>
{
public:
	PacketManager(void);
	~PacketManager(void);
	static PacketManager* getInstance();

	enum
	{
		DEFAULT_BUFFER_LENGTH = 204800,//200k
	};
	void init(const std::string& configFile);
	void init(const std::string& ip, int port);
	void update(float dt);
	void disconnect();

	bool regisiterPacketHandler(int opcode,PacketHandler*);
	
	int nameToOpcode(const std::string& name);
	void removePacketHandler(int opcode, PacketHandler* packethandler);
	void removePacketHandler(PacketHandler* packethandler);
	
	void sendPakcet(int opcode, ::google::protobuf::Message*, bool needWaiting=true);
	void sendPakcet(int opcode, char* buff, int length, bool needWaiting = true);

	void _onReceivedPacket(void* buffer, int size);
	void resetReceiveBuffer() { mReceiveBuffer.clear(); }
	void _checkReceivePacket();
	bool _regisiterPacketFactory(int opcode, const std::string& packetName,  PacketFactoryBase*);
	bool _buildDefaultMessage(int opcode, ::google::protobuf::Message*);
	::google::protobuf::Message* _getDefaultMessage(int opcode);
	void _boardcastPacketToHandler( int id, const ::google::protobuf::Message* msg, const std::string& msgStr=0);
	void _boardcastConnectionFailed(std::string ip, int port);
	void _boardcastSendFailed(int opcode);
	void _boardcastReceiveTimeout(int opcode);
	void _boardcastPacketError(int opcode);

	void setNeedReConnect() {mNeedReConnect = true;};
private:
	AsyncSocket mySocket;
	friend class pressureTest;
	PacketBase* createPacket(int opcode);

	typedef std::map<int, PacketFactoryBase*> PACKET_FACTORY_MAP;
	PACKET_FACTORY_MAP mFactories;

	typedef std::map<int,std::set<PacketHandler*> > PACKET_HANDLER_MAP;
	PACKET_HANDLER_MAP mHandlers;


	typedef std::map<std::string, int> NAME_TO_OPCODE_MAP;
	NAME_TO_OPCODE_MAP mNameToOpcode;

	typedef std::map<int, ::google::protobuf::Message*> DEFAULT_MESSAGE_MAP;
	DEFAULT_MESSAGE_MAP mDefaultMessageMap;

	bool mNeedReConnect;
	std::string mReceiveBuffer;
};

#endif//__PacketHandler_H_
