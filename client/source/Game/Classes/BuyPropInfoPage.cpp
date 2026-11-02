
#include "stdafx.h"

#include "BuyPropInfoPage.h"
#include "DataTableManager.h"
#include "StringConverter.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#include "Language.h"
#include "GameMessages.h"
#include "GameMaths.h"

REGISTER_PAGE("BuyPropInfoPage",BuyPropInfoPage);

BuyPropInfoPage::BuyPropInfoPage(void)
	:mID(-1)
{
}


BuyPropInfoPage::~BuyPropInfoPage(void)
{
}

void BuyPropInfoPage::Enter( MainFrame* )
{
	if(mID>0)
	{
		const ToolItem* item = ToolTableManager::Get()->getToolItemByID(mID);
		if(item)
		{
			CCB_FUNC(this,"mSellPrice",CCLabelBMFont,setString(StringConverter::toString(item->price).c_str()));
			std::string des;
			int lines;
			GameMaths::stringAutoReturn(item->describe,des,6,lines);
			CCB_FUNC(this,"mPropInfo",CCLabelBMFont,setString(des.c_str()));
			std::string oStr;
			GameMaths::stringAutoReturn(item->name,oStr,1,lines);
			CCB_FUNC(this,"mPropName",CCLabelBMFont,setString(oStr.c_str()));
			CCB_FUNC(this,"mPropCard",CCSprite,initWithFile(item->bodyPic.c_str()));
		}
	}
}

void BuyPropInfoPage::Execute( MainFrame* )
{

}

void BuyPropInfoPage::Exit( MainFrame* )
{

}

void BuyPropInfoPage::load( void )
{
	loadCcbiFile("PropInfo.ccbi");
}

void BuyPropInfoPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName == "onClose" || itemName == "onShutdown")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "BuyPropInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName == "onBuyProp")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "BuyPropInfoPage";
		MessageManager::Get()->sendMessage(&msg);

		MsgMainFramePushPage msg2;
		msg2.pageName = "BuyPropsCountPage";
		MessageManager::Get()->sendMessage(&msg2);
	}
}
