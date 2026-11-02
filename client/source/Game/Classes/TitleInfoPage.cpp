
#include "stdafx.h"

#include "TitleInfoPage.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "Title.h"
#include "DataTableManager.h"
#include "TitleCardPage.h"

REGISTER_PAGE("TitleInfoPage",TitleInfoPage);
USING_NS_CC;

TitleInfoPage::TitleInfoPage(void)
{
	mIsTeamView=false;
	mCard = NULL;
}

TitleInfoPage::~TitleInfoPage(void)
{
	mIsTeamView=false;
}

void TitleInfoPage::load( void )
{
	loadCcbiFile("TitleInfo.ccbi");
	mTitleCard = dynamic_cast<CCNode*>(getVariable("mTitleCard"));
	refreshPage();
	buildCard();
}

void TitleInfoPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
// 	if(itemName=="onShowAllTitle")
// 	{
// 		//È«²¿³ÆºÅ
// 		BlackBoard::Get()->ShowTitle = 0;
// 
// 		MsgMainFramePopPage msg;
// 		msg.pageName = "TitleInfoPage";
// 		MessageManager::Get()->sendMessage(&msg);
// 	}
// 	else 
	if(itemName=="onClose")
	{
		//¹Ø±Õ
		BlackBoard::Get()->ShowTitle = 0;
		MsgMainFramePopPage msg;
		msg.pageName = "TitleInfoPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void TitleInfoPage::refreshPage( void )
{
	Title title(BlackBoard::Get()->ShowTitle,mIsTeamView);
	
	CCLabelBMFont* upgradeRequirment = dynamic_cast<CCLabelBMFont* >(getVariable("mUpgradeRequirment"));
	if(upgradeRequirment)
	{
		upgradeRequirment->setString(title.getNextLevelUpCondition().c_str());
	}
	CCLabelBMFont* newQishi = dynamic_cast<CCLabelBMFont* >(getVariable("mNewQishi"));
	if(newQishi)
	{
		newQishi->setString(title.getNextQuantityString().c_str());
	}
	CCLabelBMFont* effectiveTime = dynamic_cast<CCLabelBMFont* >(getVariable("mEffectiveTime"));
	if(effectiveTime)
	{
		effectiveTime->setString(title.getLeftTimeString().c_str());
	}
	CCLabelBMFont* describe = dynamic_cast<CCLabelBMFont* >(getVariable("mDescribe"));
	if(describe)
	{
		std::string descStr= title.describe();
		if(descStr=="none")
		{
			descStr = "";
		}
		describe->setString(descStr.c_str());
	}
}

void TitleInfoPage::buildCard( void )
{
	if(mCard)
	{
		mCard->setContentID(BlackBoard::Get()->ShowTitle);
		mCard->setIsViewTeam(mIsTeamView);
		mCard->refreshContent();
	}
	else
	{
		mCard = new TitleCardPage(BlackBoard::Get()->ShowTitle,mIsTeamView);
		mCard->init();
		mTitleCard->removeAllChildren();
		mTitleCard->addChild(mCard->getContainerNode());
	}
}

void TitleInfoPage::unload( void )
{
	mTitleCard = NULL;
	delete mCard;
	mCard = NULL;
	mIsTeamView=false;
	CustomPage::unload();
}