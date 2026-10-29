
#include "stdafx.h"

#include "Disciple.h"
#include "cocos2d.h"
#include <list>
#include "math.h"

#include "DataTableManager.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "GameMaths.h"

Disciple::Disciple( unsigned int id, bool isItemID/* = false */,bool isViewTeam)
	:mDiscipleInfo(NULL)
	,mDiscipleItem(NULL)
{
	mIsViewTeam=isViewTeam;
	init(id, isItemID,isViewTeam);
}

Disciple::Disciple():mDiscipleInfo(NULL),mDiscipleItem(NULL)
{

}

void Disciple::init( unsigned int id, bool isItemID /*= false*/,bool isViewTeam )
{
	if(isItemID)
	{
		mDiscipleInfo = NULL;
		mDiscipleItem = DiscipleTableManager::Get()->getDiscipleItemByID(id);
		if(!mDiscipleItem)
			CCLOG("Disciple::init:DiscipleItem not find %d", id);
	}
	else
	{
		if(isViewTeam)
		{
			mDiscipleInfo = ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserDiscipleInfoByID(id);
		}
		else
		{
			mDiscipleInfo = ServerDateManager::Get()->getUserDiscipleInfoByID(id);
		}
		if(mDiscipleInfo)
		{
			mDiscipleItem = DiscipleTableManager::Get()->getDiscipleItemByID(mDiscipleInfo->itemid);
			if(!mDiscipleItem)
				CCLOG("Disciple::init:DiscipleItem not find %d", mDiscipleInfo->itemid);
		}
		else
		{
			CCLOG("Disciple::init:DiscipleInfo not find %d", id);
		}
	}
}


unsigned int Disciple::id()
{
	if(mDiscipleInfo)
		return mDiscipleInfo->id;
	
	return 0;
}

unsigned int Disciple::level()
{
	if(mDiscipleInfo)
		return mDiscipleInfo->level;

	return 1;
}

unsigned int Disciple::exp()
{
	if(mDiscipleInfo)
		return mDiscipleInfo->exp;

	return 0;
}

unsigned int Disciple::health()
{
	if(isInBattle())
	{
		if(mIsViewTeam)
		{
			return ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserBattleInfoByDisciple(id())->health;
		}
		else
		{
			return ServerDateManager::Get()->getUserBattleInfoByDisciple(id())->health;
		}
	}	
	if(mDiscipleInfo)
		return mDiscipleInfo->health;
	else if(mDiscipleItem)
		return mDiscipleItem->initHealth;

	return 0;
}

unsigned int Disciple::attack()
{
	if(isInBattle())
	{
		if(mIsViewTeam)
		{
			return ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserBattleInfoByDisciple(id())->attack;
		}
		else
		{
			return ServerDateManager::Get()->getUserBattleInfoByDisciple(id())->attack;
		}
	}
	if(mDiscipleInfo)
		return mDiscipleInfo->attack;
	else if(mDiscipleItem)
		return mDiscipleItem->initAttack;

	return 0;
}

unsigned int Disciple::defence()
{
	if(isInBattle())
	{
		if(mIsViewTeam)
		{
			return ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserBattleInfoByDisciple(id())->defence;
		}
		else
		{
			return ServerDateManager::Get()->getUserBattleInfoByDisciple(id())->defence;
		}
	}
	if(mDiscipleInfo)
		return mDiscipleInfo->defence;
	else if(mDiscipleItem)
		return mDiscipleItem->initDefence;

	return 0;
}

unsigned int Disciple::rp()
{
	if(isInBattle())
	{
		if(mIsViewTeam)
		{
			return ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserBattleInfoByDisciple(id())->rp;
		}
		else
		{
			return ServerDateManager::Get()->getUserBattleInfoByDisciple(id())->rp;
		}
	}

	if(mDiscipleInfo)
		return mDiscipleInfo->rp;
	else if(mDiscipleItem)
		return mDiscipleItem->initPower;

	return 0;
}

unsigned int Disciple::upgradelevel()
{
	if(mDiscipleInfo)
		return mDiscipleInfo->upgradelevel;

	return 0;
}

unsigned int Disciple::potentiality()
{
	if(mDiscipleInfo)
		return mDiscipleInfo->potentiality;

	return 0;
}

unsigned int Disciple::skill1()
{
	if(mDiscipleInfo)
		return mDiscipleInfo->skill1;
	else if(mDiscipleItem)
		return mDiscipleItem->bornSkillItemID;
	
	return 0;
}

