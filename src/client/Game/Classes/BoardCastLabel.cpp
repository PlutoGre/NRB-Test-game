
#include "stdafx.h"

#include "BoardCastLabel.h"
#include "DataTableManager.h"
#include "StringConverter.h"
#include "GameMaths.h"
MoveBoardCastLabel::MoveBoardCastLabel()
{
	mLabelFirst = NULL;
	mFontSize = 20;
	mLabelSecond = NULL;
	mDimensionsFirst = CCSizeMake(0,0);
	mDimensionsSecond = CCSizeMake(0,0);
	mAlignment = kCCTextAlignmentLeft;
	mFontName = NULL;
	mFontSize = 20;
}
MoveBoardCastLabel::~MoveBoardCastLabel()
{
	if(mLabelFirst)
	{
		mLabelFirst->release();
		mLabelFirst = NULL;
	}
	if(mLabelSecond)
	{
		mLabelSecond->release();
		mLabelSecond = NULL;
	}
}

void MoveBoardCastLabel::init(int x,int y,int width,int height)
{
	mPosition.x = x;
	mPosition.y = y + 4;
	mRect.setRect(0,0,width,height+4);
}
void MoveBoardCastLabel::initStr(CCSize dimensions,CCTextAlignment hAlignment,const char* fontName,int fontSize)
{
	mCurrIndex=0;
	if(mLabelFirst)
	{
		mLabelFirst->release();
	}
	if(mLabelSecond)
	{
		mLabelSecond->release();
	}
	mDimensionsFirst = dimensions;
	mAlignment = hAlignment;
	mFontName = fontName;
	mFontSize = fontSize;
    mStartFirstX=-mRect.size.width,mStartSecondX=-mRect.size.width;
   initFirstLabel();
   initSecondLabel();
}

void MoveBoardCastLabel::initFirstLabel()
{
	getNextMsg();
	if(!mIsHaveBoardCast)
	{
		return;
	}
	mDimensionsFirst.width=getRows(msg.c_str());
	mDimensionsFirst.height = 24;
	
	mLabelFirst = CCLabelTTF::create(msg.c_str(),mFontName, mFontSize,CCSizeMake(mDimensionsFirst.width,24),
		mAlignment, kCCVerticalTextAlignmentTop);
	mLabelFirst->setAnchorPoint(ccp(0,0));
	mLabelFirst->setColor(StringConverter::parseColor3B( VaribleManager::Get()->getSetting("BoardCastLabel_Color")));
	mLabelFirst->retain();
	mPosition.y -= (mRect.size.height+2) ;
	mLabelFirst->setTextureRect(CCRectMake(-mRect.size.width,0,mRect.size.width,mRect.size.height));
	mLabelFirst->setPosition(mPosition);
	addChild(mLabelFirst);
}


void MoveBoardCastLabel::initSecondLabel()
{
	getNextMsg();
	if(!mIsHaveBoardCast)
	{
		return;
	}
	mDimensionsSecond.width=getRows(msg.c_str());
	mDimensionsSecond.height = 24;
	mLabelSecond = CCLabelTTF::create(msg.c_str(),mFontName, mFontSize,CCSizeMake(mDimensionsSecond.width,24),
		mAlignment, kCCVerticalTextAlignmentTop);
	mLabelSecond->setAnchorPoint(ccp(0,0));
	mLabelSecond->setColor(StringConverter::parseColor3B( VaribleManager::Get()->getSetting("BoardCastLabel_Color")));
	mLabelSecond->retain();
	mLabelSecond->setTextureRect(CCRectMake(-mRect.size.width,0,mRect.size.width,mRect.size.height));
	mLabelSecond->setPosition(mPosition);
	mLabelSecond->setVisible(false);
	addChild(mLabelSecond);
}

void MoveBoardCastLabel::changeFirstLabel()
{
	getNextMsg();
	if(!mIsHaveBoardCast)
	{
		return;
	}
	mDimensionsFirst.width=getRows(msg.c_str());
	mLabelFirst->setString(msg.c_str());
	mLabelFirst->setTextureRect(CCRectMake(-mRect.size.width,0,mRect.size.width,mRect.size.height));
	mLabelFirst->setDimensions(mDimensionsFirst);
}
void MoveBoardCastLabel::changeSecondLabel()
{
	getNextMsg();
	if(!mIsHaveBoardCast)
	{
		return;
	}
	mDimensionsSecond.width=getRows(msg.c_str());
	mLabelSecond->setString(msg.c_str());
	mLabelSecond->setTextureRect(CCRectMake(-mRect.size.width,0,mRect.size.width,mRect.size.height));
	mLabelSecond->setDimensions(mDimensionsSecond);
}

void MoveBoardCastLabel::getNextMsg()
{
	if(ServerDateManager::Get()->getBoardCastInfoListTotalNum()==0)
	{
		mIsHaveBoardCast=false;
		return;
	}
	else
	{
		mIsHaveBoardCast=true;
	}
	BoardCastInfoMapIterator mIt=ServerDateManager::Get()->getBoardCastInfoIterator();
	int i=0;
	if(mCurrIndex==ServerDateManager::Get()->getBoardCastInfoListTotalNum())
	{
		mCurrIndex=0;
	}
	while(mIt.hasMoreElements())
	{
		if(i==mCurrIndex)
		{
			mBoardCastInfo=mIt.getNext();
			++mCurrIndex;
			break;
		}
		mIt.getNext();
		i++;
	}
	getCurrMsgInfo();
}

