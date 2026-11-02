
#include "stdafx.h"

#include "ArenaPage.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Arena.h"
#include "math.h"
#include "Disciple.h"
#include "GameMessages.h"
#include "PopupNotEnoughPage.h"
#include "GameMaths.h"
#include "BattleArrayInfo.h"
#include "DataTableManager.h"
#include "MessageBoxPage.h"
#include "FightPage.h"
REGISTER_PAGE("ArenaPage",ArenaPage);
USING_NS_CC;

void ArenaOpponentContent::refreshContent( void )
{
	mCanViewTeam=false;
	ArenaOpponent Arena(mID);
	CCMenuItemImage* functionPic=dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mFunctionPic"));
	CCLabelBMFont* functionName=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mFunctionLable"));
	if(functionPic && functionName)
	{
		if(Arena.canChallenge())
		{
			functionPic->setVisible(true);
			functionName->setVisible(true);
			functionPic->setNormalImage(Arena.getFunctionPic_Normal());
			functionPic->setSelectedImage(Arena.getFunctionPic_Selected());
			functionName->setString(Language::Get()->getString(Arena.getFunctionName()).c_str());
		}
		else
		{
			if(Arena.arenaType()==1)
			{
				mCanViewTeam=true;
				functionPic->setVisible(true);
				functionName->setVisible(true);
				functionName->setString(Language::Get()->getString(Arena.getFunctionName()).c_str());
			}
			else
			{
				functionPic->setVisible(false);
				functionName->setVisible(false);
			}
			
		}
	}

	CCLabelBMFont* rank=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mRank"));
	if(rank)
	{
		rank->setString(StringConverter::toString(Arena.arenaRank()).c_str());
	}
	CCLabelTTF* playerName=dynamic_cast<CCLabelTTF* >(mContentContainer->getVariable("mPlayerName"));
	if(playerName)
	{
		playerName->setString(Language::Get()->getString(Arena.name()).c_str());
	}
	CCLabelBMFont* scoreReward=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mScoreReward"));
	if(scoreReward)
	{
		scoreReward->setString((StringConverter::toString(Arena.rewardScore())+Language::Get()->getString("@ArenaScore")).c_str());
	}
	CCLabelBMFont* playerLV=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mPlayerLV"));
	if(playerLV)
	{
		playerLV->setString(StringConverter::toString(Arena.level()).c_str());
	}
	CCLabelBMFont* onceDefeatU=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mOnceDefeatU"));
	if(playerLV)
	{
		if(Arena.isDefeatBy())
			onceDefeatU->setVisible(true);
		else
			onceDefeatU->setVisible(false);
	}

	CCSprite* head1=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mHead1"));
	CCSprite* frame1=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mFrame1"));
	if(head1&&frame1)
	{
		if(Arena.dicipleItemID1() == 0)
		{
			head1->setVisible(false);
			frame1->setVisible(false);

		}
		else
		{
			head1->setVisible(true);
			frame1->setVisible(true);
			Disciple disciple(Arena.dicipleItemID1(), true);
			head1->initWithFile(disciple.iconPic().c_str());
			frame1->setTexture(disciple.getFrameNormalSpirte()->getTexture());
			frame1->setColor(disciple.getFrameColor());
		}
		
	}

	CCSprite* head2=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mHead2"));
	CCSprite* frame2=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mFrame2"));
	if(head2&&frame2)
	{
		if(Arena.dicipleItemID2() == 0)
		{
			head2->setVisible(false);
			frame2->setVisible(false);

		}
		else
		{
			head2->setVisible(true);
			frame2->setVisible(true);
			Disciple disciple(Arena.dicipleItemID2(), true);
			head2->initWithFile(disciple.iconPic().c_str());
			frame2->setTexture(disciple.getFrameNormalSpirte()->getTexture());
			frame2->setColor(disciple.getFrameColor());
		}
		
	}

	CCSprite* head3=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mHead3"));
	CCSprite* frame3=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mFrame3"));
	if(head3&&frame3)
	{
		if(Arena.dicipleItemID3() == 0)
		{
			head3->setVisible(false);
			frame3->setVisible(false);

		}
		else
		{
			head3->setVisible(true);
			frame3->setVisible(true);
			Disciple disciple(Arena.dicipleItemID3(), true);
			head3->initWithFile(disciple.iconPic().c_str());
			frame3->setTexture(disciple.getFrameNormalSpirte()->getTexture());
			frame3->setColor(disciple.getFrameColor());
		}
		
	}

}

void ArenaOpponentContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onFunciton")
	{
		if(!mCanViewTeam)
		{
			UserArenaInfo& info = ServerDateManager::Get()->getUserArenaInfo();
			if(ServerDateManager::Get()->getUserBasicInfo().vitality<=0)
			{
				POP_NOT_ENOUGH_BOX(VitalityNotEnough);
			}
			else if( info.arenaNum == 0 )
			{
				MsgMainFramePushPage msg;
				msg.pageName = "ArenaBuyChallengePage";
				MessageManager::Get()->sendMessage(&msg);
			}
			else
			{
				ArenaOpponent Arena(mID);
				std::string _fightEnemyNameKey="fightEnemyNameKey";
				if(HAS_V(_fightEnemyNameKey))
				{
					SET_V(_fightEnemyNameKey,Arena.name());
				}
				else
				{
					ADD_V(_fightEnemyNameKey,Arena.name());
				}
				BasePage* bp = CCBManager::Get()->getPage("FightPage");
				if(bp)
				{
					FightPage* fp = dynamic_cast<FightPage*>(bp);
					fp->setViewTeamStatus(mID,2,0);
				}
				OPChallengeOpponent challengeOpponent;
				challengeOpponent.set_opponentid(mID);
				challengeOpponent.set_opponentrank(ArenaOpponent(mID).arenaRank());
				PacketManager::Get()->sendPakcet(OPCODE_CHALLENGE_OPPONENT_C, &challengeOpponent);	
			}
		}
		else
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
			msg->set_isnpc(1);
			msg->set_relationid(mID);
			msg->set_relationtype(2);
			PacketManager::Get()->sendPakcet(OPCODE_SHOW_USER_BATTLE_C,msg);
		}
	}
}

void ArenaUserContent::refreshContent( void )
{
	UserArenaInfo& aInfo = ServerDateManager::Get()->getUserArenaInfo();
	const UserBasicInfo& bInfo = ServerDateManager::Get()->getUserBasicInfo();

	CCLabelBMFont* rank=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mRank"));
	if(rank)
	{
		rank->setString(StringConverter::toString(aInfo.arenaRank).c_str());
	}
	CCLabelTTF* playerName=dynamic_cast<CCLabelTTF* >(mContentContainer->getVariable("mPlayerName"));
	if(playerName)
	{
		playerName->setString(Language::Get()->getString(bInfo.name).c_str());
	}
	CCLabelBMFont* scoreReward=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mScoreReward"));
	if(scoreReward)
	{
		scoreReward->setString((StringConverter::toString(aInfo.rewardScore)+Language::Get()->getString("@ArenaScore")).c_str());
	}
	CCLabelBMFont* playerLV=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mPlayerLV"));
	if(playerLV)
	{
		playerLV->setString(StringConverter::toString(bInfo.level).c_str());
	}
	CCLabelBMFont* score=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mScore"));
	if(score)
	{
		score->setString(StringConverter::toString(aInfo.arenaScore).c_str());
	}
	CCLabelBMFont* scoreReward1=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mScoreReward1"));
	if(scoreReward1)
	{
		scoreReward1->setString(StringConverter::toString(aInfo.rewardScore).c_str());
	}
}

void ArenaUserContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onRefresh")
	{
		OPRefreshArenaScore refreshArenaScore;
		refreshArenaScore.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_REFRESH_ARENA_C, &refreshArenaScore);
		MSG_BOX("@ArenaRefreshed");
	}
}

