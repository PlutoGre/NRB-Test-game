
#include "stdafx.h"

#include "FightEndPage.h"
#include "Disciple.h"
#include "BattleArrayInfo.h"
USING_NS_CC;
void RewardContent::refreshContent( void )
{
	CCBContainer* content=getContainerNode();
	const AdventureFightEndInfo::RewardItem* rewardItem=ServerDateManager::Get()->getFightEndRewardItemById(getContentID());
	const ResInfo* ri=ResManager::Get()->getResInfoByTypeAndId(rewardItem->itemType,rewardItem->itemId,rewardItem->count);
	CCObject* nameObj=getContainerNode()->getVariable("mName");
	if(!nameObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mName");
		CCMessageBox(errMsg,"error");
	}
	CCLabelBMFont* nameBM=dynamic_cast<cocos2d::CCLabelBMFont* >(nameObj);
	if(nameBM)
	{
		nameBM->setString(StringConverter::toString(rewardItem->count).c_str());
	}
	CCObject* buttonObj=getContainerNode()->getVariable("mFrameBack");
	if(!buttonObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mFrameBack");
		CCMessageBox(errMsg,"error");
	}
	CCMenuItemImage* frameBtn=dynamic_cast<cocos2d::CCMenuItemImage* >(buttonObj);
	if(frameBtn && frameBtn->getParent())
	{
		CCMenu* menu = dynamic_cast<CCMenu*>(frameBtn->getParent());
		if(menu)
		{
			menu->setScrollViewChild(true);
		}
		frameBtn->setNormalImage(::getFrameNormalSpirte(ri->quality>0?ri->quality:4));
		frameBtn->setSelectedImage(::getFrameSelectedSpirte(ri->quality>0?ri->quality:4));
	}
	cocos2d::CCSprite* icon=dynamic_cast<cocos2d::CCSprite* >(getContainerNode()->getVariable("mFrame"));
	if(icon)
	{
		icon->initWithFile(ri->icon.c_str());
	}
}

void RewardContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{

}

void FightEndPage::refreshContent( void )
{
	CCObject* mainObj=getContainerNode()->getVariable("mMainNode");
	if(!mainObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mMainNode");
		CCMessageBox(errMsg,"error");
	}
	CCNode* mainNode=dynamic_cast<cocos2d::CCNode* >(mainObj);
	if(mainNode)
	{
		mainNode->setVisible(false);
		mNodeList.insert(ContentNodeList::value_type(AdventureFightEndInfo::MainPanel,mainNode));
	}
	CCObject* yestAttrSel=getContainerNode()->getVariable("mYestAttrSelNode");
	if(!yestAttrSel)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:yestAttrSel");
		CCMessageBox(errMsg,"error");
	}
	CCNode* yestAttrSelNode=dynamic_cast<cocos2d::CCNode* >(yestAttrSel);
	if(yestAttrSelNode)
	{
		yestAttrSelNode->setVisible(false);
		mNodeList.insert(ContentNodeList::value_type(AdventureFightEndInfo::YestAddAttrPanel,yestAttrSelNode));
	}

	CCObject* fightSelObj=getContainerNode()->getVariable("mFightSelNode");
	if(!fightSelObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mFightSelNode");
		CCMessageBox(errMsg,"error");
	}
	CCNode* fightSelNode=dynamic_cast<cocos2d::CCNode* >(fightSelObj);
	if(fightSelNode)
	{
		fightSelNode->setVisible(false);
		mNodeList.insert(ContentNodeList::value_type(AdventureFightEndInfo::TeamPanel,fightSelNode));
	}

	CCObject* tempAttrObj=getContainerNode()->getVariable("mTempAttrAddNode");
	if(!tempAttrObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mTempAttrAddNode");
		CCMessageBox(errMsg,"error");
	}
	CCNode* tempAttrNode=dynamic_cast<cocos2d::CCNode* >(tempAttrObj);
	if(tempAttrNode)
	{
		tempAttrNode->setVisible(false);
		mNodeList.insert(ContentNodeList::value_type(AdventureFightEndInfo::StepAddAttr,tempAttrNode));
	}

	CCObject* rewardObj=getContainerNode()->getVariable("mRewardNode");
	if(!rewardObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mRewardNode");
		CCMessageBox(errMsg,"error");
	}
	CCNode* rewardNode=dynamic_cast<cocos2d::CCNode* >(rewardObj);
	if(rewardNode)
	{
		rewardNode->setVisible(false);
		mNodeList.insert(ContentNodeList::value_type(AdventureFightEndInfo::RewardPanel,rewardNode));
	}
	mCurrNodeType=AdventureFightEndInfo::MainPanel;
	mRewardContent=dynamic_cast<cocos2d::extension::CCScrollView* >(getContainerNode()->getVariable("mRewardContent"));
	mFightInfo=ServerDateManager::Get()->getAdventureFightEndInfo();
}

cocos2d::CCNode* FightEndPage::getNodePanelByType(const unsigned int type)
{
	if(mNodeList.find(type)!=mNodeList.end())
	{
		return mNodeList.find(type)->second;
	}
	return NULL;
}

void FightEndPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{

}

void FightEndPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{
	if(itemName=="onStartFightButton")
	{
		if(mFightInfo->mTodayFightInfo.surplusAttackCount<=0)
		{
			MSG_BOX(Language::Get()->getString("@FightEndSurplusAttackEnough"));
		}
		else
		{
			if(mFightInfo->mContinueInRankAmount!=0||mFightInfo->mIsNewStatus==0)
			{
				OPFightEndAddYestAttr msg;
				msg.set_version(1);
				msg.set_id(AdventureFightEndInfo::HP);
				PacketManager::Get()->sendPakcet(OPCODE_FIGHTEND_ADD_YEST_ATTR_INFO_C, &msg);
			}
			else
			{
				mFightInfo->mPanelType=mFightInfo->YestAddAttrPanel;
			}
		}
	}
	else if(itemName=="onShowRankButton")
	{
		MsgMainFramePushPage msg;
		msg.pageName = "FightEndRankPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onShowRankingButton")
	{
		MsgMainFramePushPage msg;
		msg.pageName = "FightEndRankPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onGetRewardButton")
	{
		OPFightEndGetReward msg;
		msg.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_GET_FIGHTEND_REWARD_INFO_C, &msg);
	}
	else if(itemName=="onTempAddAttrSelOne")
	{
		const AdventureFightEndInfo::StepAdditionItem* it=ServerDateManager::Get()->getFightEndStepAdditionItemById(1);
		if(it->requireStar>ServerDateManager::Get()->getAdventureFightEndInfo()->mTodayFightInfo.currSurplusStar)
		{
			MSG_BOX("Star Not Enough!");
			return;
		}
		else
		{
			OPFightEndTempAddStepAttr msg;
			msg.set_version(1);
			msg.set_id(1);
			PacketManager::Get()->sendPakcet(OPCODE_FIGHTEND_ADD_TEMP_ATTR_INFO_C, &msg);
		}
	}
	else if(itemName=="onTempAddAttrSelTwo")
	{		
		const AdventureFightEndInfo::StepAdditionItem* it=ServerDateManager::Get()->getFightEndStepAdditionItemById(2);
		if(it->requireStar>ServerDateManager::Get()->getAdventureFightEndInfo()->mTodayFightInfo.currSurplusStar)
		{
			MSG_BOX("Star Not Enough!");
			return;
		}
		else
		{
			OPFightEndTempAddStepAttr msg;
			msg.set_version(1);
			msg.set_id(2);
			PacketManager::Get()->sendPakcet(OPCODE_FIGHTEND_ADD_TEMP_ATTR_INFO_C, &msg);
		}
	}
	else if(itemName=="onTempAddAttrSelThree")
	{
		const AdventureFightEndInfo::StepAdditionItem* it=ServerDateManager::Get()->getFightEndStepAdditionItemById(3);
		if(it->requireStar>ServerDateManager::Get()->getAdventureFightEndInfo()->mTodayFightInfo.currSurplusStar)
		{
			MSG_BOX("Star Not Enough!");
			return;
		}
		else
		{
			OPFightEndTempAddStepAttr msg;
			msg.set_version(1);
			msg.set_id(3);
			PacketManager::Get()->sendPakcet(OPCODE_FIGHTEND_ADD_TEMP_ATTR_INFO_C, &msg);
		}
	}
	else if(itemName=="onOneFightButton")
	{
		_addFightStarRate(3);
		OPFightEndBattle msg;
		msg.set_version(1);
		msg.set_id(1);
		PacketManager::Get()->sendPakcet(OPCODE_FIGHTEND_BATTLE_C, &msg);
	}
	else if(itemName=="onTwoFightButton")
	{
		_addFightStarRate(2);
		OPFightEndBattle msg;
		msg.set_version(1);
		msg.set_id(2);
		PacketManager::Get()->sendPakcet(OPCODE_FIGHTEND_BATTLE_C, &msg);
	}
	else if(itemName=="onThreeFightButton")
	{
		_addFightStarRate(1);
		OPFightEndBattle msg;
		msg.set_version(1);
		msg.set_id(3);
		PacketManager::Get()->sendPakcet(OPCODE_FIGHTEND_BATTLE_C, &msg);
	}
	else if(itemName=="onAddHpButton")
	{
		OPFightEndAddYestAttr msg;
		msg.set_version(1);
		msg.set_id(AdventureFightEndInfo::HP);
		PacketManager::Get()->sendPakcet(OPCODE_FIGHTEND_ADD_YEST_ATTR_INFO_C, &msg);
	}
	else if(itemName=="onAddAttackButton")
	{
		OPFightEndAddYestAttr msg;
		msg.set_version(1);
		msg.set_id(AdventureFightEndInfo::ATTATCK);
		PacketManager::Get()->sendPakcet(OPCODE_FIGHTEND_ADD_YEST_ATTR_INFO_C, &msg);
	}
	else if(itemName=="onAddDefButton")
	{
		OPFightEndAddYestAttr msg;
		msg.set_version(1);
		msg.set_id(AdventureFightEndInfo::DEFENSE);
		PacketManager::Get()->sendPakcet(OPCODE_FIGHTEND_ADD_YEST_ATTR_INFO_C, &msg);
	}
	else if(itemName=="onAddWillButton")
	{
		OPFightEndAddYestAttr msg;
		msg.set_version(1);
		msg.set_id(AdventureFightEndInfo::WILL);
		PacketManager::Get()->sendPakcet(OPCODE_FIGHTEND_ADD_YEST_ATTR_INFO_C, &msg);
	}
}

