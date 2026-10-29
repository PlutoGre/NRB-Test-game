#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "GameMaths.h"
#include "GameMessages.h"
#include "ContentBase.h"


typedef void (*onSure)();

class GoodsPopupPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
{
public:
	GoodsPopupPage(void);
	~GoodsPopupPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(GoodsPopupPage);
	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);

	virtual void load(void);

	void refreshPage(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);;

	void setSureFun(onSure sure)
	{
		mSure = sure;
	}
	void showPage(unsigned int type,unsigned int itemId,unsigned int count,unsigned int partId);
	static void showGoodsPopup(unsigned int type,unsigned int itemId,unsigned int count,unsigned int partId);
private:
	unsigned int mType;
	unsigned int mItemId;
	unsigned int mPartId;
	unsigned int mCount;
	onSure mSure;
};

#define ShowGoodsPopup(type,itemId,count,partId) dynamic_cast<GoodsPopupPage*>(CCBManager::Get()->getPage("GoodsPopupPage"))->showPage(type,itemId,count,partId);