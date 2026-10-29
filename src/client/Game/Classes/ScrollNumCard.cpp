
#include "stdafx.h"

#include "ScrollNumCard.h"

USING_NS_CC;

ScrollNumCard::ScrollNumCard()
	:normalBMFont(0),countFlag(true)

{

}

ScrollNumCard::ScrollNumCard(cocos2d::extension::CCScrollView* mScrollNum,std::string* scrollContentArray,int length)
	:normalBMFont(0)
{
	this->mScrollNum = mScrollNum;
	this->offset = 0;

	this->length = length;
	this->scrollContentArray = scrollContentArray;
	for(int i=0;i<length;i++){
		this->scrollContentArray[i] = *(scrollContentArray+i);
	}

};

ScrollNumCard::~ScrollNumCard(){
	
	removeFromParent();

	mScrollNumRootNode->removeAllChildren();
	//for(int i=0;i<length;i++){	 
	//	delete normalBMFont[i];
	//}
	delete[] normalBMFont;
	delete normalBMFontSingle;
}

void ScrollNumCard::outputContentArray(){
	for(int i=0;i<length;i++){
		std::string scrollContentStr(*(scrollContentArray+i));
	}
}

void ScrollNumCard::scrollContentView(int speed,int fontSize,int positionOffset,bool countFlag){
	
	int positionX = mScrollNum->getViewSize().width/2;
	int positionY = mScrollNum->getViewSize().height/2;

	mScrollNumRootNode = mScrollNum->getContainer();
	this->countFlag = countFlag;

	if(getParent())
		removeFromParent();
	mScrollNumRootNode->addChild(this);

	std::string normalFont("1");
	if(normalBMFont)
	{
		delete[] normalBMFont;
	}
	normalBMFont = new CCLabelBMFont*[length];
	
	for(int i=0;i<length;i++){

		normalBMFont[i] = (CCLabelBMFont*)  CCLabelBMFont::create("T","Lang/heiOutline24.fnt",10);  

		normalFont = *(scrollContentArray+i);
		normalBMFont[i]->setString(normalFont.c_str());
		normalBMFont[i]->setColor(ccRED);
		normalBMFont[i]->setScale(fontSize);

		pos = ccp(positionX,positionY-positionOffset*i);
		mScrollNumRootNode->addChild(normalBMFont[i]);
		normalBMFont[i]->setPosition(pos);

	}

	pos = ccp(0,0);
	CCSize size = CCSize(0,0);
	mScrollNum->setContentSize(size);

	mScrollNum->setContentOffset(ccp(0,0));
	mScrollNum->setContentOffsetInDuration(ccp(0,positionOffset*(length-1)),length*0.1*speed);
	if(countFlag){
		BlackBoard::Get()->fortuneComeSum++;
		BlackBoard::Get()->fortuneComeFlag=1;
	}

	float time = StringConverter::parseReal(VaribleManager::Get()->getSetting("FortuneComeRewardSpeed"));
	
	scheduleOnce(schedule_selector(ScrollNumCard::removeArr),length*0.1*speed*time);
//	mScrollNumRootNode->schedule(schedule_selector(ScrollNumCard::scrollRound,1),1.5f);

//	mScrollNumRootNode->unschedule(schedule_selector(ScrollNumCard::deleteBMFont));

}

void ScrollNumCard::removeArr(float a){
//	mScrollNum->unschedule(schedule_selector(ScrollNumCard::removeArr));

	mScrollNumRootNode->removeAllChildren();
	mScrollNumRootNode->addChild(this);

	delete[] normalBMFont;
	normalBMFont = 0;


	int positionX = mScrollNum->getViewSize().width/2;
	int positionY = mScrollNum->getViewSize().height/2;
	positionOffset = 150;

	std::string normalFont("1");
	normalBMFontSingle = (CCLabelBMFont*)  CCLabelBMFont::create("T","Lang/heiOutline24.fnt",10);  

	normalFont = *(scrollContentArray+length-1);
	normalBMFontSingle->setString(normalFont.c_str());
	normalBMFontSingle->setColor(ccRED);
	normalBMFontSingle->setScale(3);

	pos = ccp(positionX,positionY-positionOffset*(length-1));
	normalBMFontSingle->setPosition(pos);

	mScrollNumRootNode->addChild(normalBMFontSingle);

	if(countFlag){
		BlackBoard::Get()->fortuneComeSum--;
		BlackBoard::Get()->fortuneComeFlag=1;

	}

	return;
}

