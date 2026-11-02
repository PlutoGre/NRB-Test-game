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
#include "ContentBase.h"
#include "EquipCardPage.h"

class EquipHandInfoPage
	: public CustomPage
{
public:
	EquipHandInfoPage(void);
	~EquipHandInfoPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(EquipHandInfoPage);
	
	virtual void load(void);
	virtual void unload(void);

	void refreshPage(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	void buildCard(void);

	void showPage(unsigned int id,bool isItem);

	static void showEquipPage(unsigned int id,bool isItem);

private:
	bool mIsItem;
	unsigned int mId;
	EquipCardPage * mCard;
	CCNode* mEquipCard;

};

#define ShowEquipInfoPopup(id,isItem) dynamic_cast<EquipHandInfoPage*>(CCBManager::Get()->getPage("EquipHandInfoPage"))->showPage(id,isItem);
