
#include "stdafx.h"

#include "FightPage.h"
#include "FightPage_roundState.h"
#include "GameMessages.h"
#include "DataTableManager.h"
#include "FightPage_resultState.h"
#include "FightPage_resultState_FightEnd.h"
#include "FightPage_finalRound.h"
#include "GetPropManager.h"
#include "SoundManager.h"
#include "InstructionManager.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "libOS.h"

REGISTER_PAGE("FightPage",FightPage);

USING_NS_CC;

FightPage::FightPage(void)
	: mStateMachine(0)
	, mFighting(false)
	, mBattleMsg(0)
	,mShowSkip(false)
	,mIsReplay(false)
{
	
	
}


FightPage::~FightPage(void)
{
	
	delete mStateMachine;

}

void FightPage::Enter( MainFrame* )
{
	mShowSkip=false;
	CCB_FUNC(this,"mControlCover",CCMenuItemImage,setVisible(true));
    //SoundManager::Get()->playFightMusic();
	mStateMachine = new StateMachine<FightPage>(this);
	CCBContainer* _round= CCBContainer::create();
	_round->loadCcbiFile("s_curtain.ccbi");
	CCB_FUNC(this,"mRound",CCNode,addChild(_round));
	CCB_FUNC(this,"mRound",CCNode,setVisible(false));
	mCurrentStateDone = false;
	mSkip = false;
	mFighting = false;
	mLifes.clear();
	PacketManager::Get()->regisiterPacketHandler(OPCODE_USER_BATTLERET_S,this);
	const UserBasicInfo info =ServerDateManager::Get()->getUserBasicInfo();
	if(info.tutorialstep==1 && (InstructionManager::Get()->loadLocalInstructionStep()>=InstructionManager::TOTAL_INSTRUCTION||InstructionManager::Get()->loadLocalInstructionStep()==1))
	{
	}
	else
	{
		CCB_FUNC(this,"mSkipButtonNode",CCNode,setVisible(false));
	}
	if(mBattleMsg)
	{
		for(int it=0;it<mBattleMsg->round1().characters_size();++it)
		{
			CCBContainer* _battle=CCBManager::Get()->createAndLoad("Battle.ccbi");//CCBContainer::create();
			//_battle->loadCcbiFile("Battle.ccbi");
			_battle->setAnchorPoint(ccp(0,0));
			_battle->retain();
			CCBMAP::iterator batt_it = mBattleList.find(it);
			if(batt_it!=mBattleList.end())
			{
				batt_it->second->removeFromParent();
				batt_it->second->release();
				mBattleList.erase(batt_it);
			}

			mBattleList.insert(CCBMAP::value_type(it,_battle));
		}

		fight();
	}
	else
	{
		//temp code
		OPUserBattleRet* msg = receiveMessage();
		onReceivePacket(OPCODE_USER_BATTLERET_S,msg);
	}
}

void FightPage::Execute( MainFrame* mainframe)
{
	if(mSkip)
	{
		SoundManager::Get()->stopAllEffect();

		State<FightPage>* state = mStateMachine->CurrentState();
		FightPage_resultState* rs = state?dynamic_cast<FightPage_resultState*>(state):0;
		FightPage_resultState_FightEnd * rs2 = state?dynamic_cast<FightPage_resultState_FightEnd*>(state):0;
		while(rs == 0 && rs2 == 0 && !mStates.empty())
		{
			mStateMachine->ChangeState(mStates.front());
			mStates.pop_front();

			state = mStateMachine->CurrentState();
			rs = state?dynamic_cast<FightPage_resultState*>(state):0;
			rs2 = state?dynamic_cast<FightPage_resultState_FightEnd*>(state):0;
		}
		
		if(mFightType == FightTypeAnyThing)
			mCurrentStateDone = true;
		else
			mCurrentStateDone = false;
		mSkip = false;
	}
	if(mCurrentStateDone)
	{
		if(!mStates.empty())
		{
			State<FightPage>* state = mStateMachine->CurrentState();
			mStateMachine->ChangeState(mStates.front());
			if(state!=0)
				delete state;
			mCurrentStateDone = false;
			mStates.pop_front();
		}
		else
		{
			MsgMainFrameCoverHide msg;
			msg.pageName = "FightPage";
			MessageManager::Get()->sendMessage(&msg);

			SoundManager::Get()->playGeneralMusic();
		}
	}
	mStateMachine->Update();
}

