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

class SkillViewTeamInfoPage
	: public CustomPage
{
public:
	SkillViewTeamInfoPage(void);
	~SkillViewTeamInfoPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(SkillViewTeamInfoPage);
	
	virtual void load(void);
	virtual void unload(void);

	void refreshPage(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	void buildCard(void);
	void showPage(unsigned int id);
	static void showSkillPage(unsigned int id);
private:
	SkillCardPage * mCard;

	CCNode* mSkillCard;
	unsigned int mId;
};
#define ShowSkillViewInfoPopup(id) dynamic_cast<SkillViewTeamInfoPage*>(CCBManager::Get()->getPage("SkillViewTeamInfoPage"))->showPage(id);