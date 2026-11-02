#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "GameMessages.h"

class DiscipleCardPage;

class DiscipleRecruitPage
	:  public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
{
public:
	DiscipleRecruitPage(void);
	~DiscipleRecruitPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(DiscipleRecruitPage);

	virtual void load(void);

	void refreshPage(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){};

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);


	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);

	void setID(int id){mID=id;}
	void setIsSoul(bool soul, int count = 0){mIsSoul = soul;mSoulCount = count;}
	void setAdditionalSoul(bool has, unsigned int soulIDs[], int id = 0, int count_x = 0, int count_y = 0);
	void setAdditionalSoul(bool has) {mAdditionalSoulHas = has;;};
private:
	bool mIsSoul;
	int mSoulCount;
	bool mAdditionalSoulHas;
	int mAdditiionalSoulID;
	int mAdditionalSoulCount_x;
	int mAdditionalSoulCount_y;
	int mID;
	int mItemID;
	unsigned int mSoulIDs[6];

	int mTargetCount;
	float mActionTime;
	float mNextSelTime;
	int mSelCount;
	enum{
		ANIMATION_CHILD = 100,
	};
	enum STAT
	{
		STAT_PREPARE,
		STAT_ANIMATION,
		STAT_GENERAL,
		STAT_SELECTING_SOUL,
		STAT_GETSOUL,
	};
	STAT mStat;

	DiscipleCardPage* mCard;
};