void FightPage::Exit( MainFrame* )
{
    mIsReplay=false;
	mFighting = false;
	PacketManager::Get()->removePacketHandler(this);
	if(mStateMachine)
	{
		State<FightPage>* lastState = mStateMachine->CurrentState();
		delete mStateMachine;
		mStateMachine = 0;

		if(lastState)
		{
			delete lastState;
		}
	}

	for(STATEMAP::iterator it = mStates.begin();it!=mStates.end();++it)
	{
		State<FightPage>* state = *it;
		delete state;
	}
	mStates.clear();

	if(mBattleMsg)
	{
		mBattleMsg->Clear();
		delete mBattleMsg;
		mBattleMsg = 0;
	}
	CCBMAP::iterator it=mBattleList.begin();
	for(;it!=mBattleList.end();++it)
	{
		if(it->second)
		{
			CCB_FUNC(it->second,"mOwn1",CCNode,removeAllChildren());			
			CCB_FUNC(it->second,"mPoster",CCNode,removeAllChildren()); 

			it->second->release();
		}
	}
	mBattleList.clear();
	//important! remove the s_certain.ccbi
	removeAllChildren();
	_removeMaxLifeList();

	MsgFightPageExit msg;
	MessageManager::Get()->sendMessage(&msg);
}

void FightPage::load( void )
{
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
    long mem = libOS::getInstance()->avalibleMemory();
    if(mem<15)//xinzheng
    {
        CCApplication::sharedApplication()->purgeCachedData();
		CCDirector::sharedDirector()->purgeCachedData();
    }
#endif
	loadCcbiFile("battlePage.ccbi");
	CCB_FUNC(this,"mSkipButton",CCMenuItemImage,setEnabled(false));
	CCB_FUNC(this,"mSkipButtonNode",CCNode,setVisible(false));
}

void FightPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{

}

void FightPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName == "onClose")
	{
		MsgFightEnd fightEndMsg;
		if(mBattleMsg->result().has_gotitem())
		{
			fightEndMsg.gotItem = true;

			GetPropManager::TYPE type;
			int count = mBattleMsg->result().gotitem().count();
			int id;
			switch(mBattleMsg->result().gotitem().type())
			{
			case OPUserBattleRet_Result_GotItemResult_ITEMTYPE_TOOL:
				if(mBattleMsg->result().gotitem().has_tool())
				{
					GetPropManager::Get()->gotTool(mBattleMsg->result().gotitem().tool());
					type = GetPropManager::TOOL;
					id = mBattleMsg->result().gotitem().tool().id();
					GetPropManager::Get()->popUpPage(type,id,count);
				}
				break;
			case OPUserBattleRet_Result_GotItemResult_ITEMTYPE_TOOL_USE:
				if(mBattleMsg->result().gotitem().has_tool())
				{
					GetPropManager::Get()->gotTool(mBattleMsg->result().gotitem().tool());
					type = GetPropManager::TOOL_USE;
					id = mBattleMsg->result().gotitem().tool().id();
					GetPropManager::Get()->popUpPage(type,id,count);
				}
				break;
			case OPUserBattleRet_Result_GotItemResult_ITEMTYPE_EQUIPMENT:
				if(mBattleMsg->result().gotitem().has_equip())
				{
					GetPropManager::Get()->gotEquipment(mBattleMsg->result().gotitem().equip());
					type = GetPropManager::EQUIPMENT;
					id = mBattleMsg->result().gotitem().equip().id();
					GetPropManager::Get()->popUpPage(type,id,count);
				}
				break;
			case OPUserBattleRet_Result_GotItemResult_ITEMTYPE_SKILL:
				if(mBattleMsg->result().gotitem().has_skill())
				{
					GetPropManager::Get()->gotSkill(mBattleMsg->result().gotitem().skill());
					type = GetPropManager::SKILL;
					id = mBattleMsg->result().gotitem().skill().id();
					GetPropManager::Get()->popUpPage(type,id,count);
				}
				break;
			case OPUserBattleRet_Result_GotItemResult_ITEMTYPE_SOUL:
				if(mBattleMsg->result().gotitem().has_soul())
				{
					GetPropManager::Get()->gotSoul(mBattleMsg->result().gotitem().soul());
					type = GetPropManager::SOUL;
					id = mBattleMsg->result().gotitem().soul().id();
					GetPropManager::Get()->popUpPage(type,id,count);
				}
				break;
			case OPUserBattleRet_Result_GotItemResult_ITEMTYPE_SKILLBOOK:
				if(mBattleMsg->result().gotitem().has_skillbook())
				{
					GetPropManager::Get()->gotSkillBook(mBattleMsg->result().gotitem().skillbook());
					int partId=mBattleMsg->result().gotitem().skillbook().partid();
					if(partId!=0)
					{
						type = GetPropManager::SKILLBOOK;
						int skillId=mBattleMsg->result().gotitem().skillbook().skillid();
						int count=mBattleMsg->result().gotitem().skillbook().count();
						GetPropManager::Get()->popUpPage(type,partId,count,skillId);
					}
				}
			}
		}
		else
		{
			fightEndMsg.gotItem = false;
		}
		MessageManager::Get()->sendMessage(&fightEndMsg);
		
		nextRound();

	}
	else if(itemName == "onSkipBattle")
	{
		if(mFightType==WorldBoss)
		{
			mCurrentStateDone=true;
			nextRound();
		}
		else
		{
			mSkip = true;
			CCB_FUNC(this,"mRound",CCNode,setVisible(false));
		}
	}
	else if(itemName=="onControlSkip")
	{
		if(getHideSkip()==false&&ServerDateManager::Get()->getUserBasicInfo().tutorialstep==1 && (InstructionManager::Get()->loadLocalInstructionStep()>=InstructionManager::TOTAL_INSTRUCTION||ServerDateManager::Get()->getUserBasicInfo().level>2||InstructionManager::Get()->loadLocalInstructionStep()==1))
		{
			if(mCurrentStateDone)
			{
				mShowSkip=false;
			}
			else
			{
				mShowSkip=!mShowSkip;
			}
		}
		else
		{
			mShowSkip=false;
		}
		if(mCurrRound>2)
		{
			mShowSkip=false;
		}
		CCB_FUNC(this,"mSkipButtonNode",CCNode,setVisible(mShowSkip));
	}
}

