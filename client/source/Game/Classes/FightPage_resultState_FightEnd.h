#pragma once
#include "StateMachine.h"
#include "FightPage.h"

class FightPage_resultState_FightEnd 
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
public:
	FightPage_resultState_FightEnd(void);
	~FightPage_resultState_FightEnd(void);

	virtual void Enter(FightPage*);
	virtual void Execute(FightPage*);
	virtual void Exit(FightPage*);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

};

