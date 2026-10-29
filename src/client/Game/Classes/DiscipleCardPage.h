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

class DiscipleCardPage
	: public ContentBase
{
public:
	DiscipleCardPage(unsigned int id,bool isItem=false,bool isViewTeam=false)
		:ContentBase(id), isItemID(isItem),isSoul(false),mIsViewTeam(isViewTeam){};
	virtual ~DiscipleCardPage();

	virtual void refreshContent(void);

	virtual void onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag );
	void setItemID(bool b){ isItemID = b;}
	void setIsSoul(bool b){ isSoul = b;}
	void setIsViewTeam(bool b){ mIsViewTeam = b;}
	//--begin xinzheng 2013-6-4
	void setContentIDWithRemoveTextureCache(unsigned int id);
	virtual void setContentID(unsigned int id);
	//--end

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "MemberCard.ccbi";
		return ccbiFileName;
	}
private:
	bool isItemID;
	bool mIsViewTeam;
	bool isSoul;
};
