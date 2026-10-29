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
class EquipChangeContentItemData 
	: public cocos2d::extension::CCReViSvItemData
{
public:
	unsigned int		m_iEquipID;
	bool				m_bSelected;
};
#endif

class EquipChangePage;

class EquipChangeContent
	: public ContentBase
#ifdef Macro_TestUseCCReViScrollViewFacade
	, public cocos2d::extension::CCReViSvItemNodeFacade
#endif
{
public:
	EquipChangeContent(unsigned int id, EquipChangePage* ownner)
		: ContentBase(id)
		, mOwnner(ownner)
		,carriedByDiscipleName("")
	{};
	virtual ~EquipChangeContent(){};

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
	EquipChangePage* mOwnner;
	std::string carriedByDiscipleName;

#ifdef Macro_TestUseCCReViScrollViewFacade
	EquipChangeContentItemData* m_pItemData;
#endif

};

class EquipChangePage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
{
public:
	EquipChangePage(void);
	~EquipChangePage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(EquipChangePage);
	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);

	virtual void load(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);;

	void buildItem(void);
	void addItem(ContentBase* item);
	void rebuildAllItem(void);
	void clearAllItem(void);
	void refreshAllItem(void);
	void removeItem(ContentBase* item);
	//void refreshItem(DiscipleItem* item);
	void reOrderAllItem();

	void refreshPage(void);

	void setSellectedContent(EquipChangeContent* member);

#ifdef Macro_TestUseCCReViScrollViewFacade
	void setSellectedContent( EquipChangeContentItemData* member );
#endif

private:

	typedef std::vector<ContentBase * > ContentList;

	ContentList mContentList;

	CCNode* membersRootNode;

	cocos2d::extension::CCScrollView* members;

	EquipChangeContent* mCurSellectedContent;

	bool isDesOrder;

#ifdef Macro_TestUseCCReViScrollViewFacade
	cocos2d::extension::CCReViScrollViewFacade* m_pScrollViewFacade;
	EquipChangeContentItemData* m_curSelectedItemData;
#endif

};
