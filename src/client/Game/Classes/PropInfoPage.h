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
enum PROP_PANEL_TYPE
{
	PROP_USE,
	PROP_BUY,
	PROP_SHOW,
};
class PropInfoContent
	: public ContentBase
{
public:
	PropInfoContent(unsigned int id):ContentBase(id){};
	virtual ~PropInfoContent(){};
	virtual void refreshContent(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	void setPropOprPanel(unsigned int panelType);
protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "PropInfoContent.ccbi";
		return ccbiFileName;
	}
	CCLabelBMFont* mPropName;
	CCLabelBMFont* mPropSellPrice;
	CCLabelBMFont* mPropInfo;
	CCLabelBMFont* mFunction1;
	CCSprite* mPropIcon;
	CCMenuItemImage* mPropFrame;
	unsigned int mPrice;
	unsigned int mType;
};
class PropInfoPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
	, public MessageHandler
{
public:
	PropInfoPage(void);
	~PropInfoPage(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}
	CREATE_FUNC(PropInfoPage);
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
	void showPage(unsigned int id,unsigned int type,bool isReward);
	void showNextPage();

	static void showPropInfoPage(unsigned int id,unsigned int type,bool isReward);
private:
	typedef std::vector<ContentBase * > ContentList;
	ContentList mContentList;
	CCNode* membersRootNode;
	cocos2d::extension::CCScrollView* members;
	CCLabelBMFont* mTitle;
	unsigned int mToolId;
	unsigned int mPanelType;
	bool mIsReward;
};
#define  PropInfoPageShow(toolId,panelType,isReward) dynamic_cast<PropInfoPage*>(CCBManager::Get()->getPage("PropInfoPage"))->showPage(toolId,panelType,isReward);
#define  PropInfoShowNext() dynamic_cast<PropInfoPage*>(CCBManager::Get()->getPage("PropInfoPage"))->showNextPage();
