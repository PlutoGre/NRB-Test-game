#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "GameMessages.h"
enum OprationType
{
	NONE_OPRATION,
	ADD_OPRATION,
	OPEN_OPRATION,
	REDIRECT_OPTION,
	POPUP_OPRATION,
};
class PackageItem
	: public CCBContainer::CCBContainerListener
{
public:
	PackageItem(unsigned int id);;
	~PackageItem();

	void refreshItem(void);

	CCBContainer* getContainerNode(){return mItemContainer;};

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

private:
	unsigned int mID;
	CCBContainer* mItemContainer;
};

class PackagePage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
	,public MessageHandler
{
public:
	PackagePage(void);
	~PackagePage(void);

	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}

	CREATE_FUNC(PackagePage);
	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);

	virtual void load(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag){};

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	void buildItem(void);
	void addItem(PackageItem* item);
	void rebuildAllItem(void);
	void clearAllItem(void);
	void refreshAllItem(void);
	void removeItem(PackageItem* item);
	//void refreshItem(PackageItem* item);
	void onReceiveMassage( const GameMessage * message );

private:
	static bool _sSort(unsigned int id1,unsigned int id2);
	void refreshTitle(void);

	typedef std::vector<PackageItem * > PackegeItemList;

	PackegeItemList mPackegeItemList;

	CCNode* membersRootNode;

	cocos2d::extension::CCScrollView* members;

	CCPoint posOffset;

	float mExpbarInitScale;
};
