#pragma once
#include "cocos2d.h"
#include "cocos-ext.h"
#include "MainFrame.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "GameMessages.h"
#include "libOS.h"
#include "GamePackets.h"

class LeaveMsgPage
	: public CustomPage
	, public CCBContainer::CCBContainerListener
	, public MessageHandler
	, public libOSListener
{
public:
	LeaveMsgPage(void);
	~LeaveMsgPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(LeaveMsgPage);
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
	virtual void onInputboxEnter(const std::string& content);
	void initLeaveMsg();
	void inputMethodCallBack(const std::string& content);
private:	
	cocos2d::CCLabelBMFont* mMessage;
	cocos2d::CCLabelBMFont* mTitle;
	unsigned int mUserId;
	std::string mUserName;
	std::string mInputStr;
	typedef  std::vector<std::string > GMKeyVec;
	GMKeyVec mGMList;
};