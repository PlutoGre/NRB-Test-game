
#include "stdafx.h"

#include "WorldBossPage.h"
#include "TimeCalculator.h"
#include "GameMaths.h"
#define FINAL_KILL_INDEX 999
#define DAY_TIME_SECONDS 24*60*60

USING_NS_CC;

WorldBossPage::~WorldBossPage(){

}

void WorldBossPage::refreshContent( void )
{
	mFace = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mFace"));
	mJoinBattle = dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mJoinBattle"));

	mTimeCount = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getObjectByVariableName("mTimeCount"));

	WorldBossItem& worldBossItem = ServerDateManager::Get()->getWorldBossItem();

	CCB_FUNC(mContentContainer,"mmBossLV",CCLabelBMFont,setString(StringConverter::toString(worldBossItem.bossLevel).c_str()));

	long timeleft;
	
	if(TimeCalculator::Get()->hasKey("waitforworldBoss")){
		if(TimeCalculator::Get()->getTimeLeft("waitforworldBoss",timeleft))
			translateTime(timeleft,3,1,1,10); 
	}
	
}

void WorldBossPage::translateTime( int seconds , int id, int Count, int maxCount, int price)
{
	if(seconds<=0)
	{
//		mJoinBattle->setVisible(true);
		mJoinBattle->setEnabled(true);
		mTimeCount->setVisible(false);
		CCB_FUNC(mContentContainer,"mShowRank",CCMenuItemImage,setVisible(false));
	}else{

		char str[64];
		int hh = seconds/3600;
		int mm = (seconds-3600*hh)/60;
		int ss = seconds%60;
		sprintf(str,"%02d:%02d:%02d",hh,mm,ss);

		std::string replaced = str;
//		char names[64];
//		sprintf(names,"mTxt%d",id);
		mTimeCount	->setString(replaced.c_str());
//		mJoinBattle->setVisible(false);
		mJoinBattle->setEnabled(false);
		mTimeCount->setVisible(true);
		CCB_FUNC(mContentContainer,"mShowRank",CCMenuItemImage,setVisible(true));
	}

}

void WorldBossPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{
	if(itemName=="onJoinBattle")
	{
		MsgMainFramePushPage msg;
		msg.pageName="WorldBossJoinBattle";
		MessageManager::Get()->sendMessage(&msg);
		
	}else if(itemName=="onShowRank")
	{

		BlackBoard::Get()->worldbossRankFlag = 0;
		MsgMainFramePushPage msg;
		msg.pageName="WorldBossFightEnd";
		MessageManager::Get()->sendMessage(&msg);

	}
}

void WorldBossPage::loadData()
{
	
	if(!getIsInitData())
	{
		const AdventureItemInfo* info = ServerDateManager::Get()->getAdventureItemInfoByID(mID);
		OPGetWorldBossInfo msg;
		msg.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_GET_WORLD_BOSS_INFO_C, &msg);

	}
	
}

void WorldBossPage::updatePage(bool isSelect,float dt)
{
	if(isSelect&&getIsInitData())
	{
		refreshContent();
	}
}
void WorldBossPage::setPageData()
{
	const AdventureItemInfo* adventureItemInfo = ServerDateManager::Get()->getAdventureItemInfoByID(mID);
	WorldBossInfo* info = ServerDateManager::Get()->getAndCreateWorldBossInfo(adventureItemInfo->itemId);
	mBossLvLabel->setString(StringConverter::toString( info->bossLevel).c_str());
	float timeOffset = GamePrecedure::Get()->getTotalTime()-info->totalTimeStamp;

	if((info->beginTime+info->sustainedTime)<=(info->serverTime+timeOffset))
	{
		info->beginTime = info->beginTime+DAY_TIME_SECONDS;
	}

	if((info->serverTime+timeOffset)<info->beginTime)
	{
		mTimeCountNode->setVisible(true);
		mJoinNode->setVisible(false);
		mTimeCountLabel->setString(GameMaths::formatSecondsToTime(info->beginTime-(info->serverTime+timeOffset)).c_str());

		std::list<std::string> strList;
		strList.push_back(ServerDateManager::Get()->getWorldBossRankInfoById(1)->name);
		strList.push_back(ServerDateManager::Get()->getWorldBossRankInfoById(2)->name);
		strList.push_back(ServerDateManager::Get()->getWorldBossRankInfoById(3)->name);
		strList.push_back(StringConverter::toString(info->killedTime));
		strList.push_back(ServerDateManager::Get()->getWorldBossRankInfoById(FINAL_KILL_INDEX)->name);
		std::string strHistory;
		int lines;
		GameMaths::stringAutoReturn(GameMaths::replaceStringWithStringList(Language::Get()->getString("@WorldBossHistoryNote"),&strList),strHistory,15,lines);
		mHistoryNoteLabel->setString(strHistory.c_str());
	}
	else
	{
		mTimeCountNode->setVisible(false);
		mJoinNode->setVisible(true);
		std::list<std::string> strList;
		unsigned int startedTime = (int)(((info->serverTime+timeOffset)-info->beginTime)/60);
		strList.push_back(StringConverter::toString(startedTime));
		std::string strJoinTip;
		int lines;
		GameMaths::stringAutoReturn(GameMaths::replaceStringWithStringList(Language::Get()->getString("@WorldBossJoinTip"),&strList),strJoinTip,15,lines);
		mJoinTipLabel->setString(strJoinTip.c_str());
	}
}

void WorldBossPage::exitPage()
{

}

