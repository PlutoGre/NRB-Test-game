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

enum Reward_Data_Source
{
	FIGHT_END_REWARD,
	OPEN_TREASURE_REWARD,
	OPEN_GIFT_BOX_REWARD,
	KISS_GODNESS_REWARD,
};

class PackPreviewContent
	: public ContentBase
{
public:
	PackPreviewContent(unsigned int index,unsigned int id):ContentBase(id){this->mIndex=index;};
	virtual ~PackPreviewContent(){};
	virtual void refreshContent(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "PackPreviewContent.ccbi";
		return ccbiFileName;
	}
	unsigned int mIndex;
	CCLabelBMFont* mPropName;
	CCLabelBMFont* mPropNumber;
	CCSprite* mPropIcon;
	CCMenuItemImage* mPropFrame;
	void refreshFightEndReward();
	void refreshUseToolReward();
};
class PackPreviewPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
	, public MessageHandler
{
public:
	PackPreviewPage(void);
	~PackPreviewPage(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}
	CREATE_FUNC(PackPreviewPage);
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
	void showPage(unsigned int dataSource);
private:
	void buildFightEndRewardItem();
	void buildUseToolRewardItem();
	typedef std::vector<ContentBase * > ContentList;
	ContentList mContentList;
	CCNode* membersRootNode;
	cocos2d::extension::CCScrollView* members;
	CCLabelBMFont* mTitle;
	CCLabelBMFont* mPackPageMsg;
	unsigned int mDataSource;
};
#define  PackPreviewPageShow(dataSource) dynamic_cast<PackPreviewPage*>(CCBManager::Get()->getPage("PackPreviewPage"))->showPage(dataSource);
