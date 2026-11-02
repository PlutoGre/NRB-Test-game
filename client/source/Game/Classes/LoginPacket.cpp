
#include "stdafx.h"

#include "LoginPacket.h"
#include "PacketManager.h"
#include "GamePackets.h"
#include "lib91.h"
#include "libOS.h"
#include "ThreadSocket.h"
#include "InstructionManager.h"

LoginPacket::LoginPacket( void )
	: mCanSend(true)
	, mWaitingTime(LoginPacketWaitingTime)
{

}

void LoginPacket::update( float dt )
{
	if(mWaitingTime <= 0)
		mCanSend = true;
	else
		mWaitingTime -= dt;
}

void LoginPacket::sendPacket()
{
	if(mCanSend) 
	{
		_sendPacket();
		mWaitingTime = LoginPacketWaitingTime;
		mCanSend = false;
	}
}

void LoginPacket::forceSentPacket()
{
	_sendPacket();
}

void LoginPacket::_sendPacket()
{
	if(!mEnable)
		return;

	ThreadSocket::Get()->reconnect();

	OPLogin loginPack;
	loginPack.set_pwd("pswd");
	loginPack.set_version(1);
	std::string uin = libPlatformManager::getPlatform()->loginUin();
	if(!uin.empty())
		loginPack.set_puid(uin);
	else
		loginPack.set_puid("111");

	std::string platformInfo = libPlatformManager::getPlatform()->getPlatformInfo();
	std::string deviceID = libOS::getInstance()->getDeviceID();

	if(!deviceID.empty())
		loginPack.set_deviceid(deviceID);

	if(!platformInfo.empty())
		loginPack.set_platform(platformInfo);

	loginPack.set_clientguidefinished(InstructionManager::Get()->loadLocalInstructionStep()>InstructionManager::NAME_STEP);//need uin

	PacketManager::Get()->sendPakcet(OPCODE_PLAYER_LOGIN_C,&loginPack);
}