void MoveBoardCastLabel::getCurrMsgInfo()
{
	char msgKey[256];
	std::list<std::string> replaceList;
	
	if(mBoardCastInfo)
	{
		replaceList.push_back(mBoardCastInfo->name);
		if(mBoardCastInfo->itemType==1)
		{
			if(mBoardCastInfo->params.size()>0)
			{
				const SkillItem* skillItem=SkillTableManager::Get()->getSkillItemByID(*mBoardCastInfo->params.begin());
				replaceList.push_back(skillItem->name);
			}
			sprintf(msgKey,"@BoardcastSkill");
		}
		else if(mBoardCastInfo->itemType==2)
		{
			if(mBoardCastInfo->params.size()>1)
			{
				if(*mBoardCastInfo->params.begin()==1)
				{
					const EquipItem* equipItem=EquipTableManager::Get()->getEquipItemByID(*(mBoardCastInfo->params.begin()+1));
					replaceList.push_back(equipItem->name);
					replaceList.push_back(Language::Get()->getString("@EquipLabel"));
				}
				else
				{
					const SkillItem* skillItem=SkillTableManager::Get()->getSkillItemByID(*(mBoardCastInfo->params.begin()+1));
					replaceList.push_back(skillItem->name);
					replaceList.push_back(Language::Get()->getString("@SkillLabel"));
				}
			}
			sprintf(msgKey,"@BoardcastOpenTools");
		}
		else if(mBoardCastInfo->itemType==3)
		{
			if(mBoardCastInfo->params.size()>1)
			{
				if(*mBoardCastInfo->params.begin()==1)
				{
					const EquipItem* equipItem=EquipTableManager::Get()->getEquipItemByID(*(mBoardCastInfo->params.begin()+1));
					replaceList.push_back(equipItem->name);
					replaceList.push_back(Language::Get()->getString("@EquipLabel"));
				}
				else
				{
					const SkillItem* skillItem=SkillTableManager::Get()->getSkillItemByID(*(mBoardCastInfo->params.begin()+1));
					replaceList.push_back(skillItem->name);
					replaceList.push_back(Language::Get()->getString("@SkillLabel"));
				}
			}
			sprintf(msgKey,"@BoardcastKissGod");
		}
		else if(mBoardCastInfo->itemType==4)
		{
			if(mBoardCastInfo->params.size()>0)
			{
				const DiscipleItem* discipleItem=DiscipleTableManager::Get()->getDiscipleItemByID(*(mBoardCastInfo->params.begin()));
				replaceList.push_back(discipleItem->name);
				sprintf(msgKey,"@BoardcastRecruit");
			}
		}
		else if(mBoardCastInfo->itemType==5)
		{
			if(mBoardCastInfo->params.size()>1)
			{
				const SkillItem* skillItem=SkillTableManager::Get()->getSkillItemByID(*(mBoardCastInfo->params.begin()));
				replaceList.push_back(skillItem->name);
				replaceList.push_back(StringConverter::toString(*(mBoardCastInfo->params.begin()+1)));
				sprintf(msgKey,"@BoardcastUpgradeSkill");
			}
		}
		else if(mBoardCastInfo->itemType==6)
		{
			if(mBoardCastInfo->params.size()>1)
			{
				const EquipItem* equipItem=EquipTableManager::Get()->getEquipItemByID(*mBoardCastInfo->params.begin());
				replaceList.push_back(equipItem->name);
				replaceList.push_back(StringConverter::toString(*(mBoardCastInfo->params.begin()+1)));
				sprintf(msgKey,"@BoardcastUpgradeEquip");
			}
		}
	}
	else
	{
		sprintf(msgKey," ");
	}
	
	msg=GameMaths::replaceStringWithStringList(Language::Get()->getString(msgKey),&replaceList).c_str();
}


void MoveBoardCastLabel::run()
{
	update();
}

void MoveBoardCastLabel::update()
{
	if(!mIsHaveBoardCast)
	{
		return;
	}
	if(mLabelFirst->isVisible())
	{
		mLabelFirst->setTextureRect(CCRectMake(mStartFirstX++,0,mRect.size.width,mRect.size.height));
		if(mDimensionsFirst.width-mStartFirstX <=mRect.size.width)
		{
			mLabelSecond->setVisible(true);
		}
		if(mDimensionsFirst.width-mStartFirstX<=0)
		{
			mStartFirstX=-mRect.size.width;
			mLabelFirst->setVisible(false);
			changeFirstLabel();
		}
	}
	if(mLabelSecond->isVisible())
	{
		mLabelSecond->setTextureRect(CCRectMake(mStartSecondX++,0,mRect.size.width,mRect.size.height));
		if(mDimensionsSecond.width-mStartSecondX<=mRect.size.width)
		{
			mLabelFirst->setVisible(true);
		}
		if(mDimensionsSecond.width-mStartSecondX<=0)
		{
			mStartSecondX=-mRect.size.width;
			mLabelSecond->setVisible(false);
			changeSecondLabel();
		}
	}
}

int  MoveBoardCastLabel::getRows(const char* str)	
{
	int str_bytes = strlen(str);
	return str_bytes*mFontSize*0.5;		
}
