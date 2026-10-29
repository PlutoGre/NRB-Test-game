#pragma once
#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "CustomPage.h"

class BuyPropsPage: public CustomPage
	, public State<MainFrame>
	, CCBContainer::CCBContainerListener
	, public MessageHandler
{
public:
	BuyPropsPage(void);
	~BuyPropsPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}
	CREATE_FUNC(BuyPropsPage);
	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);

	virtual void load(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onReceiveMassage(const GameMessage * message);
	enum PAGE
	{
		PAGE_TOOL,
		PAGE_GIFT,
	};

	void setPage(PAGE page){mShowPage = page;}
private:
	void _refreshActivityTip();
	void createTools();
	void createGifts();
	PAGE mShowPage; 
	static bool _sSort(unsigned int id1,unsigned int id2);
	int mLastFrameRMB;
	void _refreshTitle();
	float mExpbarInitScale;
};