OPUserBattleRet * FightPage::receiveMessage()
{
	OPUserBattleRet * mBattleMessage = new OPUserBattleRet;

	OPUserBattleRet_Round * round1 = mBattleMessage->mutable_round1();
	round1->set_gotto(true);
	for(int i=0;i<8;++i)
	{
		OPUserBattleRet_Character* cha1 =  round1->add_characters();
		
		cha1->set_position(i);
		cha1->set_resourceid(rand()%16+1);
		cha1->set_life(100);
		cha1->set_power(200);
	}
	for(int i=0;i<2;++i)
	{
		OPUserBattleRet_Action* action = round1->add_actions();
		action->set_positionid(rand()%8);
		action->set_type(OPUserBattleRet_Action_Type_SKILL);
		action->set_skillid(rand()%2+18);
	}

	for(int i=0;i<1;++i)
	{
		OPUserBattleRet_Action* action = round1->add_actions();
		int pos = 1;
		action->set_positionid(pos);

		OPUserBattleRet_Action_Type type = OPUserBattleRet_Action_Type_SKILL;
		action->set_type(type);
		int skillID = 1;
		action->set_skillid(skillID);
		const SkillItem* skill = SkillTableManager::Get()->getSkillItemByID(skillID);
		if( type == OPUserBattleRet_Action_Type_SKILL)
		{
			if(skill->type == SkillItem::TYPE_MULTIPLE)
			{
				for(int j=0;j<8;++j)
				{
					if(pos%2 != j%2)
					{
						OPUserBattleRet_Damage* dmg = action->add_damage();
						dmg->set_position(j);
						dmg->set_harmvalue(50);
					}
				}
			}
			else
			{
				OPUserBattleRet_Damage* dmg = action->add_damage();
				dmg->set_position(pos%2==0?pos+1:pos-1);
				dmg->set_harmvalue(30);
			}
		}

	}
	for(int i=0;i<3;++i)
	{
		OPUserBattleRet_Action* action = round1->add_actions();
		int pos = rand()%8;
		action->set_positionid(pos);
		OPUserBattleRet_Damage* dmg = action->add_damage();
		dmg->set_harmvalue(30);
		dmg->set_position(pos%2==0?pos+1:pos-1);
		OPUserBattleRet_Action_Type type = (rand()%5==0?OPUserBattleRet_Action_Type_SKILL:OPUserBattleRet_Action_Type_ATTACK);
		action->set_type(type);
		int skillID = rand()%70+1;
		action->set_skillid(skillID);
		const SkillItem* skill = SkillTableManager::Get()->getSkillItemByID(skillID);
		if( type == OPUserBattleRet_Action_Type_SKILL)
		{
			if(skill->type == SkillItem::TYPE_MULTIPLE)
			{
				for(int j=0;j<8;++j)
				{
					if(pos%2 != j%2)
					{
						OPUserBattleRet_Damage* dmg = action->add_damage();
						dmg->set_position(j);
						dmg->set_harmvalue(30);
					}
				}
			}
			else
			{
				OPUserBattleRet_Damage* dmg = action->add_damage();
				dmg->set_position(pos%2==0?pos+1:pos-1);
				dmg->set_harmvalue(30);
			}
		}

	}
	OPUserBattleRet_Round * round2 = mBattleMessage->mutable_round2();
	round2->set_gotto(true);
	for(int i=0;i<5;++i)
	{
		OPUserBattleRet_Character* cha1 =  round2->add_characters();

		cha1->set_position(i);
		cha1->set_resourceid(i*2+1);
		cha1->set_life(999);
		cha1->set_power(200);
	}
	for(int i=0;i<3;++i)
	{
		OPUserBattleRet_Action* action = round2->add_actions();
		action->set_positionid(rand()%5);
		//action->set_damage(520);
		action->set_type(rand()%3==0?OPUserBattleRet_Action_Type_SKILL:OPUserBattleRet_Action_Type_ATTACK);
		action->set_skillid(rand()%70+1);
	}

	//final round
	OPUserBattleRet_FinalRound* finalRound = mBattleMessage->mutable_round3();
	for(int i=0;i<5;++i)
	{
		OPUserBattleRet_Character* cha1 =  finalRound->add_characters();

		cha1->set_position(i);
		cha1->set_resourceid(i*2+1);
		cha1->set_life(100+rand()%500);
		cha1->set_power(rand()%300+100);
	}
	finalRound->set_gotto(true);
	finalRound->set_win(true);
	
	OPUserBattleRet_Result* result = mBattleMessage->mutable_result();
	result->set_canviewteam(true);
	result->set_gotcoin(1999);
	result->set_gotexp(188);
	OPUserBattleRet_Result_GotItemResult *gotitem =  result->mutable_gotitem();
	gotitem->set_type(OPUserBattleRet_Result_GotItemResult_ITEMTYPE_TOOL);
	gotitem->set_count(2);
	gotitem->set_allocated_tool(new OPUserToolInfoRet_ToolInfo());
	gotitem->mutable_tool()->set_id(1);
	gotitem->mutable_tool()->set_itemid(1101001);

	result->set_winlose(OPUserBattleRet_Result_WINLOSE_JUSTWIN);
	for(int i=0;i<4;++i)
	{
		OPUserBattleRet_Result_CharacterResult* cha = result->add_characterresults();
		cha->set_gainexp(99);
		cha->set_isupgarade(rand()%2);
		cha->set_level(i+1);
		cha->set_resourceid(i*4 + 1);
	}
	return mBattleMessage;
}

