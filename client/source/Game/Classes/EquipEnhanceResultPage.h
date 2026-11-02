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

class EquipCardPage;

class EquipEnhanceResultPage
	: public CustomPage
	, public State<MainFrame>
	, public MessageHandler
{
public:
	EquipEnhanceResultPage(void);
	~EquipEnhanceResultPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(EquipEnhanceResultPage);
	
	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);

	virtual void load(void);

	void refreshPage(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	virtual void onReceiveMassage(const GameMessage * message);

	void buildCard(void);
private:
	EquipCardPage * mCard;

	CCNode* mEquipCard;
};