unsigned int Disciple::battleid()
{
	if(mDiscipleInfo)
		return mDiscipleInfo->battleid;

	return 0;
}

unsigned int Disciple::itemID()
{
	if(mDiscipleItem)
		return mDiscipleItem->itemID;

	return 0;
}

std::string Disciple::name()
{
	if(mDiscipleItem)
		return mDiscipleItem->name;

	return "";
}

unsigned int Disciple::quality()
{
	if(mDiscipleItem)
		return mDiscipleItem->quality;

	return 0;
}

unsigned int Disciple::type()
{
	if(mDiscipleItem)
		return mDiscipleItem->type;

	return 0;
}

std::string Disciple::describe()
{
	if(!mDiscipleItem)
		return "";

	std::string s;
	int lines;
	int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("Disciple_Describe"));
	GameMaths::stringAutoReturn(mDiscipleItem->describe, s, width, lines);
	return s;
}

unsigned int Disciple::bornSkillItemID()
{
	if(!mDiscipleItem)
		return 0;

	return mDiscipleItem->bornSkillItemID;
}

unsigned int Disciple::initHealth()
{
	if(!mDiscipleItem)
		return 0;

	return mDiscipleItem->initHealth;
}

unsigned int Disciple::initAttack()
{
	if(!mDiscipleItem)
		return 0;

	return mDiscipleItem->initAttack;
}

unsigned int Disciple::initDefence()
{
	if(!mDiscipleItem)
		return 0;

	return mDiscipleItem->initDefence;
}

unsigned int Disciple::initPower()
{
	if(!mDiscipleItem)
		return 0;

	return mDiscipleItem->initPower;
}

unsigned int Disciple::initExp()
{
	if(!mDiscipleItem)
		return 0;

	return mDiscipleItem->initExp;
}

unsigned int Disciple::yuanfens( unsigned int index )
{
	if(mDiscipleItem)
		return mDiscipleItem->yuanfens[index];
	else
		return -1;
}

std::string Disciple::iconPic()
{
	if(!mDiscipleItem)
		return "";

	return mDiscipleItem->iconPic;
}

std::string Disciple::bodyPic()
{
	if(!mDiscipleItem)
		return "";

	return mDiscipleItem->bodyPic;
}

std::string Disciple::namePic()
{
	if(!mDiscipleItem)
		return "";

	return mDiscipleItem->namePic;
}

std::string Disciple::backPic()
{
	if(!mDiscipleItem)
		return "";

	return mDiscipleItem->backPic;
}

cocos2d::ccColor3B Disciple::getFrameColor()
{
	std::string color = VaribleManager::Get()->getSetting("FrameColor_Quality" + StringConverter::toString(quality()));
	return StringConverter::parseColor3B(color);
}

cocos2d::CCSprite* Disciple::getFrameNormalSpirte(std::string ext/*=""*/)
{
	cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("FrameNormal_ImageFile" + ext).c_str());
	sprite->setColor(getFrameColor());
	return sprite;
}

cocos2d::CCSprite* Disciple::getFrameSelectedSpirte(std::string ext/*=""*/)
{
	cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("FrameSelected_ImageFile" + ext).c_str());
	sprite->setColor(getFrameColor());
	return sprite;
}

cocos2d::CCSprite* Disciple::getFrameDisabledSpirte(std::string ext/*=""*/)
{
	cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("FrameDisabled_ImageFile" + ext).c_str());
	sprite->setColor(getFrameColor());
	return sprite;
}

std::string Disciple::getQualityImageFile(bool isLarge)
{
	//return ::getQualityImageFile(quality());
	if(isLarge)
	{
		return VaribleManager::Get()->getSetting("ImageFileLargeQuality" + StringConverter::toString(quality()));
	}
	return VaribleManager::Get()->getSetting("ImageFileQuality" + StringConverter::toString(quality()));
}

std::string Disciple::getQualityBgImageFile()
{
	//return ::getQualityImageFile(quality());
	return VaribleManager::Get()->getSetting("MemBgImageFileQuality" + StringConverter::toString(quality()));
}

bool Disciple::isInBattle()
{
	if(mIsViewTeam)
	{
		return true;
	}
	for(int i = 0; i<MaxBattleDiscipleNum; ++i)
	{
		if(id()!=0 && ServerDateManager::Get()->getUserBattleInfo(i)
			&& ServerDateManager::Get()->getUserBattleInfo(i)->discipleId == id())
			return true;
	}
	return false;
}

