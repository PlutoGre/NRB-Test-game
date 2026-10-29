#pragma once
#include "FightPage_roundState.h"

#include "DataTableManager.h"

class FightTask
{
	static unsigned long taskcount;
protected:
	FightPage_roundState::SLOTMEMBER mSlot;
	float mStartTime;
	float mDurationTime;
	bool mStarted;
	int* pLife;
	static unsigned long int sGeneralVertexZ;

	void _addColorTo(CCBContainer*, ccColor4F, float duration, float delay = 0,float restoreTime = -1.0f);
	void _damageFlash(CCBContainer*, float delay, float duration);
public:
	FightTask();
	virtual ~FightTask();
	virtual void setAttribute(	FightPage_roundState::SLOTMEMBER slot,
						float startTime, int* life);
	enum FT_State
	{
		FT_WAIT,
		FT_DOING,
		FT_FAILED,
		FT_DONE,
	};
	FT_State update(float dt);
	void headTraceNode(CCNode*);
	
	virtual float getDuration(){return 0;}
	virtual float getNextDelay(){return 0;}

	virtual void onWaitTick(float dt){}
	virtual void onStart(){};
	virtual void onTick(float dt){}
	virtual void onEnd(){};
	virtual void onLoad(){};
};
class FightTaskEnter : public FightTask
{
public:
	virtual void onStart();
	
	virtual float getDuration(){return 0.25f;}
	virtual float getNextDelay(){return 0.05f;}
};

class FightTaskEntrance : public FightTask
{
private:
	CCPoint mPoint;
	float mNextDelay;
	static bool mSoundPlayed;
public:
	FightTaskEntrance(float _durationTime,float _nextDelay,CCPoint _point)
	{
		mDurationTime=_durationTime;
		mNextDelay=_nextDelay;
		mPoint=_point;
	};
	virtual void onStart();
	virtual void onEnd();
	virtual float getDuration(){return mDurationTime;}
	virtual float getNextDelay(){return mNextDelay;}
};

class FightTaskQueue : public FightTask
{
private:
	CCPoint mPoint;
	int mNextDelay;
	static bool mSoundPlayed;
public:
	FightTaskQueue(float _durationTime,float _nextDelay,CCPoint _point)
	{
		mDurationTime=_durationTime;
		mNextDelay=_nextDelay;
		mPoint=_point;
	};
	virtual void onStart();
	virtual void onEnd();
	virtual float getDuration(){return mDurationTime;}
	virtual float getNextDelay(){return mNextDelay;}
};

class FightTaskBuff : public FightTask
{
private:
	static int mBuffCount;
	const SkillItem* mSkillItem;
	friend class FightPage_roundState;
public:
	enum TYPE
	{
		ATTACK,
		DEFENSE,
		WILL
	};
	TYPE type;
	FightTaskBuff(const SkillItem* _skillItem,TYPE _type):type(_type),mSkillItem(_skillItem){}
	virtual void onStart();

	virtual void onEnd();
	virtual float getDuration(){return FightConfigManager::Get()->getSkillTime(FightConfigManager::BUFF,FightConfigManager::DURATION);/*1.0f;*/}
	virtual float getNextDelay(){return FightConfigManager::Get()->getSkillTime(FightConfigManager::BUFF,FightConfigManager::NEXT_DELAY);/*0.75f;*/}
};

class FightTaskAttack : public FightTask
{
public:
	virtual void onStart();
	virtual float getDuration(){return FightConfigManager::Get()->getSkillTime(FightConfigManager::ATTACK,FightConfigManager::DURATION);/*1.0f;*/}
	virtual float getNextDelay(){return FightConfigManager::Get()->getSkillTime(FightConfigManager::ATTACK,FightConfigManager::NEXT_DELAY);/*0.75f;*/}
};


class FightTaskDefense : public FightTask
{
private:
	int mDamage;
public:
	FightTaskDefense(int damage)
		:mDamage(damage){};
	virtual void onStart();
	virtual void onEnd();
	virtual float getDuration(){return FightConfigManager::Get()->getSkillTime(FightConfigManager::DEFENSE,FightConfigManager::DURATION);/*0.5f;*/}
	virtual float getNextDelay(){return FightConfigManager::Get()->getSkillTime(FightConfigManager::DEFENSE,FightConfigManager::NEXT_DELAY);/*0.75f;*/}
};

class FightTaskRoundStart : public FightTask
{
private:
	int mRound;
	CCBContainer* mPage;
	bool mLoadMessageSent;
public:
	FightTaskRoundStart(int round, CCBContainer* page);
	virtual void onStart();
	virtual void onEnd();
	virtual void onTick(float dt);
	virtual float getDuration(){return FightConfigManager::Get()->getSkillTime(FightConfigManager::ROUND_START,FightConfigManager::DURATION);/*1.0f;*/}
	virtual float getNextDelay(){return FightConfigManager::Get()->getSkillTime(FightConfigManager::ROUND_START,FightConfigManager::NEXT_DELAY);/*1.0f;*/}
};

