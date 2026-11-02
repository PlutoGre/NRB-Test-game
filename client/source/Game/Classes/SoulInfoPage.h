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

class DiscipleCardPage;

class SoulInfoPage
	: public CustomPage
	, public CCBContainer::CCBContainerListener
{
public:
	SoulInfoPage(void);
	~SoulInfoPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(SoulInfoPage);
	
	virtual void load(void);
	virtual void unload(void);

	void refreshPage(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){};

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	void buildCard(void);
private:
	DiscipleCardPage * mCard;

	CCNode* mDiscipleCard;
	cocos2d::extension::CCScrollView* mContent;

};
