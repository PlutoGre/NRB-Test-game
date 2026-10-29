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
#include "GamePackets.h"
#include "libOS.h"
#include "StringConverter.h"
#include "ContentBase.h"
#include "SysMsgPage.h"
#define TABLENGTH 3

enum FRIENDS_TYPE
{
	Friend_FRIENDS=1,
	Friend_RANKING=2,
	Friend_GONGFU=3,
	Friend_Message=4,
	Friend_INVITES=5,
};
enum FRIENDS_TAB_TYPE
{
	Friend_TABFRIENDS=1,
	Friend_TABENEMY=2,
	Friend_TABMESSAGE=3,
};
enum FRIENDS_NODE_TYPE
{
	FRIDEDS_NODE=1,
	MAKEFRIENDS_NODE=2,
};

class FriendsContent
	: public ContentBase
{
public:
	FriendsContent(unsigned int id,unsigned int nodeType,unsigned int tabIndex):ContentBase(id){ this->mNodeType=nodeType;this->mCurrTabIndex=tabIndex;};
	virtual ~FriendsContent(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "FriendsContent.ccbi";
		return ccbiFileName;
	}
	unsigned int mNodeType;
	unsigned int mCurrTabIndex;
	bool mLastIsSmallBtn;
};

class FriendsPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
	, public MessageHandler
	, public libOSListener
{
public:
	FriendsPage(void);
	~FriendsPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}
	CREATE_FUNC(FriendsPage);
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	virtual void onReceiveMassage(const GameMessage * message);
	virtual void onInputboxEnter(const std::string& content);
	void buildItem(void);
	void addItem(ContentBase* item);
	void rebuildAllItem(void);
	void clearAllItem(void);
	void refreshAllItem(void);
	void removeItem(ContentBase* item);
	void refreshAllContentItem();
	void setCurrId(unsigned int id,unsigned int oprType){this->mCurrReqIndex=id;this->mCurrReqOprType=oprType;};
	virtual void unload(void);
private:
	void delContainerByIndex(const int index);
	typedef std::map<unsigned int,cocos2d::CCMenuItemImage* > TAB_BUTTON_LIST;
	TAB_BUTTON_LIST mFriendsTabList;
	void loadFriendsConfig();
	void _refreshAssistBtn();
	void _setSelectBtn();
	void _clearTabList();
	void refreshCondition();
	void switchTab();
	void switchNode();
	unsigned int mCurrTabIndex;
	typedef	std::list<std::pair<FRIENDS_NODE_TYPE,cocos2d::CCNode*> >  NodeList;
	NodeList mNodeList;
	FRIENDS_NODE_TYPE mCurrNodeIndex;
	int mCurrListSize;
	unsigned int mCurrReqIndex;
	int mCurrReqOprType;
	std::string mInputCondition;
	cocos2d::CCLabelBMFont* mSelCondition;
	bool mIsReqData;
	typedef std::vector<ContentBase * > ContentList;
	ContentList mContentList;
	CCNode* membersFriendsRootNode;
	cocos2d::extension::CCScrollView* membersFriends;
	CCNode* mMessageRootNode;
	cocos2d::extension::CCScrollView* mMessageContent;
	CCNode* membersMakeFriendsRootNode;
	cocos2d::extension::CCScrollView* membersMakeFriends;
	void _refreshPage();
	float mExpbarInitScale;
	void _clearFirstEnemy();
	void _refreshMemberCount();
};

#define  DisposeFriendsPage(id,oprType) dynamic_cast<FriendsPage*>(CCBManager::Get()->getPage("FriendsPage"))->setCurrId(id,oprType);
