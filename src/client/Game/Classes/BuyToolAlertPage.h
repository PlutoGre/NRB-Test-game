#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "GameMaths.h"
#include "GameMessages.h"
#include "ContentBase.h"


typedef void (*onSure)();

class BuyToolAlertPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
{
public:
	BuyToolAlertPage(void);
	~BuyToolAlertPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(BuyToolAlertPage);
	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);

	virtual void load(void);

	void refreshPage(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);;

	void setTitle(std::string title){
		titleStr = std::string(title.c_str());
	}
	void setName(std::string name){
		nameStr = name;
	}
	void setDescription(std::string description){
		descriptionStr = description;
	}
	void setNumber1(std::string number1){
		number1Str = number1;
	}
	void setSure(std::string sure){
		sureStr = sure;
	}
	void setPic(std::string pic){
		picStr = pic;
	}


	void setSureFun(onSure sure){
		mSure = sure;
	}

private:

	std::string titleStr;
	std::string nameStr;
	std::string descriptionStr;
	std::string number1Str;
	std::string sureStr;
	std::string picStr;


	onSure mSure;


};
