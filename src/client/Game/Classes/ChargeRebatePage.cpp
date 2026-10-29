
#include "stdafx.h"

#include "ChargeRebatePage.h"
#include "TimeCalculator.h"
#include "GameMaths.h"
#include "PackPreviewPage.h"
#define FINAL_KILL_INDEX 999
#define DAY_TIME_SECONDS 24*60*60

USING_NS_CC;

ChargeRebatePage::~ChargeRebatePage(){

	CCLOG("~ChargeRebate");
}

void ChargeRebatePage::refreshContent( void )
{

	chargeColumnNode = dynamic_cast<cocos2d::CCNode* >(this->getObjectByVariableName("chargeColumnNode"));

	m2 = dynamic_cast<cocos2d::CCNode* >(this->getObjectByVariableName("m2"));
	m1 = dynamic_cast<cocos2d::CCNode* >(this->getObjectByVariableName("m1"));

	mChargeRebateContent = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getObjectByVariableName("mChargeRebateContent"));
	mMaxRecharge = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getObjectByVariableName("mMaxRecharge"));
	mMinRecharge = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getObjectByVariableName("mMinRecharge"));
	mTimeCount = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getObjectByVariableName("mTimeCount"));

	mRechargeBtn = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mRechargeBtn"));
	mRecharge = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getObjectByVariableName("mRecharge"));

	rechargeColumn = dynamic_cast<cocos2d::extension::CCScale9Sprite* >(this->getObjectByVariableName("rechargeColumn"));
	rechargeBG = dynamic_cast<CCSprite* >(this->getObjectByVariableName("rechargeBG"));

	mRewardBtn = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mRewardBtn"));
	mReceiveReward = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getObjectByVariableName("mReceiveReward"));

	
	mDuringDays = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getObjectByVariableName("mDuringDays"));
	mLastDays = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getObjectByVariableName("mLastDays"));

	
	mReceiveRewardContent1 = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getObjectByVariableName("mReceiveRewardContent1"));
	mReceiveRewardContent2 = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getObjectByVariableName("mReceiveRewardContent2"));
	mReceiveRewardContent3 = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getObjectByVariableName("mReceiveRewardContent3"));

	mChargeRebateContent->setVisible(false);
	mMaxRecharge->setVisible(false);
	mMinRecharge->setVisible(false);
	mTimeCount->setVisible(false);
	mRechargeBtn->setVisible(false);
	mRecharge->setVisible(false);
	chargeColumnNode->setVisible(false);

	mRewardBtn->setVisible(false);
	mReceiveReward->setVisible(false);
	mDuringDays->setVisible(false);
	mLastDays->setVisible(false);
	mReceiveRewardContent1->setVisible(false);
	mReceiveRewardContent2->setVisible(false);
	mReceiveRewardContent3->setVisible(false);
	
}

void ChargeRebatePage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{
	
	if(itemName=="onRechargeBtn")
	{
		MsgMainFramePushPage pushPage;
		pushPage.pageName="RechargePage";
		MessageManager::Get()->sendMessage(&pushPage);
		
	}else if(itemName=="onRewardBtn"){

		OPChargeRebate msg;
		msg.set_version(1);
		msg.set_getrebate(1);
		PacketManager::Get()->sendPakcet(OPCODE_CHARGE_REBATE_C, &msg);
		mRewardBtn->setEnabled(true);
		
	}
	
}

void ChargeRebatePage::loadData()
{					  	
	
	if(!getIsInitData())
	{
		OPChargeRebate msg;
		msg.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_CHARGE_REBATE_C,&msg);
	}
	
}

