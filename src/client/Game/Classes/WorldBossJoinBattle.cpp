
#include "stdafx.h"

#include "WorldBossJoinBattle.h"
#include "TimeCalculator.h"
#include "GameMaths.h"
#include "CCBManager.h"
#include "StringConverter.h"
#include "Disciple.h"
#include "BlackBoard.h"
#include "PopupPage.h"
#include "Popup1stPayTipPage.h"
	  

REGISTER_PAGE("WorldBossJoinBattle",WorldBossJoinBattle);
USING_NS_CC;

void WorldBossBattleTempItem::refreshContent(){
	WorldBossBattleItem * battleItem = ServerDateManager::Get()->getWorldBossBattleItemByID(mID);

	CCLabelBMFont* mRank=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mRank"));
	if(mRank)
	{
		mRank->setString(StringConverter::toString(battleItem->rank).c_str());
	}

	CCLabelTTF* mPlayerName = dynamic_cast<CCLabelTTF* >(mContentContainer->getVariable("mPlayName"));
	if(mPlayerName)
	{
		mPlayerName->setString(battleItem->name.c_str());
	}

	CCLabelBMFont* mScoreReward = dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mHurt"));
	if(mScoreReward)
	{
		mScoreReward->setString(StringConverter::toString(battleItem->damageBlood).c_str());
	}

	CCLabelBMFont* mPlayerLV = dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mPlayerLV"));
	if(mPlayerLV)
	{
		mPlayerLV->setString(StringConverter::toString(battleItem->level).c_str());
	}
}


void WorldBossBattleTempItem::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){

}

void WorldBossDamageTempItem::refreshContent(){
}

void WorldBossDamageTempItem::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){

}

cocos2d::CCObject * WorldBossBattleTempItem::getObjectByVariableName(const std::string& variableName,CCBContainer* ccbContainer){

	return NULL;
}

WorldBossJoinBattle::WorldBossJoinBattle()
{
	

}

WorldBossJoinBattle::~WorldBossJoinBattle()
{
}

void WorldBossJoinBattle::load(){
   	loadCcbiFile(getCcbiFileName());
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));
	membersRootNode = members->getContainer();

}

void WorldBossJoinBattle::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){

}

void WorldBossJoinBattle::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender){
	UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	long timeleft;
	if(TimeCalculator::Get()->hasKey("waitforfighttime")){
		TimeCalculator::Get()->getTimeLeft("waitforfighttime",timeleft);
	}
	if(itemName=="onQuit")
	{
		MsgMainFramePopPage msg;
		msg.pageName="WorldBossJoinBattle";
		MessageManager::Get()->sendMessage(&msg);

	}else if(itemName=="onShowRank"){

	}else if(itemName=="onRebornWithBuff"){

		WorldBossItem& worldBossItem = ServerDateManager::Get()->getWorldBossItem();
		if(userBasicInfo.goldcoins>=StringConverter::parseInt(VaribleManager::Get()->getSetting("WorldBossCreGoldAttack"))*(worldBossItem.useFireTime)+StringConverter::parseInt(VaribleManager::Get()->getSetting("WorldBossInitGoldAttack"))){
			OPWorldBossFight worldBossFight;
			worldBossFight.set_version(1);
			worldBossFight.set_fighttype(3);
			PacketManager::Get()->sendPakcet(OPCODE_WORLDBOSS_FIGHT_C,&worldBossFight);
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
		}
	}else if(itemName=="onReborn"){
		if(timeleft>0){
			if(userBasicInfo.goldcoins>=StringConverter::parseInt(VaribleManager::Get()->getSetting("WorldBossInitRebornAttack"))){
				OPWorldBossFight worldBossFight;
				worldBossFight.set_version(1);
				worldBossFight.set_fighttype(2);
				PacketManager::Get()->sendPakcet(OPCODE_WORLDBOSS_FIGHT_C,&worldBossFight);
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
			}
		}else{
			OPWorldBossFight worldBossFight;
			worldBossFight.set_version(1);
			worldBossFight.set_fighttype(1);
			PacketManager::Get()->sendPakcet(OPCODE_WORLDBOSS_FIGHT_C,&worldBossFight);
		}

	}
}

