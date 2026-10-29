
#include "stdafx.h"

#include "BuyPropPopupPage.h"
#include "DataTableManager.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "PropInfoPage.h"
#include "BuyPropsCountPage.h"
REGISTER_PAGE("BuyPropPopupPage",BuyPropPopupPage);
USING_NS_CC;

BuyPropPopupPage::BuyPropPopupPage(void)
{
}


BuyPropPopupPage::~BuyPropPopupPage(void)
{
}

void BuyPropPopupPage::Enter()
{
	MessageManager::Get()->regisiterMessageHandler(MSG_BUY_PROP_POPUPMSG,this);

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
		//mTitle->setString(Language::Get()->getString(mUserName).c_str());
	}
	mMessage =  dynamic_cast<cocos2d::CCLabelBMFont* >(getVariable("mMessage"));
	mPropIcon=dynamic_cast<cocos2d::CCSprite* >(getVariable("mPropIcon"));
	mNum=dynamic_cast<cocos2d::CCLabelBMFont* >(getVariable("mPropNum"));
	mName=dynamic_cast<cocos2d::CCLabelBMFont* >(getVariable("mPropName"));
	mDes=dynamic_cast<cocos2d::CCLabelBMFont* >(getVariable("mPropFeatureDes"));
}

void BuyPropPopupPage::Exit( MainFrame* frame)
{

}

void BuyPropPopupPage::Execute( MainFrame* )
{

}

void BuyPropPopupPage::load( void )
{
	loadCcbiFile("BuyPropPopupPage.ccbi");
	Enter();
}

void BuyPropPopupPage::initProp(void)
{
	refreshItemInfo();
	refreshUserInfo();
}

void BuyPropPopupPage::refreshItemInfo(void)
{
	const ToolItem * toolItem = ToolTableManager::Get()->getToolItemByID(mGId);
	if(toolItem)
	{
		if(mTitle)
		{
			mTitle->setString(Language::Get()->getString(toolItem->name).c_str());
		}
		if(mMessage)
		{
			mMessage->setString(Language::Get()->getString(toolItem->describe).c_str());
		}
		if(mName)
		{
			mName->setString(Language::Get()->getString(toolItem->name).c_str());
		}
		if(mDes)
		{
			mDes->setString(Language::Get()->getString(toolItem->describe).c_str());
		}
		if(mPropIcon)
		{
			mPropIcon->setTexture(toolItem->iconPic.c_str());
		}
	}

}

void BuyPropPopupPage::refreshUserInfo(void)
{
	const UserToolInfo * userToolInfo = ServerDateManager::Get()->getUserToolInfoByItemId(mGId);
	unsigned int count=0;
	if(userToolInfo)
	{
		count=userToolInfo->count;
	}
	if(mNum)
	{
		mNum->setString(StringConverter::toString(count).c_str());
	}
}

void BuyPropPopupPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	

	if(itemName=="onBuyPropButton")
	{
		CCBContainer* ccb = CCBManager::Get()->getPage("BuyPropsCountPage");
		if(ccb)
		{
			BuyPropsCountPage* page = dynamic_cast<BuyPropsCountPage*>(ccb);
			if(page)
			{
				page->setProductID(mGId);
				page->setBuyOneOnly(false);

				MsgMainFramePushPage msg;
				msg.pageName = "BuyPropsCountPage";
				MessageManager::Get()->sendMessage(&msg);
			}
		}
	}
	else if(itemName=="onShowPropDetailButton")
	{
		PropInfoPageShow(mGId,PROP_BUY,false);
	}

	MainFrame* mf = dynamic_cast<MainFrame* >(CCBManager::Get()->getPage("MainFrame"));
	if(mf)
	{
		mf->popPage(this);
	}
}

void BuyPropPopupPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{

}


void BuyPropPopupPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_BUY_PROP_POPUPMSG)
	{
		const MsgBuyPropPopupPage * msg = dynamic_cast<const MsgBuyPropPopupPage*>(message);
		if(msg)
		{
			mGId=msg->toolId;
			initProp();
		}
	}
}