float Disciple::getDiscipleInitTotalProperty()
{
	return initHealth() + initAttack() + initDefence() + initPower();
}

float Disciple::getDiscipleTotalProperty()
{
	return health() + attack() + defence() + rp();
}

float Disciple::getDiscipleInitValueFactor()
{
	unsigned int quality = this->quality();
	if(quality == 1)
	{
		return 1.8f;
	}
	else if(quality == 2)
	{
		return 0.8f;
	}
	else if(quality == 3)
	{
		return 0.48f;
	}
	else if(quality == 4)
	{
		return 0.075f;
	}

	return 0;
}

float Disciple::getDiscipleUpgradeValueFactor()
{
	unsigned int quality = this->quality();
	if(quality == 1)
	{
		return 0.75f;
	}
	else if(quality == 2)
	{
		return 0.8f;
	}
	else if(quality == 3)
	{
		return 0.8f;
	}
	else if(quality == 4)
	{
		return 0.8f;
	}

	return 0;
}

float Disciple::getDiscipleInitValue()
{
	return getDiscipleInitTotalProperty() * getDiscipleInitValueFactor();
}

unsigned int Disciple::getDiscipleValue()
{
	return ceil((getDiscipleTotalProperty() - getDiscipleInitTotalProperty()) * getDiscipleUpgradeValueFactor() + getDiscipleInitValue());
}

unsigned int Disciple::getLevelupNeededExp()
{
	return (unsigned int)(getExpForLevel(level() + 1) - exp());
}

unsigned int Disciple::getProviderExp()
{
	return exp();
}

unsigned int Disciple::getUpgradeLevel( unsigned int provideExp )
{
	unsigned int upgradeExp = getProviderExp() + provideExp;
	for(unsigned int level = this->level() + 1; ; ++level)
	{
		if(getExpForLevel(level) > upgradeExp)
		{
			return level - 1;
		}
	}
}

// unsigned int Disciple::getTotalExp()
// {
// 	return getTotalExpByLevel(level());
// }

// unsigned int Disciple::getTotalExpByLevel( unsigned int level )
// {
// 	return (unsigned int)(getLevelExp() + getExpForLevel(level));
// }


unsigned int Disciple::getExpForLevel( unsigned int level )
{
	return (unsigned int)(DisciplesLevelParamManager::Get()->getTotalParamByLevel(level) * initExp());
}

// unsigned int Disciple::getExpForCurLevel()
// {
// 	return getExpForLevel(level());
// }

const std::string Disciple::getYuanfenDescribe( void )
{
	std::string describe("");
	for(int i = 0; i<MAX_YUANFEN_NUM; ++i)
	{
		const YuanfenItem * yuanfen = YuanfenTableManager::Get()->getYuanfenItemByID(yuanfens(i));

		if(yuanfen)
		{
			std::string s;
			int lines;
			int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("YuanFen_Describe"));
			std::string yfSrt = yuanfen->name+":"+yuanfen->describe;
			GameMaths::stringAutoReturn(yfSrt, s, width, lines);
			describe += s + "\n";
		}
	}
	return describe;
}

const std::string Disciple::getYuanfenName(void)
{
	std::string name("");
	for(int i = 0; i<MAX_YUANFEN_NUM; ++i)
	{
		const YuanfenItem * yuanfen = YuanfenTableManager::Get()->getYuanfenItemByID(yuanfens(i));

		if(yuanfen)
		{
			name +=((i%2!=0)?"    ":"")+yuanfen->name + ((i%2!=0)?"\n":"");
		}
	}
	return name;
}

unsigned int Disciple::getLevelExp()
{
	return exp() - getExpForLevel(level());
}

unsigned int Disciple::getExpForLevelUpgrade( unsigned int level )
{
	return (unsigned int)(DisciplesLevelParamManager::Get()->getParamByLevel(level) * initExp());
}

bool Disciple::canUpgradeLevel()
{
	return level() < ServerDateManager::Get()->getUserBasicInfo().level * 3;
}

