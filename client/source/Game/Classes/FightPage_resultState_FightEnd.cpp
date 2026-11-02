
#include "stdafx.h"

#include "FightPage_resultState_FightEnd.h"
#include "UserBattle.pb.h"
#include "DataTableManager.h"
#include "GameMessages.h"
#include "MessageBoxPage.h"
#include "BlackBoard.h"
#include "StringConverter.h"
#include "GameMaths.h"
#include "SoundManager.h"

FightPage_resultState_FightEnd::FightPage_resultState_FightEnd(void)
	:mPage(0)
	,mResult(0)
{
}


FightPage_resultState_FightEnd::~FightPage_resultState_FightEnd(void)
{
}

void FightPage_resultState_FightEnd::Enter( FightPage* page)
{
	page->mCurrRound=4;
	CCB_FUNC(page,"mControlCover",CCMenuItemImage,setVisible(false));
	CCB_FUNC(page,"mshi1",CCNode,setVisible(false));
	CCB_FUNC(page,"mshi2",CCNode,setVisible(false));
	mPage = page;
	mPage->retain();
	CCB_FUNC(page,"mSkipButtonNode",CCNode,setVisible(false));
	if(mResult == 0)
	{
		mResult = CCBContainer::create();
		mResult->loadCcbiFile("FightEndBattleResult.ccbi");
		mResult->setListener(this);
		mResult->retain();
	}
	for(int i=0;i<8;++i)
	{
		mCharacters[i] = CCBContainer::create();
		mCharacters[i]->loadCcbiFile("BattleResultItem.ccbi");
		mCharacters[i]->retain();
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

void FightPage_resultState_FightEnd::Execute( FightPage* )
{

}

void FightPage_resultState_FightEnd::Exit( FightPage* )
{
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

void FightPage_resultState_FightEnd::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
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
		mPage->fight();
	}
}

void FightPage_resultState_FightEnd::win(const OPUserBattleRet* msg)
{

	SoundManager::Get()->stopMusic();
	SoundManager::Get()->playEffect(VaribleManager::Get()->getSetting("winSound"));

	const OPUserBattleRet_Result& result = msg->result();
	int round=0;
	if(msg->round3().gotto())
	{	
		round=3;
		CCB_FUNC(mResult,"mRound",CCLabelBMFont,setString("3"));
	}
	else if(msg->round2().gotto())
	{	
		round=2;
		CCB_FUNC(mResult,"mRound",CCLabelBMFont,setString("2"));
	}
	else
	{	
		round=1;
		CCB_FUNC(mResult,"mRound",CCLabelBMFont,setString("1"));
	}

	for(int i=1;i<=3;++i)
	{
		char name[128];
		sprintf(name,"mStar%d",i);
		CCB_FUNC(mResult,name,CCSprite,setVisible(i<=(4-round)));
	}
	CCB_FUNC(mResult,"mFightEndMsg",CCLabelBMFont,setString(Language::Get()->getString("@FightEndBattleWinMsg").c_str()));
	const AdventureFightEndInfo* mFightInfo=ServerDateManager::Get()->getAdventureFightEndInfo();
	unsigned int rate=0;
	std::string key="fightEndStarRateKey";
	if(BlackBoard::Get()->hasVarible(key))
	{
		rate=BlackBoard::Get()->getVaribleAsUint(key);
	}
	CCB_FUNC(mResult,"mFinalStar",CCLabelBMFont,setString((StringConverter::toString((4-round))+"X"+StringConverter::toString(rate)).c_str()));
	for(int i=0;i<8;++i)
	{
		mCharacters[i]->setVisible(false);
	}
	int barrier=0;
	if(((mFightInfo->mTodayFightInfo.currBarrier-1)%3)!=0)
	{
		barrier=3-((mFightInfo->mTodayFightInfo.currBarrier-1)%3);
	}
	else
	{
		barrier=0;
	}
	std::list<std::string> _list;
	_list.push_back(StringConverter::toString(barrier));
	std::string replaced = GameMaths::replaceStringWithStringList(Language::Get()->getString("@MoreXPassGetBuff"),&_list);
	CCB_FUNC(mResult,"mXPassGetBuff",CCLabelBMFont,setString(replaced.c_str()));
	if(((mFightInfo->mTodayFightInfo.currBarrier-1)%5)!=0)
	{
		barrier=5-((mFightInfo->mTodayFightInfo.currBarrier-1)%5);
	}
	else
	{
		barrier=0;
	}
	_list.push_back(StringConverter::toString(barrier));
	replaced = GameMaths::replaceStringWithStringList(Language::Get()->getString("@MoreXPassGetReward"),&_list);
	CCB_FUNC(mResult,"mXPassGetReward",CCLabelBMFont,setString(replaced.c_str()));
	CCB_FUNC(mResult,"mWinMsg",CCNode,setVisible(true));
}

void FightPage_resultState_FightEnd::lose(const OPUserBattleRet* msg)
{

	SoundManager::Get()->stopMusic();
	SoundManager::Get()->playEffect(VaribleManager::Get()->getSetting("loseSound"));

	const OPUserBattleRet_Result& result = msg->result();

	CCB_FUNC(mResult,"mWinMsg",CCNode,setVisible(false));

	for(int i=0;i<8;++i)
	{
		mCharacters[i]->setVisible(false);
	}
	
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
		//CCTexture2D * tex = CCTextureCache::sharedTextureCache()->addImage(disc->iconPic.c_str());
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

void FightPage_resultState_FightEnd::common(const OPUserBattleRet* msg,bool isWin)
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
	userBasicInfo.exp+=result.gotexp();
	userBasicInfo.silvercoins+=result.gotcoin();
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
