
#include "stdafx.h"

#include "FightPage_roundState.h"
#include "SkillConsts.h"
#include "FightTasks.h"
#include "InstructionManager.h"
#include "DataTableManager.h"
#include "FightCardSprite.h"
#include "StringConverter.h"
#include "FightTasksSkill.h"
#include "GameMessages.h"

// const float TimeBt_Character_enter = 0.5f;
const float TimeBt_Character_enter2buff = 0.25f;
// const float TimeBt_Character_buff = 0.15f;
// const float TimeBt_Attack_physical = 0.65f;

FightPage_roundState::FightPage_roundState(const OPUserBattleRet_Round& round, int roundCount, bool hasNext, bool hasPrevious)
	:mMessage(round)
	,mRoundCount(roundCount)
	,mHasNext(hasNext)
	,mHasPrevious(hasPrevious)
{
}


FightPage_roundState::~FightPage_roundState(void)
{
}

void FightPage_roundState::Enter( FightPage* page)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_FIGHTROUND_LOADSKILL,this);
	mCurrentActionTime = 1.0f;
	page->mCurrRound=mRoundCount;
	if(!mMessage.gotto())
	{
		page->nextRound();
	}
	_createSlots(page);
	_createTasks(page);
	CCB_FUNC(page,"mSkipButtonNode",CCNode,setVisible(false));
	mSkipShown = false;
	mSkipTime = 0;
	mfirstEnter = true;
}

void FightPage_roundState::Execute( FightPage* page)
{
	float dt = GamePrecedure::Get()->getFrameTime();
	//mCurrentActionTime-=dt;

	mSkipTime+=dt;
	if(mSkipShown==false && (mSkipTime>2.0f || mRoundCount>1))
	{
		mSkipShown = true;
		const UserBasicInfo info =ServerDateManager::Get()->getUserBasicInfo();
		if(	info.tutorialstep==1 && 
			(InstructionManager::Get()->loadLocalInstructionStep()>=InstructionManager::TOTAL_INSTRUCTION||info.level>2||
			InstructionManager::Get()->loadLocalInstructionStep()==1) &&
			page->getHideSkip()==false)
		{
			CCB_FUNC(page,"mSkipButton",CCMenuItemImage,setEnabled(true));
			if(mfirstEnter)
			{
				CCB_FUNC(page,"mSkipButtonNode",CCNode,setVisible(true));
				mfirstEnter = false;
			}
			
		}
		else
		{
			CCB_FUNC(page,"mSkipButton",CCMenuItemImage,setEnabled(false));
		}
	}

// 	if(mCurrentActionTime>0)
// 		return;
	if(dt>0.5f)
		return;

	if(mTaskList.empty())
	{
		page->nextRound();
	}
	else
	{
		TASKLIST tasks;
		mTaskList.swap(tasks);
		while(!tasks.empty())
		{
			FightTask* tsk = tasks.front();
			FightTask::FT_State ret = tsk->update(dt);
			if(ret == FightTask::FT_WAIT || ret == FightTask::FT_DOING )
				mTaskList.push_back(tsk);
			else
				delete tsk;
			tasks.pop_front();
		}
	}
}

void FightPage_roundState::Exit( FightPage* page)
{
	MessageManager::Get()->removeMessageHandler(this);
	_destroySlots(page);
	TASKLIST tasks;
	mTaskList.swap(tasks);
	while(!tasks.empty())
	{
		FightTask* tsk = tasks.front();
		delete tsk;
		tasks.pop_front();
	}
}