void WorldBossJoinBattle::Enter(MainFrame*){ 
	inited = false;
	animationNum = 0;

	
	OPWorldBossRefresh worldBossRefresh;
	worldBossRefresh.set_version(1);
	PacketManager::Get()->sendPakcet(OPCODE_WORLDBOSS_REFRESH_C,&worldBossRefresh);

	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);

	CCB_FUNC(this,"mRebornWithBuffNode",CCNode,setVisible(false));
}

void WorldBossJoinBattle::Execute(MainFrame*){
	if(inited){
		countReliveTime();
	}
}

void WorldBossJoinBattle::Exit(MainFrame*){

}

void WorldBossJoinBattle::refreshPage(){


	RebornWithBuffNode = dynamic_cast<CCNode* >(getVariable("mRebornWithBuffNode"));

	mTimeCount = dynamic_cast<cocos2d::CCLabelBMFont* >(this->getVariable("mTimeCount"));

	mLable3 = dynamic_cast<CCLabelBMFont* >(getVariable("mLable3"));

	WorldBossItem& worldBossItem = ServerDateManager::Get()->getWorldBossItem();

	CCB_FUNC(this,"mAttackNum",CCLabelBMFont,setString(StringConverter::toString(worldBossItem.attackNum).c_str()));
	CCB_FUNC(this,"mBloodHurt",CCLabelBMFont,setString(StringConverter::toString(worldBossItem.damageBlood).c_str()));
	CCB_FUNC(this,"mBlood",cocos2d::extension::CCScale9Sprite,setScaleX(worldBossItem.bossLeftBlood*1.0/worldBossItem.bossTotalBlood));

	int worldBossGoldAttack = StringConverter::parseInt(VaribleManager::Get()->getSetting("WorldBossCreGoldAttack"))*(worldBossItem.useFireTime)+StringConverter::parseInt(VaribleManager::Get()->getSetting("WorldBossInitGoldAttack"));
	char worldBossGoldAttackChar[64];
	sprintf(worldBossGoldAttackChar,"%d",worldBossGoldAttack);
	std::string worldBossGoldAttackStr(worldBossGoldAttackChar);
	CCB_FUNC(this,"mGoldNeed",CCLabelBMFont,setString(worldBossGoldAttackStr.c_str()));

	int worldBossBuffAttack = StringConverter::parseInt(VaribleManager::Get()->getSetting("WorldBossInitBuffAttack").c_str());
	char worldBossBuffAttackChar[64];
	sprintf(worldBossBuffAttackChar,"%d%s",worldBossBuffAttack,"%");
	std::string worldBossBuffAttackStr(worldBossBuffAttackChar);
	CCB_FUNC(this,"mBuffValue",CCLabelBMFont,setString(worldBossBuffAttackStr.c_str()));

	CCB_FUNC(this,"mGoldNeed1",CCLabelBMFont,setString(VaribleManager::Get()->getSetting("WorldBossInitRebornAttack").c_str()));

	CCB_FUNC(this,"mQuitBtn",CCMenuItemImage,setEnabled(true));
	CCB_FUNC(this,"mRebornWithBuffBtn",CCMenuItemImage,setEnabled(true));
	CCB_FUNC(this,"mJoinBattleBtn",CCMenuItemImage,setEnabled(true));

}
void WorldBossJoinBattle::countReliveTime(){
	long timeleft;
	if(TimeCalculator::Get()->hasKey("waitforfighttime")){
		if(TimeCalculator::Get()->getTimeLeft("waitforfighttime",timeleft)){
			translateTime(timeleft,3,1,1,10); 
		}
	}

}

