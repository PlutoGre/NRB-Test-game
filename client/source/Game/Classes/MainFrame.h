#pragma once


#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "GamePrecedure.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "BoardCastLabel.h"
#include "GamePackets.h"
#include "lib91.h"
#include "libOS.h"

// class MainPage;
// class TeamPage;
using namespace cocos2d;

class MainFrame
	: public BasePage
	, public State<GamePrecedure>
	, public MessageHandler
    , public platformListener
	, public libOSListener
{
private:
	cocos2d::CCScene* mScene;


	StateMachine<MainFrame> *m_pStateMachine;

	cocos2d::CCNode * mNodeMid;
	cocos2d::CCNode * mNodeBack;
	cocos2d::CCNode * mNodeFront;
	cocos2d::CCNode * mNodeMsg;
	cocos2d::CCNode * mBroadcast;
	cocos2d::CCLayer * mNoTouch;

	BasePage* mCurShowPage;
	typedef std::map<std::string, BasePage* > PageList;
	PageList mPageList;

	void showCover(const std::string& pagename);
	void hideCover();

	void showMsg();
public:
	MainFrame(void);
	~MainFrame(void);

	static MainFrame* getInstance();
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	CREATE_FUNC(MainFrame);

	virtual void load(void);

	void showPage(const std::string& name);
	inline BasePage* getCurShowPage() {return mCurShowPage;}

	void pushPage(const std::string& name);
	void popPage(const std::string& name);
	void popPage(BasePage* page);
	void popAllPage();
	bool isPageInList(BasePage* page);
	bool isPageInList(const std::string& name);

	//this will execute when the state is entered
	virtual void Enter(GamePrecedure*);

	//this is the states normal update function
	virtual void Execute(GamePrecedure*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(GamePrecedure*);

	virtual void onReceiveMassage(const GameMessage * message);
	//void _setContent(cocos2d::CCNode* child, bool zero = true);

	void initLoadBoardCast();
	void loadBoardCast();
	MoveBoardCastLabel mBoardCastLabel;
	bool mIsInitBoardCast;
	void _loadPopPage();

	void showNoTouch();
	void hideNoTouch();

	void onPlatformLogout(libPlatform*);
	void onMessageboxEnter(int tag);

	void _refreshActivityTip(const std::string guideName,std::string key);
	
	void _saveActivityGuideStatus(const std::string guideName,std::string key);
	//
	CCScene* getRootSceneNode();
};

