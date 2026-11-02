
#include "stdafx.h"

#include "PacketBase.h"
#include "GameMaths.h"
#include <string>
#include <google/protobuf/message.h>
#include "cocos2d.h"



PacketBase::PacketBase( ::google::protobuf::Message* _message, int opcode)
	:mProtoMessage(_message)
	,mOpcode(opcode)
{

}

PacketBase::~PacketBase( void )
{
	if(mProtoMessage)
		delete mProtoMessage;
}

bool PacketBase::UnpackPacket( void* buffer , int _length )
{
	if(!mProtoMessage)
		return false;
	try
	{

		char* pBuf = (char*)buffer;
		unsigned char* pBuff = (unsigned char*)buffer;
		//encoding
		for(int i=0;i<_length;++i)
		{
			pBuff[i]=pBuff[i]^0xa5;
		}
		mInfoString.assign(pBuf,_length);
		mProtoMessage->ParseFromString(mInfoString);
		std::string debugstr = mProtoMessage->DebugString();
		std::string outStr;
		GameMaths::replaceStringWithCharacter(debugstr,'\n',' ',outStr);

		size_t allLength = outStr.length();
		if(allLength>=cocos2d::kMaxLogLen + 1)
		{
			size_t stp = 0;
			CCLOG("received packet! opcode:%d message:",mOpcode);
			while (stp<allLength)
			{
				size_t length = (((allLength-stp)>cocos2d::kMaxLogLen)?cocos2d::kMaxLogLen:(allLength-stp));
				std::string out1 = outStr.substr(stp,cocos2d::kMaxLogLen);
				CCLOG(out1.c_str());
				stp+=cocos2d::kMaxLogLen;
			}
		}
		else
		{
			CCLOG("received packet! opcode:%d message:%s",mOpcode,outStr.c_str());
		}
		return true;
	}
	catch (...)
	{
		return false;
	}
	
}

std::string PacketBase::UnpackPacket(int opcode, void *buffer, int _length )
{
	char* pBuf = (char*)buffer;
	unsigned char* pBuff = (unsigned char*)buffer;
	//encoding
	for(int i=0;i<_length;++i)
	{
		pBuff[i]=pBuff[i]^0xa5;
	}
	std::string str(pBuf,_length);
	CCLOG("received packet! opcode:%d message:%s",opcode,str.c_str());
	return str;
}

void* PacketBase::PackPacket( int &_length, const ::google::protobuf::Message *msg )
{
	if(!msg)
		msg = mProtoMessage;

	if(!msg)
		return 0;

	std::string str;
	msg->SerializeToString(&str);
	
	return PackPacket(mOpcode,_length,str);
}

void* PacketBase::PackPacket(int _opcode, int &_length,const std::string& str)
{
	unsigned char* pBuff = (unsigned char*)str.c_str();
	//encoding
	for(int i=0;i<str.length();++i)
	{
		pBuff[i]=pBuff[i]^0xa5;
	}
	char* sendstr = new char[PacketHead+str.length()];//

	char head[2] = {0x43,0x46};
	int opcode = ReverseAuto<int>(_opcode);
	int length = 0;
	length = ReverseAuto<int>(str.length());

	size_t offset = 0;
	memcpy(sendstr+offset,&head,2);
	offset+=2;
	memcpy(sendstr+offset,&opcode,4);
	offset+=4;
	memcpy(sendstr+offset,&length,4);
	offset+=4;
	memcpy(sendstr+offset,str.c_str(),str.length());
	offset+=str.length();
	_length = offset;

	return sendstr;
}

int PacketBase::getOpcode()
{
	return mOpcode;
}

const ::google::protobuf::Message * PacketBase::getMessage()
{
	return mProtoMessage;
}

std::string PacketBase::getInfoString()
{
	return mInfoString;
}
