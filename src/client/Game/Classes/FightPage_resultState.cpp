
#include "stdafx.h"

#include "FightPage_resultState.h"
#include "UserBattle.pb.h"
#include "DataTableManager.h"
#include "GameMessages.h"
#include "NumberChangeAction.h"
#include "Disciple.h"
#include "FightConfigManager.h"
#include "SoundManager.h"
#include "BlackBoard.h"
FightPage_resultState::FightPage_resultState(void)
	:mPage(0)
	,mResult(0)
	,mShowExpBarAni(false)
{
}


FightPage_resultState::~FightPage_resultState(void)
{
	mShowExpBarAni=false;
}

void FightPage_resultState::Enter( FightPage* page)
{
	CCB_FUNC(page,"mControlCover",CCMenuItemImage,setVisible(false));
	page->mCurrRound=4;
	CCB_FUNC(page,"mshi1",CCNode,setVisible(false));
	CCB_FUNC(page,"mshi2",CCNode,setVisible(false));
	mShowExpBarAni=false;
	durationTime=0.0f;
	mLevelList.clear();
	mPage = page;
	mPage->retain();
	CCB_FUNC(page,"mSkipButtonNode",CCNode,setVisible(false));

	MsgInstructionNext insMsg;
	MessageManager::Get()->sendMessage(&insMsg);

	if(mResult == 0)
	{
		mResult = CCBContainer::create();
		mResult->loadCcbiFile("b_jiesuan.ccbi");
		mResult->setListener(this);
		mResult->retain();
	}
	CCB_FUNC(mResult,"mViewtheteamW",CCNode,setVisible(mPage->showViewTeam()));
	CCB_FUNC(mResult,"mViewtheteamL",CCNode,setVisible(mPage->showViewTeam()));
	for(int i=0;i<8;++i)
	{
		mCharacters[i] = CCBContainer::create();
		mCharacters[i]->loadCcbiFile("b_jiesuanrenwu.ccbi");
		mCharacters[i]->retain();
		CCB_FUNC(mCharacters[i],"mExpBar1",CCSprite,setAnchorPoint(ccp(0,0)));
		CCB_FUNC(mCharacters[i],"mExpBar2",CCSprite,setAnchorPoint(ccp(0,0)));
		char var[64];
		sprintf(var,"mCharacter%d",i+1);
		CCB_FUNC(mResult,var,CCNode,addChild(mCharacters[i]));
	}
	CCB_FUNC(page,"mPeople",CCNode,removeAllChildren());
	CCB_FUNC(page,"mBigskill",CCNode,removeAllChildren());
	CCB_FUNC( page,"mBigskill",CCNode,addChild(mResult) );

	const OPUserBattleRet* msg = page->getMessage();

	const OPUserBattleRet_Result& result = msg->result();
	switch (result.winlose())
	{
	case OPUserBattleRet_Result_WINLOSE_BIGWIN:
	case OPUserBattleRet_Result_WINLOSE_WIN:
	case OPUserBattleRet_Result_WINLOSE_JUSTWIN:
		CCB_FUNC(mResult,"mWinbanner",CCNode,setVisible(true));
		CCB_FUNC(mResult,"mLosebanner",CCNode,setVisible(false));
		common(msg,true);
		win(msg);
		break;
	case OPUserBattleRet_Result_WINLOSE_BIGLOSE:
	case OPUserBattleRet_Result_WINLOSE_LOSE:
	case OPUserBattleRet_Result_WINLOSE_JUSTLOSE:
		CCB_FUNC(mResult,"mWinbanner",CCNode,setVisible(false));
		CCB_FUNC(mResult,"mLosebanner",CCNode,setVisible(true));
		common(msg,false);
		lose(msg);
		break;
	}
	

}

void FightPage_resultState::Execute( FightPage* )
{
	durationTime+=GamePrecedure::Get()->getFrameTime();
	if(durationTime>FightConfigManager::Get()->getResultHeadDeplay())
	{
		_showExpBarAni();
		if(durationTime>(FightConfigManager::Get()->getResultLVUpBefore()+FightConfigManager::Get()->getResultHeadDeplay()))
		{
			durationTime-=FightConfigManager::Get()->getResultLVUpBefore()-FightConfigManager::Get()->getResultHeadDeplay();
			_showLevelupAni();
		}
	}
	
}

