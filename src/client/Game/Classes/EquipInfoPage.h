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

class EquipStoneInfo
	: public ContentBase
{
public:
	EquipStoneInfo(unsigned int id,unsigned int equipId):ContentBase(id){
		this->equipId = equipId;
	};
	virtual ~EquipStoneInfo(){};

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

class EquipInfoPage
	: public CustomPage
{
public:
	EquipInfoPage(void);
	~EquipInfoPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(EquipInfoPage);
	
	virtual void load(void);
	virtual void unload(void);

	void refreshPage(void);

	virtual void rebuildAllItem(void);
	virtual void clearAllItem(void);
	virtual void buildItem();
	virtual void addItem(ContentBase* item);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	void buildCard(void);

private:
	EquipCardPage * mCard;

	cocos2d::CCNode* mInlayStone;

	CCNode* mEquipCard;
	typedef std::vector<ContentBase * > ContentList;
	ContentList mContentList;

};
