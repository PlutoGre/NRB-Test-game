#pragma once
#include "cocos2d.h"
#include "cocos-ext.h"
#include "MainFrame.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "GameMessages.h"
#include "GamePackets.h"

class FightGuidePage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
	, public MessageHandler
{
public:
	FightGuidePage(void);
	~FightGuidePage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(FightGuidePage);
	virtual void Enter(MainFrame*);

	virtual void Execute(MainFrame*);

	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	virtual void onReceiveMassage(const GameMessage * message);
	void inputMethodCallBack(const std::string& content);
	void showPage(std::string _type);
private:	
	std::string mType;
};

#define POP_FIGHT_GUIDE(_type) dynamic_cast<FightGuidePage*>(CCBManager::Get()->getPage("FightGuidePage"))->showPage(_type);