void FightPage_resultState::Exit( FightPage* )
{
	mLevelList.clear();
	if(mResult)
	{
		mResult->setListener(0);
		mResult->release();
		mResult = 0;
	}

	CCObject* obj = mPage->getVariable("mBigskill");
	CCNode* node = obj?dynamic_cast<CCNode*>(obj):0;
	if(node)
	{
		node->removeAllChildren();
	}

	for(int i=0;i<8;++i)
	{
		mCharacters[i]->release();
		mCharacters[i] = 0;
	}
	mPage->release();
	mPage = 0;
}

void FightPage_resultState::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	
	if(itemName == "onClose")
	{
		mPage->onMenuItemAction(itemName,sender);
	}
	if(itemName == "onEnhanceEquip")
	{
		BlackBoard::Get()->showFightGuide = false;
		mPage->onMenuItemAction("onClose",sender);

		MsgMainFrameChangePage msg;
		msg.pageName = "EquipPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	if(itemName == "onReplay")
	{
		mPage->mIsReplay=true;
		mPage->fight();
	}
	if(itemName=="onViewtheteam")
	{
		mPage->onMenuItemAction("onClose",sender);
		if(mPage->showViewTeam())
		{
			std::string _viewTeamPageName="ViewTeamPageKey";
			std::string _value="ViewListTeamPage";
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
			msg->set_isnpc(mPage->getViewTeamStats().npcStatus);
			msg->set_relationid(mPage->getViewTeamStats().id);
			msg->set_relationtype(mPage->getViewTeamStats().viewType);
			PacketManager::Get()->sendPakcet(OPCODE_SHOW_USER_BATTLE_C,msg);
		}	
	}
}

void FightPage_resultState::_showLevelupAni()
{
	if(mLevelList.size()>0)
	{
		const OPUserBattleRet* msg = mPage->getMessage();
		const OPUserBattleRet_Result& result = msg->result();
		LevelUpList::iterator it=mLevelList.begin();
		for(;it!=mLevelList.end();++it)
		{
			int i=*it;
			const OPUserBattleRet_Result_CharacterResult cha = result.characterresults().Get(i);
			CCB_FUNC(mCharacters[i],"mExpBar1",CCSprite,setScaleX(0));
			CCB_FUNC(mCharacters[i],"mExpBar2",CCSprite,setScaleX(0));
			mCharacters[i]->runAnimation("lvup");
			UserDiscipleInfo* info = ServerDateManager::Get()->getUserDiscipleInfoByItemID(cha.resourceid());
			float expScale=_getScaleByDiscipleId(info->id,false);
			CCB_FUNC(mCharacters[i],"mExpBar1",CCSprite,runAction(CCScaleTo::create(FightConfigManager::Get()->getResultLVUpAfter()-FightConfigManager::Get()->getResultExpFast(), expScale,1.0f)));
			CCB_FUNC(mCharacters[i],"mExpBar2",CCSprite,runAction(CCScaleTo::create(FightConfigManager::Get()->getResultLVUpAfter(), expScale,1.0f)));
			CCB_FUNC(mCharacters[i],"mExp1",CCLabelBMFont,runAction(NumberChangeAction::create(FightConfigManager::Get()->getResultLVUpAfter(),cha.gainexp())));
		}
		mLevelList.clear();
	}
}

void FightPage_resultState::_showExpBarAni()
{
	if(!mShowExpBarAni)
	{
		return ;
	}
	const OPUserBattleRet* msg = mPage->getMessage();
	const OPUserBattleRet_Result& result = msg->result();
	int i = 0;
	for(;i<result.characterresults_size();++i)
	{
		const OPUserBattleRet_Result_CharacterResult cha = result.characterresults().Get(i);

		mCharacters[i]->setVisible(true);

		CCB_FUNC(mCharacters[i],"mExp1",CCLabelBMFont,runAction(NumberChangeAction::create(FightConfigManager::Get()->getResultAllTime(),cha.gainexp())));
		UserDiscipleInfo* info = ServerDateManager::Get()->getUserDiscipleInfoByItemID(cha.resourceid());
		if(cha.isupgarade())
		{	
			CCB_FUNC(mCharacters[i],"mExpBar1",CCSprite,runAction(CCScaleTo::create(FightConfigManager::Get()->getResultLVUpBefore()-FightConfigManager::Get()->getResultExpFast(),1.0f,1.0f)));
			CCB_FUNC(mCharacters[i],"mExpBar2",CCSprite,runAction(CCScaleTo::create(FightConfigManager::Get()->getResultLVUpBefore(),1.0f,1.0f)));
			CCB_FUNC(mCharacters[i],"mMemLV1",CCLabelBMFont,runAction(NumberChangeAction::create(FightConfigManager::Get()->getResultLVUpBefore()+0.1f,cha.level())));
		}
		else
		{
			float expScale=_getScaleByDiscipleId(info->id,false);
			CCB_FUNC(mCharacters[i],"mExpBar1",CCSprite,runAction(CCScaleTo::create(FightConfigManager::Get()->getResultAllTime()-FightConfigManager::Get()->getResultExpFast(), expScale,1.0f)));
			CCB_FUNC(mCharacters[i],"mExpBar2",CCSprite,runAction(CCScaleTo::create(FightConfigManager::Get()->getResultAllTime(), expScale,1.0f)));
		}
	}
	mShowExpBarAni=false;
}

