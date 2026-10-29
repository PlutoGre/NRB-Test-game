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

class ChangeNamePage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
	, public MessageHandler
	, public libOSListener
{
public:
	ChangeNamePage(void);
	~ChangeNamePage(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(ChangeNamePage);
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	virtual void onReceiveMassage(const GameMessage * message);
	virtual void onInputboxEnter( const std::string& content);
	void inputMethodCallBack(const std::string& content);
	void randomName();
private:	
	void _readMap();
	std::string mName;
	std::map<int, std::string> mNameMap;
	bool needReSend;
};