void ArenaRewardContent::refreshContent( void )
{	
	ArenaReward arenaReward(mID);
	CCSprite* prop=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mHeadPic"));
	if(prop)
	{
		prop->initWithFile(arenaReward.iconPic().c_str());
		if(arenaReward.type()==1)
		{
			prop->setScale(1.0f);
		}
		else
		{
			prop->setScale(0.4f);
		}

	}
	CCSprite* frame=dynamic_cast<CCSprite* >(mContentContainer->getVariable("mFrame"));
	if(frame)
	{
		frame->initWithFile(::getQualityFrameNormalImageFile().c_str());
	}
	CCMenuItemImage* recieve=dynamic_cast<CCMenuItemImage* >(mContentContainer->getVariable("mRecieve"));
	if(recieve)
	{
		if(arenaReward.isEnable())
			recieve->setEnabled(true);
		else
			recieve->setEnabled(false);
	}
	CCLabelBMFont* rewardName=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mRewardName"));
	if(rewardName)
	{
		rewardName->setString(Language::Get()->getString(arenaReward.title()).c_str());
	}
	CCLabelBMFont* buttonLabel=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mButtonLabel"));
	if(buttonLabel)
	{
		buttonLabel->setString(Language::Get()->getString(arenaReward.getFunctionName()).c_str());
	}
	CCLabelBMFont* scoreNeed=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mScoreNeed"));
	if(scoreNeed)
	{
		scoreNeed->setString(StringConverter::toString(arenaReward.needScore()).c_str());
	}
	CCLabelBMFont* rewardDes=dynamic_cast<CCLabelBMFont* >(mContentContainer->getVariable("mRewardDes"));
	if(rewardDes)
	{
		std::string describe;
		int lines;
		int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("ArenaPageReward_Desc"));
		GameMaths::stringAutoReturn(arenaReward.describe(),describe,width,lines);
		rewardDes->setString(describe.c_str());
	}
}

void ArenaRewardContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onRecieve")
	{
		ADD_OR_SET_V("ArenaRewardID", mID);
		OPGetArenaReward getArenaReward;
		getArenaReward.set_rewardid(mID);
		PacketManager::Get()->sendPakcet(OPCODE_GET_ARENA_REWARD_C,&getArenaReward);

// 		MsgMainFramePushPage msg;
// 		msg.pageName = "ArenaBuyChallengePage";
// 		MessageManager::Get()->sendMessage(&msg);
	}

}

ArenaPage::ArenaPage(void)
{
	mExpbarInitScale = -1.0f;
	mCurrTabIndex = 0;
	membersRootNode = NULL;
	members = NULL;
	mContentList.clear();
}

ArenaPage::~ArenaPage(void)
{
}

void ArenaPage::Enter( MainFrame* frame)
{
	_isFirstLoad=true;
	mFrameCount = 0;
	mLoadDataStep=(unsigned int)(ceil(1.0f/GamePrecedure::Get()->getFrameTime()));
	MessageManager::Get()->regisiterMessageHandler(MSG_MAINFRAME_REFRESH,this);
	mCurrTabIndex = 0;

	OPUserArenaInfo userArenaInfo;
	userArenaInfo.set_version(1);
	PacketManager::Get()->sendPakcet(OPCODE_USER_ARENA_INFO_C,&userArenaInfo);

	OPGetArenaOpponentList getArenaOpponentList;
	getArenaOpponentList.set_version(1);
	PacketManager::Get()->sendPakcet(OPCODE_GET_ARENA_OPPONENT_LIST_C,&getArenaOpponentList);

	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	MessageManager::Get()->regisiterMessageHandler(MSG_FIGHT_END,this);
	_refreshTitle();
	//selectTab(1);
}

void ArenaPage::Exit( MainFrame* frame)
{
	MessageManager::Get()->removeMessageHandler(this);
	clearAllItem();
	mCurrTabIndex = 0;
}

void ArenaPage::Execute( MainFrame* )
{
	mFrameCount++;
	if(mFrameCount%mLoadDataStep==0)
	{
		mFrameCount=0;
		_refreshSyncTime();
	}
}

