
#include "stdafx.h"

#include "WorldBossRankPage.h"
#include "GameMessages.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "GameMaths.h"

#define FINAL_KILL_INDEX 999

REGISTER_PAGE("WorldBossRankPage",WorldBossRankPage);

void WorldBossRankContent::refreshContent(void)
{
	const WorldBossRankInfo* info = ServerDateManager::Get()->getWorldBossRankInfoById(mID);
	mPlayerId=info->playerId;
	cocos2d::CCObject* finalKillNodeObj = getObjectByVariableName("mFinalKillNode",this->getContainerNode());
	cocos2d::CCNode* finalKillNode = dynamic_cast<cocos2d::CCNode* >(finalKillNodeObj);

	cocos2d::CCObject* rankNumNodeObj = getObjectByVariableName("mRankNumNode",this->getContainerNode());
	cocos2d::CCNode* rankNumNode = dynamic_cast<cocos2d::CCNode* >(rankNumNodeObj);
	if(mID!=FINAL_KILL_INDEX)
	{
		finalKillNode->setVisible(false);
		cocos2d::CCObject* rankLabelObj = getObjectByVariableName("mRank",this->getContainerNode());
		cocos2d::CCLabelBMFont* rankLabel = dynamic_cast<cocos2d::CCLabelBMFont* >(rankLabelObj);
		rankLabel->setString(StringConverter::toString(mID).c_str());
	}
	else
	{
		rankNumNode->setVisible(false);
	}
	cocos2d::CCObject* playerNameObj = getObjectByVariableName("mPlayerName",this->getContainerNode());
	cocos2d::CCLabelBMFont* playerNameLabel = dynamic_cast<cocos2d::CCLabelBMFont* >(playerNameObj);
	playerNameLabel->setString(info->name.c_str());

	cocos2d::CCObject* playerLvObj = getObjectByVariableName("mPlayerLV",this->getContainerNode());
	cocos2d::CCLabelBMFont* playerLvLabel = dynamic_cast<cocos2d::CCLabelBMFont* >(playerLvObj);
	playerLvLabel->setString(StringConverter::toString(info->level).c_str());

	cocos2d::CCObject* scoreRewardObj = getObjectByVariableName("mScoreReward",this->getContainerNode());
	cocos2d::CCLabelBMFont* scoreRewardLabel = dynamic_cast<cocos2d::CCLabelBMFont* >(scoreRewardObj);
	scoreRewardLabel->setString(StringConverter::toString(info->damage).c_str());
}
void WorldBossRankContent::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{
	if(itemName=="onShowTeam")
	{
		//TODO:show zhenrong->sendPacket id:mPlayerId
		cocos2d::CCLog("Show player team");
	}
}
cocos2d::CCObject * WorldBossRankContent::getObjectByVariableName(const std::string& variableName,CCBContainer* ccbContainer)
{
	cocos2d::CCObject* obj = ccbContainer->getVariable(variableName);
	if(!obj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s%s","Failed to find variable:",variableName.c_str());
		CCMessageBox(errMsg,"error");
	}
	return obj;
}

WorldBossRankPage::WorldBossRankPage(void)
{
}


