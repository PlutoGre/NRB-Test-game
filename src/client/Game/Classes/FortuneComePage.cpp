
#include "stdafx.h"

#include "PopupPage.h"
#include "FortuneComePage.h"
#include "TimeCalculator.h"
#include "GameMaths.h"
#include "PackPreviewPage.h"
#include "Popup1stPayTipPage.h"

USING_NS_CC;

FortuneComePage::~FortuneComePage(){

}

void FortuneComePage::refreshContent( void )
{

	mScrollNumArr = new cocos2d::extension::CCScrollView*[5];
	mScrollNumRootNode = new CCNode*[5];

	char contentName[64];
	for(int i=0;i<5;i++){

		sprintf(contentName,"mContent%d",i+1);
		mScrollNumArr[i] =  dynamic_cast<cocos2d::extension::CCScrollView*>(this->getObjectByVariableName(contentName));
		mScrollNumArr[i]->setTouchEnabled(false);
		mScrollNumRootNode[i] = mScrollNumArr[i]->getContainer();

	}

	mContent =  dynamic_cast<cocos2d::extension::CCScrollView*>(this->getObjectByVariableName("mContent6"));
	mContent->setTouchEnabled(false);

	mTimeCount = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getObjectByVariableName("mb2"));
	mDiscribe = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getObjectByVariableName("mb1"));

	needGoldNode = dynamic_cast<cocos2d::CCNode* >(this->getObjectByVariableName("mc"));
	needGoldNum = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getObjectByVariableName("mc3"));


	mUserGold = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getObjectByVariableName("mnum1"));

	mRewardBtn = dynamic_cast<cocos2d::CCMenuItemImage* >(this->getObjectByVariableName("mRewardBtn"));
	mRewardBtn->setEnabled(true);

	cardNum = 5;
	scrollNumCardTask = new ScrollNumCardTask(cardNum);

	scrollContent();

}

void FortuneComePage::scrollContent(){
	int endNum1 = 3;
	int roundNum = 3;

	int arrLength1 = roundNum*10 + endNum1 + 1;
	std::string* arr1 = new std::string[arrLength1];

	for(int i=0;i<roundNum;i++){
		for(int j=0;j<10;j++){
			
			std::list<std::string> fortuneComeRewardList;

			fortuneComeRewardList.push_back(FortuneComeRewardTableManager::Get()->getFortuneComeRewardItemByID(10*i+j+1)->name.c_str());
			fortuneComeRewardList.push_back(FortuneComeRewardTableManager::Get()->getFortuneComeRewardItemByID(10*i+j+1)->rewardGold.c_str());
			arr1[10*i+j] = GameMaths::replaceStringWithStringList(Language::Get()->getString("@FortuneComeReward"),&fortuneComeRewardList);
			
		}
	}

	for(int i=0;i<=endNum1;i++){
		
		std::list<std::string> fortuneComeRewardList;
		fortuneComeRewardList.push_back(FortuneComeRewardTableManager::Get()->getFortuneComeRewardItemByID(roundNum*10 + i+1)->name.c_str());
		fortuneComeRewardList.push_back(FortuneComeRewardTableManager::Get()->getFortuneComeRewardItemByID(roundNum*10 + i+1)->rewardGold.c_str());
		arr1[roundNum*10 + i] = GameMaths::replaceStringWithStringList(Language::Get()->getString("@FortuneComeReward"),&fortuneComeRewardList);
		

	}

	this->scrollCardContent = new ScrollNumCard(mContent,arr1,arrLength1);
	this->scrollCardContent->scrollContentView(50,1,30,false);

}

