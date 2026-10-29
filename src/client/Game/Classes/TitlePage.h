#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ContentBase.h"
#include "CustomPage.h"

class TitleContent
	: public ContentBase
{
public:
	TitleContent(unsigned int id):ContentBase(id){};
	virtual ~TitleContent(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "TitleContent.ccbi";
		return ccbiFileName;
	}
};

class TitlePage
	: public CustomPage
{
public:
	TitlePage(void);
	~TitlePage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(TitlePage);
	
	virtual void load(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender){};

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