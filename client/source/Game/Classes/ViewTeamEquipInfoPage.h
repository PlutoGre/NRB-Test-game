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

class EquipCardPage;

class ViewTeamEquipStoneInfo
	: public ContentBase
{
public:
	ViewTeamEquipStoneInfo(unsigned int id,unsigned int equipId):ContentBase(id){
		this->equipId = equipId;
	};
	virtual ~ViewTeamEquipStoneInfo(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "EquipmentInfoStoneContent.ccbi";
		return ccbiFileName;
	}
private:
	unsigned int equipId;
};

class ViewTeamEquipInfoPage
	: public CustomPage
{
public:
	ViewTeamEquipInfoPage(void);
	~ViewTeamEquipInfoPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(ViewTeamEquipInfoPage);
	
	virtual void load(void);
	virtual void unload(void);

	void refreshPage(void);

	virtual void rebuildAllItem(void);
	virtual void clearAllItem(void);
	virtual void buildItem();
	virtual void addItem(ContentBase* item);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	void buildCard(void);
	void showPage(unsigned int id);
private:
	unsigned int mId;
	EquipCardPage * mCard;

	cocos2d::CCNode* mInlayStone;

	CCNode* mEquipCard;
	typedef std::vector<ContentBase * > ContentList;
	ContentList mContentList;

};

#define ShowEquipViewInfoPopup(id) dynamic_cast<ViewTeamEquipInfoPage*>(CCBManager::Get()->getPage("ViewTeamEquipInfoPage"))->showPage(id);