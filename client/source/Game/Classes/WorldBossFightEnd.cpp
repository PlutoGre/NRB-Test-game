
#include "stdafx.h"

#include "WorldBossFightEnd.h"
#include "TimeCalculator.h"
#include "GameMaths.h"
#include "CCBManager.h"
#include "CCBContainer.h"

REGISTER_PAGE("WorldBossFightEnd",WorldBossFightEnd);
USING_NS_CC;

void WorldBossRankItem::refreshContent(void){

	WorldBossBattleItem * battleItem = ServerDateManager::Get()->getWorldBossBattleItemByID(mID);
	mPlayerId=battleItem->playerId;
	CCLabelBMFont* mRank=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mRank"));
	if(mRank)
	{
		mRank->setString(StringConverter::toString(battleItem->rank).c_str());
	}

	CCLabelTTF* mPlayerName = dynamic_cast<CCLabelTTF* >(mContentContainer->getVariable("mPlayerName"));

	if(mPlayerName)
	{
		mPlayerName->setString(battleItem->name.c_str());
	}

	CCLabelBMFont* mScoreReward = dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mScoreReward"));
	if(mScoreReward)
	{
		mScoreReward->setString(StringConverter::toString(battleItem->damageBlood).c_str());
	}

	CCLabelBMFont* mPlayerLV = dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mPlayerLV"));
	if(mPlayerLV)
	{
		mPlayerLV->setString(StringConverter::toString(battleItem->level).c_str());
	}

	CCLabelBMFont* worldBossFinalKill = dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mWorldBossFinalKill"));

	if(worldBossFinalKill){

		if(battleItem->isFinal){
			worldBossFinalKill->setVisible(true);
			std::string outstr;
			int lines;
			GameMaths::stringAutoReturn(Language::Get()->getString("@WorldBossFinalKill").c_str(),outstr,2,lines);
			worldBossFinalKill->setString(outstr.c_str());

			CCB_FUNC(mContentContainer,"mRank",CCLabelBMFont,setVisible(false));
		}else{
			worldBossFinalKill->setVisible(false);
		}

	}



}

void WorldBossRankItem::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{
	if(itemName=="onShowTeam")
	{
		std::string _viewTeamPageName="ViewTeamPageKey";
		std::string _value="ViewTeamPage";
		if(HAS_V(_viewTeamPageName))
		{
			SET_V(_viewTeamPageName,_value);
		}
		else
		{
			ADD_V(_viewTeamPageName,_value);
		}
		OPShowUserBattleArray* msg = new OPShowUserBattleArray;
		msg->set_version(1);
		msg->set_isnpc(0);
		msg->set_relationid(mPlayerId);
		msg->set_relationtype(1);
		PacketManager::Get()->sendPakcet(OPCODE_SHOW_USER_BATTLE_C,msg);
	}
}

WorldBossFightEnd::WorldBossFightEnd(){

}

WorldBossFightEnd::~WorldBossFightEnd(){

}

void WorldBossFightEnd::Enter(MainFrame*){
	rebuildAllItem();
}

void WorldBossFightEnd::Execute(MainFrame*){

}

void WorldBossFightEnd::Exit(MainFrame*){

}

void WorldBossFightEnd::load(void){
   	loadCcbiFile(getCcbiFileName());
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));
	membersRootNode = members->getContainer();
}

void WorldBossFightEnd::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){

}

void WorldBossFightEnd::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender){
	if(itemName=="onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName="WorldBossFightEnd";
		MessageManager::Get()->sendMessage(&msg);
		CCLOG("on Close");

	}else if(itemName=="onShowRank"){

	}
}
void WorldBossFightEnd::buildItem(void){

	WorldBossBattleMapIterator itr = ServerDateManager::Get()->getWorldBossBattleIterator();

	while (itr.hasMoreElements())
	{
		WorldBossRankItem * worldBossRankItem = new WorldBossRankItem(itr.getNext()->id);
		worldBossRankItem->init();
		addItem(worldBossRankItem);
	}

}

void WorldBossFightEnd::addItem(ContentBase* item){

	mContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*(mContentList.size()-1));
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*(mContentList.size()));
	members->setContentSize(size);
	members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
//	members->setContentOffset(ccp(0, item->getContainerNode()->getContentSize().height- members->getContentSize().height*members->getScaleY()));

}

void WorldBossFightEnd::rebuildAllItem(void){
	refreshPage();
	clearAllItem();
	buildItem();

}
void WorldBossFightEnd::refreshPage(){
	if(BlackBoard::Get()->worldbossRankFlag==0){
		CCB_FUNC(this,"mTitle",CCLabelBMFont,setString(Language::Get()->getString("@WorldBossLastRankTitle").c_str()));
		CCB_FUNC(this,"mRankNote",CCLabelBMFont,setString(Language::Get()->getString("@WorldBossLastRankNote").c_str()));
	}else if(BlackBoard::Get()->worldbossRankFlag==1){
		CCB_FUNC(this,"mTitle",CCLabelBMFont,setString(Language::Get()->getString("@WorldBossRankTitle").c_str()));
		CCB_FUNC(this,"mRankNote",CCLabelBMFont,setString(Language::Get()->getString("@WorldBossRankNote").c_str()));
	}
}

void WorldBossFightEnd::clearAllItem(void){
	ContentList::iterator personItr = mContentList.begin();
	while(personItr != mContentList.end()){
		delete (*personItr);
		++personItr;
	}
	ContentList contentList;
	mContentList.swap(contentList);

}

void WorldBossFightEnd::refreshAllItem(void){

}

void WorldBossFightEnd::removeItem(ContentBase* item){

}

	