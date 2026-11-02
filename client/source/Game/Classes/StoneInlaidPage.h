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

class StoneInlaidPage;
class StoneEquipContentItem
	: public ContentBase
{
public:
	StoneEquipContentItem(unsigned int id,StoneInlaidPage* ownner):ContentBase(id),mOwnner(ownner){};
	virtual ~StoneEquipContentItem(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	void inlaidFunction();
protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "StoneSystemInlayContentItem.ccbi";
		return ccbiFileName;
	}
private:
	StoneInlaidPage* mOwnner;

};
class StoneInlaidContentItem
	: public ContentBase
{
public:
	StoneInlaidContentItem(unsigned int id,StoneInlaidPage* ownner):ContentBase(id),mOwnner(ownner),carriedByDiscipleName(""){};
	virtual ~StoneInlaidContentItem(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	void setSellect(bool sellect);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "StoneSystemStoneContent.ccbi";
		return ccbiFileName;
	}

private:
	cocos2d::CCMenuItemImage* mSelect;
	StoneInlaidPage* mOwnner;
	std::string carriedByDiscipleName;
	unsigned int itemId;

};

class StoneUpgradeContentItem
	: public ContentBase
{
public:
	StoneUpgradeContentItem(unsigned int id,StoneInlaidPage* ownner):ContentBase(id),mOwnner(ownner){};
	virtual ~StoneUpgradeContentItem(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "StoneSystemUpGradeContent.ccbi";
		return ccbiFileName;
	}
private:
	StoneInlaidPage * mOwnner;
	const ToolItem * toolItem;
	unsigned int itemId;

};
class StoneEquipContent
	: public ContentBase
{
public:
	StoneEquipContent(unsigned int id,StoneInlaidPage* ownner):ContentBase(id),mOwnner(ownner){};
	virtual ~StoneEquipContent(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	void buildItem(void);
	void addItem(ContentBase* item);
	void rebuildAllItem(void);
	void clearAllItem(void);
	void refreshAllItem(void);
	void removeItem(ContentBase* item);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "StoneSystemInlayContent.ccbi";
		return ccbiFileName;
	}
private:
	CCNode * SSICItem;
	typedef std::vector<ContentBase * > ContentList;
	ContentList mContentList;
	StoneInlaidPage * mOwnner;
};

class StoneInlaidPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
	, public MessageHandler
{
public:
	StoneInlaidPage(void);
	~StoneInlaidPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(StoneInlaidPage);
	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);

	virtual void load(void);
	static bool _sSort(unsigned int id1,unsigned int id2);

	void refreshPage(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	virtual void onReceiveMassage(const GameMessage * message);

	void buildItem(void);
	void addItem(ContentBase* item);
	void rebuildAllItem(void);
	void clearAllItem(void);
	void refreshAllItem(void);
	void removeItem(ContentBase* item);
	//void refreshItem(EquipItem* item);

	void buildEquipItem();

	void choosePage(int tag);

	void setSellectedContent( StoneInlaidContentItem* member );
	void reOrderAllItem();
	unsigned int itemId;
	unsigned int chooseNum;

private:
	void sellectTab(unsigned int tabIndex);

	typedef std::vector<ContentBase * > ContentList;

	ContentList mContentList;

	CCNode* membersRootNode;

	cocos2d::extension::CCScrollView* members;

	cocos2d::CCNode* mPageInlay;
	cocos2d::extension::CCScrollView* mInlayContent;

	cocos2d::CCNode* mPageSU;
	cocos2d::extension::CCScrollView* mStoneUpContent;
	cocos2d::CCNode* mStoneUpContentNode;

//	unsigned int  mCurrTabIndex;

	typedef std::map<unsigned int, cocos2d::CCMenuItemImage*> TabMap;
	TabMap mTabMap;
	float mLongBtnHeight;
	int tag;
	cocos2d::CCMenuItemImage* mTabInlay;
	cocos2d::CCMenuItemImage* mTabUpgrade;
	StoneInlaidContentItem* mCurSellectedContent;

	bool isDesOrder;
};
