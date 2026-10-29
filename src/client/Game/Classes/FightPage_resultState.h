#pragma once
#include "StateMachine.h"
#include "FightPage.h"

class FightPage_resultState 
	: public State<FightPage>
	, public CCBContainer::CCBContainerListener
{
private:
	FightPage * mPage;
	CCBContainer* mResult;

	enum{MAX_CHARACTERS = 8};
	CCBContainer* mCharacters[MAX_CHARACTERS];

	void common(const OPUserBattleRet* msg,bool isWin);
	void win(const OPUserBattleRet* msg);
	void lose(const OPUserBattleRet* msg);

	typedef std::vector<unsigned int> LevelUpList;
	LevelUpList mLevelList;
	float durationTime;
	bool mShowExpBarAni;
	void _showLevelupAni();
	void _showExpBarAni();
	float _getScaleByDiscipleId(int id,bool original=false,bool isUp=false,int gainExp=0);
public:
	FightPage_resultState(void);
	~FightPage_resultState(void);

	//this will execute when the state is entered
	virtual void Enter(FightPage*);

	//this is the states normal update function
	virtual void Execute(FightPage*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(FightPage*);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	

};