void FightPage_roundState::_createSlots(FightPage* page)
{

	cocos2d::CCNode* node =  dynamic_cast<cocos2d::CCNode*>(page->getVariable("mPeople"));
	int slotsize = mMessage.characters_size();
	int maxPositionSize = 0;
	for(int i=0;i<slotsize;++i)
	{
		if(mMessage.characters(i).position()>maxPositionSize)
			maxPositionSize = mMessage.characters(i).position();
	}
	
	mBattleCount=FightPage::calcMaxBattle(maxPositionSize);
	mFightPoint=FightConfigManager::Get()->getFightPointById(mBattleCount);
	if(!mFightPoint)
	{
		CCLOG(" mFightPoint is not exist _battle:%d",mBattleCount);
		return;
	}
	mMaxNewLineSize =mFightPoint->newline;
	mIsHavNewline=FightPage::isHavNewline(maxPositionSize);
	for(int i=0;i<slotsize;++i)
	{
		bool _isSelf=false;
		int _dept=0;
		
		SLOTMEMBER memeber;
		int position = mMessage.characters().Get(i).position();
		bool isNewline = FightPage::isNewline(position,mMaxNewLineSize);

		memeber.position=position;
		if(position%FightPage::TEAM_COUNT == FightPage::TEAM_ENEMY)
		{
		}
		else
		{
			_isSelf=true;
			_dept=isNewline?-1:0;
		}
		memeber.isNewline=isNewline;
		memeber.life = mMessage.characters().Get(i).life();
		memeber.plife = &memeber.life;
		memeber.maxlife = page->setLiftMaxByPos(memeber.currBattlePosition(),memeber.life,_isSelf);//memeber.life;
		memeber.battle = page->mBattleList.find(i)->second;
		float unit = FightPage::calcXunit(position,mMaxNewLineSize,isNewline);
		CCNode* _head=dynamic_cast<CCNode*>(memeber.battle->getVariable("mHead"));
		float unitwidth =_head?_head->getContentSize().width:memeber.battle->getContentSize().width;
		memeber.battle->setPositionX(unitwidth*(unit - 0.5f)+page->getContentSize().width*0.5);
		memeber.battle->setPositionY(0);
		memeber.battle->setVisible(true);

		{
			CCObject* turnObj = memeber.battle->getVariable("mTurn");
			CCNode* turnNode = turnObj?dynamic_cast<CCNode*>(turnObj):0;

			CCObject* turnHeadObj = memeber.battle->getVariable("mHead");
			CCNode* turnHeadNode = turnHeadObj?dynamic_cast<CCNode*>(turnHeadObj):0;


			//if(turnNode && turnNode2 && turnNode3 && turnNode4 && turnNode5 && turnNode6 )
			if(turnNode)
			{

				if(memeber.battle->getParent())
					memeber.battle->removeFromParent();
				if(!_isSelf)
				{
					turnNode->setRotation(0.0f);
					node->addChild(memeber.battle,isNewline?1:-1);
				}
				else
				{
					turnNode->setRotation(180.0f);
					turnHeadNode->setRotation(180.0f);
					node->addChild(memeber.battle,isNewline?2:0);
				}
			}
		}

		memeber.resourceID = mMessage.characters().Get(i).resourceid();
		const DiscipleItem* item =  DiscipleTableManager::Get()->getDiscipleItemByID(memeber.resourceID);
		if(item)
		{
			std::string bgFightPic = VaribleManager::Get()->getSetting("ImageFileSelf");
			std::string ftFightPic = /*"person/poster_char_xiangjishi.png";//*/item->bodyPic;
			
			if(!_isSelf)
			{
				CCB_FUNC(memeber.battle,"mOwn2",CCSprite,initWithFile(VaribleManager::Get()->getSetting("ImageFileBloodEnemy").c_str()));
				bgFightPic = VaribleManager::Get()->getSetting("ImageFileEnemy");
			}
			else
			{
				CCB_FUNC(memeber.battle,"mOwn2",CCSprite,initWithFile(VaribleManager::Get()->getSetting("ImageFileBloodSelf").c_str()));
			}
			float lifePersentage =1.0f;
			if(memeber.life!=memeber.maxlife)
			{
				lifePersentage=((float)memeber.life/(float)memeber.maxlife);
			}
			FightCardSprite *cardhead = FightCardSprite::create(bgFightPic.c_str());
			cardhead->setAnchorPoint(ccp(0,0));

			CCB_FUNC(memeber.battle,"mOwn1",CCNode,removeAllChildren());
			CCB_FUNC(memeber.battle,"mOwn1",CCNode,addChild(cardhead,1,SLOTMEMBER::TAG_BG));
			cardhead->setLifeTo(lifePersentage,0.0f);
			if(ftFightPic == "none")ftFightPic = "person/poster_char_xiangjishi.png";
			cardhead = FightCardSprite::create(ftFightPic.c_str());
			if(!cardhead)cardhead = FightCardSprite::create("person/poster_char_xiangjishi.png");

			cardhead->setAnchorPoint(ccp(0,0));
			CCB_FUNC(memeber.battle,"mPoster",CCNode,removeAllChildren()); 
			CCB_FUNC(memeber.battle,"mPoster",CCNode,addChild(cardhead,1,SLOTMEMBER::TAG_HEAD)); 
			cardhead->setLifeTo(lifePersentage,0.0f);	
			if(item->quality>0)
			{
				char key[128];
				sprintf(key,"MemImageFileQuality%d",item->quality);
				CCB_FUNC(memeber.battle,"mQuality",CCSprite,initWithFile(VaribleManager::Get()->getSetting(key).c_str()));
			}
			if(memeber.life==-1)
			{
				CCB_FUNC(memeber.battle,"mBlood",CCLabelBMFont,setString("???"));
			}
			else
			{
				CCB_FUNC(memeber.battle,"mBlood",CCLabelBMFont,setString(StringConverter::toString(memeber.life).c_str()));
			}
			CCB_FUNC(memeber.battle,"mPosition",CCLabelBMFont,setString(StringConverter::toString(memeber.currBattlePosition()).c_str()));
			
			CCPoint _point=getStartPosition(page,memeber.battle,_head,_isSelf,mIsHavNewline,isNewline);
			memeber.battle->setPosition(_point.x,_point.y);
			
			memeber.mEntrancePoint=getEntrancePosition(page,memeber.battle,_head,_isSelf,mIsHavNewline,isNewline);
			memeber.mStopPoint=getQueuePosition(page,memeber.battle,_head,(isNewline?(memeber.currBattlePosition()-mFightPoint->newline):memeber.currBattlePosition()),_isSelf,mIsHavNewline,isNewline);
			//CCLOG("postion:%d,currBattle:%d,pos-x:%f,pos-y:%f",position,memeber.currBattlePosition(),_point.x,_point.y);
			//memeber.battle->setPosition(memeber.mStopPoint.x,memeber.mStopPoint.y);
			//CCLOG("stop_postion:%d,currBattle:%d,pos-x:%f,pos-y:%f",position,memeber.currBattlePosition(),memeber.mStopPoint.x,memeber.mStopPoint.y);
		}
        else
        {
            char errstr[64];
            sprintf(errstr,"failed to get disciple: resoureceID:%d", memeber.resourceID);
            CCMessageBox(errstr, "error");
            //return;
        }

		mSlots.insert(std::make_pair(
			mMessage.characters().Get(i).position(),
			memeber	));

	}
}

