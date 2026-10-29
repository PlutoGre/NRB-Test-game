#pragma once
#include "AdventureBase.h"
#include "CCBContainer.h"
#define AdventureAddPowerType 1
enum AddPowerTime
{
	AMSTART_TIME=4,
	AMEND_TIME,
	PMSTART_TIME=10,
	PMEND_TIME,
};
enum AddPowerType
{
	AM_ADD=1,
	PM_ADD=2,
	CAN_NOT_ADD=3,
};
enum Reward_Type
{
	CAN_NOT_REWARD,
	CAN_REWARD,
	ALREADY_REWARD,
};
class AddPowerPage
	:public AdventureBase
{
public:
	AddPowerPage(unsigned int id):AdventureBase(id)
	{
		mIsInitData=false;
	};
	virtual ~AddPowerPage(){};
	virtual void refreshContent(void);
	void loadData();
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	void updatePage(bool isShow,float dt=0.0f); 
	void exitPage();
	void showSuccessDialog();
protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "AddPowerPage.ccbi";
		return ccbiFileName;
	}
private:	
	cocos2d::CCLabelBMFont* mAMLabelTxt;
	cocos2d::CCLabelBMFont* mPMLabelTxt;
//	cocos2d::CCNode* mCanEat;
	cocos2d::CCMenuItemImage* mButton;
//	cocos2d::CCNode* mCanNotEat;
	bool mIsShow;
	AddPowerType mAddPowerType;

	void sendAddPowerPacket();
	void changePowerStatus();
};