void ArenaPage::load( void )
{
	loadCcbiFile("Arena.ccbi");
	CCNode* expbar=dynamic_cast<CCNode* >(getVariable("mExpBar"));
	if(expbar && mExpbarInitScale<0)
		mExpbarInitScale = expbar->getScaleX();

	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));
	membersRootNode = members->getContainer();

	if(mCurrTabIndex==0)
	{
		CCNode* arenaScore = dynamic_cast<CCNode* >(getVariable("mArenaScore"));
		if(arenaScore)
			arenaScore->setVisible(false);
	}
}

void ArenaPage::buildItem( void )
{
	if(mCurrTabIndex == 1)
	{
		ArenaOpponentDateCollect::ArenaOpponentIDCollect collect = ArenaOpponentDateCollect::Get()->getArenaOpponentCollection(ArenaOpponentFilter_Type4);
		ArenaOpponentDateCollect::ArenaOpponentIDCollect::iterator itr = collect.begin();
		for(; itr != collect.end(); ++itr)
		{
			ArenaOpponentContent * item = new ArenaOpponentContent((*itr));
			item->init();
			addItem(item);
		}

		ArenaUserContent * item = new ArenaUserContent(0);
		item->init();
		addItem(item);

		collect = ArenaOpponentDateCollect::Get()->getArenaOpponentCollection(ArenaOpponentFilter_Type2_3);
		itr = collect.begin();
		bool b = true;
		int tempCount = 1;
		for(; itr != collect.end(); ++itr)
		{
			ArenaOpponentContent * item = new ArenaOpponentContent((*itr));
			item->init();
			addItem(item,b);
			if(tempCount>0){
				tempCount--;
				b = true;
			}else{
				b = false;
			}
		}

		collect = ArenaOpponentDateCollect::Get()->getArenaOpponentCollection(ArenaOpponentFilter_Type1);
		itr = collect.begin();
		for(; itr != collect.end(); ++itr)
		{
			ArenaOpponentContent * item = new ArenaOpponentContent((*itr));
			item->init();
			addItem(item,false);
		}
	}
	else
	{
		ArenaRewardDateCollect::ArenaRewardIDCollect collect = ArenaRewardDateCollect::Get()->getArenaRewardCollection();
		ArenaRewardDateCollect::ArenaRewardIDCollect::iterator itr = collect.begin();
		for(; itr != collect.end(); ++itr)
		{
			ArenaReward arenaReward((*itr));
			bool isShowItem = true;
			if(arenaReward.type()==2)
			{
				UserArenaInfo& info = ServerDateManager::Get()->getUserArenaInfo();
				if(info.gotRewardIDs.find(arenaReward.itemID()) != info.gotRewardIDs.end())
				{
					isShowItem = false;
				}
			}	
			if(isShowItem)
			{
				ArenaRewardContent * item = new ArenaRewardContent((*itr));
				item->init();
				addItem(item);
			}
		}
	}
	
	members->forceRecaculateChildren();
}

void ArenaPage::addItem( ContentBase* item, bool needSetOffSet/* = true*/ )
{
	mContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp(0,item->getContainerNode()->getContentSize().height*(mContentList.size()-1));
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mContentList.size());
	members->setContentSize(size);
	if(needSetOffSet)
		members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
}

void ArenaPage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void ArenaPage::clearAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		delete (*itr);
		++itr;
	}
	ContentList list;
	mContentList.swap(list);
	membersRootNode->removeAllChildren();
}

void ArenaPage::refreshAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		(*itr)->refreshContent();
		++itr;
	}
}

void ArenaPage::removeItem( ContentBase* item )
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

void ArenaPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onArena")
	{
		selectTab(1);	
	}
	else if(itemName=="onReward")
	{
		selectTab(2);
	}
	else if(itemName=="onRefresh")
	{
		_isFirstLoad=false;
		OPRefreshArenaScore refreshArenaScore;
		refreshArenaScore.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_REFRESH_ARENA_C, &refreshArenaScore);
	}
	else if(itemName=="onPlayerInfo")
	{
		MsgMainFramePushPage msg;
		msg.pageName = "PlayerInfPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void ArenaPage::_refreshTitle()
{
	const UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	CCLabelTTF* name = dynamic_cast<CCLabelTTF* >(getVariable("mName"));
	if(name)
	{
		name->setString(userBasicInfo.name.c_str());
	}
	CCSprite* vipLevel=dynamic_cast<CCSprite* >(getVariable("mVipPic"));
	if(vipLevel)
	{
		const std::string vipImage = ::getVipLevelImageFile(userBasicInfo.viplevel);
		if(vipImage!="")
		{
			vipLevel->setVisible(true);
			vipLevel->initWithFile(vipImage.c_str());
		}else
		{
			vipLevel->setVisible(false);
		}
	}
	CCNode* expbar=dynamic_cast<CCNode* >(getVariable("mExpBar"));
	if(expbar)
	{
		if(mExpbarInitScale<0)
			mExpbarInitScale = expbar->getScaleX();

		float expscale = 0.0f;
		if(ServerDateManager::Get()->getNextLevelExp()>0)
			expscale = ServerDateManager::Get()->getUserExpPercent();
		if(expscale>1.0f)expscale=1.0f;
		expbar->setScaleX(expscale*mExpbarInitScale);
	}
	CCLabelBMFont* exp = dynamic_cast<CCLabelBMFont* >(getVariable("mExp"));
	if(exp)
	{
		exp->setString(ServerDateManager::Get()->getUserExpPercentStr().c_str());
	}
	CCLabelBMFont* coin = dynamic_cast<CCLabelBMFont* >(getVariable("mCoin"));
	if(coin)
	{
		coin->setString((StringConverter::toString(ServerDateManager::Get()->getUserBattleSize())+"/"+StringConverter::toString(BattleArrayInfo::Get()->getBattleEnableSize())).c_str());
	}
	CCLabelBMFont* qi = dynamic_cast<CCLabelBMFont* >(getVariable("mQi"));
	if(qi)
	{
		qi->setString((StringConverter::toString(userBasicInfo.vitality)+"/"+StringConverter::toString(userBasicInfo.maxvitality)).c_str());
	}
	CCB_FUNC(this,"mLV",CCLabelBMFont,setString(StringConverter::toString(userBasicInfo.level).c_str()));
}

void ArenaPage::refreshPage( void )
{
	UserArenaInfo& info = ServerDateManager::Get()->getUserArenaInfo();
	if(mCurrTabIndex == 1)
	{
		CCNode* arenaScore = dynamic_cast<CCNode* >(getVariable("mArenaScore"));
		if(arenaScore)
			arenaScore->setVisible(false);
		CCNode* label = dynamic_cast<CCNode* >(getVariable("mLable"));
		if(label)
			label->setVisible(true);

		CCLabelBMFont* ranking = dynamic_cast<CCLabelBMFont* >(getVariable("mRanking"));
		if(ranking)
		{
			ranking->setString(StringConverter::toString(info.arenaRank).c_str());
		}
		CCLabelBMFont* remainedTimes = dynamic_cast<CCLabelBMFont* >(getVariable("mRemainedTimes"));
		if(remainedTimes)
		{
			remainedTimes->setString(StringConverter::toString(info.arenaNum).c_str());
		}
	}
	else if(mCurrTabIndex == 2)
	{
		CCNode* arenaScore = dynamic_cast<CCNode* >(getVariable("mArenaScore"));
		if(arenaScore)
			arenaScore->setVisible(true);
		CCNode* label = dynamic_cast<CCNode* >(getVariable("mLable"));
		if(label)
			label->setVisible(false);

		CCLabelBMFont* score = dynamic_cast<CCLabelBMFont* >(getVariable("mScore"));
		if(score)
		{
			score->setString(StringConverter::toString(info.arenaScore).c_str());
		}
		CCLabelBMFont* timeLabel = dynamic_cast<CCLabelBMFont* >(getVariable("mTimeLabel"));
		if(_isFirstLoad)
		{
			_calcNextSyncTime(info.rewardRefreshTime);
			if(timeLabel)
			{
				timeLabel->setString(GameMaths::formatSecondsToTime(mNextSyncTime).c_str());
			}
		}
		
	}
}
bool ArenaPage::judgeLoseDate(){
	if(BlackBoard::Get()->loseArenaData==1){
		return false;

	}else{
		MSG_BOX_LAN("@LoseArenaData");
		CCNode* arenaScore = dynamic_cast<CCNode* >(getVariable("mArenaScore"));
		if(arenaScore)
			arenaScore->setVisible(false);
		CCNode* label = dynamic_cast<CCNode* >(getVariable("mLable"));
		if(label)
			label->setVisible(false);
		return true;
	}


}

void ArenaPage::_calcNextSyncTime(unsigned int lastSyncTime)
{
	const UserBasicInfo info=ServerDateManager::Get()->getUserBasicInfo();
	int lastSyncOver=info.servertime-lastSyncTime;
	while(lastSyncOver>=ArenaSyncTime)
	{
		lastSyncOver-=ArenaSyncTime;
	}
	mNextSyncTime=ArenaSyncTime-lastSyncOver;
}

void ArenaPage::_refreshSyncTime()
{
	if(mNextSyncTime>0)
	{
		--mNextSyncTime;
	}
	CCB_FUNC(this,"mTimeLabel",CCLabelBMFont,setString(GameMaths::formatSecondsToTime(mNextSyncTime).c_str()));
	if(mNextSyncTime==0)
	{
		mNextSyncTime=ArenaSyncTime;
		OPRefreshArenaScore refreshArenaScore;
		refreshArenaScore.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_REFRESH_ARENA_C, &refreshArenaScore);
	}

}
void ArenaPage::selectTab( unsigned int index )
{
	if(mCurrTabIndex != index)
	{
		mCurrTabIndex = index;
		if(!judgeLoseDate()){
			rebuildAllItem();
			refreshPage();
		}
	}

	if(mCurrTabIndex == 2)
	{
		dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTab1"))->unselected();
		dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTab2"))->selected();
	}
	else
	{
		dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTab1"))->selected();
		dynamic_cast<cocos2d::CCMenuItemImage*>(getVariable("mTab2"))->unselected();
	}
}

