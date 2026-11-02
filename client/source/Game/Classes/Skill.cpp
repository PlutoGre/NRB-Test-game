
#include "stdafx.h"

#include "Skill.h"

#include <list>
#include "math.h"

#include "DataTableManager.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "Disciple.h"
#include "GameMaths.h"

Skill::Skill( unsigned int id, bool isItemID/* = false*/,bool isViewTeam )
	:mSkillInfo(NULL)
	,mSkillItem(NULL)
{
	mIsViewTeam=isViewTeam;
	if(isItemID)
	{
		mSkillItem = SkillTableManager::Get()->getSkillItemByID(id);
		if(!mSkillItem)
			CCLOG("Skill : SkillItem not find %d", id);
	}
	else
	{
		if(mIsViewTeam)
		{
			mSkillInfo = ServerDateManager::Get()->getViewTeamBattleInfoItem().getSkillInfoByID(id);
		}
		else
		{
			mSkillInfo = ServerDateManager::Get()->getSkillInfoByID(id);
		}
		if(mSkillInfo)
		{
			mSkillItem = SkillTableManager::Get()->getSkillItemByID(mSkillInfo->itemid);
			if(!mSkillItem)
				CCLOG("Skill : SkillItem not find %d", mSkillInfo->itemid);
		}
		else
		{
			CCLOG("Skill : SkillInfo not find %d", id);
			mSkillItem = SkillTableManager::Get()->getSkillItemByID(id);
			if(!mSkillItem)
				CCLOG("Skill : SkillItem really not find %d", id);
		}
	}
}

cocos2d::ccColor3B Skill::getFrameColor()
{
	std::string color = VaribleManager::Get()->getSetting("FrameColor_Quality" + StringConverter::toString(quality()));
	return StringConverter::parseColor3B(color);
}

cocos2d::CCSprite* Skill::getFrameNormalSpirte(std::string ext/*=""*/)
{
	cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("FrameNormal_ImageFile" + ext).c_str());
	sprite->setColor(getFrameColor());
	return sprite;
}

cocos2d::CCSprite* Skill::getFrameSelectedSpirte(std::string ext/*=""*/)
{
	cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("FrameSelected_ImageFile" + ext).c_str());
	sprite->setColor(getFrameColor());
	return sprite;
}

cocos2d::CCSprite* Skill::getFrameDisabledSpirte(std::string ext/*=""*/)
{
	cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("FrameDisabled_ImageFile" + ext).c_str());
	sprite->setColor(getFrameColor());
	return sprite;
}

std::string Skill::getQualityImageFile()
{
	return VaribleManager::Get()->getSetting("ImageFileQuality" + StringConverter::toString(quality()));
}

std::string Skill::getSkillAtkTypePic()
{
	return VaribleManager::Get()->getSetting("SkillAttack_ImageFile" + StringConverter::toString(type()));
}

std::string Skill::getSkillCarriedBy()
{
	UserBattleInfoListIterator itr = ServerDateManager::Get()->getUserBattleIterator();
	while (itr.hasMoreElements())
	{
		UserBattleInfo* info = itr.getNext();
		if(info->skill1 == id() || info->skill2 == id())
		{
			Disciple disciple(info->discipleId);
			return disciple.name();
		}
	}
	
	return "";
}

unsigned int Skill::getSkillInitValue()
{
	unsigned int quality = this->quality();
	unsigned int type = this->type();
	if(quality == 1)
	{
		if(type <= 2)
			return 664;
		else
			return 848;
	}
	else if(quality == 2)
	{
		if(type <= 2)
			return 376;
		else
			return 528;
	}
	else if(quality == 3)
	{
		if(type <= 2)
			return 192;
		else
			return 240;
	}
	else if(quality == 4)
	{
		if(type <= 2)
			return 72;
		else
			return 0;
	}

	return 0;
}

unsigned int Skill::getSkillValue()
{
	return ceil((1.5 * level() - 0.5) * this->getSkillInitValue());
}

std::string Skill::getSkillAtkType()
{
// 		1 	单体攻击
// 		2 	群体攻击
// 		3 	血量BUFF
// 		4 	攻击BUFF
// 		5 	防御BUFF
// 		6 	内力BUFF
	unsigned int type = this->type();
	switch(type)
	{
	case 1:
		return Language::Get()->getString("@Single_attack");
	case 2:
		return Language::Get()->getString("@Multi_attack");
// 	case 3:
// 		return "%";
// 	case 4:
// 		return "%";
// 	case 5:
// 		return "%";
// 	case 6:
// 		return "%";
	}
	return "";
}

bool Skill::allowUpgrade()
{
	return level() < 10;
}

unsigned int Skill::id()
{
	if(mSkillInfo)
		return mSkillInfo->id;

	return 0;
}

unsigned int Skill::itemID()
{
	if(mSkillInfo)
		return mSkillInfo->itemid;
	else if(mSkillItem)
		return mSkillItem->itemID;
	else
		return 0;
}

const std::string& Skill::name()
{
	if(mSkillItem)
		return mSkillItem->name;
	static std::string emptyString("");
	return emptyString;
}

unsigned int Skill::quality()
{
	if(!mSkillItem)
		return 0;
	return mSkillItem->quality;
}

unsigned int Skill::type()
{
	if(!mSkillItem)
		return 0;
	return mSkillItem->type;
}

std::string Skill::describe()
{
	if(!mSkillItem)
		return "";
    std::string s;
    int lines;
    int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("Skill_Describe"));
    GameMaths::stringAutoReturn(mSkillItem->describe, s, width, lines);
    return s;
}

