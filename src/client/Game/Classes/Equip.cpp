
#include "stdafx.h"

#include "Equip.h"

#include <list>
#include "math.h"

#include "DataTableManager.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "GameMaths.h"
#include "Disciple.h"

Equip::Equip( unsigned int id, bool isItemID/* = false*/ ,bool isViewTeam)
	:mEquipInfo(NULL)
	,mEquipItem(NULL)
{
	mIsViewTeam=isViewTeam;
	if(isItemID)
	{
		mEquipInfo = NULL;
		mEquipItem = EquipTableManager::Get()->getEquipItemByID(id);
		if(!mEquipItem)
			CCLOG("Equip: EquipItem not find %d", id);
	}
	else
	{
		if(mIsViewTeam)
		{
			mEquipInfo = ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserEquipInfoByID(id);
		}
		else
		{
			mEquipInfo = ServerDateManager::Get()->getUserEquipInfoByID(id);
		}
		if(mEquipInfo)
		{
			mEquipItem = EquipTableManager::Get()->getEquipItemByID(mEquipInfo->itemid);
			if(!mEquipItem)
				CCLOG("Equip: EquipItem not find %d", mEquipInfo->itemid);
		}
		else
		{
			CCLOG("Equip: EquipInfo not find %d", id);
		}
	}
}

cocos2d::ccColor3B Equip::getFrameColor()
{
	std::string color = VaribleManager::Get()->getSetting("FrameColor_Quality" + StringConverter::toString(quality()));
	return StringConverter::parseColor3B(color);
}

cocos2d::CCSprite* Equip::getFrameNormalSpirte(std::string ext/*=""*/)
{
	cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("FrameNormal_ImageFile" + ext).c_str());
	sprite->setColor(getFrameColor());
	return sprite;
}

cocos2d::CCSprite* Equip::getFrameSelectedSpirte(std::string ext/*=""*/)
{
	cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("FrameSelected_ImageFile" + ext).c_str());
	sprite->setColor(getFrameColor());
	return sprite;
}

cocos2d::CCSprite* Equip::getFrameDisabledSpirte(std::string ext/*=""*/)
{
	cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("FrameDisabled_ImageFile" + ext).c_str());
	sprite->setColor(getFrameColor());
	return sprite;
}

std::string Equip::getQualityImageFile()
{
	return VaribleManager::Get()->getSetting("ImageFileQuality" + StringConverter::toString(quality()));
}

std::string Equip::getEquipAtkTypePic()
{
	return VaribleManager::Get()->getSetting("EquipAttack_ImageFile" + StringConverter::toString(type()));
}

float Equip::getEquipInitValue()
{
	float _init=(float)initQuantity() * getQualityFactor();
	return _init;
}

float Equip::getEquipGrowthValue()
{
	float _Growth=(float)(getTotalProperty() - initQuantity()) * 0.875;
	return _Growth;
}

unsigned int Equip::getEquipValue()
{
	int _value=ceil(getEquipGrowthValue() + getEquipInitValue());
	return _value<0?0:_value;
}

bool Equip::allowUpgrade()
{
	return (level() < 3 * ServerDateManager::Get()->getUserBasicInfo().level) && getConsumeMoneyNeeded() < ServerDateManager::Get()->getUserBasicInfo().silvercoins;
}

bool Equip::allowLevelUpgrade()
{
	return (level() < 3 * ServerDateManager::Get()->getUserBasicInfo().level);
}

unsigned int Equip::id()
{
	if(mEquipInfo)
		return mEquipInfo->id;

	return 0;
}

unsigned int Equip::itemID()
{
	if(mEquipInfo)
		return mEquipInfo->itemid;
	else if(mEquipItem)
		return mEquipItem->itemID;

	return 0;
}

const std::string& Equip::name()
{
	if(!mEquipItem)
		return "";
	return mEquipItem->name;
}

unsigned int Equip::quality()
{
	if(!mEquipItem)
		return 0;
	return mEquipItem->quality;
}

unsigned int Equip::type()
{
	if(!mEquipItem)
		return 0;
	return mEquipItem->type;
}

std::string Equip::describe(std::string lengthStr)
{
	if(!mEquipItem)
		return "";
	std::string s;
	int lines;
	int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("Equip_Describe"));
	if(lengthStr != ""){
		width = StringConverter::parseInt(VaribleManager::Get()->getSetting(lengthStr));
	}
	GameMaths::stringAutoReturn(mEquipItem->describe, s, width, lines);
	return s;
}

unsigned int Equip::initQuantity()
{
	if(!mEquipItem)
		return 0;
	return mEquipItem->initQuantity;
}

