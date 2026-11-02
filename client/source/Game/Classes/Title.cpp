
#include "stdafx.h"

#include "Title.h"

#include <list>
#include "math.h"

#include "DataTableManager.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "BlackBoard.h"
#include "Disciple.h"
#include "GameMaths.h"

unsigned int Title::getTypeByItemID( unsigned int itemid )
{
	return itemid / 1000 ;
}

unsigned int Title::getItemIDByType( unsigned int itemid, unsigned int level/* = 1 */)
{
	return itemid * 1000 + level ;
}


Title::Title( unsigned int id/*, ID_TYPE type = T_ITEMTYPE*/,bool isViewTeam)
{
	mIsViewTeam=isViewTeam;
// 	if(type == T_ID)
// 	{
// 		mTitleInfo = ServerDateManager::Get()->getTitleInfoByID(id);
// 		if(mTitleInfo)
// 		{
// 			mTitleItem = TitleTableManager::Get()->getTitleItemByID(mTitleInfo->itemid);
// 		}
// 		else
// 		{
// 
// 		}
// 	}
// 	else if(type = T_ITEMID)
// 	{
// 		mTitleInfo = NULL;
// 		mTitleItem = TitleTableManager::Get()->getTitleItemByID(id);
// 	}
// 	else if(type = T_ITEMTYPE)
// 	{
// 		mTitleInfo =  NULL;
// 
// 		TitleInfoListIterator itr = ServerDateManager::Get()->getTitleIterator();
// 		while(itr.hasMoreElements())
// 		{
// 			TitleInfo* tInfo = itr.getNext();
// 			if(id == getTypeByItemID(tInfo->itemid))
// 			{
// 				mTitleInfo = tInfo;
// 				break;
// 			}
// 		}
// 
// 		if(mTitleInfo)
// 		{
// 			mTitleItem = TitleTableManager::Get()->getTitleItemByID(mTitleInfo->itemid);
// 		}
// 		else
// 		{
// 			mTitleItem = TitleTableManager::Get()->getTitleItemByID(getItemIDByType(id));
// 		}
// 	}
// 	else
// 	{
// 
// 	}
	mTitleLvOneId = id*1000+1;
	mTitleInfo =  NULL;
	if(!mIsViewTeam)
	{
		TitleInfoListIterator itr = ServerDateManager::Get()->getTitleIterator();
		while(itr.hasMoreElements())
		{
			TitleInfo* tInfo = itr.getNext();
			if(id == getTypeByItemID(tInfo->itemid))
			{
				mTitleInfo = tInfo;
				break;
			}
		}
	}
	else
	{
		TitleInfoListIterator itr = ServerDateManager::Get()->getViewTeamBattleInfoItem().getTitleIterator();
		while(itr.hasMoreElements())
		{
			TitleInfo* tInfo = itr.getNext();
			if(id == getTypeByItemID(tInfo->itemid))
			{
				mTitleInfo = tInfo;
				break;
			}
		}
	}

	if(mTitleInfo)
	{
		mTitleItem = TitleTableManager::Get()->getTitleItemByID(mTitleInfo->itemid);
	}
	else
	{
		mTitleItem = TitleTableManager::Get()->getTitleItemByID(getItemIDByType(id));
	}
}

unsigned int Title::id()
{
	if(mTitleInfo)
		return mTitleInfo->id;
	
	return 0;
}

unsigned int Title::itemID()
{
	if(mTitleInfo)
		return mTitleInfo->itemid;
	else if(mTitleItem)
		mTitleItem->itemID;

	return 0;
}

const std::string& Title::name()
{
	if(mTitleItem)
		return mTitleItem->name;
	static std::string emptyString("");
	return emptyString;
}

unsigned int Title::buffType()
{
	return mTitleItem->buffType;
}

unsigned int Title::time()
{
	return mTitleItem->time;
}

std::string Title::levelupCondition()
{
	std::string s;
	int lines;
	int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("Title_Describe"));
	GameMaths::stringAutoReturn(mTitleItem->levelupCondition, s, width, lines);
	return s;
}
std::string Title::getNextLevelUpCondition()
{
	std::string s="";
	int lines;
	int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("Title_Describe"));
	unsigned int itemId = mTitleLvOneId;
	if(mTitleInfo)
	{
		itemId = getNextTitleItemID();
	}
	const TitleItem* titleItem = TitleTableManager::Get()->getTitleItemByID(itemId);
	if(titleItem)
	{
		GameMaths::stringAutoReturn(titleItem->levelupCondition, s, width, lines);
	}
	return s;
}

