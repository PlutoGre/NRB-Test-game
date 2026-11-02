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

class TitleCardPage;

class TitleInfoPage
	: public CustomPage
{
public:
	TitleInfoPage(void);
	~TitleInfoPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(TitleInfoPage);
	
	virtual void load(void);
	virtual void unload(void);

	void refreshPage(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	void buildCard(void);

	void setIsTeamView(bool isTeamView){mIsTeamView=isTeamView;};

private:
	bool mIsTeamView;
	TitleCardPage * mCard;

	CCNode* mTitleCard;
};