void FortuneComePage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{
	if(itemName=="onRechargeBtn")
	{
		MsgMainFramePushPage pushPage;
		pushPage.pageName="RechargePage";
		MessageManager::Get()->sendMessage(&pushPage);
		
	}else if(itemName=="onGetReward"){
		//onErnie
		
		UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
		FortuneComeItem& fortuneComeItem = ServerDateManager::Get()->getFortuneComeItem();
		if(userBasicInfo.goldcoins>=fortuneComeItem.needGold){

			needGoldNode->setVisible(false);
			OPFortuneCome msg;
			msg.set_version(1);
			msg.set_ernie(1);
			PacketManager::Get()->sendPakcet(OPCODE_FORTUNE_COME_C, &msg);
			mRewardBtn->setEnabled(false);

		}else{
			MsgMainFramePopPage msg;
			msg.pageName = "BuyPropsCountPage";
			MessageManager::Get()->sendMessage(&msg);
			if(!ServerDateManager::Get()->_getMarketInfo().firstPay)
			{
				POP_BOX(NotEnoughGold);
			}
			else
			{
				POP_1STPAY_TIP_BOX(GoldNotEnough);
			}
			/*
			MsgMainFramePushPage pushPage;
			pushPage.pageName="RechargePage";
			MessageManager::Get()->sendMessage(&pushPage);
			*/

		}
	}
}

void FortuneComePage::loadData()
{					   
	if(!getIsInitData())
	{
		OPFortuneCome msg;
		msg.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_FORTUNE_COME_C, &msg);
	}
}

void FortuneComePage::updatePage(bool isSelect,float dt)
{
	if(isSelect&&getIsInitData())
	{
		setPageData();
	}
}
void FortuneComePage::setPageData()
{
	FortuneComeItem& fortuneComeItem = ServerDateManager::Get()->getFortuneComeItem();

	needGoldNum->setString(StringConverter::toString(fortuneComeItem.needGold).c_str());

	if(!TimeCalculator::Get()->hasKey("fortunecome_recurittime"))
	{
		return;
	}
	long timeleft;
	if(TimeCalculator::Get()->getTimeLeft("fortunecome_recurittime",timeleft))
		translateTime(timeleft); 

	if(fortuneComeItem.rewardStat == 1){
		if(BlackBoard::Get()->fortuneComeFlag == 1&&BlackBoard::Get()->fortuneComeSum==0){

			MsgMainFramePushPage msg;
			msg.pageName = "FortuneComeReward";
			MessageManager::Get()->sendMessage(&msg);

	 		needGoldNode->setVisible(true);
			fortuneComeItem.rewardStat = 0;

			if(fortuneComeItem.chanceNum == 0){
				MsgAdventureRemoveItem msg;
				msg.index = mID;
				MessageManager::Get()->sendMessage(&msg);
			}
			mRewardBtn->setEnabled(true);

			mUserGold->setString(StringConverter::toString(ServerDateManager::Get()->getUserBasicInfo().goldcoins).c_str());

			std::list<std::string> duringList;
			duringList.push_back(StringConverter::toString(fortuneComeItem.lastDays).c_str());
			duringList.push_back(StringConverter::toString(fortuneComeItem.chanceNum).c_str());
			mDiscribe->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@FortuneComeContent1"),&duringList).c_str());
		}

		if(BlackBoard::Get()->fortuneComeFlag == 0&&BlackBoard::Get()->fortuneComeSum==0){
			roundNumCard(fortuneComeItem.rewardGold);
		}

		return;
	}

	mUserGold->setString(StringConverter::toString(ServerDateManager::Get()->getUserBasicInfo().goldcoins).c_str());

	std::list<std::string> duringList;
	duringList.push_back(StringConverter::toString(fortuneComeItem.lastDays).c_str());
	duringList.push_back(StringConverter::toString(fortuneComeItem.chanceNum).c_str());
	mDiscribe->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@FortuneComeContent1"),&duringList).c_str());



}

void FortuneComePage::translateTime( int seconds)
{
	if(seconds<=0){

		MsgAdventureRemoveItem msg;
		msg.index = mID;
		MessageManager::Get()->sendMessage(&msg);

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

void FortuneComePage::roundNumCard(int rewardGold){

	int num = rewardGold;
	for(int i=0;i<5;i++){
		num = rewardGold;
		for(int j=i;j>0;j--){
			num *= 10;
		}
		num = num/10000%10;
		scrollNumCardTask->setScrollNumCard(mScrollNumArr[i],i+1,num,i);
		scrollNumCardTask->scrollContentView(1,3,150,true,i);	

	}

}

void FortuneComePage::exitPage()
{
	delete [] mScrollNumArr;
	delete [] mScrollNumRootNode;
	delete scrollNumCardTask;

}

