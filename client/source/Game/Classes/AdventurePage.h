#pragma once

#include "AddPowerPage.h"
#include "FightEndPage.h"
#include "WorldBossPage.h"
#include "TeacherTeachPage.h"
#include "WishPage.h"
#include "MainFrame.h"
#include "StateMachine.h"
#include "BoundlessHolePage.h"
#include "PackPreviewPage.h"
#include "PropInfoPage.h"
#include "SkillBookPage.h"
#include "LevelUpRewardPage.h"
#include "AdventureInvitePage.h"
#include "ChargeRebatePage.h"
#include "FortuneComePage.h"
enum Adventure_Type
{
	Power=1,
	Present=2,
	Wish=3,
	Teach=4,
	FightEnd=5,
	WordBoss=6,
	BoundlessHole=7,
	SkillBook=8,
	LevelUpReward=9,
	ContinueLogin=10,
	Invite=11,
	ChargeRebate=12,
	FortuneCome=13,
	LuaScriptStart=100,
};
class NavContent
	: public AdventureBase
{
public:
	NavContent(unsigned int id):AdventureBase(id){ };
	virtual ~NavContent(){};
	virtual void refreshContent(void);
	CCBContainer* getContainerNode(){return mContentContainer;};
	virtual void loadData();
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	virtual void updatePage(bool isSelect,float dt=0.0f); 
	void exitPage();
	bool isShowActivityTip();
protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "AdventureNavButton.ccbi";
		return ccbiFileName;
	}
private:
	cocos2d::CCMenuItemImage* mButton;
	const std::string _getTipSaveKey();
	void _saveActivityGuideStatus(std::string _key,unsigned int _version);
};

class AdventurePage
	: public CustomPage
	, public cocos2d::extension::CCScrollViewDelegate
	, public State<MainFrame>
	,public MessageHandler
{
public:
	AdventurePage(void);
	~AdventurePage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}
	CREATE_FUNC(AdventurePage);
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void scrollViewDidScroll(cocos2d::extension::CCScrollView* view){};
	virtual void scrollViewDidDeaccelerateStop(cocos2d::extension::CCScrollView* view, cocos2d::CCPoint initSpeed);
	virtual void scrollViewDidZoom(cocos2d::extension::CCScrollView* view){};
	void buildItem(void);
	void addContentItem(unsigned int index,unsigned int id,AdventureBase* item);
	void addNavItem(unsigned int index,unsigned int id,AdventureBase* item);
	void onReceiveMassage( const GameMessage * message );
	void changePage(const unsigned int index,bool navOffset = false);
	void resetNavOffset(const unsigned int index);
	unsigned int getCurrentIndex(){return mCurrIndex;};
	void checkAdActivityGuideStatus();
protected:
	NavContent* getNavContentByIndex(const unsigned int index);
	AdventureBase* getAdventureBaseByIndex(const unsigned int index);
	inline const int getIdByIndex(const unsigned int index);
	void initPage();
	void rebuildAllItem(void);
	void clearAllItem(void);
	void reSizeAllItem(void);
	void removeNavItem(AdventureBase* item);
	void removeContentItem(AdventureBase* item);
	void removeItem(unsigned int index);
	void refreshAllNavItem();
	void refreshAllContentItem();
	void refreshAllItem();
private:
	typedef std::map<unsigned int,AdventureBase* > ContentList;
	typedef std::map<unsigned int,unsigned int > PageIndexList;
	typedef std::map<unsigned int,bool> PageInitStats;
	PageInitStats mPageInitStats;
	PageIndexList mPageIndexList;
	ContentList mContentList;
	ContentList mNavContentList;
	CCSize mContentSize;
	CCSize mNavSize;
	float mPageWidth;
	float mNavWidth;
	cocos2d::extension::CCScrollView* mNavContent;
	CCNode* mNavContentNode;
	cocos2d::extension::CCScrollView* mPageContent;
	CCNode* mPageContentNode;
	unsigned int mCurrIndex;
	bool mIsInitData;
	void _setPageStats(unsigned int _pageId,bool _stats);
	unsigned int _getIndexById(unsigned int id);
	bool _checkAdventureInfo();
};

#define  CheckAdventureGuide() dynamic_cast<AdventurePage* >(CCBManager::Get()->getPage("AdventurePage"))->checkAdActivityGuideStatus();