unsigned int Skill::initQuantity()
{
	if(!mSkillItem)
		return 0;
	return mSkillItem->initQuantity;
}

unsigned int Skill::initGrowthRate()
{
	if(!mSkillItem)
		return 0;
	return mSkillItem->initGrowthRate;
}

std::string Skill::iconPic()
{
	if(!mSkillItem)
		return "";
	return mSkillItem->iconPic;
}

std::string Skill::bodyPic()
{
	if(!mSkillItem)
		return "";
	return mSkillItem->bodyPic;
}

std::string Skill::namePic()
{
	if(!mSkillItem)
		return "";
	return mSkillItem->namePic;
}

unsigned int Skill::level()
{
	if(mSkillInfo)
		return mSkillInfo->level;

	return 1;
}

float Skill::quantity()
{
	if(mSkillInfo)
		return mSkillInfo->quantity;
	else if(mSkillItem)
		return mSkillItem->initQuantity;
	return 0.0f;
}
std::string Skill::quantityStr()
{
	if(this->type()>=3)
	{
		return StringConverter::toString(((int)ceil(quantity()*10000))/100)+"%";
	}else
	{
		return StringConverter::toString((int)ceil(quantity()));
	}
	return "0";

}

unsigned int Skill::consume()
{
	if(mSkillInfo)
		return mSkillInfo->consume;

	return 0;
}


bool Skill::isBornSkill()
{
	if(mSkillInfo)
		return mSkillInfo->isBornSkill;

	return false;
}

bool Skill::isSkillCarriedBy()
{
	UserBattleInfoListIterator itr = ServerDateManager::Get()->getUserBattleIterator();
	while (itr.hasMoreElements())
	{
		UserBattleInfo* info = itr.getNext();
		if(info->skill1 == id() || info->skill2 == id())
			return true;
	}
	return false;
}

float Skill::getConsumeRateValue()
{
	return 20;
}

unsigned int Skill::getConsumeMoneyNeeded()
{
	return 100 * getQualityFactor() * ( pow(level(), 2.0));
}

unsigned int Skill::getQualityFactor()
{
	return 5 - quality();
}

unsigned int Skill::getConsumeExp()
{
	return getQualityFactor() * pow(2.0 ,level() -1.0);
}

unsigned int Skill::getUpgradeExp()
{
	return getQualityFactor() * pow((float)2.0 ,(float)level());
}

float Skill::getUpgradeRatio( unsigned int consume )
{
	return (float)(this->consume() + consume)/ this->getUpgradeExp();
}

float Skill::desplayUpgradeRatio( unsigned int consume )
{
	float ratio = getUpgradeRatio(consume);
	float desplayRatio;
	if(ratio < 0.1875)
		desplayRatio = ratio * 80;
	else if(0.1875 < ratio && ratio <= 0.3125)
		desplayRatio = (ratio - 0.1875) * 40 +15;
	else if(0.3125 < ratio && ratio <= 0.75)
		desplayRatio = (ratio - 0.3125) * 160 / 7 +20;
	else if(0.75 < ratio && ratio <= 1.5)
		desplayRatio = ratio * 40;
	else if(1.5 < ratio)
		desplayRatio = 60;

	return desplayRatio;
}

std::string Skill::getCurUpgradeRatioString()
{
	unsigned int num = BlackBoard::Get()->getSkillConsumeNum();
	unsigned int totalConsume = 0;
	for(unsigned int index = 0; index<num; ++index)
	{
		totalConsume += Skill(BlackBoard::Get()->getSkillConsume(index)).getConsumeExp();
	}
	unsigned int ratio = ceil(desplayUpgradeRatio(totalConsume));
	return StringConverter::toString(ratio) + "%";
}

bool Skill::isHavInfo()
{
	if(mSkillInfo)
	{
		return true;
	}
	return false;
}
bool Skill::isHavItem()
{
	if(mSkillItem)
	{
		return true;
	}
	return false;
}

bool SkillSort_default(unsigned int id1, unsigned int id2)
{
	Skill skill1(id1);
	Skill skill2(id2);

	std::string carriedByStr1 = skill1.getSkillCarriedBy();
	std::string carriedByStr2 = skill2.getSkillCarriedBy();
	if(carriedByStr1!=""&&carriedByStr2=="")
	{
		return false;
	}
	else if(carriedByStr2!=""&&carriedByStr1=="")
	{
		return true;
	}

	if(skill1.isSkillCarriedBy() && !skill2.isSkillCarriedBy())
		return false;
	else if(skill2.isSkillCarriedBy() && !skill1.isSkillCarriedBy())
		return true;

	if(skill1.quality() < skill2.quality())
		return false;
	else if(skill2.quality() < skill1.quality())
		return true;

	if(skill1.level() > skill2.level())
		return false;
	else if(skill2.level() > skill1.level())
		return true;

	return false;

}

bool skillFilter_default( unsigned int id )
{
	if(Skill(id).isBornSkill())
		return false;

	return true;
}

SkillDateCollect::SkillIDCollect SkillDateCollect::getSkillCollection(SkillFilter sFilter/*=skillFilter_default*/, SkillSort sSort/*=SkillSort_default*/)
{
	SkillIDCollect myList;
	SkillInfoListIterator itr = ServerDateManager::Get()->getSkillIterator();
	while(itr.hasMoreElements())
	{
		unsigned int id = itr.getNext()->id;
		if(sFilter(id))
		{
			myList.push_back(id);
		}
	}
	myList.sort(sSort);
	return myList;
}
