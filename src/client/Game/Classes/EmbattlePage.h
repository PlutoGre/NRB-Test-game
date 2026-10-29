#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CustomPage.h"
#include "ContentBase.h"

class EmbattlePage;

class EmbattleContent
	: public ContentBase
{
public:
	EmbattleContent(unsigned int id, EmbattlePage* ownner)
		:ContentBase(id)
		, mOwnner(ownner)
	{};
	virtual ~EmbattleContent(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	void setSellect(bool sellect, bool change);

	void setBattleID(unsigned int battleid){mBattleID = battleid;};
	unsigned int getBattleID(void){return mBattleID;};
protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "EmbattleContent.ccbi";
		return ccbiFileName;
	}

private:
	cocos2d::CCMenuItemImage* mSelect;
	cocos2d::CCLabelBMFont* mButtonLabel;
	cocos2d::CCMenuItemImage* mChangeButton;
	EmbattlePage* mOwnner;
	unsigned int mBattleID;
};

class EmbattlePage
	: public CustomPage
{
public:
	EmbattlePage(void);
	~EmbattlePage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(EmbattlePage);
	
	virtual void load(void);
	virtual void unload(void);

	void buildItem(void);
	void addItem(ContentBase* item);
	void rebuildAllItem(void);
	void clearAllItem(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	void setSellectedContent(EmbattleContent* member);

	void swapBattle(EmbattleContent* member);
private:

	typedef std::vector<ContentBase * > ContentList;

	ContentList mContentList;

	CCNode* mContentNode;

	EmbattleContent* mCurSellectedContent;
};