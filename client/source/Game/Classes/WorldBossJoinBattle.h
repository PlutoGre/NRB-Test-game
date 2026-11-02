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

class WorldBossBattleTempItem
	: public ContentBase
{
public:
	WorldBossBattleTempItem(unsigned int id):ContentBase(id){};
	virtual ~WorldBossBattleTempItem(){};
	virtual void refreshContent(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "WorldBossHurtRankContent.ccbi";
		return ccbiFileName;
	}
private:
	cocos2d::CCObject * getObjectByVariableName(const std::string& variableName,CCBContainer* ccbContainer);
	unsigned int mPlayerId;
};

class WorldBossDamageTempItem
	: public ContentBase
{
public:
	WorldBossDamageTempItem(unsigned int id):ContentBase(id){};
	virtual ~WorldBossDamageTempItem(){};
	virtual void refreshContent(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	std::string damageContent;

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "WorldBossHurtRankContent.ccbi";
		return ccbiFileName;
	}
private:
	cocos2d::CCObject * getObjectByVariableName(const std::string& variableName,CCBContainer* ccbContainer);
	unsigned int mPlayerId;
};

class WorldBossJoinBattle
		: public CustomPage
		, public State<MainFrame>
		, public MessageHandler
		, public CCBContainer::CCBContainerListener
{
public:
	WorldBossJoinBattle(void);
	~WorldBossJoinBattle(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}
  	virtual void load(void);
 	CREATE_FUNC(WorldBossJoinBattle);
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void refreshPage();
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	virtual void countReliveTime();
	virtual void translateTime( int seconds , int id, int Count, int maxCount, int price);
	virtual void sendPackageByM();
	virtual void refreshDamageByS(); 
	void rebuildAllBattleItem(void);
	void clearAllBattleItem(void);
	void buildBattleItem(void);
	void addBattleItem(ContentBase* item);
	void refreshAllBattleItem(void);
	void removeBattleItem(ContentBase* item);
	void rebuildAllDamageItem(void);
	void clearAllDamageItem(void);
	void buildDamageItem(void);
	void addDamageItem(WorldBossDamageTempItem* item);
	void refreshAllDamageItem(void);
	void removeDamageItem(WorldBossDamageTempItem* item);
	void onReceiveMassage( const GameMessage * message );
	void fightMovie(int discipleId,int damageBlood);
	void scrollDamageItems();
	void static countLeftTime(int finished);

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "WorldBossOngoing.ccbi";
		return ccbiFileName;
	}
	bool mIsInitData;

private:
	typedef std::vector<ContentBase * > ContentList;
	ContentList mRankList;
	cocos2d::extension::CCScrollView* mContentScollView;
	cocos2d::CCObject * getObjectByVariableName(const std::string& variableName,CCBContainer* ccbContainer);

	ContentList mBattleContentList;
	CCNode*membersRootNode;
	cocos2d::extension::CCScrollView* members;
	void setPageData();


	cocos2d::CCLabelBMFont* mLable1;
	cocos2d::CCLabelBMFont* mLable2;
	cocos2d::CCLabelBMFont* mLable3;
	cocos2d::CCLabelBMFont* mLable4;
	cocos2d::CCLabelBMFont* mAttackNum;
	cocos2d::CCLabelBMFont* mBloodHurt;
	cocos2d::CCLabelBMFont* mTimeCount;
	cocos2d::CCSprite* mGoldPic;
	cocos2d::CCLabelBMFont* mGoldNeed;
	cocos2d::CCSprite* mBuffPic;
	cocos2d::CCLabelBMFont* mBuffValue;
	cocos2d::CCSprite* mGoldPic1;
	cocos2d::CCLabelBMFont* mGoldNeed1;
	cocos2d::CCNode* rebornNode;
	cocos2d::CCNode* RebornWithBuffNode;


	unsigned int currSecond;
//	std::string damageTempArr[] ;
	std::string damageTemp1 ;
	std::string damageTemp2 ;
	std::string damageTemp3 ;

	bool inited;
	int animationNum;


};