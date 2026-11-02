
#include "stdafx.h"
#include "BlackBoard.h"
#include "FightEndRankPage.h"
REGISTER_PAGE("FightEndRankPage",FightEndRankPage);
USING_NS_CC;

void RankContent::refreshContent( void )
{
	CCBContainer* content=getContainerNode();
	const FightEndRankInfo::RankItem* rankItem=ServerDateManager::Get()->getFightEndRankItemByIndexAndId(mIndex,getContentID());
	mPlayerId=rankItem->playerId;
	CCObject* rankObj=getContainerNode()->getVariable("mRank");
	if(!rankObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mRank");
		CCMessageBox(errMsg,"error");
	}
	CCLabelBMFont* rankBM=dynamic_cast<cocos2d::CCLabelBMFont* >(rankObj);
	if(rankBM)
	{
		rankBM->setString(StringConverter::toString(rankItem->id).c_str());
	}

	CCObject* lvObj=getContainerNode()->getVariable("mPlayerLV");
	if(!lvObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mPlayerLV");
		CCMessageBox(errMsg,"error");
	}
	CCLabelBMFont* playerLvBM=dynamic_cast<cocos2d::CCLabelBMFont* >(lvObj);
	if(playerLvBM)
	{
		playerLvBM->setString(StringConverter::toString(rankItem->level).c_str());
	}

	CCObject* nameObj=getContainerNode()->getVariable("mPlayerName");
	if(!nameObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mPlayerName");
		CCMessageBox(errMsg,"error");
	}
	CCLabelTTF* nameBM=dynamic_cast<cocos2d::CCLabelTTF* >(nameObj);
	if(nameBM)
	{
		nameBM->setString(rankItem->playerName.c_str());
	}

	CCObject* passLevelObj=getContainerNode()->getVariable("mPassBattle");
	if(!passLevelObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mPassBattle");
		CCMessageBox(errMsg,"error");
	}
	CCLabelBMFont* passBattleBM=dynamic_cast<cocos2d::CCLabelBMFont* >(passLevelObj);
	if(passBattleBM)
	{
		std::list<std::string> replaceList;
		replaceList.push_back(StringConverter::toString(rankItem->passBattle));
		passBattleBM->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@FightEndRankPassBattle"),&replaceList).c_str());
	}

	CCObject* obtainStarObj=getContainerNode()->getVariable("mObtainStar");
	if(!obtainStarObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mObtainStar");
		CCMessageBox(errMsg,"error");
	}
	CCLabelBMFont* obtainStarBM=dynamic_cast<cocos2d::CCLabelBMFont* >(obtainStarObj);
	if(obtainStarBM)
	{
		obtainStarBM->setString((Language::Get()->getString("@ObtainStar")+StringConverter::toString(rankItem->obtainStar)).c_str());
	}

	CCObject* sussessionRankObj=getContainerNode()->getVariable("mSuccessionInRank");
	if(!sussessionRankObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mSuccessionInRank");
		CCMessageBox(errMsg,"error");
	}
	CCLabelBMFont* sussessionRankBM=dynamic_cast<cocos2d::CCLabelBMFont* >(sussessionRankObj);
	if(sussessionRankBM)
	{
		if(rankItem->sussessionCount<=0)
		{
			sussessionRankBM->setVisible(false);
		}
		else
		{
			sussessionRankBM->setVisible(true);
			std::list<std::string> replaceList;
			replaceList.push_back(StringConverter::toString(rankItem->sussessionCount));
			sussessionRankBM->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@FightEndSussessionInRank"),&replaceList).c_str());
		}
	}
}

void RankContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
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

FightEndRankPage::FightEndRankPage(void)
{
	time_t t = time(0); 
	tm* tm=localtime(&t) ;
	mCurrDay=tm->tm_mday;
	mCurrTabIndex = 1;
	membersRootNode = NULL;
	members = NULL;
	mContentList.clear();
}


FightEndRankPage::~FightEndRankPage(void)
{
	clearAllItem();
}