void FightPage_roundState::_destroySlots( FightPage* page)
{
	SLOTLIST::iterator it = mSlots.begin();
	int i=0;
	FightPage::VecIntList _selfDeadList;
	FightPage::VecIntList _enemyDeadList;
	for(;it!=mSlots.end();++it,++i)
	{
		int position=it->second.position;
		
		if(it->second.life<=0)
		{
			bool _isSelf=true;
			if(position%FightPage::TEAM_COUNT == FightPage::TEAM_ENEMY)
			{
				_isSelf=false;
			}
			if(_isSelf)
			{
				_selfDeadList.push_back(it->second.currBattlePosition());
			}
			else
			{
				_enemyDeadList.push_back(it->second.currBattlePosition());
			}
		}
		CCObject* turnObj = it->second.battle->getVariable("mTurn");
		CCNode* turnNode = turnObj?dynamic_cast<CCNode*>(turnObj):0;

		CCObject* turnHeadObj = it->second.battle->getVariable("mHead");
		CCNode* turnHeadNode = turnHeadObj?dynamic_cast<CCNode*>(turnHeadObj):0;
		if(turnNode)
		{
			turnNode->setRotation(0.0f);
			turnHeadNode->setRotation(0.0f);
		}
		CCB_FUNC(it->second.battle,"mOwn1",CCNode,removeAllChildren());
		CCB_FUNC(it->second.battle,"mPoster",CCNode,removeAllChildren()); 
		it->second.battle->runAnimation("stand");
	}
	cocos2d::CCNode* node =  dynamic_cast<cocos2d::CCNode*>(page->getVariable("mPeople"));
	if(node)
		node->removeAllChildren();
	cocos2d::CCNode* nodeBigSkill =  dynamic_cast<cocos2d::CCNode*>(page->getVariable("mBigskill"));
	if(nodeBigSkill)
		nodeBigSkill->removeAllChildren();
	cocos2d::CCNode* nodeDefense =  dynamic_cast<cocos2d::CCNode*>(page->getVariable("mDefense"));
	if(nodeDefense)
		nodeDefense->removeAllChildren();
	cocos2d::CCNode* nodeAttackSmall =  dynamic_cast<cocos2d::CCNode*>(page->getVariable("mAttackSmallskill"));
	if(nodeAttackSmall)
		nodeAttackSmall->removeAllChildren();
	page->removeLifeMaxByDeadPos(_selfDeadList,_enemyDeadList);
	_selfDeadList.clear();
	_enemyDeadList.clear();
}

