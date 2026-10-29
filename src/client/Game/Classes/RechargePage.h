#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "MainFrame.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "DataTableManager.h"
#include "GameMessages.h"
#include "StringConverter.h"
#include "libPlatform.h"

class RechargePage
	: public CustomPage
	, public CCBContainer::CCBContainerListener
	, public State<MainFrame>
	, public MessageHandler
	, public platformListener
{
public:
	RechargePage(void);
	~RechargePage(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT_MESSAGE;}

	CREATE_FUNC(RechargePage);
	//this will execute when the state is entered
	void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);

	virtual void load(void);
	void initContent();

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	virtual void onReceiveMassage(const GameMessage * message);

	virtual void onBuyinfoSent(lib91*, bool success, const BUYINFO&, const std::string& log);
private:
	CCBContainer* readFirstPayInfo();
	bool mShowRechargeMessage;
};