float Equip::initGrowthRate()
{
	if(!mEquipItem)
		return 0;
	return mEquipItem->initGrowthRate;
}

std::string Equip::iconPic()
{
	if(!mEquipItem)
		return "";
	return mEquipItem->iconPic;
}

std::string Equip::bodyPic()
{
	if(!mEquipItem)
		return "";
	return mEquipItem->bodyPic;
}

std::string Equip::namePic()
{
	if(!mEquipItem)
		return "";
	return mEquipItem->namePic;
}

unsigned int Equip::level()
{
	if(!mEquipItem)
		return 0;
	if(mEquipInfo)
		return mEquipInfo->level;
	
	return 1;
}

unsigned int Equip::refinexp()
{
	if(mEquipInfo)
		return mEquipInfo->refinexp;

	return 0;
}

unsigned int Equip::refinelevel()
{
	if(mEquipInfo)
		return mEquipInfo->refinelevel;

	return 0;
}

bool Equip::isEquipCarriedBy()
{
	UserBattleInfoListIterator itr = ServerDateManager::Get()->getUserBattleIterator();
	while (itr.hasMoreElements())
	{
		UserBattleInfo* info = itr.getNext();
		if(info->equip1 == id() || info->equip2 == id() || info->equip3 == id())
			return true;
	}
	return false;
}

std::string Equip::getEquipCarriedBy()
{
	UserBattleInfoListIterator itr = ServerDateManager::Get()->getUserBattleIterator();
	while (itr.hasMoreElements())
	{
		UserBattleInfo* info = itr.getNext();
		if(info->equip1 == id() || info->equip2 == id() || info->equip3 == id())
		{
			Disciple disciple(info->discipleId);
			return disciple.name();
		}
	}
	return "";
}

unsigned int Equip::getConsumeMoneyNeeded()
{
	return getUpgradeMoneyConsumeFactor() * getUpgradeMoneyConsumeLevelFactor();
}

float Equip::getQualityFactor()
{
	unsigned int quality = this->quality();
	if(quality > EquipQualityNum)
	{
		return 0;
	}
	return EquipQualityFactor[quality - 1];

}

unsigned int Equip::getTotalProperty()
{
	if(mEquipInfo)
	{
		unsigned int total = 0;
		std::map<unsigned int, unsigned int>::const_iterator itr = mEquipInfo->propertyInfo.begin();
		while(itr != mEquipInfo->propertyInfo.end())
		{
			total += itr->second;
			++itr;
		}
		return total==0?initQuantity():total;
	}
	else
		return initQuantity();
}

unsigned int Equip::getUpgradeMoneyConsumeFactor()
{
	unsigned int quality = this->quality();
	unsigned int type = this->type();
	if(quality > EquipQualityNum || type > EquipTypeNum)
	{
		//
		return 0;
	}

	return EquipUpgradeMoneyConsumeFactor[quality - 1][type - 1];
}

unsigned int Equip::getUpgradeMoneyConsumeLevelFactor()
{
	return EquipLevelParamManager::Get()->getParamByLevel(level());
}

unsigned int Equip::propertyValue( unsigned int type )
{
	if(mEquipInfo)
	{
		std::map<unsigned int, unsigned int>::const_iterator itr = mEquipInfo->propertyInfo.find(type);
		if(itr != mEquipInfo->propertyInfo.end())
			return itr->second;

		return 0;
	}
	
	return initQuantity();
}

unsigned int Equip::stoneColor(unsigned int stonePosition){
	if(mEquipInfo){
		if(mEquipInfo->stoneMap.find(stonePosition) == mEquipInfo->stoneMap.end()){
			return StoneTableManager::Get()->getStoneColorItemBySign("grey")->id;
		}
		UserStoneInfo* userStoneInfo = mEquipInfo->stoneMap.find(stonePosition)->second;
		return userStoneInfo->color;
	}else{
		return StoneTableManager::Get()->getStoneColorItemBySign("grey")->id;
	}
}

unsigned int Equip::stoneStatus(unsigned int stonePosition){
	if(mEquipInfo){
		if(mEquipInfo->stoneMap.find(stonePosition) == mEquipInfo->stoneMap.end()){
			return StoneTableManager::Get()->getStoneStatusItemBySign("locked")->status;
		}
		UserStoneInfo* userStoneInfo = mEquipInfo->stoneMap.find(stonePosition)->second;
		return userStoneInfo->status;
	}else{
		return StoneTableManager::Get()->getStoneStatusItemBySign("locked")->status;
	}
	/*
	UserEquipInfo* info = ServerDateManager::Get()->getUserEquipInfoByID(id());
	if(info!=NULL)
	{
		std::string stoneStr = info->stoneStr;
		Json::Value mData;
		Json::Reader jreader;
		jreader.parse(stoneStr,mData,false);
		Json::Value dList = mData["d"];
		Json::Value append = mData["a"];
		return dList[stonePosition - 1]["s"].asInt();
	}
	*/
}