void FightPage::nextRound()
{
	mCurrentStateDone = true;
	if(mFightType==WorldBoss)
	{
		GetPropManager::Get()->popUpResultPage(mBattleMsg);
		MsgFightEnd fightEndMsg;
		fightEndMsg.gotItem = false;
		MessageManager::Get()->sendMessage(&fightEndMsg);
	}
}

void FightPage::onReceivePacket( const int opcode, const ::google::protobuf::Message* packet )
{
	if(opcode == _getOpcode() && packet && !mFighting)
	{
		const OPUserBattleRet* msg = dynamic_cast<const OPUserBattleRet*>(packet);
		if(msg)
		{
			if(mBattleMsg)
			{
				mBattleMsg->Clear();
				delete mBattleMsg;
			}
			mBattleMsg = new OPUserBattleRet;
			mBattleMsg->CopyFrom(*msg);

			if(mBattleMsg)
				fight();
		}
	}
}

void FightPage::fight()
{
	if(mBattleMsg)
	{
		SoundManager::Get()->playFightMusic();

		mCurrRound=1;
		CCB_FUNC(this,"mControlCover",CCMenuItemImage,setVisible(true));
		CCB_FUNC(this,"mSkipButton",CCMenuItemImage,setEnabled(false));
		mFighting = true;
		std::string _fightEnemyNameKey="fightEnemyNameKey";
		std::string _fightEnemyName="???";
		if(HAS_V(_fightEnemyNameKey))
		{
			_fightEnemyName=GET_V_S(_fightEnemyNameKey);
		}
		if(mFightType==WorldBoss)
		{
			_fightEnemyName=VaribleManager::Get()->getSetting("WorldBossName");
		}
		CCB_FUNC(this,"mPlayerName",CCLabelTTF,setString(ServerDateManager::Get()->getUserBasicInfo().name.c_str()));
		CCB_FUNC(this,"mEnemyName",CCLabelTTF,setString(_fightEnemyName.c_str()));
		CCB_FUNC(this,"mnum1",CCLabelBMFont,setString(StringConverter::toString(mBattleMsg->teamarchievements().team1archievements()).c_str()));
		CCB_FUNC(this,"mnum2",CCLabelBMFont,setString(StringConverter::toString(mBattleMsg->teamarchievements().team2archievements()).c_str()));
		//time_t tmNow = time(NULL);
		//tm *ptmNow = localtime(&tmNow);
		//float startTime=ptmNow->tm_sec;
		//CCLOG("load battle startTime:%f",startTime);
		_removeMaxLifeList();
		
		//time_t tmEnd = time(NULL);
		//tm* ptmEnd = localtime(&tmEnd);
		//float battleTime=ptmEnd->tm_sec;
		//CCLOG("load battle endTime:%f,Cost:%f",battleTime,(battleTime<startTime?(battleTime+60):battleTime)-startTime);
		
		mStates.push_back(new FightPage_roundState(mBattleMsg->round1(),1,mBattleMsg->round2().gotto(),false));
		if(mBattleMsg->round2().gotto())
		{
			mStates.push_back(new FightPage_roundState(mBattleMsg->round2(),2,mBattleMsg->round3().gotto(),true));
			if(mBattleMsg->round3().gotto())
				mStates.push_back(new FightPage_finalRound);
		}

		if(mFightType==FightEndBattle)
		{
			mStates.push_back(new FightPage_resultState_FightEnd);
		}
		else
		{
			if(mFightType!=WorldBoss && mFightType!=FightTypeAnyThing)
			{
				mStates.push_back(new FightPage_resultState);
			}
		}

		mCurrentStateDone = true;
		mSkip = false;
	}
}

