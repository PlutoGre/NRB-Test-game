#pragma once


#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "CCBContainer.h"
#include "CCBManager.h"
#include "CustomPage.h"

/** 首页：参考版海面布局、实时角色信息与现有功能入口。 */
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
	// 新首页保留旧 CCB 变量及回调，角色数据仍由现有消息同步。
	std::string mHomeHeaderSignature;
	int mHomePopup;
	void _buildReferenceHome();
	void _refreshReferenceTitle();
	void _refreshReferenceBattle();
	void _refreshReferenceShortcuts();
	void onReferenceHomeClick(cocos2d::CCObject* sender);
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