void FightPage_roundState::_createTasks(FightPage* page)
{
	float startTime = 0.0f;
	float finishTime = 0.0f;
	mSkillLoaded = false;
	/**
	//create start
	if(mHasPrevious)
	{
		FightTaskRoundStart * task = new FightTaskRoundStart(mRoundCount,page);
		task->setAttribute(mSlots.begin()->second,0,&mSlots.begin()->second.life);

		finishTime = startTime+task->getDuration();
		startTime+=task->getNextDelay();
		mTaskList.push_back(task);
	}

	//create enter
	SLOTLIST::iterator itEnter = mSlots.begin();
	int i=0;
	float enterDelay = 0;
	float enterTaskFinish = 0;
	for(;itEnter!=mSlots.end();++itEnter)
	{
		i++;
		FightTaskEnter * task = new FightTaskEnter;
		enterDelay = task->getNextDelay();
		enterTaskFinish = task->getDuration();
		task->setAttribute(itEnter->second,startTime+((float)i)*enterDelay,&itEnter->second.life);
		mTaskList.push_back(task);
	}
	startTime+=((float)i)*enterDelay+TimeBt_Character_enter2buff;
	finishTime = startTime-enterDelay+enterTaskFinish;
	**/
	//create Entrance
	SLOTLIST::iterator itEntrance = mSlots.begin();
	int i=0;
	float enterDelay = 0;
	float enterTaskFinish = 0;
	for(;itEntrance!=mSlots.end();++itEntrance)
	{
		i++;
		SLOTMEMBER memeber=itEntrance->second;
		FightTaskEntrance * task = new FightTaskEntrance(FightConfigManager::Get()->getEntranceDurationTime(),FightConfigManager::Get()->getEntranceDelayTime(),memeber.mEntrancePoint);
		enterDelay = task->getNextDelay();
		enterTaskFinish = task->getDuration();
		task->setAttribute(itEntrance->second,startTime+((float)i)*enterDelay,&itEntrance->second.life);
		mTaskList.push_back(task);
	}
	startTime+=enterTaskFinish+((float)i)*enterDelay+FightConfigManager::Get()->getIntervalDelayTime();
	finishTime = startTime-enterDelay;

	//create Queue
	SLOTLIST::iterator itQueue = mSlots.begin();
	enterDelay = 0;
	enterTaskFinish = 0;
	for(i=0;itQueue!=mSlots.end();++itQueue)
	{
		i++;
		SLOTMEMBER memeber=itQueue->second;
		FightTaskQueue * task = new FightTaskQueue(FightConfigManager::Get()->getQueueUpDurationTime(),FightConfigManager::Get()->getQueueDelayTime(),memeber.mStopPoint);
		enterDelay = task->getNextDelay();
		enterTaskFinish = task->getDuration();
		task->setAttribute(itQueue->second,startTime+((float)i)*enterDelay,&itQueue->second.life);
		mTaskList.push_back(task);
	}
	startTime+=enterTaskFinish+((float)i)*enterDelay;
	finishTime = startTime-enterDelay;

	//create s_curtain start
	static FightPage_roundState::SLOTMEMBER tempSlot;tempSlot.life = 0;
	FightTaskRoundStart * taskStart = new FightTaskRoundStart(mRoundCount,page);
	taskStart->setAttribute(tempSlot,startTime,&tempSlot.life);
	//taskStart->setAttribute(mSlots.begin()->second,startTime,&mSlots.begin()->second.life);
	finishTime = startTime+taskStart->getDuration();
	startTime+=taskStart->getNextDelay();
	mTaskList.push_back(taskStart);

	//create buffs
	std::map<int,std::set<int> > buffs;
	int BuffMaxRound = 0;
	for(int i = 0;i<mMessage.actions_size();++i)
	{
		const OPUserBattleRet_Action& act = mMessage.actions().Get(i);
		if(act.type() == OPUserBattleRet_Action_Type_SKILL)
		{
			int skillid = act.skillid();
			int characterid = act.positionid();
			const SkillItem *skill = SkillTableManager::Get()->getSkillItemByID(skillid);
			SLOTLIST::iterator it = mSlots.find(characterid);
			if(skill && it!=mSlots.end() && skill->type&ST_BUFF)
			{

				if(buffs.find(characterid)!=buffs.end())
				{
					buffs[characterid].insert(skillid);
					if(buffs[characterid].size()>BuffMaxRound)BuffMaxRound = buffs[characterid].size();
				}
				else
				{
					std::set<int> skills;
					skills.insert(skillid);
					if(skills.size()>BuffMaxRound)BuffMaxRound = skills.size();
					buffs.insert(std::make_pair(characterid,skills));
				}
			}
		}
	}
	if(!buffs.empty())
	{
		int lastPositionID = -1;
		for(int roundCount = 0; roundCount<BuffMaxRound ;++roundCount)
		{
			int positionCount = 0;
			std::map<int,std::set<int> >::iterator itBuff = buffs.begin();
			for( ; itBuff != buffs.end() ; ++itBuff,++positionCount)
			{

				if(!itBuff->second.empty())
				{
					std::set<int>::iterator itSkill = itBuff->second.begin();
					int skillid = *itSkill;
					int characterid = itBuff->first;
					const SkillItem *skill = SkillTableManager::Get()->getSkillItemByID(skillid);
					SLOTLIST::iterator it = mSlots.find(characterid);

					FightTask* task = 0;
					if(skill && it!=mSlots.end())
					{
						if(skill->type == ST_BUFF_ATTACK)
						{
							task = new FightTaskBuff(skill,FightTaskBuff::ATTACK);
						}
						else if(skill->type == ST_BUFF_DEFENSE)
						{
							task = new FightTaskBuff(skill,FightTaskBuff::DEFENSE);
						}
						else if(skill->type == ST_BUFF_POWER)
						{
							task = new FightTaskBuff(skill,FightTaskBuff::WILL);
						}

					}

					if(task)
					{
						if(lastPositionID == it->second.position)
							startTime+=0.6f;
						
						lastPositionID = it->second.position;

						task->setAttribute(it->second,startTime,&it->second.life);

						finishTime = startTime + task->getDuration();
						startTime+=task->getNextDelay();
						
						mTaskList.push_back(task);
					}

					itBuff->second.erase(itSkill);
				}
			}
		}
		FightTaskBuff::mBuffCount = 0;
		//startTime+=((float)BuffMaxRound)*1.0f;
		startTime+=1.75f;
	}
	
	//create actions
	for(int i = 0;i<mMessage.actions_size();++i)
	{
		FightTask* task = 0;
		const OPUserBattleRet_Action& act = mMessage.actions().Get(i);

		int skillid = act.skillid();
		int characterid = act.positionid();
		const SkillItem *skill = SkillTableManager::Get()->getSkillItemByID(skillid);
		SLOTLIST::iterator it = mSlots.find(characterid);

		int opponentid = _getOpponentID(characterid);
		SLOTLIST::iterator itOp = mSlots.find(opponentid);

		int lastAttack = -1;
		if(	act.type() == OPUserBattleRet_Action_Type_ATTACK && 
			it!=mSlots.end() && itOp!=mSlots.end())
		{
			task = new FightTaskAttack;
			task->setAttribute(it->second,startTime,&it->second.life);
			mTaskList.push_back(task);

			for(int dmgCount = 0; dmgCount< act.damage().size(); ++dmgCount)
			{
				SLOTLIST::iterator itOp2 = mSlots.find(act.damage().Get(dmgCount).position());
				if(itOp2!=mSlots.end())
				{
					task = new FightTaskDefense(act.damage().Get(dmgCount).harmvalue());
					task->setAttribute(itOp2->second,startTime,&itOp2->second.life);
					mTaskList.push_back(task);
				}
			}

			finishTime = startTime + task->getDuration();
			startTime+=task->getNextDelay();
		}
		else if(act.type() == OPUserBattleRet_Action_Type_SKILL&& 
			it!=mSlots.end() && itOp!=mSlots.end())
		{
			if(skill && (skill->type == SkillItem::TYPE_MULTIPLE || skill->type == SkillItem::TYPE_SINGLE))
			{
				float defDelay = 0;
				if(startTime<finishTime)startTime = finishTime;
				//big skill 
				if(skill->ccbFile != "s_battle")
				{
// 					if(skill->type == SkillItem::TYPE_MULTIPLE)
// 					{
// 						defDelay = FightConfigManager::Get()->getSkillTime(FightConfigManager::BIG_MULTIPLE,FightConfigManager::DEFENSE_DELAY);//2.2f;
// 						task = new FightTaskShake(FightTaskShake::TYPE_1,page);
// 					}
// 					else
// 					{
// 						defDelay = FightConfigManager::Get()->getSkillTime(FightConfigManager::BIG_SINGLE,FightConfigManager::DEFENSE_DELAY);//1.0f;
// 						task = new FightTaskShake(FightTaskShake::TYPE_2,page);
// 					}
// 					static SLOTMEMBER noUse1;
// 					if(mSlots.size()>0)
// 						noUse1 = mSlots[0];
// 					
// 					static int noUse2 = 0;
// 					task->setAttribute(noUse1,startTime + defDelay ,&noUse2);
// 					mTaskList.push_back(task);

					float prepare_dt = GET_FLASH_TIME(CT_BIG_MULTIPLE,CA_PAREPARE);
					finishTime+=prepare_dt;
					startTime+=prepare_dt;

					const DiscipleItem* _discipleItem=DiscipleTableManager::Get()->getDiscipleItemByID(it->second.resourceID);
					if(skill->type == SkillItem::TYPE_MULTIPLE)
					{
						task = new FightTaskSkillBigMultiple(skillid,finishTime,act.positionid(),_discipleItem);
						task->setAttribute(it->second,startTime,&it->second.life);
						for(int dmgCount = 0; dmgCount< act.damage().size(); ++dmgCount)
						{
							SLOTLIST::iterator itOp2 = mSlots.find(act.damage().Get(dmgCount).position());
							if(itOp2!=mSlots.end())
							{
								((FightTaskSkillBigMultiple*)task)->addEnemy(itOp2->second,act.damage().Get(dmgCount).harmvalue(),&itOp2->second.life);
							}
						}
						mTaskList.push_back(task);
					}
					else
					{
						task = new FightTaskSkillBigSingle(skillid,finishTime,act.positionid(),_discipleItem);
						task->setAttribute(it->second,startTime,&it->second.life);
						for(int dmgCount = 0; dmgCount< act.damage().size(); ++dmgCount)
						{
							SLOTLIST::iterator itOp2 = mSlots.find(act.damage().Get(dmgCount).position());
							if(itOp2!=mSlots.end())
							{
								((FightTaskSkillBigSingle*)task)->setEnemy(itOp2->second,act.damage().Get(dmgCount).harmvalue(),&itOp2->second.life);
							}
						}
						mTaskList.push_back(task);
					}
					
				}
				else
				{
					defDelay = FightConfigManager::Get()->getSkillTime(FightConfigManager::SMALL,FightConfigManager::DEFENSE_DELAY);
					task = new FightTaskSkillSmall(skillid,finishTime,act.positionid());
					task->setAttribute(it->second,startTime,&it->second.life);
					for(int dmgCount = 0; dmgCount< act.damage().size(); ++dmgCount)
					{
						SLOTLIST::iterator itOp2 = mSlots.find(act.damage().Get(dmgCount).position());
						if(itOp2!=mSlots.end())
						{
							itOp2->second.plife = & (itOp2->second.life);
							((FightTaskSkillSmall*)task)->addEnemy(itOp2->second,act.damage().Get(dmgCount).harmvalue());
						}
					}
					mTaskList.push_back(task);
				}

				finishTime = startTime + task->getDuration();
				startTime+= task->getNextDelay();
			}
		}


	}

	if(page->IsWorldBoss())
	{
		float _duration=0.0f;
		float _nextDelay=0.0f;
		for(int i = 0;i<mMessage.actions_size();++i)
		{
			FightTask* task = new FightTaskAttack;
			int characterid = 1+i*2;
			SLOTLIST::iterator it = mSlots.find(characterid);

			int opponentid = _getOpponentID(characterid);
			SLOTLIST::iterator itOp = mSlots.find(opponentid);

			int lastAttack = -1;
			if(it!=mSlots.end() && itOp!=mSlots.end())
			{
				task->setAttribute(it->second,startTime,&it->second.life);
				mTaskList.push_back(task);

				task = new FightTaskDefense(WorldBossAttack);
				task->setAttribute(itOp->second,startTime,&itOp->second.life);
				mTaskList.push_back(task);

				if(i==0)
				{
					_duration= task->getDuration();
					_nextDelay=	task->getNextDelay();
				}

			}
		}
		finishTime = startTime + _duration;
		startTime+=_nextDelay;
	}


	if(finishTime<startTime)
		mTaskList.push_back(new FightTaskWait(startTime,startTime - finishTime+0.8f));

	//create s_curtain end
	/*
	FightTaskRoundOver * task = new FightTaskRoundOver(mRoundCount,page);
	task->setAttribute(mSlots.begin()->second,startTime,&mSlots.begin()->second.life);
	mTaskList.push_back(task);
	*/
	//create final
	/*
	if(mHasNext)
	{
		FightTaskRoundOver * task = new FightTaskRoundOver(mRoundCount,page);
		task->setAttribute(mSlots.begin()->second,startTime,&mSlots.begin()->second.life);
		mTaskList.push_back(task);
	}
	*/
}