bool DiscipleSort_default(unsigned int id1, unsigned int id2)
{
	Disciple Disciple1(id1);
	Disciple Disciple2(id2);

// 	if(Disciple1.isInBattle() && !Disciple2.isInBattle())
// 		return true;
// 	else if(Disciple2.isInBattle() && !Disciple1.isInBattle())
// 		return false;

	if(Disciple1.quality() < Disciple2.quality())
		return true;
	else if(Disciple2.quality() < Disciple1.quality())
		return false;

	if(Disciple1.level() > Disciple2.level())
		return true;
	else
		return false;

 	return true;

}
bool DiscipleSort_qualityASC(unsigned int id1, unsigned int id2)
{
	Disciple Disciple1(id1);
	Disciple Disciple2(id2);

	// 	if(Disciple1.isInBattle() && !Disciple2.isInBattle())
	// 		return true;
	// 	else if(Disciple2.isInBattle() && !Disciple1.isInBattle())
	// 		return false;

	if(Disciple1.quality() > Disciple2.quality())
		return true;
	else if(Disciple2.quality() > Disciple1.quality())
		return false;

	if(Disciple1.level() > Disciple2.level())
		return true;
	else
		return false;

	return true;

}

bool DiscipleFilter_default( unsigned int id )
{
	return true;
}

DiscipleDateCollect::DiscipleIDCollect DiscipleDateCollect::getDiscipleCollection(DiscipleFilter sFilter/*=DiscipleFilter_default*/, DiscipleSort sSort/*=DiscipleSort_default*/)
{
	DiscipleIDCollect myList;
	UserDiscipleInfoListIterator itr = ServerDateManager::Get()->getDiscipleIterator();
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

Soul::Soul( unsigned int id )
	:mSoulInfo(NULL)
	,mDiscipleItem(NULL)
{
	mSoulInfo = ServerDateManager::Get()->getUserSoulInfoByID(id);
	if(mSoulInfo)
	{
		mDiscipleItem = DiscipleTableManager::Get()->getDiscipleItemByID(mSoulInfo->itemid);
		if(!mDiscipleItem)
			CCLOG("Soul: SoulItem not find %d", mSoulInfo->itemid);
	}
	else
	{
		CCLOG("Soul: SoulInfo not find %d", id);
		mSoulInfo = NULL;
		mDiscipleItem = DiscipleTableManager::Get()->getDiscipleItemByID(id);
		if(!mDiscipleItem)
			CCLOG("Soul::SoulItem not find %d", id);
	}
}

unsigned int Soul::itemID()
{
	if(!mDiscipleItem)
		return 0;

	return mDiscipleItem->itemID;
}

std::string Soul::name()
{
	if(!mDiscipleItem)
		return "noName";

	return mDiscipleItem->name;
}

unsigned int Soul::quality()
{
	if(!mDiscipleItem)
		return 0;

	return mDiscipleItem->quality;
}

unsigned int Soul::type()
{
	if(!mDiscipleItem)
		return 0;
	return mDiscipleItem->type;
}

std::string Soul::describe()
{
	if(!mDiscipleItem)
		return "";

	std::string s;
	int lines;
	int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("Disciple_Describe"));
	GameMaths::stringAutoReturn(mDiscipleItem->describe, s, width, lines);
	return s;
}

unsigned int Soul::bornSkillItemID()
{
	if(!mDiscipleItem)
		return 0;
	return mDiscipleItem->bornSkillItemID;
}

unsigned int Soul::initHealth()
{
	if(!mDiscipleItem)
		return 0;
	return mDiscipleItem->initHealth;
}

unsigned int Soul::initAttack()
{
	if(!mDiscipleItem)
		return 0;
	return mDiscipleItem->initAttack;
}

unsigned int Soul::initDefence()
{
	if(!mDiscipleItem)
		return 0;
	return mDiscipleItem->initDefence;
}

unsigned int Soul::initPower()
{
	if(!mDiscipleItem)
		return 0;
	return mDiscipleItem->initPower;
}

unsigned int Soul::initExp()
{
	if(!mDiscipleItem)
		return 0;
	return mDiscipleItem->initExp;
}

unsigned int Soul::yuanfens( unsigned int index )
{
	if(!mDiscipleItem)
		return 0;
	return mDiscipleItem->yuanfens[index];
}

std::string Soul::iconPic()
{
	if(!mDiscipleItem)
		return "";
	return mDiscipleItem->iconPic;
}

std::string Soul::bodyPic()
{
	if(!mDiscipleItem)
		return "";
	return mDiscipleItem->bodyPic;
}

std::string Soul::namePic()
{
	if(!mDiscipleItem)
		return "";
	return mDiscipleItem->namePic;
}

