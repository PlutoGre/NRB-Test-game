#pragma once
#include "AdventureBase.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "CCBManager.h"

class ChargeRebatePage
	:public AdventureBase
{
public:
	ChargeRebatePage(unsigned int id):AdventureBase(id)
	{
		mIsInitData=true;
		showFlag=true;
	};
	~ChargeRebatePage();
	virtual void refreshContent(void);
	CCBContainer* getContainerNode(){return mContentContainer;};
	void loadData();
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	void updatePage(bool isSelect,float dt=0.0f); 
	void exitPage();
	void translateTime( int seconds );


protected:
	virtual const std::string& getCcbiFileName(void) const
	{
//		static std::string ccbiFileName = "AddPowerPage.ccbi";
		static std::string ccbiFileName = "ChargeRebate.ccbi";
		return ccbiFileName;
	}
	bool mIsInitData;
private:
	cocos2d::CCMenuItemImage* mRechargeBtn;
	cocos2d::CCLabelBMFont* mRecharge;
	cocos2d::CCMenuItemImage* mRewardBtn;
	cocos2d::CCLabelBMFont* mReceiveReward;

	cocos2d::extension::CCScale9Sprite* rechargeColumn;
	cocos2d::CCSprite* rechargeBG;

	cocos2d::CCNode* chargeColumnNode;

	cocos2d::CCNode* m2;
	cocos2d::CCNode* m1;

	cocos2d::CCLabelBMFont* mChargeRebateTitle;
	cocos2d::CCLabelBMFont* mChargeRebateContent;
	cocos2d::CCLabelBMFont* mMaxRecharge;
	cocos2d::CCLabelBMFont* mMinRecharge;
	cocos2d::CCLabelBMFont* mTimeCount;

	cocos2d::CCLabelBMFont* mDuringDays;
	cocos2d::CCLabelBMFont* mLastDays;

	cocos2d::CCLabelBMFont* mReceiveRewardContent1;
	cocos2d::CCLabelBMFont* mReceiveRewardContent2;
	cocos2d::CCLabelBMFont* mReceiveRewardContent3;

	void setPageData();
	bool showFlag;
};