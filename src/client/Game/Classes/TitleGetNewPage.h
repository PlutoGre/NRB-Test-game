#pragma once
#include "CustomPage.h"
#include "MainFrame.h"
#include "StateMachine.h"
class TitleGetNewPage
	: public CustomPage
	, public State<MainFrame>
{
public:
	TitleGetNewPage(void);
	~TitleGetNewPage(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(TitleGetNewPage);

	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
private: 
	void refreshPage();
};