unsigned int Equip::stoneId(unsigned int stonePosition){
	if(mEquipInfo){
		if(mEquipInfo->stoneMap.find(stonePosition) == mEquipInfo->stoneMap.end()){
			return 0;
		}
		UserStoneInfo* userStoneInfo = mEquipInfo->stoneMap.find(stonePosition)->second;
		return userStoneInfo->stoneid;
	}else{
		return 0;
	}
	/*
	std::string stoneStr = ServerDateManager::Get()->getUserEquipInfoByID(id())->stoneStr;
	Json::Value mData;
	Json::Reader jreader;
	jreader.parse(stoneStr,mData,false);
	Json::Value dList = mData["d"];
	Json::Value append = mData["a"];
	return dList[stonePosition - 1]["i"].asInt();
	*/

}

unsigned int Equip::stoneNum(){
	if(mEquipInfo->stoneMap.size() == 0){
		return 5 - this->quality();
	}else{
		return mEquipInfo->stoneMap.size();
	}
	/*
	std::string stoneStr = ServerDateManager::Get()->getUserEquipInfoByID(id())->stoneStr;
	Json::Value mData;
	Json::Reader jreader;
	jreader.parse(stoneStr,mData,false);
	Json::Value dList = mData["d"];
	Json::Value append = mData["a"];

	return dList.size();
	*/
}

unsigned int Equip::stoneBuffType(){
	return mEquipInfo->bufftype;
	/*
	std::string stoneStr = ServerDateManager::Get()->getUserEquipInfoByID(id())->stoneStr;
	Json::Value mData;
	Json::Reader jreader;
	jreader.parse(stoneStr,mData,false);
	Json::Value dList = mData["d"];
	Json::Value append = mData["a"];
	return append["b"].asInt();
	*/

}

std::string Equip::stoneBuffValue(){
	if(mEquipInfo->buffvalue == ""){
		return "0";
	}
	return mEquipInfo->buffvalue;
	/*
	std::string stoneStr = ServerDateManager::Get()->getUserEquipInfoByID(id())->stoneStr;
	Json::Value mData;
	Json::Reader jreader;
	jreader.parse(stoneStr,mData,false);
	Json::Value dList = mData["d"];
	Json::Value append = mData["a"];
	return append["v"].asString().c_str();
	*/

}

unsigned int Equip::quantity()
{
	return propertyValue(type());
}

bool EquipSort_default(unsigned int id1, unsigned int id2)
{
	Equip Equip1(id1);
	Equip Equip2(id2);

	std::string carriedByStr1 = Equip1.getEquipCarriedBy();
	std::string carriedByStr2 = Equip2.getEquipCarriedBy();
	if(carriedByStr1!=""&&carriedByStr2=="")
	{
		return true;
	}
	else if(carriedByStr2!=""&&carriedByStr1=="")
	{
		return false;
	}

	if(Equip1.quality() < Equip2.quality())
		return true;
	else if(Equip2.quality() < Equip1.quality())
		return false;

	if(Equip1.level() > Equip2.level())
		return true;
	else if(Equip1.level() < Equip2.level())
		return false;

	if(Equip1.quantity() > Equip2.quantity())
		return true;
	else if(Equip1.quantity() < Equip2.quantity())
		return false;

	return false;

}

bool EquipFilter_default( unsigned int id )
{
	return true;
}

bool EquipFilter_Type1( unsigned int id )
{
	Equip equip(id);
	if(equip.type() != 1)
		return false;

	return true;
}

bool EquipFilter_Type2( unsigned int id )
{
	Equip equip(id);
	if(equip.type() != 2)
		return false;

	return true;
}

bool EquipFilter_Type3( unsigned int id )
{
	Equip equip(id);
	if(equip.type() != 3)
		return false;

	return true;
}

EquipDateCollect::EquipIDCollect EquipDateCollect::getEquipCollection(EquipFilter sFilter/*=EquipFilter_default*/, EquipSort sSort/*=EquipSort_default*/)
{
	EquipIDCollect myList;
	UserEquipInfoListIterator itr = ServerDateManager::Get()->getUserEquipIterator();
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
