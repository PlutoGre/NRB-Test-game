
#include "stdafx.h"

#include "waitingManager.h"
#include "libOS.h"
#include "OP.pb.h"
#include "GamePackets.h"
#include "cocos2d.h"
#include "LoginPacket.h"

waitingManager::waitingManager( void ) :mStillWaitingTime(0)
{
	PacketManager::Get()->regisiterPacketHandler(OPCODE_ERROR_RET_S, this);
}

void waitingManager::startWaiting( int opcode_to_hide, float showtime /*= 10.0f*/ )
{
	libOS::getInstance()->setWaiting(true);
    PacketManager::Get()->regisiterPacketHandler(opcode_to_hide, this);
	if(showtime>mStillWaitingTime)
		mStillWaitingTime = showtime;

	std::map<int,int>::iterator it = mReasons.find(opcode_to_hide);
	if(it == mReasons.end())
	{
		mReasons.insert(std::make_pair(opcode_to_hide,1));
	}
	else
		it->second++;

	CCLOG("waitingManager::startWaiting! opcode:%d count:%d time:%f:",opcode_to_hide,mReasons[opcode_to_hide],mStillWaitingTime);
}

void waitingManager::update( float dt )
{
	if(mReasons.size() >0)
	{
		mStillWaitingTime-=dt;
		if(mStillWaitingTime<=0)
		{
			CCLOG("waitingManager update Timeout");
			libOS::getInstance()->setWaiting(false);
			mStillWaitingTime = 0;
			PacketManager::Get()->removePacketHandler(this);

			std::map<int,int> reasons = mReasons;
			mReasons.clear();
			std::map<int,int>::iterator itr = reasons.begin();
			while (itr != reasons.end())
			{
				PacketManager::Get()->_boardcastReceiveTimeout(itr->first);
				++itr;
			}
		}
	}
}

void waitingManager::_decreaseReason( int opcode )
{
	std::map<int,int>::iterator it = mReasons.find(opcode);
	if(it != mReasons.end())
	{
		it->second--;
		CCLOG("waitingManager::decreaseReason! opcode:%d count:%d",opcode,it->second);
		if(it->second<=0)
		{
			mReasons.erase(it);
            PacketManager::Get()->removePacketHandler(opcode, this);
		}
	}
	else
		CCLOG("waitingManager::decreaseReason! opcode:%d is not find",opcode);
	if(mReasons.empty())
		libOS::getInstance()->setWaiting(false);

}

bool waitingManager::getWaitting()
{
	return !mReasons.empty();
}

void waitingManager::onReceivePacket( const int opcode, const ::google::protobuf::Message* packet )
{
	if(opcode != OPCODE_ERROR_RET_S)
		_decreaseReason(opcode);
	else
	{
		const OPErrorReporter * errorReporterRet = dynamic_cast<const OPErrorReporter*>(packet);
		_decreaseReason(errorReporterRet->errorid());
	}
}

void waitingManager::onSendPacketFailed( const int opcode )
{
	_decreaseReason(opcode+1);
}

