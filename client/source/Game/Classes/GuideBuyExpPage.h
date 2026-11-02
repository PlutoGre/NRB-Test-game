#pragma once
#include "cocos2d.h"
#include "cocos-ext.h"
#include "MainFrame.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "GameMessages.h"
#include "GamePackets.h"

class GuideBuyExpPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
	, public MessageHandler
{
public:
	GuideBuyExpPage(void);
	~GuideBuyExpPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(GuideBuyExpPage);
	virtual void Enter(MainFrame*);

	virtual void Execute(MainFrame*);

	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	virtual void onReceiveMassage(const GameMessage * message);
	void inputMethodCallBack(const std::string& content);
	void showPage();
private:	
	std::string mType;
	int mCurrLevel;
};

#define POP_GUIDE_BuyExp() dynamic_cast<GuideBuyExpPage*>(CCBManager::Get()->getPage("GuideBuyExpPage"))->showPage();