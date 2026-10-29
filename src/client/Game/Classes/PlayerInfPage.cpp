
#include "stdafx.h"

#include "PlayerInfPage.h"
#include "BattleArrayInfo.h"
#include "TimeCalculator.h"
REGISTER_PAGE("PlayerInfPage",PlayerInfPage);
USING_NS_CC;

PlayerInfPage::PlayerInfPage(void)
{
}


PlayerInfPage::~PlayerInfPage(void)
{
}

void PlayerInfPage::Enter( MainFrame* frame)
{
	refreshPage();
}

void PlayerInfPage::Exit( MainFrame* frame)
{
}

void PlayerInfPage::Execute( MainFrame* )
{
	refreshTimeCount();
}

void PlayerInfPage::load( void )
{
	loadCcbiFile("PlayerInf.ccbi");
	initPage();
}

 void PlayerInfPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender)
 {
	 if(itemName=="onClose")
	 {
		 MsgMainFramePopPage mPopPage;
		 mPopPage.pageName = "PlayerInfPage";
		 MessageManager::Get()->sendMessage(&mPopPage);
	 }
 }

 void PlayerInfPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
 {
 }

void PlayerInfPage::initPage()
{
	mName =  dynamic_cast<cocos2d::CCLabelTTF* >(getVariable("mName"));
	mVIPLV = dynamic_cast<cocos2d::CCSprite* >(getVariable("mVIPLV"));
	mLV = dynamic_cast<cocos2d::CCLabelBMFont* >(getVariable("mLV"));
	mWeiwang = dynamic_cast<cocos2d::CCLabelBMFont* >(getVariable("mWeiwang"));
	mDizishang = dynamic_cast<cocos2d::CCLabelBMFont* >(getVariable("mDizishang"));
	mDiziyou = dynamic_cast<cocos2d::CCLabelBMFont* >(getVariable("mDiziyou"));
	mGold = dynamic_cast<cocos2d::CCLabelBMFont* >(getVariable("mGold"));
	mCoin = dynamic_cast<cocos2d::CCLabelBMFont* >(getVariable("mCoin"));
	mTi = dynamic_cast<cocos2d::CCLabelBMFont* >(getVariable("mTi"));
	mTihuifu = dynamic_cast<cocos2d::CCLabelBMFont* >(getVariable("mTihuifu"));
	mTiquanhuifu = dynamic_cast<cocos2d::CCLabelBMFont* >(getVariable("mTiquanhuifu"));
	mQi = dynamic_cast<cocos2d::CCLabelBMFont* >(getVariable("mQi"));
	mQihuifu = dynamic_cast<cocos2d::CCLabelBMFont* >(getVariable("mQihuifu"));
	mQiquanhuifu = dynamic_cast<cocos2d::CCLabelBMFont* >(getVariable("mQiquanhuifu"));
}
void PlayerInfPage::refreshTimeCount()
{
	const UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	long timeleft;
	if(!TimeCalculator::Get()->getTimeLeft("nextPowerRecoverTime",timeleft))
	{
		timeleft=0;
	}
	mTi->setString(StringConverter::toString(userBasicInfo.power).c_str());
	mTihuifu->setString(GameMaths::formatSecondsToTime(timeleft).c_str());
	if(!TimeCalculator::Get()->getTimeLeft("allPowerRecoverTime",timeleft))
	{
		timeleft=0;
	}
	mTiquanhuifu->setString(GameMaths::formatSecondsToTime(timeleft).c_str());

	
	mQi->setString(StringConverter::toString(userBasicInfo.vitality).c_str());
	if(!TimeCalculator::Get()->getTimeLeft("nextVitalityRecoverTime",timeleft))
	{
		timeleft=0;
	}
	mQihuifu->setString(GameMaths::formatSecondsToTime(timeleft).c_str());
	if(!TimeCalculator::Get()->getTimeLeft("allVitalityRecoverTime",timeleft))
	{
		timeleft=0;
	}
	mQiquanhuifu->setString(GameMaths::formatSecondsToTime(timeleft).c_str());
}
void PlayerInfPage::refreshPage()
{
	const UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	mName->setString(userBasicInfo.name.c_str());
	if(mVIPLV)
	{

		const std::string vipImage = ::getVipLevelImageFile(userBasicInfo.viplevel);
		if(vipImage!="")
		{
			mVIPLV->setVisible(true);
			mVIPLV->initWithFile(vipImage.c_str());
		}else
		{
			mVIPLV->setVisible(false);
		}
	}
	mLV->setString(StringConverter::toString(userBasicInfo.level).c_str());
	mWeiwang->setString(ServerDateManager::Get()->getUserExpPercentStr().c_str());
	
	const unsigned int userBattleSize = ServerDateManager::Get()->getUserBattleSize();
	mDizishang->setString(StringConverter::toString(userBattleSize).c_str());
	mDiziyou->setString(StringConverter::toString(BattleArrayInfo::Get()->getBattleEnableSize()).c_str());

	mGold->setString(StringConverter::toString(userBasicInfo.goldcoins).c_str());
	mCoin->setString(StringConverter::toString(userBasicInfo.silvercoins).c_str());
	ServerDateManager::Get()->getBasicNextTime();
	refreshTimeCount();
}