void FightPage::setMessage( const OPUserBattleRet* msg,bool isShowThreeEvaluation,bool isHideSkip)
{
	mHideSkip = isHideSkip;
	mIsShowThreeEvaluation=isShowThreeEvaluation;
	if(mBattleMsg)
	{
		mBattleMsg->Clear();
		delete mBattleMsg;
	}
	mBattleMsg = new OPUserBattleRet;
	mBattleMsg->CopyFrom(*msg);

	//dispose discipleinfo and battleinfo

	for(int i = 0;i<mBattleMsg->result().characterresults_size();++i)
	{
		const OPUserBattleRet_Result_CharacterResult cha = mBattleMsg->result().characterresults().Get(i);

		UserDiscipleInfo* info = ServerDateManager::Get()->getUserDiscipleInfoByItemID(cha.resourceid());
		if(info)
		{
			info->preLevel = info->level;
			info->exp+=cha.gainexp();
			info->level = cha.level();
		}
	}

	for(int i = 0;i<mBattleMsg->disciple_size();++i)
	{
		const OPUserDiscipleInfoRet_DiscipleInfo& info = mBattleMsg->disciple(i);
		UserDiscipleInfo * userDiscipleInfo = ServerDateManager::Get()->getAndCreatDiscipleInfo(info.id());
		userDiscipleInfo->itemid		= info.itemid();
		userDiscipleInfo->level			= info.level();
		userDiscipleInfo->exp			= info.exp();
		userDiscipleInfo->health		= info.health();
		userDiscipleInfo->attack		= info.attack();
		userDiscipleInfo->defence		= info.defence();
		userDiscipleInfo->rp			= info.rp();
		userDiscipleInfo->upgradelevel	= info.upgradelevel();
		userDiscipleInfo->potentiality	= info.potentiality();
		userDiscipleInfo->skill1		= info.skill1();
		userDiscipleInfo->battleid		= info.battleid();
	}
	for(int i=0;i<mBattleMsg->battlearrayinfo_size();++i)
	{
		const OPUserBattleArrayRet_BattleArrayInfo infoBattle=mBattleMsg->battlearrayinfo().Get(i);
		UserBattleInfo* userBattleInfo = ServerDateManager::Get()->getAndCreatUserBattleInfo(infoBattle.id());
		userBattleInfo->id			=infoBattle.id();
		userBattleInfo->discipleId	=infoBattle.discipleid();
		userBattleInfo->attack		=infoBattle.attack();
		userBattleInfo->defence		=infoBattle.defence();
		userBattleInfo->health		=infoBattle.health();
		userBattleInfo->rp			=infoBattle.rp();
		userBattleInfo->position	=infoBattle.position();
	}
	mFightType=CareerBattle;
}

void FightPage::setMessage( char* msgChar, int length ,bool isShowThreeEvaluation/*=false*/,bool isHideSkip /*= false*/ )
{
	OPUserBattleRet msg;
	std::string msgStr(msgChar,length);
	msg.ParseFromString(msgStr);
	setMessage(&msg,isShowThreeEvaluation,isHideSkip);
}

int FightPage::setLifeMax( int pos,int life )
{
	std::map<int,int>::iterator it = mLifes.find(pos);
	if(it==mLifes.end())
	{
		mLifes.insert(std::make_pair(pos,life));
		return life;
	}
	else if(it->second<life)
		it->second = life;

	return it->second;
}

int FightPage::setLiftMaxByPos(int pos,int life,bool isSelf)
{
	if(isSelf)
	{
		if(mSelfLifeList.size()>=pos)
		{
			return getLifeMaxByPos(pos,isSelf);
		}
		else
		{
			mSelfLifeList.push_back(life);
			return life;
		}
	}
	else
	{
		if(mEnemyLifeList.size()>=pos)
		{
			return getLifeMaxByPos(pos,isSelf);
		}
		else
		{
			mEnemyLifeList.push_back(life);
			return life;
		}
	}
}