int FightPage_roundState::_getOpponentID( int id )
{
	if((id%2)==0)
		return id+1;
	else
		return id-1;
}

CCPoint FightPage_roundState::getStartPosition(FightPage* _page,CCBContainer* _battle,CCNode* _headFace,bool isSelf,bool _isHavNewline,bool _isNewline)
{
	CCPoint _point=CCPoint(0,0);
	if(!isSelf)
	{
		if(_isHavNewline)
		{
			_point.setPoint(_battle->getPositionX(),(_page->getContentSize().height*_page->getScaleY()+mFightPoint->startY)+_headFace->getContentSize().height*_headFace->getScaleY()*_headFace->getAnchorPoint().y);
		}
		else
		{
			_point.setPoint(_battle->getPositionX(),(_page->getContentSize().height*_page->getScaleY()+mFightPoint->startY)+_headFace->getContentSize().height*_headFace->getScaleY()*_headFace->getAnchorPoint().y);
		}
	}
	else
	{
		if(_isHavNewline)
		{
			_point.setPoint(_battle->getPositionX(),((-1)*_headFace->getContentSize().height*_headFace->getScaleY()*_headFace->getAnchorPoint().y+(-1)*mFightPoint->startY));
		}
		else
		{
			_point.setPoint(_battle->getPositionX(),((-1)*_headFace->getContentSize().height*_headFace->getScaleY()*_headFace->getAnchorPoint().y+(-1)*mFightPoint->startY));
		}
	}
	_point.setPoint(_point.x+_headFace->getContentSize().width*_headFace->getScaleX()*0.25,_point.y);
	return _point;
}

