#pragma once
#include "StateMachine.h"
#include "CustomPage.h"
#include "MainFrame.h"
enum Activity_Pop_Type
{
	SendSoulTwo = 1,
	SendSoulThree = 2
};
class ActivityPopPage
	:public CustomPage
	,public State<MainFrame>
{
public:
	ActivityPopPage(void);
	~ActivityPopPage(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}
	CREATE_FUNC(ActivityPopPage);
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	void showPage(Activity_Pop_Type _type);
private:
	Activity_Pop_Type mType;
	void _refreshPage();
};
#define ActivityPop(type) dynamic_cast<ActivityPopPage*>(CCBManager::Get()->getPage("ActivityPopPage"))->showPage(type);
