#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "MainFrame.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "ContentBase.h"
#include "MessageManager.h"
#include "StateMachine.h"
#define ArenaSyncTime 10800
class ArenaOpponentContent
	: public ContentBase
{
public:
	ArenaOpponentContent(unsigned int id):ContentBase(id){};
	virtual ~ArenaOpponentContent(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "ArenaContent.ccbi";
		return ccbiFileName;
	}
private:
	bool mCanViewTeam;
};

class ArenaUserContent
	: public ContentBase
{
public:
	ArenaUserContent(unsigned int id):ContentBase(id){};
	virtual ~ArenaUserContent(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "ArenaContentUser.ccbi";
		return ccbiFileName;
	}
};

class ArenaRewardContent
	: public ContentBase
{
public:
	ArenaRewardContent(unsigned int id):ContentBase(id){};
	virtual ~ArenaRewardContent(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "ArenaRewardContent.ccbi";
		return ccbiFileName;
	}

};

class ArenaPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
	, public MessageHandler
{
public:
	ArenaPage(void);
	~ArenaPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(ArenaPage);
	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);

	virtual void load(void);

	void refreshPage(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){};

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);;

	void buildItem(void);
	void addItem(ContentBase* item, bool needSetOffSet = true);
	void rebuildAllItem(void);
	void clearAllItem(void);
	void refreshAllItem(void);
	void removeItem(ContentBase* item);
	//void refreshItem(ArenaItem* item);

	void selectTab(unsigned int index);

	virtual void onReceiveMassage(const GameMessage * message);
private:

	typedef std::vector<ContentBase * > ContentList;

	ContentList mContentList;

	CCNode* membersRootNode;

	cocos2d::extension::CCScrollView* members;

	unsigned int mCurrTabIndex;

	unsigned int mFrameCount;
	unsigned int mLoadDataStep;
	unsigned int mNextSyncTime;

	float mExpbarInitScale;
	bool _isFirstLoad;
	bool judgeLoseDate();
	void _calcNextSyncTime(unsigned int lastSyncTime);
	void _refreshSyncTime();
	void _refreshTitle();
};