int FightPage::getLifeMax( int pos )
{
	std::map<int,int>::iterator it = mLifes.find(pos);
	if(it!=mLifes.end())
	{
		return it->second;
	}
	return 0;
}

int FightPage::getLifeMaxByPos(int pos,bool isSelf)
{
	if(isSelf)
	{
		if(mSelfLifeList.size()>=pos)
		{
			VecIntList::iterator it=mSelfLifeList.begin();
			for(int _currPost=1;it!=mSelfLifeList.end();++it,++_currPost)
			{
				if(_currPost==pos)
				{
					return *it;
				}
			}
			return 0;
		}
		return 0;
	}
	else
	{
		if(mEnemyLifeList.size()>=pos)
		{
			VecIntList::iterator it=mEnemyLifeList.begin();
			for(int _currPost=1;it!=mEnemyLifeList.end();++it,++_currPost)
			{
				if(_currPost==pos)
				{
					return *it;
				}
			}
			return 0;
		}
		return 0;
	}
}

void FightPage::removeLifeMaxByDeadPos(VecIntList _selfList,VecIntList _enemyList)
{
	VecIntList::iterator it=mSelfLifeList.begin();
	VecIntList::iterator _posIt=_selfList.begin();
	for(int _currPost=1;_posIt!=_selfList.end()&&it!=mSelfLifeList.end();++_posIt,++_currPost)
	{
		if((*_posIt)==_currPost)
		{
			it = mSelfLifeList.erase(it); 
		}
		else
		{
			++it;
		}
	}
	it=mEnemyLifeList.begin();
	_posIt=_enemyList.begin();
	for(int _currPost=1;_posIt!=_enemyList.end()&&it!=mEnemyLifeList.end();++_posIt,++_currPost)
	{
		if((*_posIt)==_currPost)
		{
			it = mEnemyLifeList.erase(it); 
		}
		else
		{
			++it;
		}
	}
}

void FightPage::_removeMaxLifeList()
{
	mSelfLifeList.clear();
	mEnemyLifeList.clear();
}

float FightPage::caculateXunit( int id, int count )
{
	count++;//make real count
	if(count%2==1)count++;//if odd, make even
	float offset = 0;
	int singlelineCount = count/2;
	if(singlelineCount%2==0)//if single line odd
		offset = 0.5f;
	return (float)((id/2)-(singlelineCount/2))+offset;
}

float FightPage::calcXunit(int id,int count,bool isNewline)
{
	
	if(isNewline)
	{
		id=id-(count)*2;
	}
	
	float offset = 0;
	if(count%2==0)//if single line odd
	{
		offset = 0.5f;
	}
	return (float)((id/2)-(count/2))+offset;
}

int FightPage::calcMaxBattle(int count)
{
	int _battle=count;
	if(count%2==0)
	{
		_battle=count/2+1;
	}
	else
	{
		_battle=(count+1)/2;
	}
	return _battle;
}

int FightPage::calcCurrPosition(int position,int maxPosition,bool isNewline)
{
	int _position=1;
	if(isNewline)
	{
		position=position-maxPosition*2;
	}
	if(position%2==0)
	{
		_position=position/2+1;
	}
	else
	{
		_position=(position+1)/2;
	}
	return _position;
}

 bool FightPage::isNewline(int id,int count)
 {
	 if(id>=count*2)
	 {
		 return true;
	 }
	 return false;
 }

 bool FightPage::isHavNewline(int maxPosition)
 {
	 if(maxPosition>=(MaxBattleNumLine*2))
	 {
		 return true;
	 }
	 return false;
 }

bool FightPage::getHideSkip()
{
	return mHideSkip;
}

FightPage* FightPage::getInstance()
{
	return dynamic_cast<FightPage*>(CCBManager::Get()->getPage("FightPage"));
}
