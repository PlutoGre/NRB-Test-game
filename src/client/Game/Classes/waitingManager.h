#pragma once
#include "Singleton.h"
#include "PacketManager.h"

class waitingManager
	:	public Singleton<waitingManager>
	,	public PacketHandler
{
protected:
	float mStillWaitingTime;
	std::map<int,int> mReasons;
	friend class Singleton<waitingManager>;
	waitingManager(void);;
	void _decreaseReason(int opcode);
public:
	void update(float dt);
	bool getWaitting();

	void startWaiting(int opcode_to_hide, float showtime = 10.0f);

	virtual void onReceivePacket(const int opcode, const ::google::protobuf::Message* packet);
	virtual void onSendPacketFailed(const int opcode);
	virtual void onConnectFailed(std::string ip, int port){};
	virtual void onTimeout(const int opcode){};
	virtual void onPacketError(const int opcode){};
	virtual Handler_Type getHandleType(void) {return Double_Handler;};
};

