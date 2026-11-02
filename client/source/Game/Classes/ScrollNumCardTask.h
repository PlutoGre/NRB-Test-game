#pragma once
#include "AdventureBase.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "CCBManager.h"
#include "ScrollNumCard.h"

class ScrollNumCardTask
{
public:

	ScrollNumCardTask(int length);

	~ScrollNumCardTask();

	void setScrollNumCard(cocos2d::extension::CCScrollView* scrollNumCard,int roundNum,int endNum,int cardNum);

	void scrollContentView(int speed,int fontSize,int positionOffset,bool countFlag,int cardNum);


private:
	cocos2d::extension::CCScrollView* mScrollNum;
	CCNode* mScrollNumRootNode;
	CCNode* mScrollNumRootNode1;

	std::string* scrollContentArray;
	int length;
	int offset;

	ScrollNumCard** scrollNumCardArr;


//	std::string** arr;






};