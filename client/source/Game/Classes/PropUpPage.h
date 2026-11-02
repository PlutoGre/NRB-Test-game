#pragma once
#include "MainFrame.h"
#include "CCBManager.h"
#include "CustomPage.h"

class PropUpPage: public CustomPage
	, public State<MainFrame>
{
public:
	enum USAGE
	{
		FOR_SHOP_BUYTOOL,
	};
private:
	USAGE mUsage;
public:
	PropUpPage(void);
	~PropUpPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT_MESSAGE;}

	CREATE_FUNC(PropUpPage);
	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);
	virtual void load(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	
	void setUsage(USAGE usage){mUsage = usage;}

};

