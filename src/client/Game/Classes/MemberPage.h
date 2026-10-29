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
#define MEMBERTABLENGTH 2
struct MemberEntity 
{
public:
	unsigned int id;
	unsigned int quality;
	unsigned int level;
	unsigned int memberType;
	unsigned int memberValue;
	MemberEntity(void){}

};
class MemberPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
{
public:
	MemberPage(void);
	~MemberPage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(MemberPage);
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

	enum MP_TYPE_TAG
	{
		MEMBER=1,
		SOUL=2,
	};

	void toggleContentView();

	typedef std::map<unsigned int,MemberEntity* > MEMBERENTITY_MAP;
	static bool cmp(const MEMBERENTITY_MAP::iterator a,const MEMBERENTITY_MAP::iterator b);
	
private:

	typedef std::list<MEMBERENTITY_MAP::iterator> MEMBERENTITY_LIST;

	typedef std::map<unsigned int,cocos2d::CCMenuItemSprite* > TAB_BUTTON_LIST;

	TAB_BUTTON_LIST mMPTabList;

	MEMBERENTITY_MAP mMPMemberList;

	MEMBERENTITY_MAP mMPSoulList;

	void loadMemberConfig();

	void switchMPTab();

	unsigned int mCurrTabIndex;

	inline MEMBERENTITY_LIST* cloneEntityList(MemberPage::MEMBERENTITY_MAP* entityList);

};