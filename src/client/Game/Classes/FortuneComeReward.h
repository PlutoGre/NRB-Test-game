#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "CharacterConsts.h"
#include "GameMessages.h"
#include "GameMaths.h"
#include "GamePackets.h"
#include "DataTableManager.h"
#include "ServerDateManager.h"
#include "ContentBase.h"
#include "StringConverter.h"

class FortuneComeReward
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
{
public:
	FortuneComeReward(void);
	~FortuneComeReward(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}
	CREATE_FUNC(FortuneComeReward);
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
private:
	CCLabelBMFont* mPopMsg;
	CCLabelBMFont* mMessage;

};