void WorldBossJoinBattle::translateTime( int seconds , int id, int Count, int maxCount, int price)
{

	if(currSecond == seconds){

	}else if(currSecond != seconds){

		if(seconds%30==0){
			sendPackageByM();
			scrollDamageItems();
		}
		if(seconds%2==0){
			refreshDamageByS();
		}

		currSecond = seconds;

	}

	if(seconds<=0)
	{
		RebornWithBuffNode->setVisible(false);
		CCB_FUNC(this,"mJoinLable",CCLabelBMFont,setString(Language::Get()->getString("@JoinBattle").c_str()));
	}else{

		char str[64];
		int ss = seconds;
		sprintf(str,"%02d",ss);

		std::string replaced = str;

		mTimeCount	->setString(replaced.c_str());
		RebornWithBuffNode->setVisible(true);
		CCB_FUNC(this,"mJoinLable",CCLabelBMFont,setString(Language::Get()->getString("@RebornNow").c_str()));

	}

}

void WorldBossJoinBattle::sendPackageByM(){
	OPWorldBossRefresh worldBossRefresh;
	worldBossRefresh.set_version(1);
	PacketManager::Get()->sendPakcet(OPCODE_WORLDBOSS_REFRESH_C,&worldBossRefresh);

}

void WorldBossJoinBattle::scrollDamageItems(){

	/*
	this->scrollCardContent = new ScrollNumCard(mContent,arr1,arrLength1);
	this->scrollCardContent->scrollContentView(50,1,30,false);
	*/

}

void WorldBossJoinBattle::refreshDamageByS(){
	 rebuildAllDamageItem();
}

void WorldBossJoinBattle::rebuildAllBattleItem(void){
	clearAllBattleItem();
	buildBattleItem();
}

void WorldBossJoinBattle::clearAllBattleItem(void){

	ContentList::iterator battleItr = mBattleContentList.begin();
	while(battleItr != mBattleContentList.end()){
		delete (*battleItr);
		++battleItr;
	}
	
	ContentList contentList;
	mBattleContentList.swap(contentList);
	
	membersRootNode->removeAllChildren();

}

void WorldBossJoinBattle::buildBattleItem(void){
	WorldBossBattleMapIterator itr = ServerDateManager::Get()->getWorldBossBattleIterator();
	while (itr.hasMoreElements())
	{
		int tempId = itr.getNext()->id;
		WorldBossBattleTempItem * worldBossBattleTempItem = new WorldBossBattleTempItem(tempId);
		worldBossBattleTempItem->init();
		addBattleItem(worldBossBattleTempItem);
	}

}

void WorldBossJoinBattle::addBattleItem(ContentBase* item){
	mBattleContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*(mBattleContentList.size()-1));
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mBattleContentList.size());
	members->setContentSize(size);
	members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));

}



void WorldBossJoinBattle::refreshAllBattleItem(void){

}

void WorldBossJoinBattle::removeBattleItem(ContentBase* item){

}


void WorldBossJoinBattle::rebuildAllDamageItem(void){
	clearAllDamageItem();
	buildDamageItem();
}
void WorldBossJoinBattle::clearAllDamageItem(void){

	std::string nullStr("");
	mLable3 = dynamic_cast<CCLabelBMFont* >(getVariable("mLable3"));
	if(mLable3){
		mLable3->setString(nullStr.c_str());
	}

}
void WorldBossJoinBattle::buildDamageItem(void){
	WorldBossDamageItem * worldBossDamageItem;

	for(int i=1;i<=1;i++){

		worldBossDamageItem = ServerDateManager::Get()->getNextWorldBossDamageItem();
		WorldBossDamageTempItem * worldBossDamageTempItem = new WorldBossDamageTempItem(i);

		if(worldBossDamageItem != NULL){
			std::list<std::string> strList;
			strList.push_back(worldBossDamageItem->name);
			strList.push_back(StringConverter::toString(worldBossDamageItem->attackNum));
			strList.push_back(StringConverter::toString(worldBossDamageItem->damageBlood));
			worldBossDamageTempItem->damageContent = GameMaths::replaceStringWithStringList(Language::Get()->getString("@WorldBossDamageContent"),&strList);

			addDamageItem(worldBossDamageTempItem);
			fightMovie(worldBossDamageItem->discipleId,worldBossDamageItem->damageBlood);
		}

	}

}

