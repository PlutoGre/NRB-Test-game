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

class SkillCardPage
	: public ContentBase
{
public:
	SkillCardPage(unsigned int id):ContentBase(id), mIsItemID(false),mViewTeam(false){};
	virtual ~SkillCardPage(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	void setItemID(bool b){ mIsItemID = b;}
	void setViewTeam(bool b) {mViewTeam=b; }
protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "SkillCard.ccbi";
		return ccbiFileName;
	}
	bool mIsItemID;
	bool mViewTeam;
};
