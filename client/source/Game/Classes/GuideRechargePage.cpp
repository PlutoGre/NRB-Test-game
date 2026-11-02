
#include "stdafx.h"

#include "GuideRechargePage.h"
#include "MessageBoxPage.h"
REGISTER_PAGE("GuideRechargePage",GuideRechargePage);
USING_NS_CC;

GuideRechargePage::GuideRechargePage(void)
{
}


GuideRechargePage::~GuideRechargePage(void)
{
	MessageManager::Get()->removeMessageHandler(this);
}

void GuideRechargePage::Enter(MainFrame*)
{
	
}

void GuideRechargePage::Exit( MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(this);
}

void GuideRechargePage::Execute( MainFrame* )
{

}

void GuideRechargePage::load( void )
{
	loadCcbiFile("GuidFirstPay.ccbi");
}

void GuideRechargePage::showPage()
{
	MsgMainFramePushPage msg;
	msg.pageName="GuideRechargePage";
	MessageManager::Get()->sendMessage(&msg);
	
}
void GuideRechargePage::inputMethodCallBack(const std::string& content)
{
}

void GuideRechargePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onRecharge")
	{
		MsgMainFramePushPage pushPage;
		pushPage.pageName="RechargePage";
		MessageManager::Get()->sendMessage(&pushPage);
	}
	MsgMainFramePopPage popPage;
	popPage.pageName="GuideRechargePage";
	MessageManager::Get()->sendMessage(&popPage);
}

void GuideRechargePage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{

}


void GuideRechargePage::onReceiveMassage( const GameMessage * message )
{
	
}