std::string Soul::backPic()
{
	if(!mDiscipleItem)
		return "";
	return mDiscipleItem->backPic;
}

unsigned int Soul::id()
{
	if(!mDiscipleItem)
		return 0;
	return mSoulInfo->id;
}

unsigned int Soul::count()
{
	if(!mDiscipleItem)
		return 0;
	return mSoulInfo->count;
}

bool Soul::canRecruit()
{
	const UserDiscipleInfo * userDiscipleInfo = ServerDateManager::Get()->getUserDiscipleInfoByItemID(itemID());
	if(!userDiscipleInfo)
		return true;

	return false;
}

bool Soul::canUpgrade()
{
	const UserDiscipleInfo * userDiscipleInfo = ServerDateManager::Get()->getUserDiscipleInfoByItemID(itemID());
	if(userDiscipleInfo && userDiscipleInfo->upgradelevel < 10)
		return true;

	return false;
}

unsigned int Soul::getNeededNum()
{
	unsigned int quality = this->quality();
	unsigned int needNum = 0;;
	if(canRecruit())
	{
		if(quality == 1)
		{
			needNum = 30;
		}
		else if(quality ==2)
		{
			needNum = 15;
		}
		else if(quality ==3)
		{
			needNum = 5;
		}
		else if(quality ==4)
		{
			needNum = 1;
		}
	}
	else if(canUpgrade())
	{
		unsigned int increase;
		if(quality == 1)
		{
			needNum = 45;
			increase = 15;
		}
		else if(quality ==2)
		{
			needNum = 30;
			increase = 15;
		}
		else if(quality ==3)
		{
			needNum = 15;
			increase = 10;
		}
		else if(quality ==4)
		{
			needNum = 5;
			increase = 10;
		}
		const UserDiscipleInfo * userDiscipleInfo = ServerDateManager::Get()->getUserDiscipleInfoByItemID(this->itemID());
		if(userDiscipleInfo)
			needNum += userDiscipleInfo->upgradelevel * increase;
	}
	
	return needNum;
}

unsigned int Soul::getGapNum()
{
	if(getNeededNum() > this->count())
		return getNeededNum() - this->count();
	else
		return 0;
}

std::string Soul::getNumText()
{
	std::string text("");
	unsigned int num = getGapNum();
	if(num > 0)
	{
		if(canRecruit())
			text = Language::Get()->getString("@SoulNeedRecruit");
		else if(canUpgrade())
			text = Language::Get()->getString("@SoulNeedUpgrade");
	}

	char buff[128] = {0};
	sprintf(buff , text.c_str() , num);

	return buff;
}

std::string Soul::getOprText()
{
	std::string text("");
	if(getGapNum() == 0)
	{
		if(canRecruit())
			text += Language::Get()->getString("@MPRecruit");
		else if(canUpgrade())
			text += Language::Get()->getString("@MPBreak");
	}

	return text;
}

float Soul::getSoulInitValueFactor()
{
	unsigned int quality = this->quality();
	if(quality == 1)
	{
		return 1.8f;
	}
	else if(quality == 2)
	{
		return 0.8f;
	}
	else if(quality == 3)
	{
		return 0.48f;
	}
	else if(quality == 4)
	{
		return 0.075f;
	}

	return 0;
}

float Soul::getSoulInitTotalProperty()
{
	return initHealth() + initAttack() + initDefence() + initPower();
}

unsigned int Soul::getSoulValue()
{
	return ceil(getSoulInitTotalProperty() * getSoulInitValueFactor());
}

unsigned int Soul::getDiscipleID()
{
	const UserDiscipleInfo * userDiscipleInfo = ServerDateManager::Get()->getUserDiscipleInfoByItemID(itemID());
	if(userDiscipleInfo)
		return userDiscipleInfo->id;

	return 0;
}

cocos2d::CCSprite* Soul::getFrameNormalSpirte(std::string ext/*=""*/)
{
	cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("FrameNormal_ImageFile" + ext).c_str());
	std::string color = VaribleManager::Get()->getSetting("FrameColor_Quality" + StringConverter::toString(quality()));
	sprite->setColor(StringConverter::parseColor3B(color));
	return sprite;
}

cocos2d::CCSprite* Soul::getFrameSelectedSpirte(std::string ext/*=""*/)
{
	cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("FrameSelected_ImageFile" + ext).c_str());
	std::string color = VaribleManager::Get()->getSetting("FrameColor_Quality" + StringConverter::toString(quality()));
	sprite->setColor(StringConverter::parseColor3B(color));
	return sprite;
}

