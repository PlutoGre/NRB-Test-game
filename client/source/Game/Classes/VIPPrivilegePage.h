#pragma once
#include "cocos2d.h"
#include "cocos-ext.h"
#include "MainFrame.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "Language.h"
#include "DataTableManager.h"
#include "ContentBase.h"
#include "GameMessages.h"
#include "StringConverter.h"

class VIPPrivilegeContentItem
	: public ContentBase{
public:
	VIPPrivilegeContentItem(unsigned int id):ContentBase(id){};
	virtual ~VIPPrivilegeContentItem(){};
	virtual void refreshContent(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	int describeIndex;
protected:
	virtual const std::string& getCcbiFileName(void) const{
		static std::string ccbiFileName = "VIPPrivilegeContentItem.ccbi";
		return ccbiFileName;
	}
};

class VIPPrivilegeContent
	: public ContentBase
	,public CustomPage{
		//	, public CustomPage{
public:
	VIPPrivilegeContent(unsigned int id):ContentBase(id){};
	virtual ~VIPPrivilegeContent(){};
	virtual void refreshContent(void);
	virtual void onMenuItemAction(const std::string& itemName,cocos2d::CCObject* sender , int tag);

	void rebuildAllItem(int mID);
	void clearAllItem(void);
	void buildItem(int mID);
	void addItem(ContentBase* item);

	virtual PAGE_TYPE getPageType(){return PT_CONTENT;};
	void load(void){

	};

	bool initContentBase(void)
	{
		mContentContainer = CCBContainer::create();
		mContentContainer->loadCcbiFile(getCcbiFileName());
		mContentContainer->setListener(this);
		mContentContainer->retain();
		refreshContent();
		return true;
	};

protected:
	virtual const std::string& getCcbiFileName(void) const{
		static std::string ccbiFileName ;
		if(mID==ServerDateManager::Get()->getUserBasicInfo().viplevel){
			ccbiFileName = "VipContentUser.ccbi";
		}else{
			ccbiFileName = "VipContent.ccbi";
		}
		return ccbiFileName;
	}
private:
	typedef std::vector<ContentBase * > ContentItemList;
	ContentItemList mContentItemList;
	CCNode* itemMembersRootNode;
	CCObject* itemMembers;
	float itemStartPos;
	unsigned int mLineIndex;
};

class VIPPrivilegePage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener{
public:
	VIPPrivilegePage(void);
	~VIPPrivilegePage(void);

	virtual PAGE_TYPE getPageType(){return PT_CONTENT;}

	CREATE_FUNC(VIPPrivilegePage);

	void Enter(MainFrame*);

	void initContent();

	virtual void Execute(MainFrame*);

	virtual void Exit(MainFrame*);

	virtual void load(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	void buildItem(void);
	void addItem(ContentBase* item);
	void rebuildAllItem(void);
	void clearAllItem(void);
	void refreshAllItem(void);
	void removeItem(ContentBase* item);
	void refreshPage(void);
	void refreshTitle(void);

private:
	typedef std::vector<ContentBase * > ContentList;
	ContentList mContentList;
	CCNode* membersRootNode;
	cocos2d::extension::CCScrollView* members;


};