void FightEndPage::_addFightStarRate(unsigned int rate)
{
	int _index=4-rate;

	std::string _fightEnemyNameKey="fightEnemyNameKey";
	int memberId=1;
	try
	{
		memberId=mFightInfo->mFightTeamList.find(_index)->second->memberId;
	}
	catch (...)
	{
		
	}
	if(HAS_V(_fightEnemyNameKey))
	{
		SET_V(_fightEnemyNameKey,DiscipleTableManager::Get()->getDiscipleItemByID(memberId)->name);
	}
	else
	{
		ADD_V(_fightEnemyNameKey,DiscipleTableManager::Get()->getDiscipleItemByID(memberId)->name);
	}

	std::string key="fightEndStarRateKey";
	if(BlackBoard::Get()->hasVarible(key))
	{
		BlackBoard::Get()->setVarible(key,rate);
	}
	else
	{
		BlackBoard::Get()->addVarible(key,rate);
	}
}

void FightEndPage::loadData()
{
	bool _isReLoadData=false;
	{
		static time_t timestamp = time(0);
		time_t nowstamp = time(0);
		struct tm* timetm = localtime(&timestamp);
		struct tm savetm = {0};
		memcpy(&savetm, timetm, sizeof(struct tm));
		/*
			localtime返回同一个struct tm实例的指针
		*/
		struct tm* nowtm = localtime(&nowstamp);
		if (nowtm->tm_yday != savetm.tm_yday&&nowtm->tm_min>10)
		{
			timestamp=nowstamp;
			_isReLoadData=true;
		}
	}
	if(!getIsInitData()||_isReLoadData)
	{
		OPFightEndInfo msg;
		msg.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_GET_FIGHTEND_INFO_C, &msg);
	}
}

void FightEndPage::updatePage(bool isSelect,float dt)
{
	if(!getIsInitData())
	{
		return;
	}
	if(!mFightInfo)
	{
		return;
	}
	if(mFightInfo->mPanelType!=mCurrNodeType)
	{
		if(getNodePanelByType(mCurrNodeType))
		{
			getNodePanelByType(mCurrNodeType)->setVisible(false);
		}
		mCurrNodeType=mFightInfo->mPanelType;
		if(getNodePanelByType(mCurrNodeType))
		{
			getNodePanelByType(mCurrNodeType)->setVisible(true);
		}
	}
	else
	{
		if(!getNodePanelByType(mCurrNodeType)->isVisible())
		{
			getNodePanelByType(mCurrNodeType)->setVisible(true);
		}
		else
		{
			if(isSelect)
			{
				return;
			}
		}
	}
	switch (mCurrNodeType)
	{
		case AdventureFightEndInfo::MainPanel:refreshMainPanel();break;
		case AdventureFightEndInfo::YestAddAttrPanel:refreshAttrPanel();break;
		case AdventureFightEndInfo::TeamPanel:refreshFightPanel();break;
		case AdventureFightEndInfo::StepAddAttr:refreshStepAttrPanel();break;
		case AdventureFightEndInfo::RewardPanel:refreshRewardPanel();break;
	}
}

void FightEndPage::exitPage()
{
	//FightEndPage::~FightEndPage();
}