float FightPage_resultState::_getScaleByDiscipleId(int id,bool original,bool isUp,int gainExp)
{
	try
	{
		Disciple disciple(id);
		int expTotal =original?(isUp?disciple.getExpForLevel(disciple.level()):disciple.getLevelupNeededExp()):disciple.getLevelupNeededExp();
		int expNum = 0;
		if(disciple.hasDiscipleInfo())
		{
			expNum = disciple.getLevelExp();
			expNum = original?(expNum-gainExp):expNum;
		}
		expNum =expNum<0? 0:expNum;
		expTotal+=expNum;
		float expScale = 0;
		expScale = expTotal>0?(((float)expNum)/((float)expTotal)):expTotal;
		expScale =expScale>1.0f?1.0f:expScale;
		return expScale;
	}
	catch (...)
	{
		
	}
	return 0.0f;
}

void FightPage_resultState::win(const OPUserBattleRet* msg)
{
	mShowExpBarAni=true;
	const OPUserBattleRet_Result& result = msg->result();
	SoundManager::Get()->stopMusic();
	SoundManager::Get()->playEffect(VaribleManager::Get()->getSetting("winSound"));

	CCB_FUNC(mResult,"mBattleResultNote",CCLabelBMFont,setString(Language::Get()->getString("@BattleResultDiscpleReward").c_str()));
	int i = 0;
	for(;i<result.characterresults_size();++i)
	{
		const OPUserBattleRet_Result_CharacterResult cha = result.characterresults().Get(i);
		
		mCharacters[i]->setVisible(true);
		CCB_FUNC(mCharacters[i],"mCommonForWin",CCNode,setVisible(true));
		CCB_FUNC(mCharacters[i],"mCommonForLose",CCNode,setVisible(false));

		const DiscipleItem* disc = DiscipleTableManager::Get()->getDiscipleItemByID(cha.resourceid());
		CCSprite * texframe = ::getFrameNormalSpirte(disc->quality);

		CCB_FUNC(mCharacters[i],"mHead1",CCSprite,setTexture(disc->iconPic.c_str()));
		CCB_FUNC(mCharacters[i],"mFrame1",CCSprite,setTexture(texframe->getTexture()));
		CCB_FUNC(mCharacters[i],"mFrame1",CCSprite,setColor(::getFrameColor(disc->quality)));
		CCB_FUNC(mCharacters[i],"mName1",CCLabelBMFont,setString(disc->name.c_str()));

		CCB_FUNC(mCharacters[i],"mExp1",CCLabelBMFont,setString(StringConverter::toString(1).c_str()));
		UserDiscipleInfo* info = ServerDateManager::Get()->getUserDiscipleInfoByItemID(cha.resourceid());
		float expScale=0.0f;
		if(info)
		{
			expScale=_getScaleByDiscipleId(info->id,true,cha.isupgarade(),cha.gainexp());
		}
		CCB_FUNC(mCharacters[i],"mExpBar1",CCSprite,setScaleX(expScale));
		CCB_FUNC(mCharacters[i],"mExpBar2",CCSprite,setScaleX(expScale));
		
		if(cha.isupgarade())
		{	
			mLevelList.push_back(i);
			CCB_FUNC(mCharacters[i],"mMemLV1",CCLabelBMFont,setString(StringConverter::toString(info->preLevel).c_str()));
		}
		else
		{
			CCB_FUNC(mCharacters[i],"mMemLV1",CCLabelBMFont,setString(StringConverter::toString(cha.level()).c_str()));
		}
	}
}

