
#include "stdafx.h"

#include "LeaveMsgPage.h"
REGISTER_PAGE("LeaveMsgPage",LeaveMsgPage);
USING_NS_CC;

LeaveMsgPage::LeaveMsgPage(void)
{
	mGMList.push_back("/add");
	mGMList.push_back("/to");
	mGMList.push_back("/tomorrow");
	mGMList.push_back("/gamble");
	mGMList.push_back("/darkroom");
	mGMList.push_back("/compensate");
}


LeaveMsgPage::~LeaveMsgPage(void)
{
	libOS::getInstance()->removeListener(this);
	MessageManager::Get()->removeMessageHandler(this);
}

void LeaveMsgPage::Enter()
{
	MessageManager::Get()->regisiterMessageHandler(MSG_LEAVEMSG_RECIEVEMSG,this);
	libOS::getInstance()->registerListener(this);
	CCObject* obj=getVariable("mTitle");
	if(!obj)
	{
		char errMsg[256];
		sprintf(errMsg,"Failed to find variable:mTitle");
		CCMessageBox(errMsg,"error");
	}
	mTitle =  dynamic_cast<cocos2d::CCLabelBMFont* >(obj);
	if(mTitle)
	{
		mTitle->setString(Language::Get()->getString(mUserName).c_str());
	}
	mMessage =  dynamic_cast<cocos2d::CCLabelBMFont* >(getVariable("mMessage"));
}

void LeaveMsgPage::Exit( MainFrame* frame)
{
	libOS::getInstance()->removeListener(this);
	MessageManager::Get()->removeMessageHandler(this);
}

void LeaveMsgPage::Execute( MainFrame* )
{

}

void LeaveMsgPage::load( void )
{
	loadCcbiFile("LeaveMsgPage.ccbi");
	Enter();
}

void LeaveMsgPage::initLeaveMsg(void)
{
	if(mTitle)
	{
		mTitle->setString(Language::Get()->getString(mUserName).c_str());
	}
}

void LeaveMsgPage::onInputboxEnter(const std::string& content)
{
	inputMethodCallBack(content);
}

void LeaveMsgPage::inputMethodCallBack(const std::string& content)
{
	mInputStr=content;
	mMessage->setString(mInputStr.c_str());
}

void LeaveMsgPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onXClose" || itemName == "onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName="LeaveMsgPage";
		MessageManager::Get()->sendMessage(&msg);

		libOS::getInstance()->removeListener(this);
	}
	else if(itemName=="onCloseButton")
	{
		MsgMainFramePopPage msg;
		msg.pageName="LeaveMsgPage";
		MessageManager::Get()->sendMessage(&msg);

		libOS::getInstance()->removeListener(this);
	}
	else if(itemName=="onSendMsgButton")
	{
		
		bool isGMCommond=false;
		bool isGM=false;
#ifdef _DEBUG
		isGM=true;
#endif
		if(isGM)
		{
			GMKeyVec::iterator it=mGMList.begin();
			for(;it!=mGMList.end();++it)
			{
				std::string::size_type index=mInputStr.find(*it);
				if(index!=mInputStr.npos)
				{
					isGMCommond=true;
					break;
				}
			}
		}
		libOS::getInstance()->removeListener(this);
		if(isGMCommond&&isGM)
		{
			OPUpdateUserInfoByGM msg;
			msg.set_version(1);
			msg.set_msg(mInputStr);
			PacketManager::Get()->sendPakcet(OPCODE_UPDATE_USERINFO_BY_GM_C,&msg);
		}
		else
		{
			OPSendMessage info;
			info.set_version(1);
			info.set_uid(mUserId);
			info.set_msg(mInputStr);
			PacketManager::Get()->sendPakcet(OPCODE_SEND_FRIEND_MESSAGE_C,&info);
			MsgMainFramePopPage msg;
			msg.pageName="LeaveMsgPage";
			MessageManager::Get()->sendMessage(&msg);
		}
		
	}
	else if(itemName=="onTouchInput")
	{
		libOS::getInstance()->showInputbox(true);
	}
}

void LeaveMsgPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{

}


void LeaveMsgPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_LEAVEMSG_RECIEVEMSG)
	{
		const MsgLeaveMsgReceivePage * msg = dynamic_cast<const MsgLeaveMsgReceivePage*>(message);
		if(msg)
		{
			mUserId=msg->uID;
			mUserName=msg->uName;
			initLeaveMsg();
		}
	}
}