void ArenaPage::onReceiveMassage( const GameMessage * message )
{
	if(message->getTypeId() == MSG_MAINFRAME_REFRESH)
	{
		const MsgMainFrameRefreshPage * msg = dynamic_cast<const MsgMainFrameRefreshPage*>(message);
		if(msg && msg->pageName == "ArenaPage")
		{
			if(mCurrTabIndex == 0)
			{
				selectTab(1);
			}
			else
			{
				unsigned int select = mCurrTabIndex;
				mCurrTabIndex = 0;
				selectTab(select);
			}
		}
	}
	else if(message->getTypeId()==MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate * info = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(info->opcode==OPCODE_GET_USER_BASIC_INFORET_S||info->opcode==OPCODE_ADD_VITALITYRET_S)
		{
			_refreshTitle();
		}
		else if(info->opcode==OPCODE_CHALLENGE_OPPONENTRET_S
			||info->opcode==OPCODE_ADD_ARENA_NUMRET_S
			||info->opcode==OPCODE_REFRESH_ARENARET_S
			||info->opcode==OPCODE_USE_TOOLRET_S
			||info->opcode==OPCODE_USER_ARENA_INFORET_S)
		{
			refreshPage();
		}
	}
	else if(message->getTypeId()==MSG_FIGHT_END)
	{
		const MsgFightEnd * info=dynamic_cast<const MsgFightEnd* >(message);
		if(info)
		{
			_refreshTitle();
		}
		if(BlackBoard::Get()->UpdateTitleType>0)
		{
			if(BlackBoard::Get()->PreTitleId==0)
			{
				MsgMainFramePushPage msg;
				msg.pageName = "TitleGetNewPage";
				MessageManager::Get()->sendMessage(&msg);
			}
			else
			{
				MsgMainFramePushPage msg;
				msg.pageName = "TitleLVUpPage";
				MessageManager::Get()->sendMessage(&msg);
			}
		}
	}
}
