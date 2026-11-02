#pragma once
#include "CustomPage.h"
#include "MainFrame.h"
#include "GetPropManager.h"

class PopUpSurprisePage
	: public CustomPage
	, public State<MainFrame>
{
private:
	void updatePage();
	GetPropManager::TYPE mType;
	int mId;
	int mCount;
public:
	PopUpSurprisePage(void):mId(0),mType(GetPropManager::EQUIPMENT){}
	~PopUpSurprisePage(void){};

	virtual PAGE_TYPE getPageType() {return PT_CONTENT_MESSAGE;}
	CREATE_FUNC(PopUpSurprisePage);

	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);

	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	void setInfo(GetPropManager::TYPE _type, int id, int _count = 1){mId = id;mType = _type;mCount = _count;}

};