std::string Title::describe()
{
    std::string s;
    int lines;
    int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("Title_Describe"));
    GameMaths::stringAutoReturn(mTitleItem->describe, s, width, lines);
	return s;
}

const std::string& Title::iconPic()
{
	return mTitleItem->iconPic;
}

const std::string& Title::namePic()
{
	return mTitleItem->namePic;
}

unsigned int Title::type()
{
	return getTypeByItemID(itemID());
}

unsigned int Title::level()
{
	return itemID() - type() * 1000;
}

unsigned int Title::quantity()
{
	if(mTitleInfo)
		return mTitleItem->quantity;

	return 0;
}

bool Title::hasNextTitle()
{
	return TitleTableManager::Get()->getTitleItemByID(getNextTitleItemID()) != NULL;
}

unsigned int Title::getNextTitleItemID()
{
	if(itemID()>0)
	{
		return itemID()+1;
	}else
	{
		return mTitleLvOneId;
	}
	
}

unsigned int Title::nextQuantity()
{
	const TitleItem* item = TitleTableManager::Get()->getTitleItemByID(getNextTitleItemID());
	if(item)
		return item->quantity;

	return 0;
}

cocos2d::ccColor3B Title::getFrameColor()
{
	std::string color = VaribleManager::Get()->getSetting("FrameColor_Title");
	return StringConverter::parseColor3B(color);
}

cocos2d::CCSprite* Title::getFrameNormalSpirte(std::string ext/*=""*/)
{
	cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("FrameNormal_ImageFile" + ext).c_str());
	sprite->setColor(getFrameColor());
	return sprite;
}

cocos2d::CCSprite* Title::getFrameSelectedSpirte(std::string ext/*=""*/)
{
	cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("FrameSelected_ImageFile" + ext).c_str());
	sprite->setColor(getFrameColor());
	return sprite;
}

cocos2d::CCSprite* Title::getFrameDisabledSpirte(std::string ext/*=""*/)
{
	cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("FrameDisabled_ImageFile" + ext).c_str());
	sprite->setColor(getFrameColor());
	return sprite;
}

std::string Title::getTitleBuffPic()
{
	return VaribleManager::Get()->getSetting("TitleBuff_ImageFile");
}

std::string Title::getQuantityString()
{
	if(buffType() == 1)
	{
		return "+"+StringConverter::toString(quantity());
	}
	else
	{
		return "+"+StringConverter::toString(quantity()) + "%";
	}
}

std::string Title::getNextQuantityString()
{
	if(buffType() == 1)
	{
		return "+"+StringConverter::toString(nextQuantity());
	}
	else
	{
		return "+"+StringConverter::toString(nextQuantity()) + "%";
	}
}

std::string Title::getLeftTimeString()
{
	if(time() == 0)
		return Language::Get()->getString("@Forevery");
	else
	{
		return "1:00:00";
	}
}

unsigned int Title::getlefttime()
{
	return 1000;
}

unsigned int Title::startime()
{
	if(mTitleInfo)
		return mTitleInfo->startime;

	return 0;
}

bool Title::hasGet()
{
	return mTitleInfo != NULL;
}

bool TitleSort_default(unsigned int id1, unsigned int id2)
{
// 	Title Title1(id1);
// 	Title Title2(id2);
// 
// 	if(Title1.quality() < Title2.quality())
// 		return true;
// 	else if(Title2.quality() < Title1.quality())
// 		return false;
// 
// 	if(Title1.level() > Title2.level())
// 		return true;
// 	else
// 		return false;

	return true;

}

bool TitleFilter_default( unsigned int id )
{
	return true;
}

TitleDateCollect::TitleIDCollect TitleDateCollect::getTitleCollection(TitleFilter sFilter/*=TitleFilter_default*/, TitleSort sSort/*=TitleSort_default*/)
{
	TitleIDCollect myList;
	TitleInfoListIterator itr = ServerDateManager::Get()->getTitleIterator();
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