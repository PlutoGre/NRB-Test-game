#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "GameMessages.h"

class ArenaRewardAniPage
	: public CustomPage
	, public CCBContainer::CCBContainerListener
{
public:
	ArenaRewardAniPage(void);
	~ArenaRewardAniPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(ArenaRewardAniPage);

	virtual void load(void);

	void refreshPage(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender){};

	virtual void onAnimationDone(const std::string& animationName);;
};