CCPoint FightPage_roundState::getEntrancePosition(FightPage* _page,CCBContainer* _battle,CCNode* _headFace,bool _isSelf,bool _isHavNewline,bool _isNewline)
{
	CCPoint _point=_battle->getPosition();
	if(!_isSelf)
	{
		_point.setPoint(_battle->getPositionX()+(_isHavNewline?(_isNewline?mFightPoint->adjustX:0.0f):0.0f),(_page->getContentSize().height*_page->getScaleY()-(_page->getContentSize().height*_page->getScaleY()*0.5*mFightPoint->remainY/100)-(_isHavNewline?(_isNewline?mFightPoint->adjustY:0.0f):0.0f))+_headFace->getContentSize().height*_headFace->getScaleY()*_headFace->getAnchorPoint().y);
	}
	else
	{
		_point.setPoint(_battle->getPositionX()+(_isHavNewline?(_isNewline?mFightPoint->adjustX:0.0f):0.0f),(_page->getContentSize().height*_page->getScaleY()*0.5*mFightPoint->remainY/100+(_isHavNewline?(_isNewline?0.0f:mFightPoint->adjustY):0.0f))-_headFace->getContentSize().height*_headFace->getScaleY()*_headFace->getAnchorPoint().y);
	}

	_point.setPoint(_point.x+_headFace->getContentSize().width*_headFace->getScaleX()*0.25,_point.y);
	return _point;
}

