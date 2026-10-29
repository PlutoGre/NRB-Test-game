#pragma once
#include "MainFrame.h"
#include "StateMachine.h"
#include "CustomPage.h"
enum Popup_Not_Enough_type
{
	PowerNotEnough,
	VitalityNotEnough,
	SpendDiamondClearCoolTime
};
class PopupNotEnoughPage
	: public CustomPage
	, public State<MainFrame>
{
public:
	PopupNotEnoughPage(void);
	~PopupNotEnoughPage(void);
	virtual PAGE_TYPE getPageType() {return PT_CONTENT;}
	CREATE_FUNC(PopupNotEnoughPage);
	virtual void Enter(MainFrame*);
	virtual void Execute(MainFrame*);
	virtual void Exit(MainFrame*);
	virtual void load(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	void showPage(Popup_Not_Enough_type _type);

	static void Pop_NotEnough_Box(Popup_Not_Enough_type _type);
private:
	void initPage();
	Popup_Not_Enough_type mType;
	std::string mTitleKey;
	std::string mMsgKey;
	std::string mPropNameStr;
	std::string mPropIconStr;
	std::string mPropDescribeStr;
	std::string mPriceStr;
	std::string mFunction1Key;
	std::string mFunction2Key;
	int mDiamonToClearTime;//add by xinghui 2013/7/1
};
#define  POP_NOT_ENOUGH_BOX(mType) dynamic_cast<PopupNotEnoughPage*>(CCBManager::Get()->getPage("PopupNotEnoughPage"))->showPage(mType);