void WorldBossJoinBattle::fightMovie(int discipleId,int damageBlood){

	Disciple disciple(discipleId,true);

	CCB_FUNC(this,"mAttackerHead",CCSprite,	initWithFile(disciple.iconPic().c_str()));
	CCB_FUNC(this,"mHurt",CCLabelBMFont,setString(StringConverter::toString(damageBlood).c_str()));

	if(animationNum == 0){
		this->runAnimation("attackA");
	}else if(animationNum == 1){
		this->runAnimation("attackB");
	}else if(animationNum == 2){
		this->runAnimation("attackC");
	}else if(animationNum == 3){
		this->runAnimation("attackD");
	}
	animationNum = (animationNum+1)%4;

}

void WorldBossJoinBattle::addDamageItem(WorldBossDamageTempItem* item){

	damageTemp1 = damageTemp2;
	damageTemp2 = damageTemp3;
	damageTemp3 = item->damageContent;

	char charArr[164];
	std::string mLable3Str = mLable3->getString();
	std::string returnStr = "\n";
	std::string itemStr = item->damageContent;
	sprintf(charArr,"%s%s%s","","","");
	sprintf(charArr,"%s%s%s%s%s%s",damageTemp1.c_str(),returnStr.c_str(),damageTemp2.c_str(),returnStr.c_str(),damageTemp3.c_str(),returnStr.c_str());
	std::string str = charArr;

	if(mLable3){
		mLable3->setString(str.c_str());
	}

}

void WorldBossJoinBattle::refreshAllDamageItem(void){

}

void WorldBossJoinBattle::removeDamageItem(WorldBossDamageTempItem* item){

}

void WorldBossJoinBattle::onReceiveMassage( const GameMessage * message ){
	if(message->getTypeId() == MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate * info = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(info->opcode==OPCODE_WORLDBOSS_REFRESHRET_S||info->opcode==OPCODE_WORLDBOSS_FIGHTRET_S)
		{
			WorldBossItem& worldBossItem = ServerDateManager::Get()->getWorldBossItem();
			if(worldBossItem.finished == 1){
				
				countLeftTime(worldBossItem.finished);
									   
				MsgMainFramePopPage msg;
				msg.pageName="WorldBossJoinBattle";
				MessageManager::Get()->sendMessage(&msg);
				
				
				BlackBoard::Get()->worldbossRankFlag = 1;
				MsgMainFramePushPage msg1;
				msg1.pageName="WorldBossFightEnd";
				MessageManager::Get()->sendMessage(&msg1);
				
			}else{
				rebuildAllBattleItem();
				rebuildAllDamageItem();
				refreshPage();
				inited = true;

			}
//			_refreshTitle();
		}
	}
}

void WorldBossJoinBattle::countLeftTime(int finished){
	long leftTime = 0;
	time_t tick = (time_t)(GamePrecedure::Get()->getServerTime());
	struct tm *l=gmtime(&tick);

	int gmHour = (l->tm_hour+8)%24;
	int hour = StringConverter::parseInt(VaribleManager::Get()->getSetting("WorldBossOpenHour"));
	int min = StringConverter::parseInt(VaribleManager::Get()->getSetting("WorldBossOpenMin"));


	if(finished==1){
		if(gmHour>hour||(gmHour==hour&&l->tm_min>min)){
			//12:30之后，活动结束
			leftTime += (24 - gmHour)*3600;
			leftTime += (0 - l->tm_min)*60;
			leftTime += 0 - l->tm_sec;
			leftTime += hour*3600;
			leftTime += min*60;
		}else if(gmHour<hour||(gmHour==hour&&l->tm_min<min)){
			//12:30之前，游戏未开始
			leftTime += (hour - gmHour)*3600;
			leftTime += (min - l->tm_min)*60;
			leftTime += 0 - l->tm_sec;
		}

	}

	TimeCalculator::Get()->createTimeCalcultor("waitforworldBoss",leftTime);
}