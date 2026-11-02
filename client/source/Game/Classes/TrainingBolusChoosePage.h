#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "ContentBase.h"

class TrainingBolusItem
	: public ContentBase
{
public:
	TrainingBolusItem(unsigned int id):ContentBase(id){};
	virtual ~TrainingBolusItem(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "PropsContent.ccbi";
		return ccbiFileName;
	}
};

class TrainingBolusChoosePage
	: public CustomPage
	, public CCBContainer::CCBContainerListener
{
public:
	TrainingBolusChoosePage(void);
	~TrainingBolusChoosePage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(TrainingBolusChoosePage);

	virtual void load(void);
	virtual void unload(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){};

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);;

	void buildItem(void);
	void addItem(ContentBase* item);
	void rebuildAllItem(void);
	void clearAllItem(void);
private:
	typedef std::vector<ContentBase * > ContentList;

	ContentList mContentList;

	CCNode* membersRootNode;

	cocos2d::extension::CCScrollView* members;
};
