#pragma once
#include "cocos2d.h"
#include "cocos-ext.h"
#include "MainFrame.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "GameMessages.h"

class BuyPropPopupPage
	: public CustomPage
	, public CCBContainer::CCBContainerListener
	, public MessageHandler
{
public:
	BuyPropPopupPage(void);
	~BuyPropPopupPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(BuyPropPopupPage);
	//this will execute when the state is entered
	virtual void Enter();

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	virtual void onReceiveMassage(const GameMessage * message);
	void initProp();
	void refreshItemInfo(void);
	void refreshUserInfo(void);
private:	
	cocos2d::CCLabelBMFont* mMessage;
	cocos2d::CCLabelBMFont* mTitle;
	cocos2d::CCSprite* mPropIcon;
	cocos2d::CCLabelBMFont* mName;
	cocos2d::CCLabelBMFont* mDes;
	cocos2d::CCLabelBMFont* mNum;
	unsigned int mGId;
};
