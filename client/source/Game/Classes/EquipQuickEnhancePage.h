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


class EquipQuickEnhancePage
	: public CustomPage
{
public:
	EquipQuickEnhancePage(void);
	~EquipQuickEnhancePage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(EquipQuickEnhancePage);
	
	virtual void load(void);
	virtual void unload(void);

	void refreshPage(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	void buildCard(void);

private:
	EquipQuickEnhancePage * mCard;

	CCNode* mEquipCard;

};
