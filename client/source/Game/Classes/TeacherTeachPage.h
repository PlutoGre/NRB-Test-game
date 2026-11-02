#pragma once
#include "AdventureBase.h"
#include "GamePrecedure.h"
#include "GameMaths.h"

class TeacherTeachPage
	:public AdventureBase
{
public:
	TeacherTeachPage(unsigned int id):AdventureBase(id)
	{
		mIsInitData=false;
	};
	virtual ~TeacherTeachPage(){};
	virtual void refreshContent(void);
	CCBContainer* getContainerNode(){return mContentContainer;};
	void loadData();
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	void updatePage(bool isSelect,float dt=0.0f); 
	void exitPage();
protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "TeacherTeachPage.ccbi";
		return ccbiFileName;
	}
	bool mIsInitData;
private:
	void setTeacherStatus();
	cocos2d::CCSprite* mTeacherPicSprite;
	cocos2d::CCLabelBMFont* mTeacherSayLabel;
	cocos2d::CCNode* mGetExpNode;
	cocos2d::CCLabelBMFont* mGetExpValueLabel;
	cocos2d::CCLabelBMFont* mSendWineValueLabel;
	cocos2d::CCNode* mCountDownNode;
	cocos2d::CCLabelBMFont* mCountDownLabel;
	cocos2d::CCLabelBMFont* mWillGetExpValueLabel;
	enum GetExpType
	{
		GET_COMMON_EXP=1,
		GET_DOUBLE_EXP,
	};
};