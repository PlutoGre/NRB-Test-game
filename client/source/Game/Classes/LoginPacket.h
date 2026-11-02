#pragma once
#include "Singleton.h"

#define LoginPacketWaitingTime 10
class LoginPacket
	:	public Singleton<LoginPacket>
{
protected:
	friend class Singleton<LoginPacket>;
	LoginPacket(void);
public:
	void update(float dt);

	void sendPacket();

	void forceSentPacket();
	
	void setEnabled(bool enable) { mEnable = enable; }
private:
	void _sendPacket();
	bool mCanSend;
	float mWaitingTime;
	bool mEnable;
};

