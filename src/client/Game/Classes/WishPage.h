#pragma once
#include "AdventureBase.h"

class WishPage
	:public AdventureBase
{
public:
	WishPage(unsigned int id):AdventureBase(id)
	{
		mIsInitData=false;
	};
	virtual ~WishPage(){};
	virtual void refreshContent(void);
	CCBContainer* getContainerNode(){return mContentContainer;};
	void loadData();
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	void updatePage(bool isSelect,float dt=0.0f); 
	void exitPage();
protected:
	virtual const std::string& getCcbiFileName(void) const
	{
		static std::string ccbiFileName = "WishPage.ccbi";
		return ccbiFileName;
	}
	bool mIsInitData;
private:	
};
