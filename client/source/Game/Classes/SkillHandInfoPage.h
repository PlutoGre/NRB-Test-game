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

class SkillCardPage;

class SkillHandInfoPage
	: public CustomPage
{
public:
	SkillHandInfoPage(void);
	~SkillHandInfoPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(SkillHandInfoPage);
	
	virtual void load(void);
	virtual void unload(void);

	void refreshPage(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	void buildCard(void);
	void showPage(unsigned int id,bool isItem);
	static void showSkillPage(unsigned int id,bool isItem);
private:
	SkillCardPage * mCard;

	CCNode* mSkillCard;
	bool mIsItem;
	unsigned int mId;
};
#define ShowSkillInfoPopup(id,isItem) dynamic_cast<SkillHandInfoPage*>(CCBManager::Get()->getPage("SkillHandInfoPage"))->showPage(id,isItem);