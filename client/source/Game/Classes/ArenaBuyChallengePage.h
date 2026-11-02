#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"

#include "CustomPage.h"


class ArenaBuyChallengePage
	: public CustomPage
{
public:
	ArenaBuyChallengePage(void);
	~ArenaBuyChallengePage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(ArenaBuyChallengePage);

	virtual void load(void);

	void refreshPage(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
};
