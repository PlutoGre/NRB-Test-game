#pragma once
#include "FightPage.h"

class FightPage_finalRound: public State<FightPage>
{
public:
	FightPage_finalRound(void);
	~FightPage_finalRound(void);

	//this will execute when the state is entered
	virtual void Enter(FightPage*);

	//this is the states normal update function
	virtual void Execute(FightPage*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(FightPage*);

private:
	void _createSlot(FightPage*);
	void _title_start(FightPage* page);
	void _enter();
	void _rushToCenter(FightPage* page);
	void _fight_start(FightPage * page);
	bool _fight(FightPage* page);
	void _show_final(FightPage* page);


	void _effect_shake();
	enum STATUS
	{
		PREPARE,
		TITLING,
		ENTERING,
		RUSHING_TO_CENTER,
		FIGHTING,
		SHOWING_FINAL,
	} mStatus;;

	float mTotalTime;
	std::map<int, CCBContainer*> mBattleCharacter;
	std::map<int, CCBContainer*> mFaces;

	typedef std::list<std::pair<float,int> > POWERLIST;
	POWERLIST mSelfLifes;
	POWERLIST mSelfPowers;
	POWERLIST mEnemyPowers;
	POWERLIST mEnemyLifes;

	CCBContainer * mBattleEffect;
	float mBloodScale;
	float mWillScale;
	std::list<int> mPreEnter;
	float mEnterTime;
	enum
	{
		CURRENT = 0,
		MAX = 1,
		COUNT = 2
	};
	float mSelfTotalPower[COUNT];
	float mSelfTotalLife[COUNT];
	float mEnemyTotalPower[COUNT];
	float mEnemyTotalLife[COUNT];

	float mChangeStep;
};