CCPoint FightPage_roundState::getQueuePosition(FightPage* _page,CCBContainer* _battle,CCNode* _headFace,int _currBattle,bool _isSelf,bool _isHavNewline,bool _isNewline)
{
	CCPoint _point=_battle->getPosition();
	_point.x+=(_isHavNewline?(_isNewline?mFightPoint->adjustX:0.0f):0.0f);
	if(_point.x<_page->getContentSize().width*_page->getScaleX()*0.5)
	{
		_point.x-=mFightPoint->reviseStopX;
	}
	else
	{
		_point.x+=mFightPoint->reviseStopX;
	}
	if(!_isSelf)
	{
		_point.y=(_page->getContentSize().height*_page->getScaleY()+(_isHavNewline?(_currBattle-1)*mFightPoint->intervalY:0.0f))+(_headFace->getContentSize().height*_headFace->getScaleY()*(_headFace->getAnchorPoint().y));
		if(_isHavNewline)
		{
			_point.y-=(_page->getContentSize().height*_page->getScaleY()*0.5*(_isNewline?(mFightPoint->adjustStopY/100):(mFightPoint->stopY/100)));
		}
		else
		{
			_point.y-=(mFightPoint->stopY/100)*_page->getContentSize().height*_page->getScaleY()*0.5;
			_point.y+=FightConfigManager::Get()->getReviseCoordinatesByBattle(mBattleCount,_currBattle);
		}
	}
	else
	{
		_point.y=(_isHavNewline?(_currBattle-1)*mFightPoint->intervalY:0.0f)-(_headFace->getContentSize().height*_headFace->getScaleY()*(1.0f-_headFace->getAnchorPoint().y));
		if(_isHavNewline)
		{
			_point.y+=(_page->getContentSize().height*_page->getScaleY()*0.5*(_isNewline?(mFightPoint->stopY/100):(mFightPoint->adjustStopY/100)));
		}
		else
		{
			_point.y+=(mFightPoint->stopY/100)*_page->getContentSize().height*_page->getScaleY()*0.5;
			_point.y+=FightConfigManager::Get()->getReviseCoordinatesByBattle(mBattleCount,_currBattle);
		}
	}
	_point.setPoint(_point.x+_headFace->getContentSize().width*_headFace->getScaleX()*0.25,_point.y/*+_headFace->getContentSize().height*_headFace->getScaleY()*0.5*/);
	return _point;
}

void FightPage_roundState::onReceiveMassage( const GameMessage * message )
{
	if(!mSkillLoaded && message->getTypeId() == MSG_FIGHTROUND_LOADSKILL)
	{
		mSkillLoaded = true;
		TASKLIST::iterator it = mTaskList.begin();
		for(;it!=mTaskList.end();++it)
		{
			(*it)->onLoad();
		}
	}
}
