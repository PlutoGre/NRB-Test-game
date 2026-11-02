
#include "stdafx.h"

#include "ScrollNumCardTask.h"

USING_NS_CC;

ScrollNumCardTask::ScrollNumCardTask(int length){
	scrollNumCardArr = new ScrollNumCard*[length];
//	arr = new std::string* [length];
//	normalBMFont = new CCLabelBMFont*;
};

ScrollNumCardTask::~ScrollNumCardTask(){
	CCLOG("~ScrollNumCardTask-------------------------------");
	delete [] scrollNumCardArr;
//	delete [] arr;
	/*
//	mScrollNumRootNode->removeAllChildren();
	for(int i=0;i<length;i++){	 
		delete normalBMFont[i];
	}
	*/
}
void ScrollNumCardTask::setScrollNumCard(cocos2d::extension::CCScrollView* scrollNumCard,int roundNum,int endNum,int cardNum){
	
	std::string* arr = new std::string[roundNum*10 + endNum + 1];

	for(int i=0;i<roundNum;i++){
		for(int j=0;j<10;j++){
			arr[10*i+j] = StringConverter::toString(j);
		}
	}

	for(int i=0;i<=endNum;i++){
		arr[roundNum*10 + i] = StringConverter::toString(i);

	}

	this->scrollNumCardArr[cardNum] = new ScrollNumCard(scrollNumCard,arr,roundNum*10 + endNum + 1);
	

}

void ScrollNumCardTask::scrollContentView(int speed,int fontSize,int positionOffset,bool countFlag,int cardNum){
	this->scrollNumCardArr[cardNum]->scrollContentView(speed,fontSize,positionOffset,countFlag);

}