void FightPage_resultState::lose(const OPUserBattleRet* msg)
{
	const OPUserBattleRet_Result& result = msg->result();

	CCB_FUNC(mResult,"mBattleResultNote",CCLabelBMFont,setString(Language::Get()->getString("@BattleResultDiscpleHoldRound").c_str()));

	SoundManager::Get()->stopMusic();
	SoundManager::Get()->playEffect(VaribleManager::Get()->getSetting("loseSound"));

	int i = 0;
	for(int ct=0;ct<msg->round1().characters_size();++ct)
	{
		if(msg->round1().characters(ct).position()%2!=0)
			continue;

		const OPUserBattleRet_Character cha = msg->round1().characters(ct);

		mCharacters[i]->setVisible(true);
		CCB_FUNC(mCharacters[i],"mCommonForWin",CCNode,setVisible(false));
		CCB_FUNC(mCharacters[i],"mCommonForLose",CCNode,setVisible(true));

		const DiscipleItem* disc = DiscipleTableManager::Get()->getDiscipleItemByID(cha.resourceid());
		CCSprite * sprite = ::getFrameNormalSpirte(disc->quality);

		CCB_FUNC(mCharacters[i],"mHead1",CCSprite,setTexture(disc->iconPic.c_str()));
		CCB_FUNC(mCharacters[i],"mFrame1",CCSprite,setTexture(sprite->getTexture()));
		CCB_FUNC(mCharacters[i],"mFrame1",CCSprite,setColor(::getFrameColor(disc->quality)));
		CCB_FUNC(mCharacters[i],"mName1",CCLabelBMFont,setString(disc->name.c_str()));

		bool round3 = false;
		bool round2 = false;
		for(int j = 0;j<msg->round3().characters_size();j+=2)
		{
			if(msg->round3().characters().Get(j).resourceid() == cha.resourceid() && msg->round3().characters().Get(j).position()%2==0)
			{
				round3 = true;
				break;
			}
		}
		if(!round3)
		{
			for(int j = 0;j<msg->round2().characters_size();j+=2)
			{
				if(msg->round2().characters().Get(j).resourceid() == cha.resourceid() && msg->round2().characters().Get(j).position()%2==0)
				{
					round2 = true;
					break;
				}
			}
		}
		if(round3)
		{
			CCB_FUNC(mCharacters[i],"mLoseRound1",CCLabelBMFont,setString(Language::Get()->getString("@BattleResultDiscpleHoldRoundThree").c_str()));
		}
		else if(round2)
		{
			CCB_FUNC(mCharacters[i],"mLoseRound1",CCLabelBMFont,setString(Language::Get()->getString("@BattleResultDiscpleHoldRoundTwo").c_str()));
		}
		else
		{
			CCB_FUNC(mCharacters[i],"mLoseRound1",CCLabelBMFont,setString(Language::Get()->getString("@BattleResultDiscpleHoldRoundOne").c_str()));
		}
		++i;
	}
}

void FightPage_resultState::common(const OPUserBattleRet* msg,bool isWin)
{
	UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	const OPUserBattleRet_Result& result = msg->result();
	CCB_FUNC(mResult,"mViewTeamNode",CCNode,setVisible(false));

	for(int i=0;i<8;++i)
	{
		mCharacters[i]->setVisible(false);
	}

	char tempStr[64];
	sprintf(tempStr,"%d",result.gotexp());
	CCB_FUNC(mResult,"mPlayerEXP",CCLabelBMFont,setString(tempStr));
	sprintf(tempStr,"%d",result.gotcoin());
	CCB_FUNC(mResult,"mCoin",CCLabelBMFont,setString(tempStr));
	if(!mPage->mIsReplay)
	{
		userBasicInfo.exp+=result.gotexp();
		userBasicInfo.silvercoins+=result.gotcoin();
	}
	int _rountEvaluate=1;
	if(msg->round3().gotto())
	{	
		_rountEvaluate=1;
		CCB_FUNC(mResult,"mTotalRound",CCLabelBMFont,setString("3"));
	}
	else if(msg->round2().gotto())
	{	
		_rountEvaluate=2;
		CCB_FUNC(mResult,"mTotalRound",CCLabelBMFont,setString("2"));
	}
	else
	{	
		_rountEvaluate=3;
		CCB_FUNC(mResult,"mTotalRound",CCLabelBMFont,setString("1"));
	}
	for(int i=1;i<=3;++i)
	{
		char _starKey[128];
		sprintf(_starKey,"mStar%d",i);
		CCB_FUNC(mResult,_starKey,CCNode,setVisible(isWin?i<=_rountEvaluate:false));
	}
}
