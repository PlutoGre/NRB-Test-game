
#include "stdafx.h"

#include "FightGuidePage.h"
#include "MessageBoxPage.h"
REGISTER_PAGE("FightGuidePage",FightGuidePage);
USING_NS_CC;

FightGuidePage::FightGuidePage(void)
{
}


FightGuidePage::~FightGuidePage(void)
{
	MessageManager::Get()->removeMessageHandler(this);
}

void FightGuidePage::Enter(MainFrame*)
{
	CCObject* obj=getVariable("mTitle");
	if(!obj)
	{
		char errMsg[256];
		sprintf(errMsg,"Failed to find variable:mTitle");
		CCMessageBox(errMsg,"error");
	}
}

void FightGuidePage::Exit( MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(this);
}

void FightGuidePage::Execute( MainFrame* )
{

}

void FightGuidePage::load( void )
{
	std::string _ccbName="GuidMoreMember.ccbi";
	if(mType=="GToBattle")
	{
		_ccbName="GuidMoreMember.ccbi";
	}
	else if(mType=="GToEquip")
	{
		_ccbName="GuidEquipOn.ccbi";
	}
	else
	{
		_ccbName="GuidBuyMember.ccbi";
	}
	this->removeAllChildren();
	loadCcbiFile(_ccbName);
}

void FightGuidePage::showPage(std::string _type)
{
	BasePage* obj = CCBManager::Get()->getPage("FightGuidePage");
	FightGuidePage* page = dynamic_cast<FightGuidePage*>(obj);
	if(page)
	{
		page->mType=_type;
	}
	MsgMainFramePushPage msg;
	msg.pageName="FightGuidePage";
	MessageManager::Get()->sendMessage(&msg);
	
}
void FightGuidePage::inputMethodCallBack(const std::string& content)
{
}

void FightGuidePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(mType=="GToBattle")
	{
		if(itemName=="onFunction2")
		{
			MsgMainFrameChangePage msg;
			msg.pageName="TeamPage";
			MessageManager::Get()->sendMessage(&msg);
		}
	}
	else if(mType=="GToEquip")
	{
		if(itemName=="onGoto")
		{
			MsgMainFrameChangePage msg;
			msg.pageName="TeamPage";
			MessageManager::Get()->sendMessage(&msg);
		}
	}
	else
	{
		if(itemName=="onGotoShop")
		{
			MsgMainFrameChangePage msg;
			msg.pageName="MarketPage";
			MessageManager::Get()->sendMessage(&msg);
		}
	}
	MsgMainFramePopPage popPage;
	popPage.pageName="FightGuidePage";
	MessageManager::Get()->sendMessage(&popPage);
}

void FightGuidePage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{

}


void FightGuidePage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate * msg = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(msg->opcode==OPCODE_CDKEY_REWARDRET_S)
		{
			
		}
	}
}
