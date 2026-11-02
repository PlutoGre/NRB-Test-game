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

class EquipCardPage
	: public ContentBase
{
public:
	EquipCardPage(unsigned int id,bool isItem=false,bool isViewTeam=false):ContentBase(id){ mIsItem=isItem;mIsViewTeam = isViewTeam;};

	virtual ~EquipCardPage(){};

	void setCardItem(bool isItem){mIsItem=isItem; };
	void setIsViewItem(bool isViewItem){mIsViewTeam=isViewItem;};
	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "EquipCard.ccbi";
		return ccbiFileName;
	}
	bool mIsItem;
	bool mIsViewTeam;
};
