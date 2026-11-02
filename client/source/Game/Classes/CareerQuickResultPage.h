#pragma once
#include "stdafx.h"
#include "CustomPage.h"
#include "MainFrame.h"
#include "StateMachine.h"
class CareerQuickResultPage : public CustomPage, public State<MainFrame>
{
public:
	CareerQuickResultPage(void);
	~CareerQuickResultPage(void);

	CREATE_FUNC(CareerQuickResultPage);

	virtual void Enter( MainFrame* );

	virtual void Execute( MainFrame* );

	virtual void Exit( MainFrame* );

	virtual PAGE_TYPE getPageType(){return PT_CONTENT;};

	virtual void load( void );

	void setShowTextList(std::list<std::string> showText);

	void setAccidentText(std::list<std::string> accidentText);

	void setTitle(std::string title);

	void setPropertyText(std::list<std::pair<std::string, std::string>  > propertyText);

	virtual void onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender );

private:
	std::list<std::string> mShowText;
	std::list<std::string> mAccidentText;
	std::list<std::pair<std::string, std::string> > mPropertyText;
	std::string mTitle;
	cocos2d::extension::CCScrollView* mViewScroll;

};

