#pragma once
#include "AdventureBase.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "CCBManager.h"

class WorldBossPage
	:public AdventureBase
{
public:
	WorldBossPage(unsigned int id):AdventureBase(id)
	{
		mIsInitData=true;
	};
	~WorldBossPage();
	virtual void refreshContent(void);
	CCBContainer* getContainerNode(){return mContentContainer;};
	void loadData();
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	void updatePage(bool isSelect,float dt=0.0f); 
	void exitPage();
	void translateTime( int seconds , int id, int Count, int maxCount, int price);
protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "WorldBossPage.ccbi";
//		static std::string ccbiFileName = "WorldBoss.ccbi";
		return ccbiFileName;
	}
	bool mIsInitData;
private:
	cocos2d::CCMenuItemImage* mFace;
	cocos2d::CCMenuItemImage* mJoinBattle;

	cocos2d::CCLabelBMFont* mTimeCount;

	cocos2d::CCLabelBMFont* mBossLvLabel;
	cocos2d::CCNode* mTimeCountNode;
	cocos2d::CCLabelBMFont* mTimeCountLabel;
	cocos2d::CCNode* mJoinNode;
	cocos2d::CCLabelBMFont* mJoinTipLabel;
	cocos2d::CCLabelBMFont* mHistoryNoteLabel;
	void setPageData();
	enum BUYTYPE
	{
		ORDINARL = 1,
		USING_RMB = 2,
	};
	BUYTYPE mCanBuy[3];
	int mPrice[3];
};