void ChargeRebatePage::updatePage(bool isSelect,float dt)
{
	
	if(isSelect&&getIsInitData())
	{
		setPageData();
	}
	
}
void ChargeRebatePage::setPageData()
{
	
	const ChargeRebateItem& chargeRebateItem = ServerDateManager::Get()->getChargeRebateItem();
//	if(chargeRebateItem.){
	if(true){
		if(chargeRebateItem.chargeType == ChargeRebateItem::CHARGE){
			m2->setVisible(true);
			m1->setVisible(false);

			int days = (chargeRebateItem.rebateEndTime - chargeRebateItem.rebateBeginTime)/(3600*24);

			std::list<std::string> _list;
			_list.push_back(StringConverter::toString(chargeRebateItem.minRecharge).c_str());
			_list.push_back(StringConverter::toString(chargeRebateItem.minRecharge*chargeRebateItem.refund*chargeRebateItem.exchangeRate/chargeRebateItem.rebateDuringDays).c_str());
			_list.push_back(StringConverter::toString(chargeRebateItem.rebateDuringDays).c_str());
			_list.push_back(StringConverter::toString(chargeRebateItem.maxRecharge*chargeRebateItem.refund*chargeRebateItem.exchangeRate/chargeRebateItem.rebateDuringDays).c_str());

			std::string outstr;
			int lines;
			int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("ChargeRebateContent"));
			GameMaths::stringAutoReturn(GameMaths::replaceStringWithStringList(Language::Get()->getString("@ChargeRebateContent"),&_list),outstr,width,lines);

			mChargeRebateContent->setString(outstr.c_str());

			mMinRecharge->setString(StringConverter::toString(0).c_str());

			if(chargeRebateItem.rechargeInfact<=chargeRebateItem.minRecharge){
				rechargeColumn->setScaleY(rechargeBG->getScaleY()*(chargeRebateItem.rechargeInfact*1.0/chargeRebateItem.minRecharge));
				mMaxRecharge->setString(StringConverter::toString(chargeRebateItem.minRecharge).c_str());
			}else if(chargeRebateItem.rechargeInfact>chargeRebateItem.minRecharge && chargeRebateItem.rechargeInfact<=chargeRebateItem.maxRecharge){
				rechargeColumn->setScaleY(rechargeBG->getScaleY());
				mMaxRecharge->setString(StringConverter::toString(chargeRebateItem.rechargeInfact).c_str());

			}else if(chargeRebateItem.rechargeInfact>chargeRebateItem.maxRecharge){
				rechargeColumn->setScaleY(rechargeBG->getScaleY());
				mMaxRecharge->setString(StringConverter::toString(chargeRebateItem.maxRecharge).c_str());
			}

			long timeleft;
			if(TimeCalculator::Get()->getTimeLeft("chargerebate_recurittime",timeleft))
				translateTime(timeleft); 

			mChargeRebateContent->setVisible(true);
			mMaxRecharge->setVisible(true);
			mMinRecharge->setVisible(true);
			mTimeCount->setVisible(true);
			mRechargeBtn->setVisible(true);
			mRecharge->setVisible(true);
			chargeColumnNode->setVisible(true);

			mRewardBtn->setVisible(false);
			mReceiveReward->setVisible(false);
			mDuringDays->setVisible(false);
			mLastDays->setVisible(false);
			mReceiveRewardContent1->setVisible(false);
			mReceiveRewardContent2->setVisible(false);
			mReceiveRewardContent3->setVisible(false);

		}else if(chargeRebateItem.chargeType == ChargeRebateItem::REBATE){
			m2->setVisible(false);
			m1->setVisible(true);

			std::list<std::string> duringList;
			duringList.push_back(StringConverter::toString((chargeRebateItem.servertime - chargeRebateItem.rebateBeginTime)/(3600*24)+1).c_str());
			mDuringDays->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@ChargeRebateDuringDays"),&duringList).c_str());

			std::list<std::string> lastList;
			lastList.push_back(StringConverter::toString((chargeRebateItem.rebateEndTime - chargeRebateItem.servertime)/(3600*24)).c_str());
			mLastDays->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@ChargeRebateLastDays"),&lastList).c_str());

			if(chargeRebateItem.getRewardToday == 1){
				mRewardBtn->setEnabled(false);
				mReceiveReward->setString(Language::Get()->getString("@AlreadyGetReward").c_str());

			}else if(chargeRebateItem.getRewardToday == 0){
				mRewardBtn->setEnabled(true);
				mReceiveReward->setString(Language::Get()->getString("@ReceiveReward").c_str());
			}

			std::list<std::string> content1List;
			content1List.push_back(StringConverter::toString(chargeRebateItem.refundGoldCoins).c_str());
			mReceiveRewardContent1->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@ReceiveRewardContent1"),&content1List).c_str());

			std::list<std::string> content3List;
			content3List.push_back(StringConverter::toString(chargeRebateItem.refundGoldCoins).c_str());
			mReceiveRewardContent3->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@ReceiveRewardContent1"),&content3List).c_str());

			std::list<std::string> content2List;
			content2List.push_back(StringConverter::toString((chargeRebateItem.rebateEndTime - chargeRebateItem.rebateBeginTime)/(3600*24)).c_str());
			std::string outstr;
			int lines;
			int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("ChargeRebateContent"));
			GameMaths::stringAutoReturn(GameMaths::replaceStringWithStringList(Language::Get()->getString("@ReceiveRewardContent2"),&content2List),outstr,width,lines);
			mReceiveRewardContent2->setString(outstr.c_str());

			mChargeRebateContent->setVisible(false);
			mMaxRecharge->setVisible(false);
			mMinRecharge->setVisible(false);
			mTimeCount->setVisible(false);
			mRechargeBtn->setVisible(false);
			mRecharge->setVisible(false);
			chargeColumnNode->setVisible(false);

			mRewardBtn->setVisible(true);
			mReceiveReward->setVisible(true);
			mDuringDays->setVisible(true);
			mLastDays->setVisible(true);
			mReceiveRewardContent1->setVisible(true);
			mReceiveRewardContent2->setVisible(true);
			mReceiveRewardContent3->setVisible(true);
		}
	}
	
}

void ChargeRebatePage::translateTime( int seconds)
{
	
	
	if(seconds<=0){
	 	ChargeRebateItem& chargeRebateItem = ServerDateManager::Get()->getChargeRebateItem();
		if(chargeRebateItem.chargeType = ChargeRebateItem::CHARGE){
			if(chargeRebateItem.rechargeInfact>=chargeRebateItem.minRecharge){
				chargeRebateItem.chargeType = ChargeRebateItem::REBATE;
			}else if(chargeRebateItem.rechargeInfact<chargeRebateItem.minRecharge){
				if(showFlag){
					MsgAdventureRemoveItem msg;
					msg.index = mID;
					MessageManager::Get()->sendMessage(&msg);
					showFlag = false;
				}
			}
		}
	}else{
	
		int day = seconds/(3600*24);
		int hh = (seconds/3600)%24;
		int mm = (seconds-3600*hh-3600*24*day)/60;
		int ss = seconds%60;

		std::list<std::string> _list;
		_list.push_back(StringConverter::toString(day).c_str());
		_list.push_back(StringConverter::toString(hh).c_str());
		_list.push_back(StringConverter::toString(mm).c_str());
		_list.push_back(StringConverter::toString(ss).c_str());

		mTimeCount->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@ChargeRebateTimeLeft"),&_list).c_str());   
	}
	
}

void ChargeRebatePage::exitPage()
{

}

