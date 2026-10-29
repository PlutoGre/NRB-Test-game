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

class TrainPage
	: public CustomPage
	, public State<MainFrame>
	, public MessageHandler
	, public CCBContainer::CCBContainerListener
{
	enum TrainType
	{
		T_1_Train = 1,
		T_1_TrainCarefully = 2,
		T_10_Train = 3,
		T_10_TrainCarefully = 4
	};
public:
	TrainPage(void);
	~TrainPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(TrainPage);
	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);

	virtual void load(void);

	void refreshPage(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){};

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	virtual void onReceiveMassage(const GameMessage * message);

private:
	 void selectType(TrainType type);
	 cocos2d::CCMenuItemImage* mCurSelectNode;
	 TrainType mTrainType; 
};