cocos2d::CCSprite* Soul::getFrameDisabledSpirte(std::string ext/*=""*/)
{
	cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("FrameDisabled_ImageFile" + ext).c_str());
	std::string color = VaribleManager::Get()->getSetting("FrameColor_Quality" + StringConverter::toString(quality()));
	sprite->setColor(StringConverter::parseColor3B(color));
	return sprite;
}

std::string Soul::getQualityImageFile()
{
	return VaribleManager::Get()->getSetting("ImageFileQuality" + StringConverter::toString(quality()));
}

const std::string Soul::getYuanfenDescribe( void )
{
	std::string describe("");
	for(int i = 0; i<MAX_YUANFEN_NUM; ++i)
	{
		const YuanfenItem * yuanfen = YuanfenTableManager::Get()->getYuanfenItemByID(yuanfens(i));

		if(yuanfen)
		{
			std::string s;
			int lines;
			int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("YuanFen_Describe"));
			std::string yfSrt = yuanfen->name+":"+yuanfen->describe;
			GameMaths::stringAutoReturn(yfSrt, s, width, lines);
			describe += s + "\n";
		}
	}
	return describe;
}
DiscipleDateCollect::DiscipleIDCollect DiscipleDateCollect::getSoulCollection(DiscipleFilter sFilter/*=SoulFilter_default*/, DiscipleSort sSort/*=SoulSort_default*/)
{
	DiscipleIDCollect myList;
	UserSoulInfoListIterator itr = ServerDateManager::Get()->getSoulIterator();
	while(itr.hasMoreElements())
	{
		UserSoulInfo* info = itr.getNext();
		if(info->count>0)
		{
			unsigned int id = info->id;
			if(sFilter(id))
			{
				myList.push_back(id);
			}
		}
	}
	myList.sort(sSort);
	return myList;
}
bool SoulSort_default(unsigned int id1, unsigned int id2)
{
	Soul sou1(id1);
	Soul sou2(id2);

	if(sou1.quality() < sou2.quality())
		return true;
	else if(sou2.quality() < sou1.quality())
		return false;
	if(sou1.count() < sou2.count())
		return true;
	else
		return false;
}
bool SoulSort_qualityASC(unsigned int id1, unsigned int id2)
{
	Soul sou1(id1);
	Soul sou2(id2);

	if(sou1.quality() > sou2.quality())
		return true;
	else if(sou2.quality() > sou1.quality())
		return false;
	if(sou1.count() < sou2.count())
		return true;
	else
		return false;
}
bool SoulSort_recruitUpgrade(unsigned int id1, unsigned int id2)
{
	Soul sou1(id1);
	Soul sou2(id2);
	bool soul1CanRecruit = (sou1.canRecruit()&&sou1.getNeededNum()<=sou1.count());
	bool soul2CanRecruit = (sou2.canRecruit()&&sou2.getNeededNum()<=sou2.count());
	if(soul1CanRecruit&&soul2CanRecruit)
	{
		if(sou1.quality() > sou2.quality())
			return true;
		else if(sou2.quality() > sou1.quality())
			return false;
		if(sou1.count() < sou2.count())
			return true;
		else
			return false;
	}
	else if(soul1CanRecruit)
	{
		return false;
	}
	else if(soul2CanRecruit)
	{
		return true;
	}
	else
	{
		bool soul1CanUpgrade = (sou1.canUpgrade()&&sou1.getNeededNum()<=sou1.count());
		bool soul2CanUpgrade = (sou2.canUpgrade()&&sou2.getNeededNum()<=sou2.count());
		if(soul1CanUpgrade&&soul2CanUpgrade)
		{
			if(sou1.quality() > sou2.quality())
				return true;
			else if(sou2.quality() > sou1.quality())
				return false;
			if(sou1.count() < sou2.count())
				return true;
			else
				return false;
		}
		else if(soul1CanUpgrade)
		{
			return false;
		}
		else if(soul2CanUpgrade)
		{
			return true;
		}
		else
		{
			if(sou1.quality() > sou2.quality())
				return true;
			else if(sou2.quality() > sou1.quality())
				return false;
			if(sou1.count() < sou2.count())
				return true;
			else
				return false;
		}
	}
}

bool SoulFilter_default( unsigned int id )
{
	return true;
}