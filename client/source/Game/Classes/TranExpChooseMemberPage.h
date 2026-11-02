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

#define Macro_TestUseCCReViScrollViewFacade
//#undef Macro_TestUseCCReViScrollViewFacade

#ifdef Macro_TestUseCCReViScrollViewFacade
class MemberContentItemData 
	: public cocos2d::extension::CCReViSvItemData
{
public:
	unsigned int		m_iMemberID;
	bool				m_bSelected;
};
#endif

class TranExpChooseMemberPage;

class MemberContent
	: public ContentBase
#ifdef Macro_TestUseCCReViScrollViewFacade
	, public cocos2d::extension::CCReViSvItemNodeFacade
#endif
{
	friend class TranExpChooseMemberPage;
public:
	MemberContent(unsigned int id, TranExpChooseMemberPage* ownner)
		: ContentBase(id)
		, mOwnner(ownner)
	{};
	virtual ~MemberContent(){};

	virtual void refreshContent(void);

//	CCBContainer* getContainerNode(){return mItemContainer;};

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
		static std::string ccbiFileName = "TranExpCMContent.ccbi";
		return ccbiFileName;
	}

private:
	cocos2d::CCMenuItemImage* mSelect;
	TranExpChooseMemberPage* mOwnner;
// 	unsigned int mID;
// 	CCBContainer* mItemContainer;

#ifdef Macro_TestUseCCReViScrollViewFacade
	MemberContentItemData* m_pItemData;
#endif

};

class TranExpChooseMemberPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
{
public:
	TranExpChooseMemberPage(void);
	~TranExpChooseMemberPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(TranExpChooseMemberPage);
	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);

	virtual void load(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){};

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

	void setSellectedContent(MemberContent* member);

private:

	typedef std::vector<ContentBase * > ContentList;

	ContentList mContentList;

	CCNode* membersRootNode;

	cocos2d::extension::CCScrollView* members;

	cocos2d::CCLabelBMFont* mTotoalExp;
	cocos2d::CCLabelBMFont* mExpChoosed;
	cocos2d::CCLabelBMFont* mExpNeeded;

	MemberContent* mCurSellectedContent;

	bool isDesOrder;

#ifdef Macro_TestUseCCReViScrollViewFacade
	cocos2d::extension::CCReViScrollViewFacade* m_pScrollViewFacade;
#endif

};