WorldBossRankPage::~WorldBossRankPage(void)
{
}
void WorldBossRankPage::Enter( MainFrame* frame)
{
	cocos2d::CCObject* mTitleObj = getObjectByVariableName("mTitle",this);
	mTitleLabel = dynamic_cast<CCLabelBMFont*>(mTitleObj);

	cocos2d::CCObject* mRankNoteObj = getObjectByVariableName("mRankNote",this);
	mRankNoteLabel = dynamic_cast<CCLabelBMFont*>(mRankNoteObj);

	cocos2d::CCObject* mContentObj = getObjectByVariableName("mContent",this);
	mContentScollView = dynamic_cast<cocos2d::extension::CCScrollView *>(mContentObj);

	const WorldBossInfo* info = ServerDateManager::Get()->getWorldBossInfoByID(BlackBoard::Get()->getWorldBossId());
	if(BlackBoard::Get()->getWorldBossRankType()==0)
	{
		mTitleLabel->setString(Language::Get()->getString("@LasttimeBattle").c_str());
		std::list<std::string> strList;
		strList.push_back(StringConverter::toString(info->bossLevel));
		strList.push_back(StringConverter::toString((int)ceil((float)info->killedTime/60)));
		strList.push_back(ServerDateManager::Get()->getWorldBossRankInfoById(FINAL_KILL_INDEX)->name);
		std::string strRankNote;
		int lines;
		GameMaths::stringAutoReturn(GameMaths::replaceStringWithStringList(Language::Get()->getString("@LastTimeBattleRankNote"),&strList),strRankNote,30,lines);
		mRankNoteLabel->setString(strRankNote.c_str());
	}
	else
	{
		mTitleLabel->setString(Language::Get()->getString("@WorldBossBattleEnd").c_str());
		/*std::list<std::string> strList;
		strList.push_back(StringConverter::toString(info->bossLevel));
		strList.push_back(StringConverter::toString((int)ceil((float)info->killedTime/60)));
		strList.push_back(ServerDateManager::Get()->getWorldBossRankInfoByIndex(FINAL_KILL_INDEX)->name);
		std::string strRankNote;
		int lines;
		GameMaths::stringAutoReturn(GameMaths::replaceStringWithStringList(Language::Get()->getString("@LastTimeBattleRankNote"),&strList),strRankNote,15,lines);
		mRankNoteLabel->setString(strRankNote.c_str());*/
	}

	buildItem();
}

void WorldBossRankPage::Execute( MainFrame* frame)
{

}

void WorldBossRankPage::Exit( MainFrame* frame)
{

}
cocos2d::CCObject * WorldBossRankPage::getObjectByVariableName(const std::string& variableName,CCBContainer* ccbContainer)
{
	cocos2d::CCObject* obj = ccbContainer->getVariable(variableName);
	if(!obj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s%s","Failed to find variable:",variableName.c_str());
		CCMessageBox(errMsg,"error");
	}
	return obj;
}

void WorldBossRankPage::load(void)
{
	this->loadCcbiFile("WorldBossRank.ccbi");
}
void WorldBossRankPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender)
{
	if(itemName=="onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName="WorldBossRankPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}
void WorldBossRankPage::buildItem(void)
{
	WorldBossRankInfoMapIterator itr = ServerDateManager::Get()->getWorldBossRankInfoMapIterator();
	unsigned int index=0;
	std::list<unsigned int> rankLevelList;
	while(itr.hasMoreElements())
	{
		index = itr.getNext()->id;
		if(index!=FINAL_KILL_INDEX)
		{
			rankLevelList.push_back(index);
		}
		
	}
	rankLevelList.sort(&WorldBossRankPage::sortRank);
	std::list<unsigned int>::iterator rIt=rankLevelList.begin();
	for(;rIt!=rankLevelList.end();++rIt)
	{
		if(*rIt!=FINAL_KILL_INDEX)
		{
			WorldBossRankContent* content = new WorldBossRankContent(*rIt);
			content->init();
			addItem(content);
		}
	}
	WorldBossRankContent* contentFinalKill = new WorldBossRankContent(FINAL_KILL_INDEX);
	contentFinalKill->init();
	addItem(contentFinalKill);

}
int WorldBossRankPage::sortRank(unsigned int a,unsigned int b)
{
	return a>b;
}
void WorldBossRankPage::addItem(ContentBase* item)
{
	mRankList.push_back(item);
	mContentScollView->addChild(item->getContainerNode());

	CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*(mRankList.size()-1));
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mRankList.size());
	mContentScollView->setContentSize(size);
	mContentScollView->setContentOffset(ccp(0, mContentScollView->getViewSize().height - mContentScollView->getContentSize().height*mContentScollView->getScaleY()));
}

void WorldBossRankPage::clearAllItem( void )
{
	ContentList::iterator itr = mRankList.begin();
	while(itr != mRankList.end())
	{
		delete (*itr);
		++itr;
	}
	ContentList list;
	mRankList.swap(list);
	mContentScollView->removeAllChildren();
}
