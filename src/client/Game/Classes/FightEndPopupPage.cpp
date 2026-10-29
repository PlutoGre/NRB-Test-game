
#include "stdafx.h"

#include "FightEndPopupPage.h"
REGISTER_PAGE("FightEndPopupPage",FightEndPopupPage);
USING_NS_CC;
FightEndPopupPage::FightEndPopupPage(void)
{

}

FightEndPopupPage::~FightEndPopupPage(void)
{
}

void FightEndPopupPage::load( void )
{
	loadCcbiFile("FightEndPopUp.ccbi");
	mNumber=dynamic_cast<CCLabelBMFont* >(getVariable("mNumber"));
	mTitle=dynamic_cast<CCLabelBMFont* >(getVariable("mTitle"));
	mSummary=dynamic_cast<CCLabelBMFont* >(getVariable("mSummary"));
	mNote1=dynamic_cast<CCLabelBMFont* >(getVariable("mNote1"));
}

void FightEndPopupPage::showPage()
{
	MsgMainFramePushPage msg;
	msg.pageName="FightEndPopupPage";
	MessageManager::Get()->sendMessage(&msg);
}

void FightEndPopupPage::initPage()
{
	const AdventureFightEndInfo* info=ServerDateManager::Get()->getAdventureFightEndInfo();
	if(mTitle)
	{
		mTitle->setString(Language::Get()->getString("@FightEndPopupLoseTitle").c_str());
	}
	if(mSummary)
	{
		std::list<std::string> replaceList;
		replaceList.push_back(StringConverter::toString(info->mTodayFightInfo.currBarrier-1));
		mSummary->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@FightEndPopupSummary"),&replaceList).c_str());
	}
	if(mNumber)
	{
		mNumber->setString(StringConverter::toString(info->mTodayFightInfo.currObtainStar).c_str());
	}
}

void FightEndPopupPage::Enter( MainFrame* )
{
	initPage();
	
}

void FightEndPopupPage::Execute( MainFrame* )
{
	
}

void FightEndPopupPage::Exit( MainFrame* )
{
	std::string fightLoseKey="adventureFightEndRet";
	if(BlackBoard::Get()->hasVarible(fightLoseKey))
	{
		BlackBoard::Get()->setVarible(fightLoseKey,true);
	}
	else
	{
		BlackBoard::Get()->addVarible(fightLoseKey,true);
	}	
}

void FightEndPopupPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	bool isClose=false;
	if(itemName=="onClose")
	{
		isClose=true;
	}
	else if(itemName=="onCofirm")
	{
		isClose=true;
	}
	if(isClose)
	{
		MsgMainFramePopPage msg;
		msg.pageName="FightEndPopupPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void FightEndPopupPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{

}
