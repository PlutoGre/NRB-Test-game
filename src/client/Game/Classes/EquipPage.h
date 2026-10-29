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
#include "PopupManager.h"

#define Macro_TestUseCCReViScrollViewFacade

class EquipContentStone
	: public ContentBase
{
public:
	EquipContentStone(unsigned int id,unsigned int equipId):ContentBase(id){
		this->equipId = equipId;
	};
	virtual ~EquipContentStone(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "EquipmentContentStoneItem.ccbi";
		return ccbiFileName;
	}
private:
	unsigned int equipId;
};

#ifdef Macro_TestUseCCReViScrollViewFacade
class EquipContentItemData 
	: public cocos2d::extension::CCReViSvItemData
{
public:
	unsigned int		m_iEquipID;
};
#endif

class EquipContent
	: public ContentBase
#ifdef Macro_TestUseCCReViScrollViewFacade
	, public cocos2d::extension::CCReViSvItemNodeFacade
#endif
{
public:
	EquipContent(unsigned int id):ContentBase(id){};
	virtual ~EquipContent();

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	virtual void rebuildAllItem(void);
	virtual void clearAllItem(void);
	virtual void buildItem();
	virtual void addItem(ContentBase* item);
	virtual void refreshAllItem();
	virtual void removeItem(ContentBase* item);


#ifdef Macro_TestUseCCReViScrollViewFacade
	virtual void initItemView();
	virtual void refreshItemView(cocos2d::extension::CCReViSvItemData* pItem);
#ifdef _DEBUG
	unsigned int m_iIdx;
#endif
#endif

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "EquipmentContent.ccbi";
		if(ServerDateManager::Get()->getUserBasicInfo().level >=  StringConverter::parseInt(VaribleManager::Get()->getSetting("StoneInlaidLevel"))){
			ccbiFileName = "EquipmentContentStone.ccbi";
		}
		return ccbiFileName;
	}
private:
	typedef std::vector<ContentBase * > ContentList;

	ContentList mContentList;

	CCNode* mStone;

	//cocos2d::extension::CCScrollView* members;
};

class EquipPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
{
public:
	EquipPage(void);
	~EquipPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(EquipPage);
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

private:
	void sellectTab(unsigned int tabIndex);

	typedef std::vector<ContentBase * > ContentList;

	ContentList mContentList;

	CCNode* membersRootNode;

	cocos2d::extension::CCScrollView* members;

	unsigned int  mCurrTabIndex;

	typedef std::map<unsigned int, cocos2d::CCMenuItemImage*> TabMap;
	TabMap mTabMap;
	float mLongBtnHeight;

#ifdef Macro_TestUseCCReViScrollViewFacade
	cocos2d::extension::CCReViScrollViewFacade* m_pScrollViewFacade;
#endif

};
