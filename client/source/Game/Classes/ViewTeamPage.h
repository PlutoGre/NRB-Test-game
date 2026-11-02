#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "ContentBase.h"
#include "SkillViewTeamInfoPage.h"
#include "DiscipleViewTeamInfoPage.h"
class ViewTeamPage;

class ViewPersonCardContent
	: public ContentBase
{
public:
	ViewPersonCardContent(unsigned int id, ViewTeamPage* ownner);;
	virtual ~ViewPersonCardContent();

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "MemberCard.ccbi";
		return ccbiFileName;
	}

private:
	ViewTeamPage* mOwnner;

	//--begin xinzheng 2013-7-17
	std::string			mStrTex1;
	std::string			mStrTex2;
	std::string			mStrTex3;
	//--end
};

class ViewTeamOverviewCardContent
	: public ContentBase
{
public:
	ViewTeamOverviewCardContent(unsigned int id)
		:ContentBase(id)
	{};
	virtual ~ViewTeamOverviewCardContent(){};
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){};

	virtual void refreshContent(void);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "TeamOverviewCard.ccbi";
		return ccbiFileName;
	}
};

class ViewIconContent
	: public ContentBase
{
public:
	ViewIconContent(unsigned int id)
		:ContentBase(id)
	{};
	virtual ~ViewIconContent(){};

	virtual void refreshContent(void) = 0;

	void setSellect(bool sellect);
protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "framedButton.ccbi";
		return ccbiFileName;
	}

	cocos2d::CCMenuItemImage* mFrameBack;
};

class ViewPersonIconContent
	: public ViewIconContent
{
public:
	ViewPersonIconContent(unsigned int id, ViewTeamPage* ownner)
		:ViewIconContent(id)
		,mOwnner(ownner)
	{};
	virtual ~ViewPersonIconContent(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	void setSellect(bool sellect);

private:
	ViewTeamPage* mOwnner;
};

class ViewTeamOverviewIconContent
	: public ViewIconContent
{
public:
	ViewTeamOverviewIconContent(unsigned int id, ViewTeamPage* ownner)
		:ViewIconContent(id)
		,mOwnner(ownner)
	{};
	virtual ~ViewTeamOverviewIconContent(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	void setSellect(bool sellect);

private:
	ViewTeamPage* mOwnner;
};

class ViewTeamPage
	: public CustomPage
	, public cocos2d::extension::CCScrollViewDelegate
	, public State<MainFrame>
	, public MessageHandler
{
public:
	ViewTeamPage(void);
	~ViewTeamPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(ViewTeamPage);
	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);

	virtual void load(void);
	void refreshPage(void);

	void buildItem(void);
	void addCardItem(ContentBase* item);
	void addIconItem(ContentBase* item);
	void rebuildAllItem(void);
	void clearAllItem(void);

	virtual void scrollViewDidScroll(cocos2d::extension::CCScrollView* view){};
	virtual void scrollViewDidDeaccelerateStop(cocos2d::extension::CCScrollView* view, cocos2d::CCPoint initSpeed);
	virtual void scrollViewDidZoom(cocos2d::extension::CCScrollView* view){};

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	void setTarget(unsigned int index, bool isDuration=false);

	void resetNavOffset(void);

	virtual void onReceiveMassage(const GameMessage * message);

private:

	void refreshPersonPage();
	void refreshOverviewPage(void);

	typedef std::map<unsigned int, ContentBase * > ContentList;

	ContentList mPersonCardContentList;
	ContentList mPersonIconContentList;

	CCNode* mPersonCardRootNode;
	CCNode* mPersonIconRootNode;

	cocos2d::extension::CCScrollView* mPersonCards;
	cocos2d::extension::CCScrollView* mPersonIcons;

	unsigned int mCurrIndex;
	bool _checkFates(const YuanfenItem* item,unsigned int id);
};

