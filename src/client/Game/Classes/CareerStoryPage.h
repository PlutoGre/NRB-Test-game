#pragma once
#include "StateMachine.h"
#include "MainFrame.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "CareerData.h"


class CareerStoryPage: public CustomPage
	, public State<MainFrame>
{
private:
	int mChapterID;
	CareerData::ChaptersData::DIALOG_LIST::iterator mCurrentStep;
	void updateContent();
public:
	CareerStoryPage(void);
	~CareerStoryPage(void);

	virtual PAGE_TYPE getPageType() {return PT_COVER;}

	CREATE_FUNC(CareerStoryPage);
	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);

	virtual void load(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	void setChapter(int id){mChapterID = id;}
};

