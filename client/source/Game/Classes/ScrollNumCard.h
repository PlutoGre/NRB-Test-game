#pragma once
#include "AdventureBase.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "CCBManager.h"

class ScrollNumCard	: public CCNode
{
public:

	ScrollNumCard();
	ScrollNumCard(cocos2d::extension::CCScrollView* mScrollNum,std::string* scrollContentArray,int length);

	~ScrollNumCard();

	void outputContentArray();

	void scrollContentView(int speed,int fontSize,int positionOffset,bool countFlag);

	void removeArr(float);

private:
	cocos2d::extension::CCScrollView* mScrollNum;
	CCNode* mScrollNumRootNode;
	bool countFlag;

  	CCLabelBMFont** normalBMFont;

  	CCLabelBMFont* normalBMFontSingle;

	std::string* scrollContentArray;
	int length;
	int offset;

	CCPoint pos;
	int positionOffset;

};