class FightTaskRoundOver : public FightTask
{
private:
	int mRound;
	CCBContainer* mPage;
public:
	FightTaskRoundOver(int round, CCBContainer* page);
	virtual void onStart();
	virtual void onEnd();
	virtual float getDuration(){return FightConfigManager::Get()->getSkillTime(FightConfigManager::ROUND_OVER,FightConfigManager::DURATION);/*1.0f;*/}
	virtual float getNextDelay(){return FightConfigManager::Get()->getSkillTime(FightConfigManager::ROUND_OVER,FightConfigManager::NEXT_DELAY);/*1.25f;*/}
};


class FightTaskShake : public FightTask
{
public:
	enum TYPE
	{
		TYPE_1,
		TYPE_2
	};
	FightTaskShake(TYPE type, CCBContainer* page){mType = type;mPage = page;}
	virtual void onStart();
	virtual float getDuration(){return FightConfigManager::Get()->getSkillTime(FightConfigManager::SHAKE,FightConfigManager::DURATION);/*0;*/}
	virtual float getNextDelay(){return FightConfigManager::Get()->getSkillTime(FightConfigManager::SHAKE,FightConfigManager::NEXT_DELAY);/*0;*/}

private:
	TYPE mType;
	CCBContainer* mPage;
};


class FightTaskWait : public FightTask
{
	float mWaitTime;
public:
	FightTaskWait(float starttime,float waittime):mWaitTime(waittime)
    {
        mStartTime=starttime;
        mStarted = false;
        mDurationTime = getDuration();
    }
	virtual float getDuration(){return mWaitTime;/*0;*/}
	virtual float getNextDelay(){return mWaitTime;/*0;*/}
};
// 
// class FightTaskAttack : public FightTask
// {
// public:
// 	virtual bool update(float dt)
// 	{
// 		if(mSlot.battle == 0 || mSlot.skill == 0)
// 			return true;
// 
// 		if(mStartTime>0)
// 		{
// 			mStartTime-=dt;
// 			return false;
// 		}
// 		mSlot.skill->setVisible(false);
// 		if(mDirection == OPUserBattleRet_Action_Direction_DIR_LEFT_TO_RIGHT)
// 		{
// 			mSlot.battle->runAnimation("LeftAttack");
// 			mSlot.skill->setPositionX(0);
// 			mSlot.skill->setScaleX(1.0f);
// 		}
// 		else
// 		{
// 			mSlot.battle->runAnimation("RightAttack");
// 			mSlot.skill->setPositionX(mSlot.skill->getContentSize().width);
// 			mSlot.skill->setScaleX(-1.0f);
// 		}
// 		return true;
// 	}
// };
// 
// class FightTaskSkillStaring : public FightTask
// {
// public:
// 	virtual bool update(float dt)
// 	{
// 		if(mStartTime>0)
// 		{
// 			mStartTime-=dt;
// 			return false;
// 		}
// 		mSlot.skill->setVisible(true);
// 		if(mDirection == OPUserBattleRet_Action_Direction_DIR_LEFT_TO_RIGHT)
// 		{
// 			mSlot.skill->setPositionX(0);
// 			mSlot.skill->setScaleX(1.0f);
// 		}
// 		else
// 		{
// 			mSlot.skill->setPositionX(mSlot.skill->getContentSize().width);
// 			mSlot.skill->setScaleX(-1.0f);
// 		}
// 		mSlot.skill->runAnimation("EyeStaring");
// 		return true;
// 	}
// };
// 
// 
// class FightTaskSkill : public FightTask
// {
// public:
// 	virtual bool update(float dt)
// 	{
// 		if(mStartTime>0)
// 		{
// 			mStartTime-=dt;
// 			return false;
// 		}
// 		mSlot.skill->setVisible(true);
// 		if(mDirection == OPUserBattleRet_Action_Direction_DIR_LEFT_TO_RIGHT)
// 		{
// 			mSlot.battle->runAnimation("LeftAttack");
// 			mSlot.skill->setPositionX(0);
// 			mSlot.skill->setScaleX(1.0f);
// 		}
// 		else
// 		{
// 			mSlot.battle->runAnimation("RightAttack");
// 			mSlot.skill->setPositionX(mSlot.skill->getContentSize().width);
// 			mSlot.skill->setScaleX(-1.0f);
// 		}
// 		mSlot.skill->runAnimation("Skill1");
// 		return true;
// 	}
// };
// 
// class FightTaskSkillBig : public FightTask
// {
// private:
// 	cocos2d::CCNode* mNode;
// public:
// 	FightTaskSkillBig(cocos2d::CCNode* node){mNode = node;}
// 	virtual bool update(float dt)
// 	{
// 		if(mStartTime>0)
// 		{
// 			mStartTime-=dt;
// 			return false;
// 		}
// 		CCBContainer* ccb = CCBContainer::create();
// 		ccb->loadCcbiFile("s_skillb.ccbi");
// 		mNode->addChild(ccb);
// 		return true;
// 	}
// };
// class FightTaskSkillBigHide : public FightTask
// {
// private:
// 	cocos2d::CCNode* mNode;
// public:
// 	FightTaskSkillBigHide(cocos2d::CCNode* node){mNode = node;}
// 	virtual bool update(float dt)
// 	{
// 		if(mStartTime>0)
// 		{
// 			mStartTime-=dt;
// 			return false;
// 		}
// 		mNode->removeAllChildren();
// 		return true;
// 	}
// };
