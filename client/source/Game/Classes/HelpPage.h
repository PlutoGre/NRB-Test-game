#pragma once
#include "cocos2d.h"
#include "cocos-ext.h"
#include "MainFrame.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "DataTableManager.h"

class HelpPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener{
public:
	HelpPage(void);
	~HelpPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(HelpPage);
	void Enter(MainFrame*);
	void initContent();
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

private:	
};
