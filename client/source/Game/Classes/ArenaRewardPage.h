#pragma once
#include "CustomPage.h"

class ArenaRewardPage
	: public CustomPage
{
public:
	ArenaRewardPage(void){}
	~ArenaRewardPage(void){};

	virtual PAGE_TYPE getPageType() {return PT_CONTENT_MESSAGE;}
	CREATE_FUNC(ArenaRewardPage);
	void refreshPage();
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
};