void FightEndPage::refreshMainPanel()
{
	CCNode* nodePanel=getNodePanelByType(mCurrNodeType);
	if(nodePanel)
	{
		CCObject* tempStatusObj=getContainerNode()->getVariable("mFightLevelStatus");
		if(!tempStatusObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mFightLevelStatus");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* fightLevelStatus=dynamic_cast<cocos2d::CCLabelBMFont* >(tempStatusObj);
		if(fightLevelStatus)
		{
			std::list<std::string> replaceList;
			replaceList.push_back(StringConverter::toString(mFightInfo->mTodayFightInfo.attackCount-mFightInfo->mTodayFightInfo.surplusAttackCount));
			replaceList.push_back(StringConverter::toString(mFightInfo->mTodayFightInfo.surplusAttackCount));
			fightLevelStatus->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@FightBattleStats"),&replaceList).c_str());
		}

		//max passlevel/obtainstar
		CCObject* todayPassLevelObj=getContainerNode()->getVariable("mPassLevel");
		if(!todayPassLevelObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mPassLevel");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* todayPassLevelBM=dynamic_cast<cocos2d::CCLabelBMFont* >(todayPassLevelObj);
		if(todayPassLevelBM)
		{

			todayPassLevelBM->setString(StringConverter::toString(mFightInfo->mTodayFightInfo.maxPassBarrier).c_str());
		}

		CCObject* todayObtainStarObj=getContainerNode()->getVariable("mObtainStar");
		if(!todayObtainStarObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mObtainStar");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* todayObtainStarBM=dynamic_cast<cocos2d::CCLabelBMFont* >(todayObtainStarObj);
		if(todayObtainStarBM)
		{
			todayObtainStarBM->setString(StringConverter::toString(mFightInfo->mTodayFightInfo.maxObtainStar).c_str());
		}

		CCObject* fightRankingObj=getContainerNode()->getVariable("mFightRanking");
		if(!fightRankingObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mFightRanking");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* fightRankingBM=dynamic_cast<cocos2d::CCLabelBMFont* >(fightRankingObj);
		if(fightRankingBM)
		{
			if(mFightInfo->mCalculateTodayRanking==0)
			{
				fightRankingBM->setString(Language::Get()->getString("@FightNotInRanking").c_str());
			}
			else
			{
				fightRankingBM->setString(StringConverter::toString(mFightInfo->mCalculateTodayRanking).c_str());
			}
			
		}
	}
	
}

void FightEndPage::refreshAttrPanel()
{
	CCNode* nodePanel=getNodePanelByType(mCurrNodeType);
	if(nodePanel)
	{
		CCObject* yestObtainStarObj=getContainerNode()->getVariable("mYestObtainStar");
		if(!yestObtainStarObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mYestObtainStar");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* yestObtainStarBM=dynamic_cast<cocos2d::CCLabelBMFont* >(yestObtainStarObj);
		if(yestObtainStarBM)
		{
			std::list<std::string> replaceList;
			replaceList.push_back(StringConverter::toString(mFightInfo->mYestFightInfo.maxObtainStar));
			yestObtainStarBM->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@FightLevelInfo"),&replaceList).c_str());
		}

		CCObject* yestAddAttrObj=getContainerNode()->getVariable("mFirstAddition");
		if(!yestAddAttrObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mFirstAddition");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* yestAddAttrBM=dynamic_cast<cocos2d::CCLabelBMFont* >(yestAddAttrObj);
		if(yestAddAttrBM)
		{
			std::list<std::string> replaceList;
			replaceList.push_back(StringConverter::toString(mFightInfo->mYestFightInfo.addition)+"%");
			yestAddAttrBM->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@FightAdditionInfo"),&replaceList).c_str());
		}
	}
}

void FightEndPage::refreshFightPanel()
{
	CCNode* nodePanel=getNodePanelByType(mCurrNodeType);
	if(nodePanel)
	{
		CCObject* currLevelObj=getContainerNode()->getVariable("mCurrLevel");
		if(!currLevelObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mCurrLevel");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* currLevelBM=dynamic_cast<cocos2d::CCLabelBMFont* >(currLevelObj);
		if(currLevelBM)
		{
			std::list<std::string> replaceList;
			replaceList.push_back(StringConverter::toString(mFightInfo->mTodayFightInfo.currBarrier));
			currLevelBM->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@FightEndCurrBattle"),&replaceList).c_str());
		}

		CCObject* currObtainStarObj=getContainerNode()->getVariable("mCurrObtainStar");
		if(!currObtainStarObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mCurrObtainStar");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* currObtainStarBM=dynamic_cast<cocos2d::CCLabelBMFont* >(currObtainStarObj);
		if(currObtainStarBM)
		{
			std::list<std::string> replaceList;
			replaceList.push_back(StringConverter::toString(mFightInfo->mTodayFightInfo.currObtainStar));
			currObtainStarBM->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@FightEndCurrObtainStar"),&replaceList).c_str());
		}

		CCObject* surplusStarObj=getContainerNode()->getVariable("mCurrSurplusStar");
		if(!surplusStarObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mCurrSurplusStar");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* surplusStarBM=dynamic_cast<cocos2d::CCLabelBMFont* >(surplusStarObj);
		if(surplusStarBM)
		{
			std::list<std::string> replaceList;
			replaceList.push_back(StringConverter::toString(mFightInfo->mTodayFightInfo.currSurplusStar));
			surplusStarBM->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@FightEndSurplusStar"),&replaceList).c_str());
		}

		CCObject* nextRewardBattleObj=getContainerNode()->getVariable("mNextGetRewardLevel");
		if(!nextRewardBattleObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mNextGetRewardLevel");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* nextRewardBattleBM=dynamic_cast<cocos2d::CCLabelBMFont* >(nextRewardBattleObj);
		if(nextRewardBattleBM)
		{
			std::list<std::string> replaceList;
			replaceList.push_back(StringConverter::toString(5-((mFightInfo->mTodayFightInfo.currBarrier-1)%5)));
			nextRewardBattleBM->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@FightEndNextGetRewardLevel"),&replaceList).c_str());
		}

		//teaminfo
		AdventureFightEndInfo::FightTeamInfoMap::iterator it=mFightInfo->mFightTeamList.begin();
		for(int i=1;it!=mFightInfo->mFightTeamList.end();++it,++i)
		{
			const AdventureFightEndInfo::FightTeamItem* teamItem=dynamic_cast<const AdventureFightEndInfo::FightTeamItem* >(it->second);
			const DiscipleItem* disciple=DiscipleTableManager::Get()->getDiscipleItemByID(teamItem->memberId);
			char tempKey[256];
			sprintf(tempKey,"mNPCIcon_%d",i);

			CCObject* iconObj=getContainerNode()->getVariable(tempKey);
			if(!iconObj)
			{
				char errMsg[256];
				sprintf(errMsg,"%s%s","Failed to find variable:",tempKey);
				CCMessageBox(errMsg,"error");
			}

			CCSprite* iconSprite=dynamic_cast<cocos2d::CCSprite* >(iconObj);
			if(iconSprite)
			{
				if(disciple)
				{
					iconSprite->setTexture(disciple->iconPic.c_str());
					//CCRect rect(0,0,tex->getPixelsWide(),tex->getPixelsHigh());
					//iconSprite->setTextureRect(rect);
				}
			}

			sprintf(tempKey,"mNPCFrame_%d",i);
			CCObject* iconFrameObj=getContainerNode()->getVariable(tempKey);
			if(!iconFrameObj)
			{
				char errMsg[256];
				sprintf(errMsg,"%s%s","Failed to find variable:",tempKey);
				CCMessageBox(errMsg,"error");
			}

			CCMenuItemImage* iconFrameMenu=dynamic_cast<cocos2d::CCMenuItemImage* >(iconFrameObj);
			if(iconFrameMenu)
			{
				if(disciple)
				{
					iconFrameMenu->setNormalImage(getFrameNormalSpirte(disciple->quality));
					iconFrameMenu->setSelectedImage(getFrameSelectedSpirte(disciple->quality));
				}
			}

			sprintf(tempKey,"mRewardIcon_%d",i);
			CCObject* rewardIconObj=getContainerNode()->getVariable(tempKey);
			if(!rewardIconObj)
			{
				char errMsg[256];
				sprintf(errMsg,"%s%s","Failed to find variable:",tempKey);
				CCMessageBox(errMsg,"error");
			}

			CCSprite* rewardIcon=dynamic_cast<cocos2d::CCSprite* >(rewardIconObj);

			sprintf(tempKey,"mRewardKeyBg_%d",i);
			CCObject* iconBgObj=getContainerNode()->getVariable(tempKey);
			if(!iconBgObj)
			{
				char errMsg[256];
				sprintf(errMsg,"%s%s","Failed to find variable:",tempKey);
				CCMessageBox(errMsg,"error");
			}

			CCSprite* rewardBg=dynamic_cast<cocos2d::CCSprite* >(iconBgObj);
			
			sprintf(tempKey,"mRewardFrame_%d",i);
			CCObject* rewardFrameObj=getContainerNode()->getVariable(tempKey);
			if(!iconFrameObj)
			{
				char errMsg[256];
				sprintf(errMsg,"%s%s","Failed to find variable:",tempKey);
				CCMessageBox(errMsg,"error");
			}

			CCMenuItemImage* rewardFrameMenu=dynamic_cast<cocos2d::CCMenuItemImage* >(rewardFrameObj);
			
			if(teamItem->rewardKeyId==mFightInfo->NONE)
			{
				if(rewardIcon)
				{
					rewardIcon->setVisible(false);
				}
				if(rewardFrameMenu)
				{
					rewardFrameMenu->setVisible(false);
				}
				if(rewardBg)
				{
					rewardBg->setVisible(false);
				}
			}
			else
			{
				const ToolItem* toolItem=ToolTableManager::Get()->getToolItemByID(teamItem->rewardKeyId);
				if(toolItem)
				{
					if(rewardIcon)
					{
						rewardIcon->setVisible(true);
						rewardIcon->setTexture(toolItem->iconPic.c_str());
					}

					if(rewardFrameMenu)
					{
						rewardFrameMenu->setVisible(true);
						rewardFrameMenu->setNormalImage(::getFrameNormalSpirte(1));
						rewardFrameMenu->setSelectedImage(::getFrameSelectedSpirte(1));
					}
					if(rewardBg)
					{
						rewardBg->setVisible(true);
					}
				}
				else
				{
					if(rewardIcon)
					{
						rewardIcon->setVisible(false);
					}
					if(rewardFrameMenu)
					{
						rewardFrameMenu->setVisible(false);
					}
					if(rewardBg)
					{
						rewardBg->setVisible(false);
					}
				}
			}
			sprintf(tempKey,"mPlayerTeamCount_%d",i);
			CCObject* selfTeamObj=getContainerNode()->getVariable(tempKey);
			if(!selfTeamObj)
			{
				char errMsg[256];
				sprintf(errMsg,"%s%s","Failed to find variable:",tempKey);
				CCMessageBox(errMsg,"error");
			}
			CCLabelBMFont* selfTeamBM=dynamic_cast<cocos2d::CCLabelBMFont* >(selfTeamObj);
			if(selfTeamBM)
			{
				selfTeamBM->setString(StringConverter::toString(teamItem->selfMember).c_str());
			}

			sprintf(tempKey,"mNPCTeamCount_%d",i);
			CCObject* npcTeamObj=getContainerNode()->getVariable(tempKey);
			if(!npcTeamObj)
			{
				char errMsg[256];
				sprintf(errMsg,"%s%s","Failed to find variable:",tempKey);
				CCMessageBox(errMsg,"error");
			}
			CCLabelBMFont* npcTeamBM=dynamic_cast<cocos2d::CCLabelBMFont* >(npcTeamObj);
			if(npcTeamBM)
			{
				npcTeamBM->setString(StringConverter::toString(teamItem->opponentMember).c_str());
			}
		}

		//max passlevel/obtainstar
		CCObject* todayPassLevelObj=getContainerNode()->getVariable("mTodayPassLevel");
		if(!todayPassLevelObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mTodayPassLevel");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* todayPassLevelBM=dynamic_cast<cocos2d::CCLabelBMFont* >(todayPassLevelObj);
		if(todayPassLevelBM)
		{
	
			todayPassLevelBM->setString(StringConverter::toString(mFightInfo->mTodayFightInfo.maxPassBarrier).c_str());
		}

		CCObject* todayObtainStarObj=getContainerNode()->getVariable("mTodayObtainStar");
		if(!todayObtainStarObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mTodayObtainStar");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* todayObtainStarBM=dynamic_cast<cocos2d::CCLabelBMFont* >(todayObtainStarObj);
		if(todayObtainStarBM)
		{
			todayObtainStarBM->setString(StringConverter::toString(mFightInfo->mTodayFightInfo.maxObtainStar).c_str());
		}

		//currAddition info
		CCObject* hpObj=getContainerNode()->getVariable("mAddAttrHpLabel");
		if(!hpObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mAddAttrHpLabel");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* hpBM=dynamic_cast<cocos2d::CCLabelBMFont* >(hpObj);
		if(hpBM)
		{
			hpBM->setString((StringConverter::toString(mFightInfo->mTodayFightInfo.todayAddition.hp)+"%").c_str());
		}

		CCObject* attackObj=getContainerNode()->getVariable("mAddAttrAttackLabel");
		if(!attackObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mAddAttrAttackLabel");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* attackBM=dynamic_cast<cocos2d::CCLabelBMFont* >(attackObj);
		if(attackBM)
		{
			attackBM->setString((StringConverter::toString(mFightInfo->mTodayFightInfo.todayAddition.attack)+"%").c_str());
		}

		CCObject* defenseObj=getContainerNode()->getVariable("mAddAttrDefenLabel");
		if(!defenseObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mAddAttrDefenLabel");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* defenseBM=dynamic_cast<cocos2d::CCLabelBMFont* >(defenseObj);
		if(defenseBM)
		{
			defenseBM->setString((StringConverter::toString(mFightInfo->mTodayFightInfo.todayAddition.defense)+"%").c_str());
		}

		CCObject* willObj=getContainerNode()->getVariable("mAddAttrWillLabel");
		if(!willObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mAddAttrWillLabel");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* willBM=dynamic_cast<cocos2d::CCLabelBMFont* >(willObj);
		if(willBM)
		{
			willBM->setString((StringConverter::toString(mFightInfo->mTodayFightInfo.todayAddition.will)+"%").c_str());
		}

	}
}

void FightEndPage::refreshStepAttrPanel()
{
	CCNode* nodePanel=getNodePanelByType(mCurrNodeType);
	if(nodePanel)
	{
		CCObject* currLevelObj=getContainerNode()->getVariable("mTempPassLevel");
		if(!currLevelObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mTempPassLevel");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* currLevelBM=dynamic_cast<cocos2d::CCLabelBMFont* >(currLevelObj);
		if(currLevelBM)
		{
			std::list<std::string> replaceList;
			replaceList.push_back(StringConverter::toString(mFightInfo->mTodayFightInfo.currBarrier));
			currLevelBM->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@FightEndCurrBattle"),&replaceList).c_str());
		}

		CCObject* currObtainStarObj=getContainerNode()->getVariable("mTempObtainStar");
		if(!currObtainStarObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mTempObtainStar");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* currObtainStarBM=dynamic_cast<cocos2d::CCLabelBMFont* >(currObtainStarObj);
		if(currObtainStarBM)
		{
			std::list<std::string> replaceList;
			replaceList.push_back(StringConverter::toString(mFightInfo->mTodayFightInfo.currObtainStar));
			currObtainStarBM->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@FightEndCurrObtainStar"),&replaceList).c_str());
		}

		CCObject* surplusStarObj=getContainerNode()->getVariable("mSurplusStar");
		if(!surplusStarObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mCurrSurplusStar");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* surplusStarBM=dynamic_cast<cocos2d::CCLabelBMFont* >(surplusStarObj);
		if(surplusStarBM)
		{
			std::list<std::string> replaceList;
			replaceList.push_back(StringConverter::toString(mFightInfo->mTodayFightInfo.currSurplusStar));
			surplusStarBM->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@FightEndSurplusStar"),&replaceList).c_str());
		}

		//teaminfo
		AdventureFightEndInfo::StepAdditionItemMap::iterator it=mFightInfo->mStepAdditionItemList.begin();
		for(int i=1;it!=mFightInfo->mStepAdditionItemList.end();++it,++i)
		{
			const AdventureFightEndInfo::StepAdditionItem* stepAdditionItem=dynamic_cast<const AdventureFightEndInfo::StepAdditionItem* >(it->second);
			char tempKey[128];
			sprintf(tempKey,"mTempAddAttrName_%d",i);
			CCObject* attrNameObj=getContainerNode()->getVariable(tempKey);
			if(!attrNameObj)
			{
				char errMsg[256];
				sprintf(errMsg,"%s","Failed to find variable:%s",tempKey);
				CCMessageBox(errMsg,"error");
			}
			CCSprite* attrNameIcon=dynamic_cast<cocos2d::CCSprite* >(attrNameObj);
			if(attrNameIcon)
			{
				sprintf(tempKey,"Buff_ImageFile%d",stepAdditionItem->type);
				attrNameIcon->initWithFile(VaribleManager::Get()->getSetting(tempKey).c_str());
			}

			sprintf(tempKey,"mTempAddAttrValue_%d",i);
			CCObject* attrValueObj=getContainerNode()->getVariable(tempKey);
			if(!attrValueObj)
			{
				char errMsg[256];
				sprintf(errMsg,"%s%s","Failed to find variable:",tempKey);
				CCMessageBox(errMsg,"error");
			}
			CCLabelBMFont* attrValueBM=dynamic_cast<cocos2d::CCLabelBMFont* >(attrValueObj);
			if(attrValueBM)
			{
				attrValueBM->setString((StringConverter::toString(stepAdditionItem->percentage)+"%").c_str());
			}

			sprintf(tempKey,"mTempConsumeStar_%d",i);
			CCObject* consumeObj=getContainerNode()->getVariable(tempKey);
			if(!consumeObj)
			{
				char errMsg[256];
				sprintf(errMsg,"%s%s","Failed to find variable:",tempKey);
				CCMessageBox(errMsg,"error");
			}
			CCLabelBMFont* consumeBM=dynamic_cast<cocos2d::CCLabelBMFont* >(consumeObj);
			if(consumeBM)
			{
				consumeBM->setString((StringConverter::toString(stepAdditionItem->requireStar)).c_str());
			}

			sprintf(tempKey,"mTempAddAttrBtn_%d",i);
			CCObject* selBtnObj=getContainerNode()->getVariable(tempKey);
			if(!selBtnObj)
			{
				char errMsg[256];
				sprintf(errMsg,"%s%s","Failed to find variable:",tempKey);
				CCMessageBox(errMsg,"error");
			}
			CCMenuItemImage* selBtn=dynamic_cast<cocos2d::CCMenuItemImage* >(selBtnObj);
			if(selBtn)
			{
				if(stepAdditionItem->requireStar<=mFightInfo->mTodayFightInfo.currSurplusStar)
				{
					selBtn->setEnabled(true);
				}
				else
				{
					selBtn->setEnabled(false);
				}
			}
			
		}

		//currAddition info
		CCObject* hpObj=getContainerNode()->getVariable("mTempAddAttrHpLabel");
		if(!hpObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mTempAddAttrHpLabel");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* hpBM=dynamic_cast<cocos2d::CCLabelBMFont* >(hpObj);
		if(hpBM)
		{
			hpBM->setString((StringConverter::toString(mFightInfo->mTodayFightInfo.todayAddition.hp)+"%").c_str());
		}

		CCObject* attackObj=getContainerNode()->getVariable("mTempAddAttrAttackLabel");
		if(!attackObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mTempAddAttrAttackLabel");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* attackBM=dynamic_cast<cocos2d::CCLabelBMFont* >(attackObj);
		if(attackBM)
		{
			attackBM->setString((StringConverter::toString(mFightInfo->mTodayFightInfo.todayAddition.attack)+"%").c_str());
		}

		CCObject* defenseObj=getContainerNode()->getVariable("mTempAddAttrDefenLabel");
		if(!defenseObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mTempAddAttrDefenLabel");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* defenseBM=dynamic_cast<cocos2d::CCLabelBMFont* >(defenseObj);
		if(defenseBM)
		{
			defenseBM->setString((StringConverter::toString(mFightInfo->mTodayFightInfo.todayAddition.defense)+"%").c_str());
		}

		CCObject* willObj=getContainerNode()->getVariable("mTempAddAttrWillLabel");
		if(!willObj)
		{
			char errMsg[256];
			sprintf(errMsg,"%s","Failed to find variable:mTempAddAttrWillLabel");
			CCMessageBox(errMsg,"error");
		}
		CCLabelBMFont* willBM=dynamic_cast<cocos2d::CCLabelBMFont* >(willObj);
		if(willBM)
		{
			willBM->setString((StringConverter::toString(mFightInfo->mTodayFightInfo.todayAddition.will)+"%").c_str());
		}

	}
}

void FightEndPage::refreshRewardPanel()
{
	
	CCObject* stepObtainStar=getContainerNode()->getVariable("mPassLevelObtainStar");
	if(!stepObtainStar)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mPassLevelObtainStar");
		CCMessageBox(errMsg,"error");
	}
	CCLabelBMFont* stepObtainStarBM=dynamic_cast<cocos2d::CCLabelBMFont* >(stepObtainStar);
	if(stepObtainStarBM)
	{
		char temp[128];
		sprintf(temp,"%d-%d",(mFightInfo->mTodayFightInfo.currBarrier-5),(mFightInfo->mTodayFightInfo.currBarrier-1));
		std::list<std::string> _list;
		_list.push_back(temp);
		_list.push_back(StringConverter::toString(mFightInfo->mTodayFightInfo.stepObtainStar));
		stepObtainStarBM->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@FightEndStepObtainStar"),&_list).c_str());
	}
	const FightEndRewardItem* item=FightEndRewardTableManager::Get()->getFightEndRewardItemByID((mFightInfo->mTodayFightInfo.currBarrier-1));
	unsigned int silverCount=0;
	std::string descrip="";
	if(item)
	{
		descrip=item->rewardInfo;
		unsigned int battleNum=BattleArrayInfo::Get()->getBattleEnableSize();
		if(battleNum==5)
		{
			silverCount=item->battle5;
		}
		else if(battleNum==6)
		{
			silverCount=item->battle6;
		}
		else if(battleNum==7)
		{
			silverCount=item->battle7;
		}
		else if(battleNum==8)
		{
			silverCount=item->battle8;
		}
	}
	CCObject* rewardCountObj=getContainerNode()->getVariable("mRewardCount");
	if(!rewardCountObj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mRewardCount");
		CCMessageBox(errMsg,"error");
	}
	CCLabelBMFont* rewardCountBM=dynamic_cast<cocos2d::CCLabelBMFont* >(rewardCountObj);
	if(rewardCountBM)
	{
		rewardCountBM->setString(StringConverter::toString(silverCount).c_str());
	}

	CCObject* obtainStarReward=getContainerNode()->getVariable("mObtainStarRewardDes");
	if(!obtainStarReward)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mObtainStarRewardDes");
		CCMessageBox(errMsg,"error");
	}
	CCLabelBMFont* obtainStarRewardBM=dynamic_cast<cocos2d::CCLabelBMFont* >(obtainStarReward);
	if(obtainStarRewardBM)
	{
		std::list<std::string> _list;
		bool isExceed=false;
		if(mFightInfo->mTodayFightInfo.stepObtainStar>mFightInfo->mTodayFightInfo.exceedObtainStar)
		{
			isExceed=true;
			_list.push_back(StringConverter::toString(mFightInfo->mTodayFightInfo.stepObtainStar-mFightInfo->mTodayFightInfo.exceedObtainStar));
		}
		else
		{
			_list.push_back(StringConverter::toString(mFightInfo->mTodayFightInfo.exceedObtainStar));
		}
		obtainStarRewardBM->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString((isExceed?"@FightEndObtainStarRewardDes":"@FightEndObtainStarNoExceedDes")),&_list).c_str());
		CCB_FUNC(mContentContainer,"mFELable1",CCLabelBMFont,setString(Language::Get()->getString((isExceed?"@FightGetReward":"@FightGoOn")).c_str()));
	}

	CCObject* rewardDescription=getContainerNode()->getVariable("mRewardDescription");
	if(!rewardDescription)
	{
		char errMsg[256];
		sprintf(errMsg,"%s","Failed to find variable:mRewardDescription");
		CCMessageBox(errMsg,"error");
	}
	CCLabelBMFont* rewardDescriptionBM=dynamic_cast<cocos2d::CCLabelBMFont* >(rewardDescription);
	if(rewardDescriptionBM)
	{
		char temp[128];
		sprintf(temp,"%d-%d",(mFightInfo->mTodayFightInfo.currBarrier-5),(mFightInfo->mTodayFightInfo.currBarrier-1));
		std::list<std::string> _list;
		_list.push_back(temp);
		_list.push_back(descrip);
		rewardDescriptionBM->setString(GameMaths::replaceStringWithStringList(Language::Get()->getString("@FightPassLevelRewardDescription"),&_list).c_str());
	}
	rebuildAllRewardItem();
}

void FightEndPage::buildRewardItem( void )
{
	AdventureFightEndInfo::RewardItemMapIterator it=ServerDateManager::Get()->getFightEndRewardItemMapIterator();
	while(it.hasMoreElements())
	{
		RewardContent * item = new RewardContent(it.getNext()->id);
		item->init();
		addRewardItem(item);
	}
	mRewardContent->forceRecaculateChildren();
	if(mRewardContent->getContentSize().width>mRewardContent->getViewSize().width)
	{
		mRewardContent->setTouchEnabled(true);
	}
	else
	{
		mRewardContent->setTouchEnabled(false);
	}
}

void FightEndPage::addRewardItem( ContentBase* item )
{
	mRewardContentList.push_back(item);
	mRewardContent->getContainer()->addChild(item->getContainerNode());
	CCPoint pos = ccp(item->getContainerNode()->getContentSize().width*(mRewardContentList.size()-1),0);
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width*(mRewardContentList.size()-1), item->getContainerNode()->getContentSize().height);
	mRewardContent->setContentSize(size);
	mRewardContent->setContentOffset(ccp(0, mRewardContent->getViewSize().height - mRewardContent->getContentSize().height*mRewardContent->getScaleY()));
}

void FightEndPage::rebuildAllRewardItem( void )
{
	clearAllRewardItem();
	buildRewardItem();
}

void FightEndPage::clearAllRewardItem( void )
{
	ContentList::iterator itr = mRewardContentList.begin();
	while(itr != mRewardContentList.end())
	{
		delete (*itr);
		++itr;
	}
	ContentList list;
	mRewardContentList.swap(list);
	if(mRewardContent)
		mRewardContent->getContainer()->removeAllChildren();
}

FightEndPage::~FightEndPage()
{
	clearAllRewardItem();
}
