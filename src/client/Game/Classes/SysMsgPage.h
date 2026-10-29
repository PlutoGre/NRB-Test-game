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
#include "ContentBase.h"
#include "json/json.h"
#define SYSMSGTABLENGTH 4

enum SYSMSG_TYPE
{
	RANKING=2,
	GONGFU=3,
	FRIENDS_REQ=4,
	LEAVE_MSG=5,
	SYSTEM_FIGHTEND=6,
	SYSTEM=7,
	SYSTEM_WORDBOSS=8,
};
enum SYSMSG_TAB_TYPE
{
	ALL=1,
	FIGHT=2,
	TABFRIENDS=3,
	TABSYSTEM=4,
	TABFRIENDADD=5,
};
enum FIGHT_RET
{
	WIN=1,
	FAIL=2,
};
enum OPERATOR_TYPE
{
	UNOPR=1,
	COUNTERATTACK=2,
	AGREE=3,
	REFUSE=4,
};

class SysMsgContent
	: public ContentBase
{
public:
	SysMsgContent(unsigned int id):ContentBase(id){};
	virtual ~SysMsgContent(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "SysMsgContent.ccbi";
		return ccbiFileName;
	}
	Json::Value mData;
};


class SysMsgPage
	: public CustomPage
	, public State<MainFrame>
	, public MessageHandler
	, public CCBContainer::CCBContainerListener
{
public:
	SysMsgPage(void);
	~SysMsgPage(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}
	CREATE_FUNC(SysMsgPage);
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
	void refreshAllContentItem();
	void setReqType(bool isReward){ mIsReward=isReward; };
private:

	typedef std::map<unsigned int,cocos2d::CCMenuItemSprite* > TAB_BUTTON_LIST;
	TAB_BUTTON_LIST mSysMsgTabList;
	void loadSysMsgConfig();
	void switchTab();
	bool isReqData;
	unsigned int mCurrTabIndex;
	typedef std::vector<ContentBase * > ContentList;
	ContentList mContentList;
	CCNode* membersRootNode;
	cocos2d::extension::CCScrollView* members;
	void _refreshPage();
	float mExpbarInitScale;
	bool mIsReward;
};
#define  MsgReqType(rewardType) dynamic_cast<SysMsgPage*>(CCBManager::Get()->getPage("SysMsgPage"))->setReqType(rewardType);