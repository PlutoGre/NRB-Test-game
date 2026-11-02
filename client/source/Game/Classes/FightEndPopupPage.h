#pragma once
#include "cocos2d.h"
#include "cocos-ext.h"
#include "MainFrame.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "GameMessages.h"
#include "GamePackets.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "GameMaths.h"

class FightEndPopupPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
{
public:
	FightEndPopupPage(void);
	~FightEndPopupPage(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}
	CREATE_FUNC(FightEndPopupPage);
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	void showPage();
private:
	void initPage();
	CCLabelBMFont* mNumber;
	CCLabelBMFont* mTitle;
	CCLabelBMFont* mSummary;
	CCLabelBMFont* mNote1;
};
#define  ShowFightEndPopupPage() dynamic_cast<FightEndPopupPage*>(CCBManager::Get()->getPage("FightEndPopupPage"))->showPage();
