#pragma once
#include "AdventureBase.h"
class BoundlessHolePage
	:public AdventureBase
{
public:
	BoundlessHolePage(unsigned int id):AdventureBase(id)
	{
		mIsInitData=false;
		mAlreadyUpdate = false;
	};
	virtual ~BoundlessHolePage(){};
	virtual void refreshContent(void);
	void loadData();
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	void updatePage(bool isShow,float dt=0.0f); 
	void showSuccessDialog();
	void exitPage();
	void showHeartOnce(int status);
protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "BoundlessHolePage.ccbi";
		return ccbiFileName;
	}
private:	
	void _updateStatus(bool isPlayAni);
	cocos2d::CCLabelBMFont* mBoundlessStatus;
	cocos2d::CCLabelBMFont* mBoundlessStatus2;
	cocos2d::CCMenuItemImage* mBoundlessHole;
	cocos2d::CCLabelBMFont* mKissTxt1;
	cocos2d::CCLabelBMFont* mKissTxt2;
	bool mIsShow;
	int heartFlag;
	bool mAlreadyUpdate;
	bool mIsClick;//add by xinghui
};
