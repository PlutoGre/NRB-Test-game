#pragma once
#include "FightTasks.h"


class FightTaskSkill : public FightTask
{
protected:
	int mSkillID;
	const SkillItem* mSkill;
	float mLastFinishTime;
	float mNextDelay;
	float mDuration;
	int mPositionID;
	int mSoundEffectHandle;

	struct SkillRep
	{
		SkillRep():variable(0),filename(""){};
		CCSprite* variable;
		std::string filename;
	};

	std::list<SkillRep> mReplaceVaiables;
	void initReplace(CCBContainer* tobeReplace);
	void replaceNode();
public:
	FightTaskSkill(int skillID, float finishTime, int positionID);
	virtual void setAttribute(	FightPage_roundState::SLOTMEMBER slot,float startTime, int*life);

	virtual void onStart();
	virtual void onEnd();
	virtual float getDuration(){return mDuration;}
	virtual float getNextDelay(){return mNextDelay;}
};

class FightTaskSkillSmall : public FightTaskSkill
{
private:
	typedef std::map<int,FightPage_roundState::SLOTMEMBER> ENEMY_MAP;
	typedef std::map<int,unsigned int > DAMAGE_MAP;
	DAMAGE_MAP mDamage;
	ENEMY_MAP mEnemys;
	CCBContainer* mShadow;
	bool mDamaged;
	bool mFlashed;
protected:
	virtual float getDamageTime();
public:
	FightTaskSkillSmall(int skillID, float finishTime, int positionID):FightTaskSkill(skillID,finishTime,positionID){}
	virtual void setAttribute(	FightPage_roundState::SLOTMEMBER slot,float startTime, int*life);
	virtual void onStart();
	virtual void onEnd();
	virtual void onDamaged();
	void addEnemy(FightPage_roundState::SLOTMEMBER slot,unsigned int _damage);
	virtual void onTick(float dt);
};

class FightTaskSkillBig : public FightTaskSkill
{
protected:
	CCBContainer* mSkillNode;
	virtual float getPreparedEndTime(){return 0;}
	virtual float getDamageTime(){return 0;}
	bool mPreparedEnd;
	bool mDamaged;
	const DiscipleItem* mAttackDiscipleItem;
public:
	FightTaskSkillBig(int skillID, float finishTime, int positionID,const DiscipleItem* _attackDiscipleItem):mSkillNode(0),FightTaskSkill(skillID,finishTime,positionID),mAttackDiscipleItem(_attackDiscipleItem){}
	~FightTaskSkillBig();
	virtual void setAttribute(	FightPage_roundState::SLOTMEMBER slot,float startTime, int*life);
	virtual void onLoad();
	virtual void onStart();
	virtual void onEnd();
	virtual void onPrepareEnd(){};
	virtual void onDamaged(){};
	virtual void onTick(float dt);
};

class FightTaskSkillBigSingle : public FightTaskSkillBig
{
private:
	FightPage_roundState::SLOTMEMBER mEnemy;
	unsigned int mDamage;
	int* mLife;
protected:
	virtual float getPreparedEndTime();
	virtual float getDamageTime();
public:
	FightTaskSkillBigSingle(int skillID, float finishTime, int positionID,const DiscipleItem* _attackDiscipleItem):FightTaskSkillBig(skillID,finishTime,positionID,_attackDiscipleItem){}
	void setEnemy(FightPage_roundState::SLOTMEMBER slot,unsigned int _damage, int*life);
	virtual void onStart();
	virtual void onPrepareEnd();
	virtual void onDamaged();
};


class FightTaskSkillBigMultiple : public FightTaskSkillBig
{
private:
	typedef std::map<int,FightPage_roundState::SLOTMEMBER> ENEMY_MAP;
	typedef std::map<int,unsigned int > DAMAGE_MAP;
	typedef std::map<int,int* > LIFE_MAP;
	DAMAGE_MAP mDamage;
	ENEMY_MAP mEnemys;
	LIFE_MAP mLifes;
protected:
	virtual float getPreparedEndTime();
	virtual float getDamageTime();
public:
	FightTaskSkillBigMultiple(int skillID, float finishTime, int positionID,const DiscipleItem* _attackDiscipleItem):FightTaskSkillBig(skillID,finishTime,positionID,_attackDiscipleItem){;}
	void addEnemy(FightPage_roundState::SLOTMEMBER slot,unsigned int _damage, int* life);
	virtual void onStart();
	virtual void onPrepareEnd();
	virtual void onDamaged();
};