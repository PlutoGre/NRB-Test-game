#pragma once
#include "CustomPage.h"
#include "MainFrame.h"
#include "StateMachine.h"
class TitleLVUpPage
	: public CustomPage
	, public State<MainFrame>
{
public:
	TitleLVUpPage(void);
	~TitleLVUpPage(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(TitleLVUpPage);

	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
private: 
	void refreshPage();
};

