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
#include "StringConverter.h"
#include "BlackBoard.h"
#include "PopupPage.h"
#include "DataTableManager.h"
#include "TeamBuffUpgradePage.h"
#define  TeamMax 3
class TeamBuffContent
	: public ContentBase
{
public:
	TeamBuffContent(unsigned int id):ContentBase(id){};
	virtual ~TeamBuffContent(){};
	virtual void refreshContent(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "TeamBuffContent.ccbi";
		return ccbiFileName;
	}
	cocos2d::CCObject* getObjectByVariableName(const std::string& variableName);
private:
	void _refreshPage(void);
	cocos2d::CCSprite* mBuffPic;
	cocos2d::CCSprite* mBuffTypePic;
	cocos2d::CCLabelBMFont* mBuffName;
	cocos2d::CCLabelBMFont* mBuffValue;
	cocos2d::CCLabelBMFont* mBuffLV;
	cocos2d::CCLabelBMFont* mNowNum;
	cocos2d::CCLabelBMFont* mMaxNum;
	const TeamBuffItem* mTeamBuffItem;
	TeamBuffInfo* mTeamBuffStats;
};

class TeamBuffPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
	, public MessageHandler
{
public:
	TeamBuffPage(void);
	~TeamBuffPage(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(TeamBuffPage);
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	void refreshPage(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);;
	virtual void onReceiveMassage(const GameMessage * message);
	void buildItem(void);
	void addItem(ContentBase* item);
	void rebuildAllItem(void);
	void clearAllItem(void);
	void refreshAllItem(void);
	void removeItem(ContentBase* item);

private:
	cocos2d::CCObject* getObjectByVariableName(const std::string& variableName);
	void loadData();
	typedef std::vector<ContentBase * > ContentList;
	ContentList mContentList;
	CCNode* membersRootNode;
	cocos2d::extension::CCScrollView* members;
	unsigned int mCurrTabIndex;
	cocos2d::CCLabelBMFont* mSkillName;
	const SkillItem* mSkillItem;
	unsigned int _lastLevel;
	static bool cmp(unsigned int a,unsigned int b);
};