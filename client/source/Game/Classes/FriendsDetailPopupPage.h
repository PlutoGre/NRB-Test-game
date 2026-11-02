#pragma once
#include "cocos2d.h"
#include "cocos-ext.h"
#include "MainFrame.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "GameMessages.h"
enum NODE_TYPE
{
	LEAVE_MSG=1,
	ENEMY=2,
	Friend_Message=4,
};

enum ENEMY_TYPE
{
	Friend_RANKING=2,
	Friend_GONGFU=3,
};
class FriendsDetailPopupPage
	: public CustomPage
	, public CCBContainer::CCBContainerListener
	, public MessageHandler
{
public:
	FriendsDetailPopupPage(void);
	~FriendsDetailPopupPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(FriendsDetailPopupPage);
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

	void initDetailContent();

	void switchNode();

private:	
	void _clearNode();
	typedef	std::list<std::pair<NODE_TYPE,cocos2d::CCNode*> >  NodeList;
	NodeList mNodeList;
	cocos2d::CCLabelBMFont* mTitle;
	unsigned int mIndex;
	unsigned int mUserId;
	std::string mUserName;
	unsigned int mType;
	std::string mInputStr;
	int mCurrNodeType;
};
