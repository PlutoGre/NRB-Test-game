
#include "stdafx.h"

#include "Arena.h"
#include "ServerDateManager.h"
#include "DataTableManager.h"

ArenaOpponent::ArenaOpponent( unsigned int id )
{
	mArenaOpponentInfo = ServerDateManager::Get()->getArenaOpponentInfoByID(id);
}

unsigned int ArenaOpponent::id()
{
	if(mArenaOpponentInfo)
		return mArenaOpponentInfo->id;
	return 0;
}

const std::string& ArenaOpponent::name()
{
	if(mArenaOpponentInfo)
		return mArenaOpponentInfo->name;
	static std::string emptystring("");
	return emptystring;
}

unsigned int ArenaOpponent::level()
{
	if(mArenaOpponentInfo)
		return mArenaOpponentInfo->level;
	return 0;
}

unsigned int ArenaOpponent::arenaRank()
{
	if(mArenaOpponentInfo)
		return mArenaOpponentInfo->arenaRank;
	return 0;
}

unsigned int ArenaOpponent::rewardScore()
{
	if(mArenaOpponentInfo)
		return mArenaOpponentInfo->rewardScore;
	return 0;
}

unsigned int ArenaOpponent::arenaType()
{
	if(mArenaOpponentInfo)
		return mArenaOpponentInfo->arenaType;
	return 0;
}

unsigned int ArenaOpponent::dicipleItemID1()
{
	if(mArenaOpponentInfo)
		return mArenaOpponentInfo->dicipleItemID1;
	return 0;
}

unsigned int ArenaOpponent::dicipleItemID2()
{
	if(mArenaOpponentInfo)
		return mArenaOpponentInfo->dicipleItemID2;
	return 0;
}

unsigned int ArenaOpponent::dicipleItemID3()
{
	if(mArenaOpponentInfo)
		return mArenaOpponentInfo->dicipleItemID3;
	return 0;
}

std::string ArenaOpponent::getFunctionName()
{
	if(arenaType() == 1)
		return "@arena_view";
	else if(arenaType() == 2)
		return "@arena_reback";
	else if(arenaType() == 3)
		return "@arena_challenge";

	return "";
}

cocos2d::CCSprite* ArenaOpponent::getFunctionPic_Normal()
{
	if(arenaType() == 1)
		return NULL;
	else if(arenaType() == 2 || arenaType() == 3)
		return cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("Arena_Challenge_NormalPic").c_str());

	return NULL;
}

cocos2d::CCSprite* ArenaOpponent::getFunctionPic_Selected()
{
	if(arenaType() == 1)
		return NULL;
	else if(arenaType() == 2 || arenaType() == 3)
		return cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("Arena_Challenge_SelectedPic").c_str());

	return NULL;
}

bool ArenaOpponent::isDefeatBy()
{
	return arenaType() == 2;
}

bool ArenaOpponent::canChallenge()
{
	return (arenaType() == 2 || arenaType() == 3);
}

bool ArenaOpponentSort_default(unsigned int id1, unsigned int id2)
{
	ArenaOpponent ArenaOpponent1(id1);
	ArenaOpponent ArenaOpponent2(id2);

	if(ArenaOpponent1.arenaRank() > ArenaOpponent2.arenaRank())
		return true;
	else
		return false;
}

bool ArenaOpponentFilter_default( unsigned int id )
{
	return true;
}

bool ArenaOpponentFilter_Type1( unsigned int id )
{
	if(ArenaOpponent(id).arenaType() != 1)
		return false;

	return true;
}

bool ArenaOpponentFilter_Type4( unsigned int id )
{
	if(ArenaOpponent(id).arenaType() != 4)
		return false;

	return true;
}

bool ArenaOpponentFilter_Type2_3( unsigned int id )
{
	if(ArenaOpponent(id).arenaType() == 2 || ArenaOpponent(id).arenaType() == 3)
		return true;

	return false;
}

ArenaOpponentDateCollect::ArenaOpponentIDCollect ArenaOpponentDateCollect::getArenaOpponentCollection(ArenaOpponentFilter sFilter/*=ArenaOpponentFilter_default*/, ArenaOpponentSort sSort/*=ArenaOpponentSort_default*/)
{
	ArenaOpponentIDCollect myList;
	ArenaOpponentInfoListIterator itr = ServerDateManager::Get()->getArenaOpponentIterator();
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

ArenaReward::ArenaReward( unsigned int itemid )
{
	mArenaRewardItem = ArenaRewardTableManager::Get()->getArenaRewardItemByID(itemid);
}

unsigned int ArenaReward::itemID()
{
	if(mArenaRewardItem)
		return mArenaRewardItem->itemID;
	return 0;
}

unsigned int ArenaReward::type()
{
	if(mArenaRewardItem)
		return mArenaRewardItem->type;
	return 0;
}

std::string ArenaReward::title()
{
	if(mArenaRewardItem)
		return mArenaRewardItem->title;
	return "";
}

unsigned int ArenaReward::needScore()
{
	if(mArenaRewardItem)
		return mArenaRewardItem->needScore;
	return 0;
}

unsigned int ArenaReward::rewardNum()
{
	if(mArenaRewardItem)
		return mArenaRewardItem->rewardNum;
	return 0;
}

std::string ArenaReward::describe()
{
	if(mArenaRewardItem)
		return mArenaRewardItem->describe;
	return "";
}

std::string ArenaReward::iconPic()
{
	if(mArenaRewardItem)
		return mArenaRewardItem->iconPic;
	return "";
}

bool ArenaReward::isEnable()
{
	if(type() == 1)
	{
		return mArenaRewardItem->needScore < ServerDateManager::Get()->getUserArenaInfo().arenaScore;
	}
	else if(type() == 2)
	{
		UserArenaInfo& info = ServerDateManager::Get()->getUserArenaInfo();
		if(info.rewardIDs.find(itemID()) != info.rewardIDs.end())
			return true;
	}

	return false;
}

std::string ArenaReward::getFunctionName()
{
	if(type() == 1)
		return "@arena_reward_change";
	else if(type() == 2)
		return "@arena_reward_get";

	return "";
}


bool ArenaRewardFilter_default( unsigned int id )
{
	return true;
}

bool ArenaRewardSort_default( unsigned int id1, unsigned int id2 )
{
	return id1 > id2;
}

ArenaRewardDateCollect::ArenaRewardIDCollect ArenaRewardDateCollect::getArenaRewardCollection( ArenaRewardFilter sFilter/*=ArenaRewardFilter_default*/, ArenaRewardSort sSort/*=ArenaRewardSort_default*/ )
{
	ArenaRewardIDCollect myList;
	ArenaRewardTableManager::ArenaRewardListIterator itr = ArenaRewardTableManager::Get()->getArenaListIterator();;
	while(itr.hasMoreElements())
	{
		unsigned int id = itr.getNext()->itemID;
		if(sFilter(id))
		{
			myList.push_back(id);
		}
	}
	myList.sort(sSort);
	return myList;
}
