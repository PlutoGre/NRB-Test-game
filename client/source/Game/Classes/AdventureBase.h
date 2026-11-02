#pragma once
#include "cocos2d.h"
#include "cocos-ext.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "Language.h"
#include "CustomPage.h"
#include "GameMessages.h"
#include "GamePackets.h"
#include "DataTableManager.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "MessageBoxPage.h"
#include "BlackBoard.h"
#include "CCBScriptContainer.h"
class AdventureBase
	: public CCBContainer::CCBContainerListener
{
public:
	AdventureBase(unsigned int id);
	virtual ~AdventureBase();
	virtual void init(void);
	virtual void refreshContent(void) = 0;
	virtual CCBContainer* getContainerNode(){return mContentContainer;};

	unsigned int getContentID(){return mID;};
	virtual void loadData(void)=0;
	virtual void updatePage(bool isSelect,float dt=0.0f)=0;
	virtual bool getIsInitData(){return mIsInitData;};
	virtual void setIsInitData(bool _isInit){ mIsInitData=_isInit;};
	virtual void exitPage(void)=0;
protected:
	virtual const std::string& getCcbiFileName(void) const = 0;
	unsigned int mID;
	CCBContainer* mContentContainer;
	bool mIsInitData;
	cocos2d::CCObject* getObjectByVariableName(const std::string& variableName);
};

class AdventureScriptBase : public AdventureBase
{
public:
	AdventureScriptBase(unsigned int id, std::string pagename)
		: AdventureBase(id)
		,mPageName(pagename)
	{
	};
	virtual void init(void)
	{
		mCCBScriptContainer = CCBScriptContainer::create(mPageName, getContentID());
		mCCBScriptContainer->retain();
		mContentContainer = mCCBScriptContainer;
		mCCBScriptContainer->load();
	};
	virtual void refreshContent(void) {};
	virtual void loadData(void){mCCBScriptContainer->Enter(NULL);};
	virtual void updatePage(bool isSelect,float dt=0.0f)
	{
		if(isSelect)
			mCCBScriptContainer->Execute(NULL);
		else
			exitPage();
	};
	virtual void exitPage(void){mCCBScriptContainer->Exit(NULL);};

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){};
	virtual void onAnimationDone(const std::string& animationName){};

	//std::string	getPageName() { return mPageName;};
	
protected:
	virtual const std::string& getCcbiFileName(void) const {return mPageName;};
private:
	std::string mPageName;
	CCBScriptContainer* mCCBScriptContainer;
};
