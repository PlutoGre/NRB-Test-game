#pragma once

#include "MainFrame.h"
#include "CCBManager.h"
#include "CustomPage.h"


class BuyPropsCountPage
	: public CustomPage
	, public State<MainFrame>
{
public:
	BuyPropsCountPage(void);
	~BuyPropsCountPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT_MESSAGE;}

	CREATE_FUNC(BuyPropsCountPage);
	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);
	virtual void load(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	void setProductID(int id){mID = id;}
	void setBuyOneOnly(bool only){mBuyOneOnly = only;}//no usage!!!
private:
	int mCount;
	int mID;
	int mTotalPrice;
	bool mBuyOneOnly;
	int mCanBuyMaxCount;
	int mEnoughCount;

	void caculatePrice();
	bool checkGoldEnough();
	void caculateEnoughCount();
};

