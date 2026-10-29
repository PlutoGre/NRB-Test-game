#pragma once
#include "AdventureBase.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "CCBManager.h"
#include "ScrollNumCardTask.h"

class FortuneComePage
	:public AdventureBase
{
public:
	FortuneComePage(unsigned int id):AdventureBase(id)
	{
		mIsInitData=true;
		showFlag=true;
	};
	~FortuneComePage();
	virtual void refreshContent(void);
	CCBContainer* getContainerNode(){return mContentContainer;};
	void loadData();
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	void updatePage(bool isSelect,float dt=0.0f); 
	void exitPage();
	void translateTime( int seconds );

	void roundNumCard(int rewardGold);

	void scrollContent();

protected:
	virtual const std::string& getCcbiFileName(void) const
	{
//		static std::string ccbiFileName = "FortuneCome.ccbi";
		static std::string ccbiFileName = "GodofWealth.ccbi";
		return ccbiFileName;
	}
	bool mIsInitData;
private:

	cocos2d::extension::CCScrollView* mScrollNum1;
	cocos2d::extension::CCScrollView* mScrollNum2;
	cocos2d::extension::CCScrollView* mScrollNum3;
	cocos2d::extension::CCScrollView* mScrollNum4;
	cocos2d::extension::CCScrollView* mScrollNum5;

	cocos2d::extension::CCScrollView** mScrollNumArr;


	ScrollNumCardTask* scrollNumCardTask;

	ScrollNumCard* scrollCardContent;

	CCNode* mScrollNum1RootNode;
	CCNode* mScrollNum2RootNode;
	CCNode* mScrollNum3RootNode;
	CCNode* mScrollNum4RootNode;
	CCNode* mScrollNum5RootNode;
	
	CCNode** mScrollNumRootNode;

	cocos2d::extension::CCScrollView* mContent;

	cocos2d::CCLabelBMFont* mTimeCount;
	cocos2d::CCLabelBMFont* mDiscribe;

	cocos2d::CCNode* needGoldNode;
	cocos2d::CCLabelBMFont* needGoldNum;

	cocos2d::CCLabelBMFont* mUserGold;

	cocos2d::CCMenuItemImage* mRewardBtn;

	/*

	cocos2d::CCNode* chargeColumnNode;

	cocos2d::CCLabelBMFont* mChargeRebateTitle;
	cocos2d::CCLabelBMFont* mChargeRebateContent;


	cocos2d::CCLabelBMFont* mReceiveRewardContent1;
	cocos2d::CCLabelBMFont* mReceiveRewardContent2;
	*/

	void setPageData();
	bool showFlag;
	int cardNum;
};