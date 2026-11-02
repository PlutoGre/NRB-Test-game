#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "GameMaths.h"
#include "GameMessages.h"
#include "ContentBase.h"


class StoneUpgradePage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
{
public:
	StoneUpgradePage(void);
	~StoneUpgradePage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(StoneUpgradePage);
	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);

	virtual void load(void);

	void refreshPage(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);;

	void buildItem(void);
	void addItem(ContentBase* item);
	void rebuildAllItem(void);
	void clearAllItem(void);
	void refreshAllItem(void);
	void removeItem(ContentBase* item);
	//void refreshItem(EquipItem* item);

	void buildEquipItem();

	void choosePage(int tag);

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

	unsigned int  mCurrTabIndex;

	typedef std::map<unsigned int, cocos2d::CCMenuItemImage*> TabMap;
	TabMap mTabMap;
	float mLongBtnHeight;
	int tag;
	cocos2d::CCMenuItemImage* mTabInlay;
	cocos2d::CCMenuItemImage* mTabUpgrade;
	
	const UserToolInfo* userToolInfo;
	const ToolItem* toolItem;

	int mCount;
	int mID;
	int mTotalPrice;
	bool mBuyOneOnly;
	int mCanBuyMaxCount;
	int mEnoughCount;
	void caculatePrice();
	bool checkGoldEnough();
	void caculateEnoughCount();

};
