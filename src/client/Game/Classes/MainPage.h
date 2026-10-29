#pragma once


#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "CCBContainer.h"
#include "CCBManager.h"
#include "CustomPage.h"

class MainPage
	: public CustomPage
	, public State<MainFrame>
	, public MessageHandler
	, public CCBContainer::CCBContainerListener
{
private:
	enum BUTTON_TAG
	{
		FACE_BUTTON_START = 0,
		FACE_BUTTON_END = 10,
	};
	friend class InstructionStepChooseName;

	void _refreshTitle(void);
	void _refreshBattle(void);
	typedef std::map<unsigned int, CCBContainer* > FrameBtnIconList;
	FrameBtnIconList mFrameBtnList;
	void clearAllFrameBtn();

	float mExpbarInitScale;
public:
	MainPage(void);
	~MainPage(void);
	
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(MainPage);

	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	virtual void load(void);

	virtual void onReceiveMassage(const GameMessage * message);
private:
	void _refreshPageSymbol();
};

