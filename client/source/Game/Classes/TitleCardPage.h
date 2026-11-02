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

class TitleCardPage
	: public ContentBase
{
public:
	TitleCardPage(unsigned int id,bool isViewTeam=false):ContentBase(id){ mIsViewTeam=isViewTeam;};
	virtual ~TitleCardPage(){};
	void setIsViewTeam(bool isViewTeam){mIsViewTeam=isViewTeam;};
	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "TitleCard.ccbi";
		return ccbiFileName;
	}
	bool mIsViewTeam;
};