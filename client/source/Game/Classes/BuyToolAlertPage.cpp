
#include "stdafx.h"

#include "StoneUpgradePage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Equip.h"
#include "math.h"
#include "BuyToolAlertPage.h"

REGISTER_PAGE("BuyToolAlertPage",BuyToolAlertPage);
USING_NS_CC;

BuyToolAlertPage::BuyToolAlertPage(void)
{
	refreshPage();
}

BuyToolAlertPage::~BuyToolAlertPage(void)
{
}

void BuyToolAlertPage::Enter( MainFrame* frame)
{
	refreshPage();

}

void BuyToolAlertPage::Exit( MainFrame* frame)
{

}

void BuyToolAlertPage::Execute( MainFrame* )
{

}

void BuyToolAlertPage::load( void )
{
	loadCcbiFile("StoneSystemDemolitionStone.ccbi");
//	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mEquipContent"));
//	membersRootNode = members->getContainer();

}

void BuyToolAlertPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName == "onClose"){
		MsgMainFramePopPage msg;
		msg.pageName="BuyToolAlertPage";
		MessageManager::Get()->sendMessage(&msg);
	}else if(itemName == "onButton"){
		mSure();
		MsgMainFramePopPage msg;
		msg.pageName="BuyToolAlertPage";
		MessageManager::Get()->sendMessage(&msg);

	}

}
void BuyToolAlertPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag)
{


}

void BuyToolAlertPage::refreshPage( void )
{		
	CCB_FUNC(this,"mTitle",cocos2d::CCLabelBMFont,setString(titleStr.c_str()));
	CCB_FUNC(this,"mName",cocos2d::CCLabelBMFont,setString(nameStr.c_str()));
	CCB_FUNC(this,"mDescription",cocos2d::CCLabelBMFont,setString(descriptionStr.c_str()));
	CCB_FUNC(this,"mNumber1",cocos2d::CCLabelBMFont,setString(number1Str.c_str()));
	CCB_FUNC(this,"mSure",cocos2d::CCLabelBMFont,setString(sureStr.c_str()));
	CCB_FUNC(this,"mPic",cocos2d::CCSprite,initWithFile(picStr.c_str()));

}

