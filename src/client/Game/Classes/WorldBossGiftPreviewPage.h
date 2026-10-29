#pragma once
#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "CustomPage.h"

class WorldBossGiftPreviewPage
	: public CustomPage
	, public State<MainFrame>
{
public:

	struct PackContent
	{
		std::string name;
		std::string icon;
		int number;
		int quality;
		PackContent(const std::string& _name, const std::string& _icon, int number,int _quality):name(_name),icon(_icon),number(number),quality(_quality){}
		PackContent(const PackContent& other){name = other.name;icon = other.icon;number = other.number; quality = other.quality;}
		PackContent& operator=(const PackContent& other){name = other.name;icon = other.icon;number = other.number; quality = other.quality;}
	};

	typedef std::list<PackContent> PACKCONTENT_LIST;
private:
	PACKCONTENT_LIST mList;
	std::string mTitle;
	std::string mPackMsg;
	void updateContent();

public:
	WorldBossGiftPreviewPage(void);
	~WorldBossGiftPreviewPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT_MESSAGE;}
	CREATE_FUNC(WorldBossGiftPreviewPage);
	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);

	virtual void load(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);


	void setTitle(const std::string& title);
	void setPackMsg(const std::string& packMsg);
	void setContent(const PACKCONTENT_LIST& list);

	static void addContent(std::string name, std::string icon, int number, int quality);
	static void showPage(const std::string& title);
};