void FightEndRankPage::Enter( MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	buildTabItem();
	mCurrTabIndex=ServerDateManager::Get()->getBattleArraySize()-4;
	if(getTabBtnByIndex(mCurrTabIndex))
	{
		getTabBtnByIndex(mCurrTabIndex)->setEnabled(false);
	}
	loadRankData();
}

void FightEndRankPage::loadRankData()
{
	time_t t = time(0); 
	tm* tm=localtime(&t) ;
	
	if(tm->tm_mday!=mCurrDay)
	{
		mCurrDay=tm->tm_mday;
		mInitStat.clear();
	}

	if(mInitStat.find(mCurrTabIndex)==mInitStat.end())
	{
		OPFightEndRankInfo msg;
		msg.set_version(1);
		msg.set_index(mCurrTabIndex);
		PacketManager::Get()->sendPakcet(OPCODE_GET_FIGHTEND_RANK_INFO_C, &msg);
	}
	else
	{
		rebuildAllItem();
	}
}

void FightEndRankPage::buildItem( void )
{
	FightEndRankInfo::RankItemMapIterator it=ServerDateManager::Get()->getFightEndRankMapIteratorByIndex(mCurrTabIndex);
	std::list<unsigned int> rankLevelList;
	int rankLevel=0;
	const UserBasicInfo info=ServerDateManager::Get()->getUserBasicInfo();
	unsigned int playerId=ServerDateManager::Get()->getFightEndRankInfo().mPlayerId;
	const AdventureFightEndInfo* fightInfo=ServerDateManager::Get()->getAdventureFightEndInfo();
	while(it.hasMoreElements())
	{
		FightEndRankInfo::RankItem* item=it.getNext();
		if(playerId==item->playerId)
		{
			rankLevel=item->id;
		}
		rankLevelList.push_back(item->id);
	}
	rankLevelList.sort(&FightEndRankPage::sortRank);
	std::list<unsigned int>::iterator rIt=rankLevelList.begin();
	for(;rIt!=rankLevelList.end();++rIt)
	{
		RankContent * item = new RankContent(mCurrTabIndex,*rIt);
		item->init();
		addItem(item);
	}
	rankLevelList.clear();
	CCLabelBMFont* selfRankingLabel=dynamic_cast<CCLabelBMFont* >(getVariable("mSelfRankingInfo"));
	if(selfRankingLabel)
	{
		if(ServerDateManager::Get()->getBattleArraySize()==(mCurrTabIndex+4))
		{
			selfRankingLabel->setVisible(true);
			std::list<std::string> replaceList;
			if(rankLevel!=0)
			{
				replaceList.push_back(StringConverter::toString(rankLevel));
			}
			else
			{
				replaceList.push_back(Language::Get()->getString("@NotIntoTheList"));
			}
			if(fightInfo->mCalculateTodayRanking==0)
			{
				replaceList.push_back(Language::Get()->getString("@FightNotInRanking"));
			}
			else
			{
				replaceList.push_back(StringConverter::toString(fightInfo->mCalculateTodayRanking));
			}
			selfRankingLabel->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@FightEndSelfRank"),&replaceList).c_str());
		}
		else
		{
			selfRankingLabel->setVisible(false);
		}
	}
	
}

int FightEndRankPage::sortRank(unsigned int a,unsigned int b)
{
	return a>b;
}

void FightEndRankPage::addItem( ContentBase* item )
{
	mContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*(mContentList.size()-1));
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mContentList.size());
	members->setContentSize(size);
	members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
}

void FightEndRankPage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void FightEndRankPage::clearAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		delete (*itr);
		++itr;
	}
	ContentList list;
	mContentList.swap(list);
	if(membersRootNode)
		membersRootNode->removeAllChildren();
}

void FightEndRankPage::refreshAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		(*itr)->refreshContent();
		++itr;
	}
}

void FightEndRankPage::removeItem( ContentBase* item )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		if((*itr) == item)
		{
			membersRootNode->removeChild(item->getContainerNode());
			delete item;
			mContentList.erase(itr);
		}
		++itr;
	}
}


void FightEndRankPage::Exit( MainFrame* frame)
{
	removeAllTabItem();
	clearAllItem();
	MessageManager::Get()->removeMessageHandler(this);
}

