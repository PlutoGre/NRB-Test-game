
#include "stdafx.h"

#include "CDKeyInvitePage.h"
#include "MessageBoxPage.h"
#include "MessageManager.h"
#include "libOS.h"
REGISTER_PAGE("CDKeyInvitePage",CDKeyInvitePage);
USING_NS_CC;

CDKeyInvitePage::CDKeyInvitePage(void)
{
}


CDKeyInvitePage::~CDKeyInvitePage(void)
{
	libOS::getInstance()->removeListener(this);
	MessageManager::Get()->removeMessageHandler(this);
}

void CDKeyInvitePage::Enter(MainFrame*)
{
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
		mTitle->setString(Language::Get()->getString("@InviteKeyTitle").c_str());
	}
	mMessage =  dynamic_cast<cocos2d::CCLabelBMFont* >(getVariable("mMessage"));
}

void CDKeyInvitePage::Exit( MainFrame* frame)
{
	libOS::getInstance()->removeListener(this);
	MessageManager::Get()->removeMessageHandler(this);
}

void CDKeyInvitePage::Execute( MainFrame* )
{

}

void CDKeyInvitePage::load( void )
{
	loadCcbiFile("InviteFriendscode.ccbi");
}

void CDKeyInvitePage::onInputboxEnter( const std::string& content)
{
	inputMethodCallBack(content);
}

void CDKeyInvitePage::inputMethodCallBack(const std::string& content)
{
	mInputStr=content;
	mMessage->setString(mInputStr.c_str());
}

void CDKeyInvitePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName="CDKeyInvitePage";
		MessageManager::Get()->sendMessage(&msg);

		libOS::getInstance()->removeListener(this);
	}
	else if(itemName=="onCloseButton")
	{
		MsgMainFramePopPage msg;
		msg.pageName="CDKeyInvitePage";
		MessageManager::Get()->sendMessage(&msg);

		libOS::getInstance()->removeListener(this);
	}
	else if(itemName=="onSendMsgButton")
	{
		OPAddInviteKey msg;
		msg.set_key(mInputStr);
		PacketManager::Get()->sendPakcet(OPCODE_ADD_INVITEKEY_C,&msg);
	}
	else if(itemName=="onTouchInput")
	{
		libOS::getInstance()->showInputbox(true);
	}
}

void CDKeyInvitePage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{

}


void CDKeyInvitePage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate * msg = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(msg->opcode==OPCODE_ADD_INVITEKEYRET_S)
		{
		}
	}
}
