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
class DiscipleChangeContentItemData 
	: public cocos2d::extension::CCReViSvItemData
{
public:
	unsigned int		m_iDiscipleID;
	bool				m_bSelected;
};
#endif

class DiscipleChangePage;

class DiscipleChangeContent
	: public ContentBase
#ifdef Macro_TestUseCCReViScrollViewFacade
	, public cocos2d::extension::CCReViSvItemNodeFacade
#endif
{
	friend class DiscipleChangePage;
public:
	DiscipleChangeContent(unsigned int id, DiscipleChangePage* ownner)
		: ContentBase(id)
		, mOwnner(ownner)
	{};
	virtual ~DiscipleChangeContent();

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
		static std::string ccbiFileName = "MemberReplaceContent.ccbi";
		return ccbiFileName;
	}

private:
	cocos2d::CCMenuItemImage* mSelect;
	DiscipleChangePage* mOwnner;

#ifdef Macro_TestUseCCReViScrollViewFacade
	DiscipleChangeContentItemData* m_pItemData;
#endif

};

class DiscipleChangePage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
{
public:
	DiscipleChangePage(void);
	~DiscipleChangePage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(DiscipleChangePage);
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
	//void addItem(ContentBase* item);
	void rebuildAllItem(void);
	void clearAllItem(void);
	void refreshAllItem(void);
	void removeItem(ContentBase* item);
	void reOrderAllItem();
	void refreshPage(void);

	void setSellectedContent(DiscipleChangeContent* member);

private:

	typedef std::vector<ContentBase * > ContentList;

	ContentList mContentList;

	CCNode* membersRootNode;

	cocos2d::extension::CCScrollView* members;

	DiscipleChangeContent* mCurSellectedContent;

	bool isDesOrder;

	bool confirmFlag;

#ifdef Macro_TestUseCCReViScrollViewFacade
	cocos2d::extension::CCReViScrollViewFacade* m_pScrollViewFacade;
#endif

};
