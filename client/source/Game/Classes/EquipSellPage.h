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
#include "DisciplePage.h"

#include <set>

#define Macro_TestUseCCReViScrollViewFacade

#ifdef Macro_TestUseCCReViScrollViewFacade
class EquipSellContentItemData 
	: public cocos2d::extension::CCReViSvItemData
{
public:
	unsigned int		m_iEquipID;
	bool				m_bSelected;
};
#endif

class EquipSellPage;

class EquipSellContent
	: public ContentBase
#ifdef Macro_TestUseCCReViScrollViewFacade
	, public cocos2d::extension::CCReViSvItemNodeFacade
#endif
{
	friend class EquipSellPage;
public:
	EquipSellContent(unsigned int id, EquipSellPage* ownner)
		: ContentBase(id)
		, mOwnner(ownner)
	{};
	virtual ~EquipSellContent(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	void setSellect(bool sellect);

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
		static std::string ccbiFileName = "EquipmentSellContent.ccbi";
		return ccbiFileName;
	}

private:
	cocos2d::CCMenuItemImage* mSelect;
	EquipSellPage* mOwnner;
#ifdef Macro_TestUseCCReViScrollViewFacade
	EquipSellContentItemData* m_pItemData;
#endif
};

class EquipSellPage
	: public CustomPage
	, public State<MainFrame>
	, public MessageHandler
{
public:
	EquipSellPage(void);
	~EquipSellPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(EquipSellPage);
	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);

	virtual void load(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);;

	virtual void onReceiveMassage(const GameMessage * message);

	void buildItem(void);
	void addItem(ContentBase* item);
	void rebuildAllItem(void);
	void clearAllItem(void);
	void refreshAllItem(void);
	void removeItem(ContentBase* item);

	void refreshPage(void);

	void setSellectedContent(EquipSellContent* member);

#ifdef Macro_TestUseCCReViScrollViewFacade
	void setSellectedContent( EquipSellContentItemData* member );
#endif

private:

	typedef std::vector<ContentBase * > ContentList;

	ContentList mContentList;

	CCNode* membersRootNode;

	cocos2d::extension::CCScrollView* members;

	std::set<EquipSellContent*> mSelectedContents;

#ifdef Macro_TestUseCCReViScrollViewFacade
	cocos2d::extension::CCReViScrollViewFacade* m_pScrollViewFacade;
	std::set<EquipSellContentItemData*> m_setSelectedItems;
#endif

};
