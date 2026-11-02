#pragma once
#include "cocos2d.h"
#include "cocos-ext.h"
#include "MainFrame.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "GameMessages.h"
#include "GamePackets.h"
#include "DataTableManager.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "ResManager.h"
#define DAYCOUNT 3

class CardBase
	: public CCBContainer::CCBContainerListener
{
public:
	CardBase(unsigned int id);
	virtual ~CardBase();

	virtual void init(void);

	virtual void refreshContent(void) = 0;

	virtual CCBContainer* getContainerNode(){return mContentContainer;};

	unsigned int getContentID(){return mID;};

protected:
	virtual const std::string& getCcbiFileName(void) const = 0;
	unsigned int mID;
	CCBContainer* mContentContainer;
};

class CardContent
	: public CardBase
{
public:
	CardContent(unsigned int id):CardBase(id){};
	virtual ~CardContent(){};

	virtual void refreshContent(void);

	CCBContainer* getContainerNode(){return mContentContainer;};

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	void flipSprite(const unsigned int id,const unsigned int count,bool isSelect=false);

	void hideReset(CCNode* sender);
	void showReset(CCNode* sender);
	void _showQuantity(CCLabelBMFont* dt);
protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "ContinueCard.ccbi";
		return ccbiFileName;
	}
	cocos2d::CCSprite* mCardHide;
	cocos2d::CCSprite* mCardShow;
	cocos2d::CCSprite* mCardIcon;
	cocos2d::CCLabelBMFont* mQuantity;
	cocos2d::CCMenuItemImage* mMask;
	cocos2d::CCMenuItemImage* mFrame;
	cocos2d::CCNode* mCardNode;
	bool isFlip;
	bool mIsSelect;
};

class ContinueLoginPage
	: public CustomPage
	, public CCBContainer::CCBContainerListener
	, public MessageHandler
{
public:
	ContinueLoginPage(void);
	~ContinueLoginPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(ContinueLoginPage);
	//this will execute when the state is entered
	virtual void Enter();

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	virtual void onReceiveMassage(const GameMessage * message);
	void initPanel();
	void buildItem(void);
	void addItem(unsigned int index,CardContent* item);
	void flipItemByIndex(const unsigned int index);
	CardContent* getItemByIndex(const unsigned int index);
	void refreshContinueSymbol();
	void randRewardList();
	static bool cmpRewardList( int a,int b);
private:
	cocos2d::CCNode* mCardNode;
	typedef std::map<unsigned int,CardContent* > ContentList;
	typedef std::map<unsigned int,cocos2d::CCNode*> NodeList;
	typedef std::map<unsigned int,cocos2d::CCSprite*> SpriteList;
	typedef std::map<unsigned int,unsigned int> IntMap;
	ContentList mContinueCard;
	NodeList mNodeList;
	SpriteList mContinueDaySpriteList;
	SpriteList mDaySpriteList;
	cocos2d::CCLabelBMFont* mTitle;
	unsigned int mRewardCount;
	IntMap mClickList;
	IntMap mRewardMap;
	ContinueLoginInfo continueLoginInfo;
	unsigned int mCardLength;
	void generateContinueReward();
	void _clearAllItem();
};
