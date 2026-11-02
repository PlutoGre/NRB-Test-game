#pragma once
#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "CustomPage.h"

class MarketPage: public CustomPage
	, public State<MainFrame>
	, public MessageHandler
{
public:
	MarketPage(void);
	~MarketPage(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}
	CREATE_FUNC(MarketPage);
	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);

	virtual void load(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	virtual void onReceiveMassage(const GameMessage * message);

private:
	void _refreshActivityTip(const std::string guideName,std::string _key);
	void _saveActivityGuideStatus(const std::string guideName,std::string key);
	int mEclipsedTime;
	float mExpbarInitScale;
	void _updateTimes();
	void translateTime(int seconds, int id,int Count, int maxCount,int price);

	enum BUYTYPE
	{
		ORDINARL = 1,
		USING_RMB = 2,
	};
	BUYTYPE mCanBuy[3];
	int mPrice[3];
	int lastFrameRMB;
	void _refreshTitle();
	bool _checkEnoughGold(unsigned int id);
	bool mHasActivity;
};

