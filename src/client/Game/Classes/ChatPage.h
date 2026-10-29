#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "CharacterConsts.h"
#include "GameMessages.h"
#include "GameMaths.h"
#include "GamePackets.h"
#include "libOS.h"
#define ChatNextTime 5
enum ChatType
{
	WORLD_CHAT=0,
	SOCIATY_CHAT=1,
};
class ChatPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
	, public MessageHandler
	, public libOSListener
{
public:
	ChatPage(void);
	~ChatPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}
	CREATE_FUNC(ChatPage);
	//this will execute when the state is entered
	virtual void Enter(MainFrame*);
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
	void toggleContentView();

private:
	static bool _sSort(unsigned int id1,unsigned int id2);
	float mDurationTime;
	int mSurplusSecTime;
	void _refreshSendBtnStatus();
	void _loginChatServer();
	void _quitChatServer();
	void loadConfig();
	void showChatContentView();
	std::list<std::pair<int,CCBContainer*> > mFriendsContainerList;
	unsigned int mTrumpetId;
	cocos2d::CCLabelBMFont* mPropNum;
	void refreshTrumpetNum();
	bool mCanSendMsg;
};