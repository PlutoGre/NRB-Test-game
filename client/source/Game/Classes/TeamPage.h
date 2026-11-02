#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "ContentBase.h"

class TeamPage;

class PersonCardContent
	: public ContentBase
{
public:
	PersonCardContent(unsigned int id, TeamPage* ownner);;
	virtual ~PersonCardContent();

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	//--begin xinzheng 2013-6-4
	void setContentIDWithRemoveTextureCache(unsigned int id);
	virtual void setContentID(unsigned int id);
	//--end

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "MemberCard.ccbi";
		return ccbiFileName;
	}

private:
	TeamPage* mOwnner;
	//--begin xinzheng 2013-6-5
	unsigned int m_iDiscipleId;
	//--end
};

class PersonChooseCardContent
	: public ContentBase
{
public:
	PersonChooseCardContent(unsigned int id, TeamPage* ownner)
		:ContentBase(id)
		,mOwnner(ownner)
	{};
	virtual ~PersonChooseCardContent(){};
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	virtual void refreshContent(void);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "TeamNoMem.ccbi";
		return ccbiFileName;
	}

private:
	TeamPage* mOwnner;
};

class TeamOverviewCardContent
	: public ContentBase
{
public:
	TeamOverviewCardContent(unsigned int id)
		:ContentBase(id)
	{};
	virtual ~TeamOverviewCardContent(){};
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){};

	virtual void refreshContent(void);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "TeamOverviewCard.ccbi";
		return ccbiFileName;
	}
};

class IconContent
	: public ContentBase
{
public:
	IconContent(unsigned int id)
		:ContentBase(id)
	{};
	virtual ~IconContent(){};

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

class PersonIconContent
	: public IconContent
{
public:
	PersonIconContent(unsigned int id, TeamPage* ownner)
		:IconContent(id)
		,mOwnner(ownner)
	{};
	virtual ~PersonIconContent(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	void setSellect(bool sellect);

private:
	TeamPage* mOwnner;
};

class TeamOverviewIconContent
	: public IconContent
{
public:
	TeamOverviewIconContent(unsigned int id, TeamPage* ownner)
		:IconContent(id)
		,mOwnner(ownner)
	{};
	virtual ~TeamOverviewIconContent(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	void setSellect(bool sellect);

private:
	TeamPage* mOwnner;
};

class TeamUnlockIconContent
	: public IconContent
{
public:
	TeamUnlockIconContent(unsigned int id)
		:IconContent(id)
	{};
	virtual ~TeamUnlockIconContent(){};

	virtual void refreshContent(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	void setSellect(bool sellect);

};

class TeamPage
	: public CustomPage
	, public cocos2d::extension::CCScrollViewDelegate
	, public State<MainFrame>
	, public MessageHandler
{
public:
	TeamPage(void);
	~TeamPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(TeamPage);
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

