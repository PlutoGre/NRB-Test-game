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

class DiscipleHandInfoPage
	: public CustomPage
	, public CCBContainer::CCBContainerListener
{
public:
	DiscipleHandInfoPage(void);
	~DiscipleHandInfoPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(DiscipleHandInfoPage);
	
	virtual void load(void);
	virtual void unload(void);

	void refreshPage(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){};

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	void buildCard(void);
	void showPage(unsigned int id,bool isItem);
	static void showDisciplePage(unsigned int id,bool isItem);
private:
	DiscipleCardPage * mCard;
	CCNode* mDiscipleCard;
	cocos2d::extension::CCScrollView* mContent;
	bool mIsItem;
	unsigned int mId;
};
#define ShowDiscipleInfoPopup(id,isItem) dynamic_cast<DiscipleHandInfoPage*>(CCBManager::Get()->getPage("DiscipleHandInfoPage"))->showPage(id,isItem);