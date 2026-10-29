#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "CharacterConsts.h"
#include "GameMessages.h"
#include "GameMaths.h"
#include "GamePackets.h"
#include "DataTableManager.h"
#include "ServerDateManager.h"
#include "ContentBase.h"
#include "StringConverter.h"
#define TabLength 4
class RankContent
	: public ContentBase
{
public:
	RankContent(unsigned int index,unsigned int id):ContentBase(id){this->mIndex=index;};
	virtual ~RankContent(){};
	virtual void refreshContent(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "FightEndRankContent.ccbi";
		return ccbiFileName;
	}
	unsigned int mIndex;
	unsigned int mPlayerId;
};
class FightEndRankPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
	, public MessageHandler
{
public:
	FightEndRankPage(void);
	~FightEndRankPage(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}
	CREATE_FUNC(FightEndRankPage);
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	virtual void onReceiveMassage(const GameMessage * message);
	void buildItem(void);
	void addItem(ContentBase* item);
	void rebuildAllItem(void);
	void clearAllItem(void);
	void refreshAllItem(void);
	void removeItem(ContentBase* item);
	void removeAllTabItem();
	void buildTabItem();
	CCMenuItemSprite* getTabBtnByIndex(unsigned int index);
private:
	void loadRankData();
	typedef std::map<unsigned int,cocos2d::CCMenuItemSprite* > TAB_BUTTON_LIST;
	TAB_BUTTON_LIST mTabList;
	typedef std::vector<ContentBase * > ContentList;
	ContentList mContentList;
	typedef std::map<unsigned int,bool> DataInitStatMap;
	DataInitStatMap mInitStat;
	CCNode* membersRootNode;
	cocos2d::extension::CCScrollView* members;
	unsigned int mCurrTabIndex;
	unsigned int mCurrDay;
	static int sortRank(unsigned int a,unsigned int b);
};