void FightEndRankPage::Execute( MainFrame* )
{

}

void FightEndRankPage::load( void )
{
	loadCcbiFile("FightEndRankPage.ccbi");
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));
	membersRootNode = members->getContainer();
}

void FightEndRankPage::removeAllTabItem()
{
	TAB_BUTTON_LIST::iterator it=mTabList.begin();
	for(;it!=mTabList.end();++it)
	{
		if(it->second)
		{
			it->second->release();
		}
		++it;
	}
	mTabList.clear();
}

void FightEndRankPage::buildTabItem()
{
	for(int i=1;i<=TabLength;++i)
	{
		char tabName[64];
		sprintf(tabName,"mTab_%d",i);
		CCObject* menuImage=getVariable(tabName);
		if(!menuImage)
		{
			char errMsg[256];
			sprintf(errMsg,"Failed to find variable:%s",tabName);
			CCMessageBox(errMsg,"error");
		}
		CCMenuItemImage* menuBtn=dynamic_cast<CCMenuItemImage* >(menuImage);
		if(menuBtn)
		{
			menuBtn->setEnabled(true);
			menuBtn->retain();
			mTabList.insert(TAB_BUTTON_LIST::value_type(i,menuBtn));
		}
	}
}

CCMenuItemSprite* FightEndRankPage::getTabBtnByIndex(unsigned int index)
{
	if(mTabList.find(index)!=mTabList.end())
	{
		return mTabList.find(index)->second;
	}
	return NULL;
}

void FightEndRankPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{

}

void FightEndRankPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	bool toggle=false;
	if(itemName=="onShowFiveBattleRank")
	{
		if(mCurrTabIndex!=1)
		{
			toggle=true;
			if(getTabBtnByIndex(mCurrTabIndex))
			{
				getTabBtnByIndex(mCurrTabIndex)->setEnabled(true);
			}
			mCurrTabIndex=1;
			if(getTabBtnByIndex(mCurrTabIndex))
			{
				getTabBtnByIndex(mCurrTabIndex)->setEnabled(false);
			}
		}
	}
	else if(itemName=="onShowSixBattleRank")
	{
		if(mCurrTabIndex!=2)
		{
			toggle=true;
			if(getTabBtnByIndex(mCurrTabIndex))
			{
				getTabBtnByIndex(mCurrTabIndex)->setEnabled(true);
			}
			mCurrTabIndex=2;
			if(getTabBtnByIndex(mCurrTabIndex))
			{
				getTabBtnByIndex(mCurrTabIndex)->setEnabled(false);
			}
		}
	}
	else if(itemName=="onShowSevenBattleRank")
	{
		if(mCurrTabIndex!=3)
		{
			toggle=true;
			if(getTabBtnByIndex(mCurrTabIndex))
			{
				getTabBtnByIndex(mCurrTabIndex)->setEnabled(true);
			}
			mCurrTabIndex=3;
			if(getTabBtnByIndex(mCurrTabIndex))
			{
				getTabBtnByIndex(mCurrTabIndex)->setEnabled(false);
			}
		}
	}
	else if(itemName=="onShowEightBattleRank")
	{
		if(mCurrTabIndex!=4)
		{
			toggle=true;
			if(getTabBtnByIndex(mCurrTabIndex))
			{
				getTabBtnByIndex(mCurrTabIndex)->setEnabled(true);
			}
			mCurrTabIndex=4;
			if(getTabBtnByIndex(mCurrTabIndex))
			{
				getTabBtnByIndex(mCurrTabIndex)->setEnabled(false);
			}
		}
	}
	if(itemName=="onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "FightEndRankPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	if(toggle)
	{
		loadRankData();
	}

}

void FightEndRankPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId()==MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate* msg=dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(msg->opcode==OPCODE_GET_FIGHTEND_RANK_INFORET_S)
		{
			if(mInitStat.find(mCurrTabIndex)==mInitStat.end())
			{
				mInitStat.insert(DataInitStatMap::value_type(mCurrTabIndex,true));
			}
			rebuildAllItem();
		}
	}
}