
#include "stdafx.h"

#include "ServerDateManager.h"
#include "MessageBoxPage.h"
#include "BlackBoard.h"
#include "STLUtility.h"
#include "TimeCalculator.h"
#include "StringConverter.h"
#include "AdventurePage.h"
unsigned int ServerDateManager::getNextLevelExp()
{
	unsigned int nextExp=0;
	const PlayerGradeItem* item=PlayerGradeTableManager::Get()->getGradeItemByID(mUserBasicInfo.level);
	int maxLevel=StringConverter::parseInt(VaribleManager::Get()->getSetting("PlayerMaxLevel"));
	if(item)
	{
		while(item->exp<=mUserBasicInfo.exp)
		{
			if(mUserBasicInfo.level>=maxLevel)
			{
				mUserBasicInfo.level=maxLevel;
				mUserBasicInfo.exp=item->exp;
				break;
			}
			else
			{
				mUserBasicInfo.level++;
				item=PlayerGradeTableManager::Get()->getGradeItemByID(mUserBasicInfo.level);
			}
		}
		nextExp=item->exp;
	}
	return nextExp;
}
float ServerDateManager::getUserExpPercent()
{
	int targetExpLine = ServerDateManager::Get()->getNextLevelExp();
	int nowExpLine = 0;
	if(mUserBasicInfo.level>1)
	{
		const PlayerGradeItem* item=PlayerGradeTableManager::Get()->getGradeItemByID(mUserBasicInfo.level-1);
		if(item)
		{
			nowExpLine =item->exp;
		}
		
	}
	return (float)(mUserBasicInfo.exp-nowExpLine)/(targetExpLine-nowExpLine);
}
const std::string ServerDateManager::getUserExpPercentStr()
{
	std::string expStr = "0/0";
	int targetExpLine = ServerDateManager::Get()->getNextLevelExp();
	int nowExpLine = 0;
	if(mUserBasicInfo.level>1)
	{
		const PlayerGradeItem* item=PlayerGradeTableManager::Get()->getGradeItemByID(mUserBasicInfo.level-1);
		if(item)
		{
			nowExpLine =item->exp;
		}
	}
	int currExp=mUserBasicInfo.exp-nowExpLine;
	if(currExp<0)
	{
		OPUserBasicInfo info;
		info.set_version(1);
		PacketManager::Get()->sendPakcet(OPCODE_GET_USER_BASIC_INFO_C,&info);
		currExp=0;
	}
	expStr = StringConverter::toString(currExp)+"/"+StringConverter::toString(targetExpLine-nowExpLine);
	return expStr;
}

UserToolInfoListIterator ServerDateManager::getToolIterator( void )
{
	return UserToolInfoListIterator(mUserToolInfoList.begin(), mUserToolInfoList.end());
}

UserToolInfo * ServerDateManager::getUserToolInfoByID( unsigned int id )
{
	if(mUserToolInfoList.find(id) != mUserToolInfoList.end())
		return mUserToolInfoList.find(id)->second;

	return NULL;
}

UserToolInfo * ServerDateManager::getUserToolInfoByItemId(unsigned int id)
{
	if(mUserToolItemKeyList.find(id)!=mUserToolItemKeyList.end())
	{
		return getUserToolInfoByID(mUserToolItemKeyList.find(id)->second);
	}
	return NULL;
}

UserToolInfo * ServerDateManager::getUserToolInfoByIndex( unsigned int index )
{
	if(index>=mUserToolInfoList.size())
		return NULL;
	UserToolInfoList::iterator itr = mUserToolInfoList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

void ServerDateManager::creatUserToolKeyMapByToolItem(const UserToolInfo* userToolInfo)
{
	if(!userToolInfo)
	{
		return;
	}
	if(mUserToolItemKeyList.find(userToolInfo->itemid)!=mUserToolItemKeyList.end())
	{
		return;
	}
	else
	{
		mUserToolItemKeyList.insert(UserItemKeyIdMap::value_type(userToolInfo->itemid,userToolInfo->id));
	}
}

UserToolInfo* ServerDateManager::getAndCreatToolInfo( unsigned int id )
{
	UserToolInfo* info = NULL;
	if(mUserToolInfoList.find(id) != mUserToolInfoList.end())
		info = mUserToolInfoList.find(id)->second;
	else
	{
		info = new UserToolInfo();
		info->id = id;
		mUserToolInfoList.insert(UserToolInfoList::value_type(id,info));
	}
	return info;
}

UserDiscipleInfoListIterator ServerDateManager::getDiscipleIterator( void )
{
	return UserDiscipleInfoListIterator(mUserDiscipleInfoList.begin(), mUserDiscipleInfoList.end());
}

UserDiscipleInfo * ServerDateManager::getUserDiscipleInfoByID( unsigned int id )
{
	if(mUserDiscipleInfoList.find(id) != mUserDiscipleInfoList.end())
		return mUserDiscipleInfoList.find(id)->second;

	return NULL;
}

UserDiscipleInfo * ServerDateManager::getUserDiscipleInfoByItemID( unsigned int itemId )
{
	UserDiscipleInfoList::iterator itr = mUserDiscipleInfoList.begin();
	while(itr != mUserDiscipleInfoList.end())
	{
		if(itr->second->itemid == itemId)
			return itr->second;
		++itr;
	}

	return NULL;
}

UserDiscipleInfo * ServerDateManager::getUserDiscipleInfoByIndex( unsigned int index )
{
	if(index>=mUserDiscipleInfoList.size())
		return NULL;
	UserDiscipleInfoList::iterator itr = mUserDiscipleInfoList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;

}

void ServerDateManager::removeDiscipleInfoById(unsigned int id)
{
	if(mUserDiscipleInfoList.find(id) != mUserDiscipleInfoList.end())
	{
		delete mUserDiscipleInfoList.find(id)->second;
		mUserDiscipleInfoList.erase(mUserDiscipleInfoList.find(id));
	}
}

UserDiscipleInfo* ServerDateManager::getAndCreatDiscipleInfo( unsigned int id )
{
	UserDiscipleInfo* info = NULL;
	if(mUserDiscipleInfoList.find(id) != mUserDiscipleInfoList.end())
		info = mUserDiscipleInfoList.find(id)->second;
	else
	{
		info = new UserDiscipleInfo();
		info->id = id;
		mUserDiscipleInfoList.insert(UserDiscipleInfoList::value_type(id,info));
	}
	return info;
}

UserSoulInfoListIterator ServerDateManager::getSoulIterator( void )
{
	return UserSoulInfoListIterator(mUserSoulInfoList.begin(), mUserSoulInfoList.end());
}

void ServerDateManager::increaseSoulInfoById(unsigned int id,int count)
{
	UserSoulInfoList::iterator itr = mUserSoulInfoList.find(id);
	if(itr!=mUserSoulInfoList.end())
	{
		itr->second->count+=count;
		if(itr->second->count<=0)
		{
			delete itr->second;
			mUserSoulInfoList.erase(itr);
		}
	}
}

UserSoulInfo * ServerDateManager::getUserSoulInfoByDiscipleID( unsigned int id )
{
	UserDiscipleInfo * disciple = getUserDiscipleInfoByID(id);
	UserSoulInfoList::iterator itr = mUserSoulInfoList.begin();
	while(itr != mUserSoulInfoList.end())
	{
		if(itr->second->itemid == disciple->itemid)
			return itr->second;
		++itr;
	}
	return NULL;
}

UserSoulInfo * ServerDateManager::getUserSoulInfoByID( unsigned int id )
{
	if(mUserSoulInfoList.find(id) != mUserSoulInfoList.end())
		return mUserSoulInfoList.find(id)->second;

	return NULL;
}

UserSoulInfo * ServerDateManager::getUserSoulInfoByIndex( unsigned int index )
{
	if(index>=mUserSoulInfoList.size())
		return NULL;
	UserSoulInfoList::iterator itr = mUserSoulInfoList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

UserSoulInfo* ServerDateManager::getAndCreatSoulInfo( unsigned int id )
{
	UserSoulInfo* info = NULL;
	if(mUserSoulInfoList.find(id) != mUserSoulInfoList.end())
		info = mUserSoulInfoList.find(id)->second;
	else
	{
		info = new UserSoulInfo();
		info->id = id;
		mUserSoulInfoList.insert(UserSoulInfoList::value_type(id,info));
	}
	return info;
}

SkillInfoListIterator ServerDateManager::getSkillIterator( void )
{
	return SkillInfoListIterator(mSkillInfoList.begin(), mSkillInfoList.end());
}

SkillInfo * ServerDateManager::getSkillInfoByID( unsigned int id )
{
	if(mSkillInfoList.find(id) != mSkillInfoList.end())
		return mSkillInfoList.find(id)->second;

	return NULL;
}

SkillInfo * ServerDateManager::getSkillInfoByIndex( unsigned int index )
{
	if(index>=mSkillInfoList.size())
		return NULL;
	SkillInfoList::iterator itr = mSkillInfoList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

void ServerDateManager::removeSkillInfoById(unsigned int id)
{
	if(mSkillInfoList.find(id) != mSkillInfoList.end())
	{
		delete mSkillInfoList.find(id)->second;
		mSkillInfoList.erase(mSkillInfoList.find(id));
	}
}

SkillInfo* ServerDateManager::getAndCreatSkillInfo( unsigned int id )
{
	SkillInfo* info = NULL;
	if(mSkillInfoList.find(id) != mSkillInfoList.end())
		info = mSkillInfoList.find(id)->second;
	else
	{
		info = new SkillInfo();
		info->id = id;
		mSkillInfoList.insert(SkillInfoList::value_type(id,info));
	}
	return info;
}

UserEquipInfoListIterator ServerDateManager::getUserEquipIterator( void )
{
	return UserEquipInfoListIterator(mUserEquipInfoList.begin(), mUserEquipInfoList.end());
}

void ServerDateManager::removeEquipInfoById(unsigned int id)
{
	if(mUserEquipInfoList.find(id) != mUserEquipInfoList.end())
	{

		UserEquipInfo * userEquipInfo = mUserEquipInfoList.find(id)->second;
		int mapSize = userEquipInfo->stoneMap.size();
		for(int index = 1;index <= mapSize;index++){
			if(userEquipInfo->stoneMap.find(index) != userEquipInfo->stoneMap.end()){
				delete userEquipInfo->stoneMap.find(index)->second;
				userEquipInfo->stoneMap.erase(userEquipInfo->stoneMap.find(index));
			}

		}
		
		delete mUserEquipInfoList.find(id)->second;
		mUserEquipInfoList.erase(mUserEquipInfoList.find(id));
	}
}

UserEquipInfo * ServerDateManager::getUserEquipInfoByID( unsigned int id )
{
	if(mUserEquipInfoList.find(id) != mUserEquipInfoList.end())
		return mUserEquipInfoList.find(id)->second;

	return NULL;
}

UserEquipInfo * ServerDateManager::getEquipInfoByIndex( unsigned int index )
{
	if(index>=mUserEquipInfoList.size())
		return NULL;
	UserEquipInfoList::iterator itr = mUserEquipInfoList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

UserEquipInfo* ServerDateManager::getAndCreatUserEquipInfo( unsigned int id )
{
	UserEquipInfo* info = NULL;
	if(mUserEquipInfoList.find(id) != mUserEquipInfoList.end())
		info = mUserEquipInfoList.find(id)->second;
	else
	{
		info = new UserEquipInfo();
		info->id = id;
		mUserEquipInfoList.insert(UserEquipInfoList::value_type(id,info));
	}
	return info;
}

UserBattleInfoListIterator ServerDateManager::getUserBattleIterator( void )
{
	return UserBattleInfoListIterator(mUserBattleArray.begin(), mUserBattleArray.end());
}

unsigned int ServerDateManager::getUserBattleSize( void )
{
	unsigned int size = 0;
	UserBattleInfoList::iterator itr = mUserBattleArray.begin();
	for(;itr!=mUserBattleArray.end();++itr)
	{
		if(itr->second->discipleId!=0)
		{
			size++;
		}
	}
	return size;
}

UserBattleInfo* ServerDateManager::getUserBattleInfoByDisciple( unsigned int discipleID )
{
// 	for(unsigned int i=0; i<MaxBattleDiscipleNum; i++)
// 	{
// 		if(mUserBattleArray[i].discipleId == discipleID)
// 			return &mUserBattleArray[i];
// 	}
// 	return NULL;

	for(UserBattleInfoList::iterator itr = mUserBattleArray.begin();
		itr != mUserBattleArray.end(); ++itr)
	{
		if(itr->second->discipleId == discipleID)
			return itr->second;
	}
	return NULL;
}

UserBattleInfo* ServerDateManager::getUserBattleInfo( unsigned int position )
{
	for(UserBattleInfoList::iterator itr = mUserBattleArray.begin();
		itr != mUserBattleArray.end(); ++itr)
	{
		if(itr->second->position == position)
			return itr->second;
	}
	return NULL;

// 	for(unsigned int i=0; i<MaxBattleDiscipleNum; i++)
// 	{
// 		if(mUserBattleArray[i].position == index)
// 			return &mUserBattleArray[i];
// 	}
// 	return NULL;
}

UserBattleInfo * ServerDateManager::getUserBattleInfoByIndex( unsigned int index )
{
	if(index>=mUserBattleArray.size())
		return NULL;
	UserBattleInfoList::iterator itr = mUserBattleArray.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

UserBattleInfo* ServerDateManager::getAndCreatUserBattleInfo( unsigned int id )
{
// 	for(unsigned int i=0; i<MaxBattleDiscipleNum; i++)
// 	{
// 		if(mUserBattleArray[i].id == id)
// 			return &mUserBattleArray[i];
// 	}
// 	return NULL;

	UserBattleInfo* info = NULL;
	if(mUserBattleArray.find(id) != mUserBattleArray.end())
		info = mUserBattleArray.find(id)->second;
	else
	{
		info = new UserBattleInfo();
		info->id = id;
		mUserBattleArray.insert(UserBattleInfoList::value_type(id,info));
	}
	return info;
}

TitleInfoListIterator ServerDateManager::getTitleIterator( void )
{
	return TitleInfoListIterator(mTitleInfoList.begin(), mTitleInfoList.end());
}

TitleInfo * ServerDateManager::getTitleInfoByID( unsigned int id )
{
	if(mTitleInfoList.find(id) != mTitleInfoList.end())
		return mTitleInfoList.find(id)->second;

	return NULL;
}

TitleInfo * ServerDateManager::getTitleInfoByIndex( unsigned int index )
{
	if(index>=mTitleInfoList.size())
		return NULL;
	TitleInfoList::iterator itr = mTitleInfoList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

TitleInfo* ServerDateManager::getAndCreatTitleInfo( unsigned int id )
{
	TitleInfo* info = NULL;
	if(mTitleInfoList.find(id) != mTitleInfoList.end())
		info = mTitleInfoList.find(id)->second;
	else
	{
		info = new TitleInfo();
		info->id = id;
		info->itemid = 0;
		mTitleInfoList.insert(TitleInfoList::value_type(id,info));
	}
	return info;
}

UserSysMsgInfo * ServerDateManager::getUserSysMsgEntityByID(unsigned int id)
{
	if(mSysMsgList.find(id) != mSysMsgList.end())
		return mSysMsgList.find(id)->second;

	return NULL;
}

UserSysMsgInfo * ServerDateManager::getUserSysMsgEntityByIndex( unsigned int index )
{
	if(index>=mSysMsgList.size())
		return NULL;
	UserSysMsgEntityMap::iterator itr = mSysMsgList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

UserSysMsgEntityListIterator ServerDateManager::getSysMsgEntityIterator(void)
{
	return UserSysMsgEntityListIterator(mSysMsgList.begin(), mSysMsgList.end());
}

bool ServerDateManager::cmpSysMsgEntity(const UserSysMsgEntityMap::iterator a,const UserSysMsgEntityMap::iterator b)
{
	return a->second->eventTime>b->second->eventTime;
}

UserSysMsgEntityList* ServerDateManager::cloneSysMsgEntityList(const unsigned int tabIndex)
{
	UserSysMsgEntityList* cloneMap=new UserSysMsgEntityList;
	UserSysMsgEntityMap::iterator it=mSysMsgList.begin();
	for(;it!=mSysMsgList.end();++it)
	{
		if(tabIndex==1)
		{
			cloneMap->push_back(it);
		}
		else
		{
			if(tabIndex==2)
			{
				if((it->second->sysMsgType==2||it->second->sysMsgType==3))
				{
					cloneMap->push_back(it);
				}
				else
				{
					continue;
				}
				continue;
			}
			if(tabIndex==3)
			{
				if(it->second->sysMsgType==4||it->second->sysMsgType==5)
				{
					cloneMap->push_back(it);
				}
				else
				{
					continue;
				}
				continue;
			}
			if(tabIndex==4)
			{
				if(it->second->sysMsgType>=6)
				{
					cloneMap->push_back(it);
				}
				else
				{
					continue;
				}
				continue;
			}
			if(tabIndex==5)
			{
				if(it->second->sysMsgType==4)
				{
					cloneMap->push_back(it);
				}
				else
				{
					continue;
				}
				continue;
			}
		}

	}
	cloneMap->sort(&ServerDateManager::cmpSysMsgEntity);
	return cloneMap;
}

RewardDataMapIterator ServerDateManager::getSysMsgRewardMapIteratorById(unsigned int id)
{
	if(mSysMsgList.find(id)!=mSysMsgList.end()&&mSysMsgList.find(id)->second->rewardDataMap.size()!=0)
	{
		return RewardDataMapIterator(mSysMsgList.find(id)->second->rewardDataMap.begin(),mSysMsgList.find(id)->second->rewardDataMap.end());
	}
}

RewardData* ServerDateManager::createSysMsgRewardById(unsigned int id,unsigned int rewardId)
{
	RewardData* rewardItem=NULL;
	UserSysMsgInfo* info = NULL;
	if(mSysMsgList.find(id) != mSysMsgList.end())
		info = mSysMsgList.find(id)->second;
	else
	{
		info = new UserSysMsgInfo();
		info->id = id;
		mSysMsgList.insert(UserSysMsgEntityMap::value_type(id,info));
	}
	if(info->rewardDataMap.find(rewardId)!=info->rewardDataMap.end())
	{
		rewardItem=info->rewardDataMap.find(rewardId)->second;
	}
	else
	{
		rewardItem = new RewardData();
		rewardItem->id = id;
		info->rewardDataMap.insert(RewardDataMap::value_type(rewardId,rewardItem));
	}
	return rewardItem;
}

RewardData * ServerDateManager::getSysMsgRewardByIndex( unsigned int id,unsigned int rewardIndex )
{
	UserSysMsgInfo* info = getUserSysMsgEntityByID(id);
	if(info)
	{
		if(rewardIndex>=info->rewardDataMap.size())
			return NULL;
		RewardDataMap::iterator itr = info->rewardDataMap.begin();
		while(rewardIndex>0)
		{
			++itr;
			--rewardIndex;
		}

		return itr->second;
	}
	return NULL;
}

unsigned int ServerDateManager::getSysMsgRewardTotalNum( unsigned int id )
{
	UserSysMsgInfo* info = getUserSysMsgEntityByID(id);
	if(info)
		return info->rewardDataMap.size();
	return 0;
}

void ServerDateManager::clearSysMsgRewardMap(unsigned int msgId)
{
	UserSysMsgInfo* info = NULL;
	if(mSysMsgList.find(msgId) != mSysMsgList.end())
	{
		info = mSysMsgList.find(msgId)->second;
		RewardDataMap::iterator it=info->rewardDataMap.begin();
		for(;it!=info->rewardDataMap.end();++it)
		{
			if(it->second)
			{
				delete it->second;
			}
		}
		info->rewardDataMap.clear();
	}
}

UserSysMsgInfo* ServerDateManager::getAndCreatSysMsgEntity( unsigned int id )
{
	UserSysMsgInfo* info = NULL;
	if(mSysMsgList.find(id) != mSysMsgList.end())
		info = mSysMsgList.find(id)->second;
	else
	{
		info = new UserSysMsgInfo();
		info->id = id;
		mSysMsgList.insert(UserSysMsgEntityMap::value_type(id,info));
	}
	return info;
}

UserFriendsInfo * ServerDateManager::getUserFriendsInfoByID(unsigned int id,unsigned int nodeType)
{
	if(nodeType==1)
	{
		if(mFriendsEntityList.find(id) != mFriendsEntityList.end())
			return mFriendsEntityList.find(id)->second;
	}
	else if(nodeType==2)
	{
		if(mInviteEntityList.find(id) != mInviteEntityList.end())
			return mInviteEntityList.find(id)->second;
	}

	return NULL;
}

UserFriendsInfo * ServerDateManager::getUserFriendsInfoIndex( unsigned int index,unsigned int nodeType)
{
	UserFriendsInfoList::iterator itr;
	if(nodeType==1)
	{
		if(index>=mFriendsEntityList.size())
			return NULL;

		itr = mFriendsEntityList.begin();
	}
	else if(nodeType==2)
	{
		if(index>=mInviteEntityList.size())
			return NULL;

		itr = mInviteEntityList.begin();
	}
	else
		return NULL;

	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

UserFriendsInfoListIterator ServerDateManager::getFriendsInfoIterator(unsigned int nodeType)
{
	if(nodeType==1)
	{
		return UserFriendsInfoListIterator(mFriendsEntityList.begin(), mFriendsEntityList.end());
	}
	else if(nodeType==2)
	{
		return UserFriendsInfoListIterator(mInviteEntityList.begin(), mInviteEntityList.end());
	}
}

bool ServerDateManager::cmpUserFriendsInfo(const UserFriendsInfoList::iterator a,const UserFriendsInfoList::iterator b)
{
	return a->second->level>b->second->level;
}

UserFriendsInfoIterList* ServerDateManager::cloneUserFriendsInfoByType(const unsigned int nodeType,const unsigned int tabIndex)
{
	if(nodeType==1)
	{
		return cloneUserFriendsInfoByList(&mFriendsEntityList,tabIndex,nodeType);
	}
	else if(nodeType==2)
	{
		return cloneUserFriendsInfoByList(&mInviteEntityList,tabIndex,nodeType);
	}
}

UserFriendsInfoIterList* ServerDateManager::cloneUserFriendsInfoByList(UserFriendsInfoList* entityList,const unsigned int tabIndex,const unsigned int nodeType)
{
	UserFriendsInfoIterList* cloneMap=new UserFriendsInfoIterList;
	UserFriendsInfoList::iterator it=entityList->begin();
	for(;it!=entityList->end();++it)
	{
		if(nodeType==1)
		{
			if(tabIndex==1)
			{
				if(it->second->type==1)
				{
					cloneMap->push_back(it);
				}
			}
			else
			{
				if(tabIndex==2)
				{
					if((it->second->type==2||it->second->type==3))
					{
						cloneMap->push_back(it);
					}
					else
					{
						continue;
					}
					continue;
				}
				if(tabIndex==3)
				{
					if(it->second->type==4)
					{
						cloneMap->push_back(it);
					}
					else
					{
						continue;
					}
					continue;
				}
			}
		}
		else
		{
			cloneMap->push_back(it);
		}
	}
	cloneMap->sort(&ServerDateManager::cmpUserFriendsInfo);
	return cloneMap;
}

void ServerDateManager::delUserFriendsInfoById(unsigned int id,unsigned int nodeType)
{
	if(nodeType==1)
	{
		if(mFriendsEntityList.find(id) != mFriendsEntityList.end())
		{
			mFriendsEntityList.erase(mFriendsEntityList.find(id));
		}
	}
	else if(nodeType==2)
	{
		if(mInviteEntityList.find(id) != mInviteEntityList.end())
		{
			mInviteEntityList.erase(mInviteEntityList.find(id));
		}
	}
}

void ServerDateManager::clearInviteFriendList(void)
{
	mInviteEntityList.clear();
}

UserFriendsInfo* ServerDateManager::getAndCreatFriendInfo( unsigned int id )
{
	UserFriendsInfo* info = NULL;
	if(mFriendsEntityList.find(id) != mFriendsEntityList.end())
		info = mFriendsEntityList.find(id)->second;
	else
	{
		info = new UserFriendsInfo();
		info->id = id;
		mFriendsEntityList.insert(UserFriendsInfoList::value_type(id,info));
	}
	return info;
}

UserFriendsInfo* ServerDateManager::getAndCreatInviteFriendInfo( unsigned int id )
{
	UserFriendsInfo* info = NULL;
	if(mInviteEntityList.find(id) != mInviteEntityList.end())
		info = mInviteEntityList.find(id)->second;
	else
	{
		info = new UserFriendsInfo();
		info->id = id;
		mInviteEntityList.insert(UserFriendsInfoList::value_type(id,info));
	}
	return info;
}

void ServerDateManager::clearChatInfo()
{
	UserChatInfoList::iterator itr = mUserChatInfoList.begin();
	while(itr!=mUserChatInfoList.end())
	{
		delete itr->second;
		itr++;
	}
	mUserChatInfoList.clear();
}

UserChatInfoListIterator ServerDateManager::getChatIterator( void )
{
	return UserChatInfoListIterator(mUserChatInfoList.begin(), mUserChatInfoList.end());
}

UserChatInfo * ServerDateManager::getUserChatInfoByID( unsigned int id )
{
	if(mUserChatInfoList.find(id) != mUserChatInfoList.end())
		return mUserChatInfoList.find(id)->second;

	return NULL;
}

UserChatInfo* ServerDateManager::getAndCreatChatInfo( unsigned int id )
{
	UserChatInfo* info = NULL;
	if(mUserChatInfoList.find(id) != mUserChatInfoList.end())
		info = mUserChatInfoList.find(id)->second;
	else
	{
		info = new UserChatInfo();
		info->id = id;
		mUserChatInfoList.insert(UserChatInfoList::value_type(id,info));
	}
	return info;
}

UserChatInfo * ServerDateManager::getUserChatInfoByIndex( unsigned int index )
{
	if(index>=mUserChatInfoList.size())
		return NULL;
	UserChatInfoList::iterator itr = mUserChatInfoList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

ShopItem* ServerDateManager::getAndCreatShopItem( unsigned int id )
{
	ShopItem* info = NULL;
	if(mShopList.find(id) != mShopList.end())
		info = mShopList.find(id)->second;
	else
	{
		info = new ShopItem();
		mShopList.insert(ShopItemList::value_type(id,info));
	}
	return info;
}

ShopItem* ServerDateManager::getShopItemIndex( unsigned int index )
{
	if(index>=mShopList.size())
		return NULL;
	ShopItemList::iterator itr = mShopList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

BoardCastInfo * ServerDateManager::getBoardCastInfoByID(unsigned int id)
{
	if
		(mBoardCastInfoList.find(id) != mBoardCastInfoList.end())
		return mBoardCastInfoList.find(id)->second;

	return NULL;
}


BoardCastInfo * ServerDateManager::getBoardCastInfoByIndex( unsigned int index )
{
	if(index>=mBoardCastInfoList.size())
		return NULL;
	BoardCastInfoMap::iterator itr = mBoardCastInfoList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

BoardCastInfoMapIterator ServerDateManager::getBoardCastInfoIterator(void)
{
	return BoardCastInfoMapIterator(mBoardCastInfoList.begin(), mBoardCastInfoList.end());
}

BoardCastInfo* ServerDateManager::getAndCreatBoardCastInfoEntity( unsigned int id )
{
	BoardCastInfo* info = NULL;
	if(mBoardCastInfoList.find(id) != mBoardCastInfoList.end())
		info = mBoardCastInfoList.find(id)->second;
	else
	{
		info = new BoardCastInfo();
		info->id = id;
		mBoardCastInfoList.insert(BoardCastInfoMap::value_type(id,info));
	}
	return info;
}

ContinueLoginInfo::ContinueReward* ServerDateManager::_getContinueReward(unsigned int index)
{
	ContinueLoginInfo::ContinueReward* info = NULL;
	if(mContinueLoginInfo.continueReward.find(index) != mContinueLoginInfo.continueReward.end())
		info = mContinueLoginInfo.continueReward.find(index)->second;
	else
	{
		info = new ContinueLoginInfo::ContinueReward();
		info->id = index;
		mContinueLoginInfo.continueReward.insert(ContinueLoginInfo::ContinueRewardMap::value_type(index,info));
	}
	return info;
}

AdventureItemInfo * ServerDateManager::getAdventureItemInfoByID(unsigned int id)
{
	if(mAdventureItemInfoList.find(id) != mAdventureItemInfoList.end())
		return mAdventureItemInfoList.find(id)->second;

	return NULL;
}

AdventureItemInfo * ServerDateManager::getAdventureItemInfoByIndex( unsigned int index )
{
	if(index>=mAdventureItemInfoList.size())
		return NULL;
	AdventureItemInfoMap::iterator itr = mAdventureItemInfoList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

AdventureItemInfoMapIterator ServerDateManager::getAdventureItemInfoMapIterator(void)
{
	return AdventureItemInfoMapIterator(mAdventureItemInfoList.begin(), mAdventureItemInfoList.end());
}

void ServerDateManager::removeAdventureItemInfoById(const unsigned int id)
{
	if(mAdventureItemInfoList.find(id) != mAdventureItemInfoList.end())
	{
		removeAdventureChildListInfo(id);
		mAdventureItemInfoList.erase(mAdventureItemInfoList.find(id));
	}
}

bool ServerDateManager::minusSkillBookCountById(unsigned int id)
{
	if(mAdventureItemInfoList.find(id) != mAdventureItemInfoList.end())
	{
		AdventureItemInfo* itemInfo=getAndCreatAdventureItemInfo(id);
		const AdventureItem* adventureItem=AdventureTableManager::Get()->getAdventureItemByID(itemInfo->adventureId);
		if(adventureItem&&adventureItem->adventureType==SkillBook)
		{
			itemInfo->count--;
			pieceSkillBookBySkillId(itemInfo->itemId);
			if(itemInfo->count>0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}

void ServerDateManager::addSkillBookPieceChanceBySkillId(unsigned int skillId,unsigned int count)
{
	bool isHavSkillBook=false;
	AdventureItemInfoMap::iterator it=mAdventureItemInfoList.begin();
	for(;it!=mAdventureItemInfoList.end();++it)
	{
		if(it->second->itemId==skillId)
		{
			const AdventureItem* item=AdventureTableManager::Get()->getAdventureItemByID(it->second->adventureId);
			if(item->adventureType==SkillBook)
			{
				isHavSkillBook=true;
				it->second->count+=count;
				break;
			}
		}
	}
	if(!isHavSkillBook)
	{
		AdventureItemInfo* item=getAndCreatAdventureItemInfo(getAdventureMaxIdNext());
		if(item)
		{
			item->adventureId=SkillBook;
			item->itemId=skillId;
			item->count=count;
		}
	}
}

unsigned int ServerDateManager::getAdventureSkillBookPieceCount(unsigned int skillId)
{
	AdventureItemInfoMap::iterator it=mAdventureItemInfoList.begin();
	for(;it!=mAdventureItemInfoList.end();++it)
	{
		if(it->second->itemId==skillId)
		{
			const AdventureItem* item=AdventureTableManager::Get()->getAdventureItemByID(it->second->adventureId);
			if(item->adventureType==SkillBook)
			{
				return it->second->count;
			}
		}
	}
	return 0;
}

AdventureItemInfo* ServerDateManager::getAndCreatAdventureItemInfo( unsigned int id )
{
	AdventureItemInfo* info = NULL;
	if(mAdventureItemInfoList.find(id) != mAdventureItemInfoList.end())
		info = mAdventureItemInfoList.find(id)->second;
	else
	{
		if(mAdventureMaxId<id)
		{
			mAdventureMaxId=id;
		}
		info = new AdventureItemInfo();
		info->id = id;
		mAdventureItemInfoList.insert(AdventureItemInfoMap::value_type(id,info));
	}
	return info;
}

SkillBookInfo* ServerDateManager::getAndCreatSkillBookInfoBySkillId(unsigned int id)
{
	SkillBookInfo* info = NULL;
	if(mSkillBookInfoList.find(id) != mSkillBookInfoList.end())
		info = mSkillBookInfoList.find(id)->second;
	else
	{
		info = new SkillBookInfo();
		mSkillBookInfoList.insert(SkillBookInfoMap::value_type(id,info));
	}
	return info;
}

SkillBookInfo * ServerDateManager::getSkillBookInfoByIndex( unsigned int index )
{
	if(index>=mSkillBookInfoList.size())
		return NULL;
	SkillBookInfoMap::iterator itr = mSkillBookInfoList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

void ServerDateManager::addSkillBookPartBySkillIdAndPartId(unsigned int skillId,unsigned int partId,unsigned int count)
{
	if(skillId>0)
	{
		SkillBookInfo* info=getAndCreatSkillBookInfoBySkillId(skillId);
		if(getAdventureSkillBookPieceCount(skillId)<=0)
		{
			addSkillBookPieceChanceBySkillId(skillId,1);
		}
		if(info->mSkillBookItemList.find(partId)!=info->mSkillBookItemList.end())
		{
			info->mSkillBookItemList.find(partId)->second->partCount=count;
		}
		else
		{
			SkillBookInfo::SkillBookItem* partItem=new SkillBookInfo::SkillBookItem();
			partItem->partId=partId;
			partItem->partCount=count;
			info->mSkillBookItemList.insert(SkillBookInfo::SkillBookItemMap::value_type(partId,partItem));
		}
	}
}

SkillBookInfo::SkillBookItem* ServerDateManager::getSkillBookItemBySkillIdAndPartId(unsigned int skillId,unsigned int partId)
{
	SkillBookInfo* info=getAndCreatSkillBookInfoBySkillId(skillId);
	if(info->mSkillBookItemList.find(partId)!=info->mSkillBookItemList.end())
	{
		return info->mSkillBookItemList.find(partId)->second;
	}
	else
	{
		SkillBookInfo::SkillBookItem* partItem=new SkillBookInfo::SkillBookItem();
		partItem->partId=partId;
		partItem->partCount=0;
		info->mSkillBookItemList.insert(SkillBookInfo::SkillBookItemMap::value_type(partId,partItem));
		return partItem;
	}
}

SkillBookInfo::SkillBookItem * ServerDateManager::getSkillBookItemBySkillIdAndIndex( unsigned int skillId,unsigned int index )
{
	SkillBookInfo* info=getAndCreatSkillBookInfoBySkillId(skillId);
	if(!info)
		return NULL;

	if(index>=info->mSkillBookItemList.size())
		return NULL;
	SkillBookInfo::SkillBookItemMap::iterator itr = info->mSkillBookItemList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

unsigned int ServerDateManager::getSkillBookItemMapTotalNum( unsigned int skillId )
{
	SkillBookInfo* info=getAndCreatSkillBookInfoBySkillId(skillId);
	if(info)
		return info->mSkillBookItemList.size();
	return 0;
}

SkillBookInfo::SkillBookItemMapIterator ServerDateManager::getSkillBookItemMapIterator(unsigned int skillId)
{
	SkillBookInfo* info=getAndCreatSkillBookInfoBySkillId(skillId);
	return SkillBookInfo::SkillBookItemMapIterator(info->mSkillBookItemList.begin(),info->mSkillBookItemList.end());
}

SkillBookInfoMapIterator ServerDateManager::getSkillBookInfoMapIterator(void)
{
	return SkillBookInfoMapIterator(mSkillBookInfoList.begin(),mSkillBookInfoList.end());
}

void ServerDateManager::pieceSkillBookBySkillId(unsigned int skillId)
{
	SkillBookInfo* info=getAndCreatSkillBookInfoBySkillId(skillId);
	if(info)
	{
		SkillBookInfo::SkillBookItemMap::iterator it=info->mSkillBookItemList.begin();
		for(;it!=info->mSkillBookItemList.end();++it)
		{
			if(it->second->partCount>0)
			{
				--it->second->partCount;
			}
		}
	}
}

SkillBookLootInfo* ServerDateManager::getSkillBookLootItemById(unsigned int id)
{
	if(mSkillBookLootList.find(id) != mSkillBookLootList.end())
		return mSkillBookLootList.find(id)->second;
	return NULL;
}

SkillBookLootInfo * ServerDateManager::getSkillBookLootItemByIndex( unsigned int index )
{
	if(index>=mSkillBookLootList.size())
		return NULL;
	SkillBookLootInfoMap::iterator itr = mSkillBookLootList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

SkillBookLootInfoMapIterator ServerDateManager::getSkillBookLootMapIterator(void)
{
	return SkillBookLootInfoMapIterator(mSkillBookLootList.begin(),mSkillBookLootList.end());
}

SkillBookLootInfo* ServerDateManager::getAndCreatSkillBookLootInfoById(unsigned int id)
{
	SkillBookLootInfo* info = NULL;
	if(mSkillBookLootList.find(id) != mSkillBookLootList.end())
		info = mSkillBookLootList.find(id)->second;
	else
	{
		info = new SkillBookLootInfo();
		info->id = id;
		mSkillBookLootList.insert(SkillBookLootInfoMap::value_type(id,info));
	}
	return info;
}

void ServerDateManager::clearSkillBookLoot(void)
{
	SkillBookLootInfoMap::iterator it=mSkillBookLootList.begin();
	while(it!=mSkillBookLootList.end())
	{
		SkillBookLootInfo::PlayerTeamItemMap::iterator itp=it->second->mPlayerTeamList.begin();
		while(itp!=it->second->mPlayerTeamList.end())
		{
			delete itp->second;
			++itp;
		}
		it->second->mPlayerTeamList.clear();
		delete it->second;
		++it;
	}
	mSkillBookLootList.clear();
}

void ServerDateManager::removeFightEndAllTeamItem()
{
	AdventureFightEndInfo::FightTeamInfoMap::iterator it=mAdventureFightEndInfo.mFightTeamList.begin();
	while(it!=mAdventureFightEndInfo.mFightTeamList.end())
	{
		delete it->second;
		++it;
	}
	mAdventureFightEndInfo.mFightTeamList.clear();
}

AdventureFightEndInfo::FightTeamItem * ServerDateManager::getFightEndFightItemById(unsigned int id)
{
	if(mAdventureFightEndInfo.mFightTeamList.find(id) != mAdventureFightEndInfo.mFightTeamList.end())
		return mAdventureFightEndInfo.mFightTeamList.find(id)->second;

	return NULL;
}

AdventureFightEndInfo::FightTeamItem * ServerDateManager::getFightEndFightItemByIndex( unsigned int index )
{
	if(index>=mAdventureFightEndInfo.mFightTeamList.size())
		return NULL;
	AdventureFightEndInfo::FightTeamInfoMap::iterator itr = mAdventureFightEndInfo.mFightTeamList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

AdventureFightEndInfo::FightTeamInfoMapIterator ServerDateManager::getFightEndFightTeamInfoMapIterator(void)
{
	return AdventureFightEndInfo::FightTeamInfoMapIterator(mAdventureFightEndInfo.mFightTeamList.begin(), mAdventureFightEndInfo.mFightTeamList.end());
}

AdventureFightEndInfo::FightTeamItem* ServerDateManager::addFightEndFightItemById(unsigned int id)
{
	AdventureFightEndInfo::FightTeamItem* info = NULL;
	if(mAdventureFightEndInfo.mFightTeamList.find(id) != mAdventureFightEndInfo.mFightTeamList.end())
		info = mAdventureFightEndInfo.mFightTeamList.find(id)->second;
	else
	{
		info = new AdventureFightEndInfo::FightTeamItem();
		info->id = id;
		mAdventureFightEndInfo.mFightTeamList.insert(AdventureFightEndInfo::FightTeamInfoMap::value_type(id,info));
	}
	return info;
};

void ServerDateManager::removeFightEndAllStepAdditionItem()
{
	AdventureFightEndInfo::StepAdditionItemMap::iterator it=mAdventureFightEndInfo.mStepAdditionItemList.begin();
	while(it!=mAdventureFightEndInfo.mStepAdditionItemList.end())
	{
		delete it->second;
		++it;
	}
	mAdventureFightEndInfo.mStepAdditionItemList.clear();
}

AdventureFightEndInfo::StepAdditionItem * ServerDateManager::getFightEndStepAdditionItemById(unsigned int id)
{
	if(mAdventureFightEndInfo.mStepAdditionItemList.find(id) !=mAdventureFightEndInfo.mStepAdditionItemList.end())
		return mAdventureFightEndInfo.mStepAdditionItemList.find(id)->second;

	return NULL;
}

AdventureFightEndInfo::StepAdditionItem * ServerDateManager::getFightEndStepAdditionItemByIndex( unsigned int index )
{
	if(index>=mAdventureFightEndInfo.mStepAdditionItemList.size())
		return NULL;
	AdventureFightEndInfo::StepAdditionItemMap::iterator itr = mAdventureFightEndInfo.mStepAdditionItemList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

AdventureFightEndInfo::StepAdditionItemMapIterator ServerDateManager::getFightEndStepAdditionItemMapIterator(void)
{
	return AdventureFightEndInfo::StepAdditionItemMapIterator(mAdventureFightEndInfo.mStepAdditionItemList.begin(),mAdventureFightEndInfo.mStepAdditionItemList.end());
}

AdventureFightEndInfo::StepAdditionItem* ServerDateManager::addFightEndStepAdditionItemById(unsigned int id)
{
	AdventureFightEndInfo::StepAdditionItem* info = NULL;
	if(mAdventureFightEndInfo.mStepAdditionItemList.find(id) != mAdventureFightEndInfo.mStepAdditionItemList.end())
		info =mAdventureFightEndInfo.mStepAdditionItemList.find(id)->second;
	else
	{
		info = new AdventureFightEndInfo::StepAdditionItem();
		info->id = id;
		mAdventureFightEndInfo.mStepAdditionItemList.insert(AdventureFightEndInfo::StepAdditionItemMap::value_type(id,info));
	}
	return info;
};

void ServerDateManager::removeFightEndAllRewardItem()
{
	AdventureFightEndInfo::RewardItemMap::iterator it=mAdventureFightEndInfo.mRewardItemList.begin();
	while(it!=mAdventureFightEndInfo.mRewardItemList.end())
	{
		delete it->second;
		++it;
	}
	mAdventureFightEndInfo.mRewardItemList.clear();
}

//AdventureFightEndInfo::RewardItem * ServerDateManager::getFightEndRewardItemById(unsigned int id)
//{
//	if(mAdventureFightEndInfo.mRewardItemList.find(id) != mAdventureFightEndInfo.mRewardItemList.end())
//		return mAdventureFightEndInfo.mRewardItemList.find(id)->second;
//	return NULL;
//}

AdventureFightEndInfo::RewardItemMapIterator ServerDateManager::getFightEndRewardItemMapIterator(void)
{
	return AdventureFightEndInfo::RewardItemMapIterator(mAdventureFightEndInfo.mRewardItemList.begin(),mAdventureFightEndInfo.mRewardItemList.end());
}

AdventureFightEndInfo::RewardItem* ServerDateManager::addFightEndRewardItemById(unsigned int id)
{
	AdventureFightEndInfo::RewardItem* info = NULL;
	if(mAdventureFightEndInfo.mRewardItemList.find(id) != mAdventureFightEndInfo.mRewardItemList.end())
		info = mAdventureFightEndInfo.mRewardItemList.find(id)->second;
	else
	{
		info = new AdventureFightEndInfo::RewardItem();
		info->id = id;
		mAdventureFightEndInfo.mRewardItemList.insert(AdventureFightEndInfo::RewardItemMap::value_type(id,info));
	}
	return info;
};

AdventureFightEndInfo::RewardItem * ServerDateManager::getFightEndRewardItemById( unsigned int id )
{
	if(mAdventureFightEndInfo.mRewardItemList.find(id) != mAdventureFightEndInfo.mRewardItemList.end())
		return mAdventureFightEndInfo.mRewardItemList.find(id)->second;
	return NULL;
}

AdventureFightEndInfo::RewardItem * ServerDateManager::getFightEndRewardItemByIndex( unsigned int index )
{
	if(index>=mAdventureFightEndInfo.mRewardItemList.size())
		return NULL;
	AdventureFightEndInfo::RewardItemMap::iterator itr = mAdventureFightEndInfo.mRewardItemList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

void ServerDateManager::clearFightEndAllMap()
{ 
	removeFightEndAllTeamItem();
	removeFightEndAllStepAdditionItem();
	removeFightEndAllRewardItem();
};

////////////////////////////////////////////////////////////////////////
HandbookPersonInfoIterator ServerDateManager::getHandbookPersonIterator( void )
{
	return HandbookPersonInfoIterator(mHandbookPersonInfoList.begin(), mHandbookPersonInfoList.end());
}

HandbookPersonInfo * ServerDateManager::getHandbookPersonInfoByID( unsigned int id )
{
	if(mHandbookPersonInfoList.find(id) != mHandbookPersonInfoList.end())
		return mHandbookPersonInfoList.find(id)->second;

	return NULL;
}

HandbookPersonInfo * ServerDateManager::getHandbookPersonInfoByIndex( unsigned int index )
{
	if(index>=mHandbookPersonInfoList.size())
		return NULL;
	HandbookPersonInfoList::iterator itr = mHandbookPersonInfoList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

HandbookPersonInfo* ServerDateManager::getAndCreatHandbookPersonInfo( unsigned int id )
{
	HandbookPersonInfo* info = NULL;
	if(mHandbookPersonInfoList.find(id) != mHandbookPersonInfoList.end())
		info = mHandbookPersonInfoList.find(id)->second;
	else
	{
		info = new HandbookPersonInfo();
		info->id = id;
		mHandbookPersonInfoList.insert(HandbookPersonInfoList::value_type(id,info));
	}
	return info;
}

HandbookEquipmentInfoIterator ServerDateManager::getHandbookEquipmentIterator(){
	return HandbookEquipmentInfoIterator(mHandbookEquipmentInfoList.begin(),mHandbookEquipmentInfoList.end());
}
HandbookEquipmentInfo * ServerDateManager::getHandbookEquipmentInfoByID(unsigned int id){
	if(mHandbookEquipmentInfoList.find(id) != mHandbookEquipmentInfoList.end()){
		return mHandbookEquipmentInfoList.find(id)->second;
	}
	return NULL;
}

HandbookEquipmentInfo * ServerDateManager::getHandbookEquipmentInfoByIndex( unsigned int index )
{
	if(index>=mHandbookEquipmentInfoList.size())
		return NULL;
	HandbookEquipmentInfoList::iterator itr = mHandbookEquipmentInfoList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

HandbookEquipmentInfo * ServerDateManager::getAndCreatHandbookEquipmentInfo(unsigned int id){
	HandbookEquipmentInfo * info = NULL;
	if(mHandbookEquipmentInfoList.find(id) != mHandbookEquipmentInfoList.end()){
		info = mHandbookEquipmentInfoList.find(id)->second;
	}else{
		info = new HandbookEquipmentInfo();
		info->id = id;
		mHandbookEquipmentInfoList.insert(HandbookEquipmentInfoList::value_type(id,info));
	}
	return info;
}

HandbookSkillInfoIterator ServerDateManager::getHandbookSkillIterator(void){
	return HandbookSkillInfoIterator(mHandbookSkillInfoList.begin(),mHandbookSkillInfoList.end());
}
HandbookSkillInfo * ServerDateManager::getHandbookSkillInfoByID(unsigned int id){
	if(mHandbookSkillInfoList.find(id) != mHandbookSkillInfoList.end()){
		return mHandbookSkillInfoList.find(id)->second;
	}
	return NULL;
}

HandbookSkillInfo * ServerDateManager::getHandbookSkillInfoByIndex( unsigned int index )
{
	if(index>=mHandbookSkillInfoList.size())
		return NULL;
	HandbookSkillInfoList::iterator itr = mHandbookSkillInfoList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

HandbookSkillInfo* ServerDateManager::getAndCreatHandbookSkillInfo(unsigned int id){
	HandbookSkillInfo* info = NULL;
	if(mHandbookSkillInfoList.find(id) != mHandbookSkillInfoList.end()){
		info = mHandbookSkillInfoList.find(id)->second;
	}else{
		info = new HandbookSkillInfo();
		info->id = id;
		mHandbookSkillInfoList.insert(HandbookSkillInfoList::value_type(id,info));
	}
	return info;

};
////////////////////////////////////////////////////////////////////////

AdventureTeacherInfo* ServerDateManager::getAndCreateAdventureTeacherInfo(unsigned int id)
{
	AdventureTeacherInfo* info = NULL;
	if(mAdventureTeacherInfoList.find(id)!=mAdventureTeacherInfoList.end())
	{
		info = mAdventureTeacherInfoList.find(id)->second;
	}
	else
	{
		if(id>mAdventureMaxId)
		{
			mAdventureMaxId=id;
		}
		info = new AdventureTeacherInfo();
		info->id = id;
		mAdventureTeacherInfoList.insert(AdventureTeacherInfoMap::value_type(id,info));
	}
	return info;
}

void ServerDateManager::removeAdventureTeacherInfoById(unsigned int id)
{
	if(mAdventureTeacherInfoList.find(id) != mAdventureTeacherInfoList.end())
	{
		mAdventureTeacherInfoList.erase(mAdventureTeacherInfoList.find(id));
	}
}

void ServerDateManager::removeAdventureChildListInfo(const unsigned int id)
{
	const AdventureItemInfo* info = mAdventureItemInfoList.find(id)->second;
	const AdventureItem* adventureItem=AdventureTableManager::Get()->getAdventureItemByID(info->adventureId);
	if(adventureItem->adventureType==Teach)
	{
		removeAdventureTeacherInfoById(id);
	}
}

AdventureTeacherInfo* ServerDateManager::getAdventureTeacherInfoByID(unsigned int id)
{
	if(mAdventureTeacherInfoList.find(id)!=mAdventureTeacherInfoList.end())
	{
		return mAdventureTeacherInfoList.find(id)->second;
	}
	return NULL;
}
DiscipleExpAddedInfo* ServerDateManager::getAndCreateDiscipleExpAddedInfo(unsigned int id)
{
	DiscipleExpAddedInfo* info = NULL;
	if(mDiscipleExpAddedInfoList.find(id)!=mDiscipleExpAddedInfoList.end())
	{
		info = mDiscipleExpAddedInfoList.find(id)->second;
	}
	else
	{
		info = new DiscipleExpAddedInfo();
		info->id = id;
		mDiscipleExpAddedInfoList.insert(DiscipleExpAddedInfoMap::value_type(id,info));
	}
	return info;
}
void ServerDateManager::removeAllDiscipleExpAddedInfo()
{
	DiscipleExpAddedInfoMap::iterator itr = mDiscipleExpAddedInfoList.begin();
	while(itr!=mDiscipleExpAddedInfoList.end())
	{
		delete itr->second;
		itr++;
	}
	mDiscipleExpAddedInfoList.clear();
}

DiscipleExpAddedInfoMapIterator ServerDateManager::getDiscipleExpAddedIterator(void)
{
	return DiscipleExpAddedInfoMapIterator(mDiscipleExpAddedInfoList.begin(),mDiscipleExpAddedInfoList.end());
}

DiscipleExpAddedInfo* ServerDateManager::getDiscipleExpAddedInfoByID(unsigned int id)
{
	if(mDiscipleExpAddedInfoList.find(id)!=mDiscipleExpAddedInfoList.end())
	{
		return mDiscipleExpAddedInfoList.find(id)->second;
	}
	return NULL;
}

DiscipleExpAddedInfo* ServerDateManager::getDiscipleExpAddedInfoByIndex( unsigned int index )
{
	if(index>=mDiscipleExpAddedInfoList.size())
		return NULL;
	DiscipleExpAddedInfoMap::iterator itr = mDiscipleExpAddedInfoList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

FightEndRankInfo::RankItemMapIterator ServerDateManager::getFightEndRankMapIteratorByIndex(const unsigned int index)
{
	if(index==1)
	{
		return FightEndRankInfo::RankItemMapIterator(mFightEndRankInfo.mFiveRankList.begin(), mFightEndRankInfo.mFiveRankList.end());
	}
	else if(index==2)
	{
		return FightEndRankInfo::RankItemMapIterator(mFightEndRankInfo.mSixRankList.begin(), mFightEndRankInfo.mSixRankList.end());
	}
	else if(index==3)
	{
		return FightEndRankInfo::RankItemMapIterator(mFightEndRankInfo.mSevenRankList.begin(), mFightEndRankInfo.mSevenRankList.end());
	}
	else
	{
		return FightEndRankInfo::RankItemMapIterator(mFightEndRankInfo.mEightRankList.begin(), mFightEndRankInfo.mEightRankList.end());
	}
}

unsigned int ServerDateManager::getFightEndRankMapTotalNum(const unsigned int index)
{
	if(index==1)
	{
		return mFightEndRankInfo.mFiveRankList.size();
	}
	else if(index==2)
	{
		return mFightEndRankInfo.mSixRankList.size();
	}
	else if(index==3)
	{
		return mFightEndRankInfo.mSevenRankList.size();
	}
	else
	{
		return mFightEndRankInfo.mEightRankList.size();
	}
	return 0;
}

FightEndRankInfo::RankItem* ServerDateManager::getFightEndRankItemByIndexAndId(const unsigned int index,const unsigned int id)
{
	if(index==FightEndRankInfo::FIVE_BATTLE)
	{
		if(mFightEndRankInfo.mFiveRankList.find(id)!=mFightEndRankInfo.mFiveRankList.end())
		{
			return mFightEndRankInfo.mFiveRankList.find(id)->second;
		}
		return NULL;
	}
	else if(index==FightEndRankInfo::SIX_BATTLE)
	{
		if(mFightEndRankInfo.mSixRankList.find(id)!=mFightEndRankInfo.mSixRankList.end())
		{
			return mFightEndRankInfo.mSixRankList.find(id)->second;
		}
		return NULL;
	}
	else if(index==FightEndRankInfo::SEVEN_BATTLE)
	{
		if(mFightEndRankInfo.mSevenRankList.find(id)!=mFightEndRankInfo.mSevenRankList.end())
		{
			return mFightEndRankInfo.mSevenRankList.find(id)->second;
		}
		return NULL;
	}
	else
	{
		if(mFightEndRankInfo.mEightRankList.find(id)!=mFightEndRankInfo.mEightRankList.end())
		{
			return mFightEndRankInfo.mEightRankList.find(id)->second;
		}
		return NULL;
	}
	return NULL;
}

FightEndRankInfo::RankItem* ServerDateManager::getFightEndRankItemByTypeAndIndex(unsigned int type, unsigned int index)
{
	FightEndRankInfo::RankItemMap::iterator itr;
	if(type==FightEndRankInfo::FIVE_BATTLE)
	{
		if(index>=mFightEndRankInfo.mFiveRankList.size())
			return NULL;
		itr = mFightEndRankInfo.mFiveRankList.begin();
	}
	else if(type==FightEndRankInfo::SIX_BATTLE)
	{
		if(index>=mFightEndRankInfo.mSixRankList.size())
			return NULL;
		itr = mFightEndRankInfo.mSixRankList.begin();
	}
	else if(type==FightEndRankInfo::SEVEN_BATTLE)
	{
		if(index>=mFightEndRankInfo.mSevenRankList.size())
			return NULL;
		itr = mFightEndRankInfo.mSevenRankList.begin();
	}
	else
	{
		if(index>=mFightEndRankInfo.mEightRankList.size())
			return NULL;
		itr = mFightEndRankInfo.mEightRankList.begin();
	}
	
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

FightEndRankInfo::RankItem* ServerDateManager::addRankItemByIndexAndId(const unsigned int index,const unsigned int id)
{
	FightEndRankInfo::RankItem* info = NULL;
	if(index==FightEndRankInfo::FIVE_BATTLE)
	{
		if(mFightEndRankInfo.mFiveRankList.find(id) != mFightEndRankInfo.mFiveRankList.end())
			info = mFightEndRankInfo.mFiveRankList.find(id)->second;
		else
		{
			info = new FightEndRankInfo::RankItem();
			info->id = id;
			mFightEndRankInfo.mFiveRankList.insert(FightEndRankInfo::RankItemMap::value_type(id,info));
		}
	}
	else if(index==FightEndRankInfo::SIX_BATTLE)
	{
		if(mFightEndRankInfo.mSixRankList.find(id) != mFightEndRankInfo.mSixRankList.end())
			info = mFightEndRankInfo.mSixRankList.find(id)->second;
		else
		{
			info = new FightEndRankInfo::RankItem();
			info->id = id;
			mFightEndRankInfo.mSixRankList.insert(FightEndRankInfo::RankItemMap::value_type(id,info));
		}
	}
	else if(index==FightEndRankInfo::SEVEN_BATTLE)
	{
		if(mFightEndRankInfo.mSevenRankList.find(id) != mFightEndRankInfo.mSevenRankList.end())
			info = mFightEndRankInfo.mSevenRankList.find(id)->second;
		else
		{
			info = new FightEndRankInfo::RankItem();
			info->id = id;
			mFightEndRankInfo.mSevenRankList.insert(FightEndRankInfo::RankItemMap::value_type(id,info));
		}
	}
	else if(index==FightEndRankInfo::EIGHT_BATTLE)
	{
		if(mFightEndRankInfo.mEightRankList.find(id) !=mFightEndRankInfo.mEightRankList.end())
			info = mFightEndRankInfo.mEightRankList.find(id)->second;
		else
		{
			info = new FightEndRankInfo::RankItem();
			info->id = id;
			mFightEndRankInfo.mEightRankList.insert(FightEndRankInfo::RankItemMap::value_type(id,info));
		}
	}
	return info;
};


void ServerDateManager::clearAllFightEndRankItem()
{
	for(int i=1;i<=4;++i)
	{
		clearFightEndRankItemByIndex(i);
	}
}

void ServerDateManager::clearFightEndRankItemByIndex(const unsigned int index)
{
	if(index==FightEndRankInfo::FIVE_BATTLE)
	{
		FightEndRankInfo::RankItemMap::iterator it=mFightEndRankInfo.mFiveRankList.begin();
		while(it!=mFightEndRankInfo.mFiveRankList.end())
		{
			delete it->second;
			++it;
		}
		mFightEndRankInfo.mFiveRankList.clear();
	}
	else if(index==FightEndRankInfo::SIX_BATTLE)
	{
		FightEndRankInfo::RankItemMap::iterator it=mFightEndRankInfo.mSixRankList.begin();
		while(it!=mFightEndRankInfo.mSixRankList.end())
		{
			delete it->second;
			++it;
		}
		mFightEndRankInfo.mSixRankList.clear();
	}
	else if(index==FightEndRankInfo::SEVEN_BATTLE)
	{
		FightEndRankInfo::RankItemMap::iterator it=mFightEndRankInfo.mSevenRankList.begin();
		while(it!=mFightEndRankInfo.mSevenRankList.end())
		{
			delete it->second;
			++it;
		}
		mFightEndRankInfo.mSevenRankList.clear();
	}
	else
	{
		FightEndRankInfo::RankItemMap::iterator it=mFightEndRankInfo.mEightRankList.begin();
		while(it!=mFightEndRankInfo.mEightRankList.end())
		{
			delete it->second;
			++it;
		}
		mFightEndRankInfo.mEightRankList.clear();
	}
}

AdventureLevelUpRewardInfoIterator ServerDateManager::getAdventureLevelUpRewardInfoIterator(void){
	return AdventureLevelUpRewardInfoIterator(mAdventureLevelUpRewardInfoList.begin(),mAdventureLevelUpRewardInfoList.end());
}

AdventureLevelUpRewardInfo* ServerDateManager::getAdventureLevelUpRewardInfoByID(unsigned int id){
	if(mAdventureLevelUpRewardInfoList.find(id) != mAdventureLevelUpRewardInfoList.end()){
		return mAdventureLevelUpRewardInfoList.find(id)->second;
	}
	return NULL;

}

AdventureLevelUpRewardInfo* ServerDateManager::getAdventureLevelUpRewardInfoByIndex( unsigned int index )
{
	if(index>=mAdventureLevelUpRewardInfoList.size())
		return NULL;
	AdventureLevelUpRewardInfoList::iterator itr = mAdventureLevelUpRewardInfoList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

AdventureLevelUpRewardInfo* ServerDateManager::getAndCreatAdventureLevelUpRewardInfo(unsigned int id){
	AdventureLevelUpRewardInfo* info = NULL;
	if(mAdventureLevelUpRewardInfoList.find(id) != mAdventureLevelUpRewardInfoList.end()){
		info = mAdventureLevelUpRewardInfoList.find(id)->second;
	}else{
		info = new AdventureLevelUpRewardInfo();
		info->id = id;
		mAdventureLevelUpRewardInfoList.insert(AdventureLevelUpRewardInfoList::value_type(id,info));
	}
	return info;
}

void ServerDateManager::creatAdventureLevelUpGetRewardInfo(unsigned int rechargeNum,unsigned int goldCoins){
	UserBasicInfo& userBasicInfo = getUserBasicInfo();
	userBasicInfo.rechargenum = rechargeNum;
	userBasicInfo.goldcoins = goldCoins;
	return;
}


WorldBossInfo* ServerDateManager::getAndCreateWorldBossInfo(unsigned int id)
{
	WorldBossInfo* info = NULL;
	if(mWorldBossList.find(id)!=mWorldBossList.end())
	{
		info = mWorldBossList.find(id)->second;
	}
	else
	{
		info = new WorldBossInfo();
		info->bossId = id;
		mWorldBossList.insert(WorldBossInfoMap::value_type(id,info));
	}
	return info;
}
const WorldBossInfo* ServerDateManager::getWorldBossInfoByID(unsigned int id)
{
	if(mWorldBossList.find(id)!=mWorldBossList.end())
	{
		return mWorldBossList.find(id)->second;
	}
	return NULL;
}

WorldBossRankInfo* ServerDateManager::getAndCreateWorldBossRankInfo(unsigned int index)
{
	WorldBossRankInfo* info = NULL;
	if(mWorldBossRankList.find(index)!=mWorldBossRankList.end())
	{
		info = mWorldBossRankList.find(index)->second;
	}
	else
	{
		info = new WorldBossRankInfo();
		info->id = index;
		mWorldBossRankList.insert(WorldBossRankInfoMap::value_type(index,info));
	}
	return info;
}
WorldBossRankInfo* ServerDateManager::getWorldBossRankInfoById(unsigned int id)
{
	if(mWorldBossRankList.find(id)!=mWorldBossRankList.end())
	{
		return mWorldBossRankList.find(id)->second;
	}
	return NULL;
}

WorldBossRankInfo* ServerDateManager::getWorldBossRankInfoByIndex( unsigned int index )
{
	if(index>=mWorldBossRankList.size())
		return NULL;
	WorldBossRankInfoMap::iterator itr = mWorldBossRankList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

WorldBossRankInfoMapIterator ServerDateManager::getWorldBossRankInfoMapIterator(void)
{
	return WorldBossRankInfoMapIterator(mWorldBossRankList.begin(),mWorldBossRankList.end());
}
TeamBuffInfoMapIterator ServerDateManager::getTeamBuffInfoMapIterator(void)
{
	return TeamBuffInfoMapIterator(mTeamBuffInfoList.begin(),mTeamBuffInfoList.end());
}

TeamBuffInfo* ServerDateManager::getTeamBuffInfoById(const unsigned int id)
{
	if(mTeamBuffInfoList.find(id)!=mTeamBuffInfoList.end())
	{
		return mTeamBuffInfoList.find(id)->second;
	}
	return NULL;
}

TeamBuffInfo* ServerDateManager::getTeamBuffInfoByIndex( unsigned int index )
{
	if(index>=mTeamBuffInfoList.size())
		return NULL;
	TeamBuffInfoMap::iterator itr = mTeamBuffInfoList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

TeamBuffInfo* ServerDateManager::getAndCreatTeamBuffById(unsigned int id)
{
	TeamBuffInfo* info = NULL;
	if(mTeamBuffInfoList.find(id)!=mTeamBuffInfoList.end())
	{
		info = mTeamBuffInfoList.find(id)->second;
	}
	else
	{
		info = new TeamBuffInfo();
		info->teamId = id;
		mTeamBuffInfoList.insert(TeamBuffInfoMap::value_type(id,info));
	}
	return info;
}

RewardData* ServerDateManager::getAndCreateRewardData(unsigned int id)
{
	RewardData* info = NULL;
	if(mRewardDataList.find(id)!=mRewardDataList.end())
	{
		info = mRewardDataList.find(id)->second;
	}
	else
	{
		info = new RewardData();
		info->id = id;
		mRewardDataList.insert(RewardDataMap::value_type(id,info));
	}
	return info;
}
ResInfoMapIterator ServerDateManager::getResInfoIterator(void)
{
	return  ResInfoMapIterator(mResInfoList.begin(),mResInfoList.end());
}
ResInfo* ServerDateManager::getAndCreateResInfo(unsigned int id)
{
	ResInfo* info = NULL;
	if(mResInfoList.find(id)!=mResInfoList.end())
	{
		info = mResInfoList.find(id)->second;
	}
	else
	{
		info = new ResInfo();
		info->id = id;
		mResIDList.push_back(info->id);
		mResInfoList.insert(ResInfoMap::value_type(id,info));
	}
	return info;
}
ResInfo* ServerDateManager::getResInfoById(unsigned int id)
{
	if(mResInfoList.find(id)!=mResInfoList.end())
	{
		return mResInfoList.find(id)->second;
	}
	return NULL;
}

ResInfo* ServerDateManager::getResInfoByIndex( unsigned int index )
{
	if(index>=mResInfoList.size())
		return NULL;
	ResInfoMap::iterator itr = mResInfoList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

void ServerDateManager::clealAllResInfo()
{
	ResInfoMap::iterator it = mResInfoList.begin();
	while(it!=mResInfoList.end())
	{
		delete it->second;
		it++;
	}
	mResInfoList.clear();
	mResIDList.clear();
}
void ServerDateManager::rewardDataToResInfo()
{
	RewardDataMap::iterator itr =  mRewardDataList.begin();

	while(itr!=mRewardDataList.end())
	{
		RewardData* data = itr->second;
		ResInfo* info = NULL;
		if(mResInfoList.find(data->id)!=mResInfoList.end())
		{
			info = mResInfoList.find(data->id)->second;
		}
		else
		{
			info = new ResInfo();
			info->id = data->id;
			mResIDList.push_back(info->id);
			mResInfoList.insert(ResInfoMap::value_type(data->id,info));
		}
		info->count = data->count;
		info->type = data->itemType;
		info->itemId = data->itemId;
		ResManager::Get()->setResInfo(info);
		itr++;
	}
	RewardDataMap::iterator it = mRewardDataList.begin();
	while(it!=mRewardDataList.end())
	{
		delete it->second;
		it++;
	}
	mRewardDataList.clear();
}
bool resSort(unsigned int id1, unsigned int id2)
{
	ResInfo* info1 = ServerDateManager::Get()->getResInfoById(id1);
	ResInfo* info2 = ServerDateManager::Get()->getResInfoById(id2);
	if(info1->quality<=0 && info2->quality<=0)
	{
		return false;
	}
	if(info1->quality==0 && info2->quality>0)
	{
		return false;
	}
	if(info2->quality==0 && info1->quality>0)
	{
		return true;
	}
	if(info1->quality < info2->quality)
		return true;
	else if(info2->quality < info1->quality)
		return false;
	if(ResManager::Get()->getResMainType(info1->type)==EQUIP_TYPE&&ResManager::Get()->getResMainType(info2->type)!=EQUIP_TYPE)
	{
		return true;
	}
	if(ResManager::Get()->getResMainType(info1->type)!=EQUIP_TYPE&&ResManager::Get()->getResMainType(info2->type)==EQUIP_TYPE)
	{
		return false;
	}
	if(ResManager::Get()->getResMainType(info1->type)==SKILL_TYPE&&ResManager::Get()->getResMainType(info2->type)!=SKILL_TYPE)
	{
		return true;
	}
	if(ResManager::Get()->getResMainType(info1->type)!=SKILL_TYPE&&ResManager::Get()->getResMainType(info2->type)==SKILL_TYPE)
	{
		return false;
	}
	return false;
}
void ServerDateManager::sortResInfoList(ResSort rSort)
{
	 ResIDCollect res = mResIDList;
	 mResIDList.sort(rSort);
	 res = mResIDList;
}

ArenaOpponentInfoListIterator ServerDateManager::getArenaOpponentIterator( void )
{
	return ArenaOpponentInfoListIterator(mArenaOpponentInfoList.begin(), mArenaOpponentInfoList.end());
}

ArenaOpponentInfo * ServerDateManager::getArenaOpponentInfoByID( unsigned int id )
{
	if(mArenaOpponentInfoList.find(id) != mArenaOpponentInfoList.end())
		return mArenaOpponentInfoList.find(id)->second;

	return NULL;
}

ArenaOpponentInfo * ServerDateManager::getArenaOpponentInfoByIndex( unsigned int index )
{
	if(index>=mArenaOpponentInfoList.size())
		return NULL;
	ArenaOpponentInfoList::iterator itr = mArenaOpponentInfoList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}

	return itr->second;
}

ArenaOpponentInfo* ServerDateManager::getAndCreatArenaOpponentInfo( unsigned int id )
{
	ArenaOpponentInfo* info = NULL;
	if(mArenaOpponentInfoList.find(id) != mArenaOpponentInfoList.end())
		info = mArenaOpponentInfoList.find(id)->second;
	else
	{
		info = new ArenaOpponentInfo();
		info->id = id;
		mArenaOpponentInfoList.insert(ArenaOpponentInfoList::value_type(id,info));
	}
	return info;
}

WorldBossBattleMapIterator ServerDateManager::getWorldBossBattleIterator( void ){
	return WorldBossBattleMapIterator(mWorldBossBattleList.begin(),mWorldBossBattleList.end());
}

WorldBossBattleItem * ServerDateManager::getWorldBossBattleItemByID( unsigned int id ){

	/*
	WorldBossBattleItem * battleItem = new WorldBossBattleItem();
	battleItem->level = id;
	battleItem->damageBlood = id;
	battleItem->name = "hello"+id;
	battleItem->rank = id;
	return battleItem;
	*/
	if(mWorldBossBattleList.find(id) != mWorldBossBattleList.end()){
		return mWorldBossBattleList.find(id)->second;
	}
	return NULL;
}

void ServerDateManager::clearWorldBossBattle(){

	WorldBossBattleMap::iterator itr = mWorldBossBattleList.begin();
	while(itr != mWorldBossBattleList.end()){
		delete (itr->second);
		++itr;
	}
	WorldBossBattleMap worldBossBattleMap;
	mWorldBossBattleList.swap(worldBossBattleMap);
	mWorldBossBattleList.clear();
}

WorldBossBattleItem* ServerDateManager::getAndCreateWorldBossBattleItem(unsigned int id){
	WorldBossBattleItem* item = NULL;
	if(mWorldBossBattleList.find(id) != mWorldBossBattleList.end()) {
		item = mWorldBossBattleList.find(id)->second;
	}else{
		item = new WorldBossBattleItem();
		mWorldBossBattleList.insert(WorldBossBattleMap::value_type(id,item));
	}
	return item;
}

/*
WorldBossBattleMapIterator ServerDateManager::getWorldBossLastBattleIterator( void ){
	return WorldBossBattleMapIterator(mWorldBossLastBattleList.begin(),mWorldBossLastBattleList.end());
}

WorldBossBattleItem * ServerDateManager::getWorldBossLastBattleItemByID( unsigned int id ){

	if(mWorldBossLastBattleList.find(id) != mWorldBossLastBattleList.end()){
		return mWorldBossLastBattleList.find(id)->second;
	}
	return NULL;
}

void ServerDateManager::clearWorldBossLastBattle(){

	WorldBossBattleMap::iterator itr = mWorldBossLastBattleList.begin();
	while(itr != mWorldBossLastBattleList.end()){
		delete (itr->second);
		++itr;
	}
	WorldBossBattleMap worldBossLastBattleMap;
	mWorldBossLastBattleList.swap(worldBossLastBattleMap);
	mWorldBossLastBattleList.clear();
}

WorldBossBattleItem* ServerDateManager::getAndCreateWorldBossLastBattleItem(unsigned int id){
	WorldBossBattleItem* item = NULL;
	if(mWorldBossLastBattleList.find(id) != mWorldBossLastBattleList.end()) {
		item = mWorldBossLastBattleList.find(id)->second;
	}else{
		item = new WorldBossBattleItem();
		mWorldBossLastBattleList.insert(WorldBossBattleMap::value_type(id,item));
	}
	return item;
}
*/

void ServerDateManager::clearWorldBossDamage(){

	WorldBossDamageMap::iterator itr = mWorldBossDamageList.begin();
	while(itr != mWorldBossDamageList.end()){
		delete (itr->second);
		++itr;
	}
	WorldBossDamageMap worldBossDamageMap;
	mWorldBossDamageList.swap(worldBossDamageMap);
	mWorldBossDamageList.clear();
}


WorldBossDamageMapIterator ServerDateManager::getWorldBossDamageIterator(void ){
	return WorldBossDamageMapIterator(mWorldBossDamageList.begin(),mWorldBossDamageList.end());
}

WorldBossDamageItem * ServerDateManager::getWorldBossDamageItemByID(unsigned int id){
	if(mWorldBossDamageList.find(id) != mWorldBossDamageList.end()){
		return mWorldBossDamageList.find(id)->second;
	}
	return NULL;
}

WorldBossDamageItem* ServerDateManager::getAndCreateWorldBossDamageItem(unsigned int id){
	WorldBossDamageItem* item = NULL;
	if(mWorldBossDamageList.find(id) != mWorldBossDamageList.end()){
		item = mWorldBossDamageList.find(id)->second;
	}else{
		item = new WorldBossDamageItem();
		item->id = id;
		mWorldBossDamageList.insert(WorldBossDamageMap::value_type(id,item));
	}
	return item;
}

WorldBossDamageItem* ServerDateManager::getNextWorldBossDamageItem(){
	if(mWorldBossDamageList.find(worldBossDamageItemIndex) == mWorldBossDamageList.end()){

		worldBossDamageItemIndex = 1;
	}
	if(mWorldBossDamageList.find(worldBossDamageItemIndex) == mWorldBossDamageList.end()){
		return NULL;

	}
	return mWorldBossDamageList.find(worldBossDamageItemIndex++)->second;

}

/*
ChargeRebateItem* ServerDateManager::getChargeRebateItem(){
//	delete chargeRebateItem;
	chargeRebateItem = new ChargeRebateItem();
	chargeRebateItem->id = 1;
	chargeRebateItem->servertime = 1369710000;
	chargeRebateItem->chargeType = ChargeRebateItem::CHARGE;
	chargeRebateItem->recurittime = 1;
	chargeRebateItem->minRecharge = 100;
	chargeRebateItem->maxRecharge = 2000;
	chargeRebateItem->refund = 1;
	chargeRebateItem->rechargeInfact = 2140;
	chargeRebateItem->rebateBeginTime = 1369584000;
	chargeRebateItem->rebateEndTime = 1370448000;
	chargeRebateItem->getRewardToday = 0;
	chargeRebateItem->exchangeRate = 10;


	return chargeRebateItem;
}
*/

void ServerDateManager::clearAllArenaOpponentInfoList()
{
// 	ArenaOpponentInfoList::iterator itr = mArenaOpponentInfoList.begin();
// 	while (itr!=mArenaOpponentInfoList.end())
// 	{
// 		delete itr->second;
// 		++itr;
// 	}
// 	mArenaOpponentInfoList.clear();

	SAFE_RELEASE_MAP(mArenaOpponentInfoList);
	//for_each(mArenaOpponentInfoList.begin(), mArenaOpponentInfoList.end(), DeleteObj_Map());
}

/************************************************************************/
/*                         Sync-Power/Vitality                          */
/************************************************************************/
void ServerDateManager::updateBasicSyncTime()
{
	std::string basicSyncKey="basicSyncKey";
	bool isSync=false;
	unsigned int eachRecoverTime=30*60;
	if(BlackBoard::Get()->hasVarible(basicSyncKey))
	{
		isSync=BlackBoard::Get()->getVaribleAsBool(basicSyncKey);
		if(!isSync)
		{
			bool isNeedSync=false;
			long timeleft;
			bool isNeedReset=false;
			if(mUserBasicInfo.power<mUserBasicInfo.maxpower)
			{
				if(!TimeCalculator::Get()->getTimeLeft("nextPowerRecoverTime",timeleft))
				{
					isNeedSync=true;
				}
				if(timeleft<=0)
				{
					isNeedSync=true;
					//isNeedReset=true;
				}
			}
			else
			{
				TimeCalculator::Get()->getTimeLeft("nextPowerRecoverTime",timeleft);
				if(timeleft>0)
				{
					isNeedReset=true;
				}
			}
			if(mUserBasicInfo.vitality<mUserBasicInfo.maxvitality)
			{
				if(!TimeCalculator::Get()->getTimeLeft("nextVitalityRecoverTime",timeleft))
				{
					isNeedSync=true;
				}
				if(timeleft<=0)
				{
					isNeedSync=true;
					//isNeedReset=true;
				}
			}
			else
			{
				TimeCalculator::Get()->getTimeLeft("nextVitalityRecoverTime",timeleft);
				if(timeleft>0)
				{
					isNeedReset=true;
				}
			}
			//to server sync UserBasicInfo
			if(isNeedSync)
			{
				if(BlackBoard::Get()->hasVarible(basicSyncKey))
				{
					BlackBoard::Get()->setVarible(basicSyncKey,true);
				}
				else
				{
					BlackBoard::Get()->addVarible(basicSyncKey,true);
				}
				OPUserBasicInfo info;
				info.set_version(1);
				PacketManager::Get()->sendPakcet(OPCODE_GET_USER_BASIC_INFO_C,&info);
			}
			if(isNeedReset)
			{
				resetBasicNextSyncTime();
			}
		}
	}
}

void ServerDateManager::resetBasicNextSyncTime()
{
	std::string basicSyncKey="basicSyncKey";
	unsigned int eachRecoverTime=30*60;
	long timeleft;
	if(mUserBasicInfo.power<mUserBasicInfo.maxpower)
	{
		if((TimeCalculator::Get()->hasKey("nextPowerRecoverTime")&&TimeCalculator::Get()->getTimeLeft("nextPowerRecoverTime",timeleft))||!TimeCalculator::Get()->hasKey("nextPowerRecoverTime"))
		{
			int lastSurplusTime=(mUserBasicInfo.servertime-mUserBasicInfo.poweraddtime);
			while(eachRecoverTime<lastSurplusTime)
			{
				lastSurplusTime-=eachRecoverTime;
			}
			TimeCalculator::Get()->createTimeCalcultor("nextPowerRecoverTime",eachRecoverTime-lastSurplusTime);
			TimeCalculator::Get()->createTimeCalcultor("allPowerRecoverTime",(mUserBasicInfo.maxpower-mUserBasicInfo.power)*eachRecoverTime-lastSurplusTime);
		}
	}
	else
	{
		TimeCalculator::Get()->createTimeCalcultor("nextPowerRecoverTime",0);
		TimeCalculator::Get()->createTimeCalcultor("allPowerRecoverTime",0);
	}
	if(mUserBasicInfo.vitality<mUserBasicInfo.maxvitality)
	{
		if((TimeCalculator::Get()->hasKey("nextVitalityRecoverTime")&&TimeCalculator::Get()->getTimeLeft("nextVitalityRecoverTime",timeleft))||!TimeCalculator::Get()->hasKey("nextVitalityRecoverTime"))
		{
			int lastSurplusTime=(mUserBasicInfo.servertime-mUserBasicInfo.vitalityaddtim);
			while(eachRecoverTime<lastSurplusTime)
			{
				lastSurplusTime-=eachRecoverTime;
			}
			TimeCalculator::Get()->createTimeCalcultor("nextVitalityRecoverTime",eachRecoverTime-lastSurplusTime);
			TimeCalculator::Get()->createTimeCalcultor("allVitalityRecoverTime",(mUserBasicInfo.maxvitality-mUserBasicInfo.vitality)*eachRecoverTime-lastSurplusTime);
		}
	}
	else
	{
		TimeCalculator::Get()->createTimeCalcultor("nextVitalityRecoverTime",0);
		TimeCalculator::Get()->createTimeCalcultor("allVitalityRecoverTime",0);
	}

	if(BlackBoard::Get()->hasVarible(basicSyncKey))
	{
		BlackBoard::Get()->setVarible(basicSyncKey,false);
	}
	else
	{
		BlackBoard::Get()->addVarible(basicSyncKey,false);
	}
}
void ServerDateManager::getBasicNextTime()
{
	try
	{
		std::string basicSyncKey="basicSyncKey";
		unsigned int eachRecoverTime=30*60;
		long timeleft = 0;
		if(mUserBasicInfo.power<mUserBasicInfo.maxpower)
		{
			if((TimeCalculator::Get()->hasKey("nextPowerRecoverTime")&&TimeCalculator::Get()->getTimeLeft("nextPowerRecoverTime",timeleft))||!TimeCalculator::Get()->hasKey("nextPowerRecoverTime"))
			{
				TimeCalculator::Get()->createTimeCalcultor("allPowerRecoverTime",(mUserBasicInfo.maxpower-mUserBasicInfo.power-1)*eachRecoverTime+timeleft);
			}
		}
		else
		{
			TimeCalculator::Get()->createTimeCalcultor("nextPowerRecoverTime",0);
			TimeCalculator::Get()->createTimeCalcultor("allPowerRecoverTime",0);
		}
		timeleft = 0;
		if(mUserBasicInfo.vitality<mUserBasicInfo.maxvitality)
		{
			if((TimeCalculator::Get()->hasKey("nextVitalityRecoverTime")&&TimeCalculator::Get()->getTimeLeft("nextVitalityRecoverTime",timeleft))||!TimeCalculator::Get()->hasKey("nextVitalityRecoverTime"))
			{
				TimeCalculator::Get()->createTimeCalcultor("allVitalityRecoverTime",(mUserBasicInfo.maxvitality-mUserBasicInfo.vitality-1)*eachRecoverTime+timeleft);
			}
		}
		else
		{
			TimeCalculator::Get()->createTimeCalcultor("nextVitalityRecoverTime",0);
			TimeCalculator::Get()->createTimeCalcultor("allVitalityRecoverTime",0);
		}
	}
	catch (...)
	{

	}
}
ServerDateManager::ServerDateManager()
{
// 	UserArenaInfo& info = getUserArenaInfo();
// 	info.arenaRank = 1293;
// 	info.arenaScore = 5000;
// 	info.arenaNum = 6;
// 	info.rewardScore = 140;
// 	info.rewardRefreshTime = 123456;
// 	info.rewardIDs[3] = 3;
// 	info.rewardIDs[4] = 4;
}

ServerDateManager* ServerDateManager::getInstance()
{
	return ServerDateManager::Get();
}

ContinueLoginInfo& ServerDateManager::getContinueLoginInfo()
{
	return  mContinueLoginInfo;
}

ContinueLoginInfo::ContinueReward* ServerDateManager::getContinueRewardByIndex( unsigned int index )
{
	if(mContinueLoginInfo.continueReward.find(index)!=mContinueLoginInfo.continueReward.end())
	{
		return mContinueLoginInfo.continueReward.find(index)->second;
	}
	return NULL;
}

unsigned int ServerDateManager::getContinueLoginInfoMaxIndex()
{
	return this->mContinueLoginInfo.continueReward.size();
}

int UserEquipInfo::getPropertyInfoByIndex( int index )
{
	if(index>=propertyInfo.size())
		return 0;
	std::map<unsigned int, unsigned int>::iterator itr = propertyInfo.begin();
	while(index>0)
	{
		++itr;
		--index;
	}
	return itr->second;
}

void UserEquipInfo::setPropertyInfoByIndex( int index, int value )
{
	if(index>=propertyInfo.size())
		return;
	std::map<unsigned int, unsigned int>::iterator itr = propertyInfo.begin();
	while(index>0)
	{
		++itr;
		--index;
	}
	itr->second = value;
}

void UserEquipInfo::insertPropertyInfo( int index, int value )
{
	propertyInfo.insert(std::make_pair(index, value));
}

void UserEquipInfo::setStoneInfoByPosition(int index,int value)
{
	int stoneInfo = value;
	int position = (stoneInfo & 0xE0000000)>>29;
	int color = (stoneInfo & 0x1C000000) >> 26;
	int status = (stoneInfo & 0x03000000)>>24;
	int stoneid = stoneInfo & 0x00FFFFFF;
	UserStoneInfo* userStoneInfo=NULL;
	if(position>stoneMap.size())
	{
		userStoneInfo = new UserStoneInfo;
		stoneMap.insert(std::make_pair(position,userStoneInfo));
	}
	else
	{
		std::map<unsigned int, UserStoneInfo* >::iterator itr = stoneMap.begin();
		while(itr!=stoneMap.end())
		{
			if(itr->first==position)
			{
				userStoneInfo=itr->second;
			}
			else
			{
			}
			++itr;
		}
	}
	if(userStoneInfo)
	{
		userStoneInfo->color = color;
		userStoneInfo->status = status;
		userStoneInfo->stoneid = stoneid;
	}
}

unsigned int UserEquipInfo::getPorpertyInfoSize()
{
	return propertyInfo.size();
}

int UserEquipInfo::getPropertyInfoByKey( int key )
{
	return propertyInfo[key];
}

void UserEquipInfo::setPropertyInfoByKey( int key, int value )
{
	propertyInfo[key] = value;
}

bool MarketInfo::hasVipGift( int vipGift )
{
	return gotVipGifts.find(vipGift) != gotVipGifts.end();
}

void MarketInfo::clearVipGift()
{
	gotVipGifts.clear();
}

void MarketInfo::insertVipGift( int vipGift )
{
	gotVipGifts.insert(vipGift);
}

int UserBattleInfo::getFateInfosByIndex( int index )
{
	if(index>=fateInfos.size())
		return 0;
	std::map<unsigned int, unsigned int>::iterator itr = fateInfos.begin();
	while(index>0)
	{
		++itr;
		--index;
	}
	return itr->second;
}

void UserBattleInfo::setFateInfosByIndex( int index, int value )
{
	if(index>=fateInfos.size())
		return;
	std::map<unsigned int, unsigned int>::iterator itr = fateInfos.begin();
	while(index>0)
	{
		++itr;
		--index;
	}
	itr->second = value;
}

void UserBattleInfo::insertFateInfos( int index, int value )
{
	fateInfos.insert(std::make_pair(index, value));
}

unsigned int UserBattleInfo::getFateInfosSize()
{
	return fateInfos.size();
}

int UserBattleInfo::getFateInfosByKey( int key )
{
	if(hasFateInfo(key))
		return fateInfos[key];
	return 0;
}

void UserBattleInfo::setFateInfosByKey( int key, int value )
{
	fateInfos[key] = value;
}

void UserBattleInfo::clearFateInfos()
{
	fateInfos.clear();
}

bool UserBattleInfo::hasFateInfo( int key )
{
	return fateInfos.find(key) != fateInfos.end();
}

void BoardCastInfo::addParam( int param )
{
	params.push_back(param);
}

unsigned int BoardCastInfo::getParamsSize()
{
	return params.size();
}

int BoardCastInfo::getParamByIndex( int index )
{
	return params[index];
}

void BoardCastInfo::clearParams()
{
	params.clear();
}

unsigned int ContinueLoginInfo::getContinueRewardMapSize()
{
	return continueReward.size();
}

bool ContinueLoginInfo::hasContinueReward( unsigned int key )
{
	return continueReward.find(key)!= continueReward.end();
}

ContinueLoginInfo::ContinueReward* ContinueLoginInfo::getContinueRewardByKey( unsigned int key )
{
	return continueReward[key];
}

void ContinueLoginInfo::insertContinueReward( unsigned int key, ContinueReward* info )
{
	continueReward.insert(ContinueRewardMap::value_type(key,info));
}

ContinueLoginInfo::ContinueReward* ContinueLoginInfo::getContinueRewardByIndex( unsigned int index )
{
	if(index>=continueReward.size())
		return NULL;
	ContinueRewardMap::iterator itr = continueReward.begin();
	while(index>0)
	{
		++itr;
		--index;
	}
	return itr->second;
}


unsigned int SkillBookInfo::getSkillBookItemMapSize()
{
	return mSkillBookItemList.size();
}

bool SkillBookInfo::hasSkillBookItem( unsigned int key )
{
	return mSkillBookItemList.find(key)!= mSkillBookItemList.end();
}

SkillBookInfo::SkillBookItem* SkillBookInfo::getSkillBookItemByKey( unsigned int key )
{
	return mSkillBookItemList[key];
}

void SkillBookInfo::insertSkillBookItem( unsigned int key, SkillBookItem* info )
{
	mSkillBookItemList.insert(SkillBookItemMap::value_type(key,info));
}

SkillBookInfo::SkillBookItem* SkillBookInfo::getContinueRewardByIndex( unsigned int index )
{
	if(index>=mSkillBookItemList.size())
		return NULL;
	SkillBookItemMap::iterator itr = mSkillBookItemList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}
	return itr->second;
}

unsigned int UserSysMsgInfo::getRewardDataMapSize()
{
	return rewardDataMap.size();
}

bool UserArenaInfo::hasReward( unsigned int key )
{
	return rewardIDs.find(key)!=rewardIDs.end();
}

unsigned int AdventureFightEndInfo::getFightTeamListSize()
{
	return mFightTeamList.size();
}

AdventureFightEndInfo::FightTeamItem* AdventureFightEndInfo::getFightTeamByIndex( unsigned int index )
{
	if(index>=mFightTeamList.size())
		return NULL;
	FightTeamInfoMap::iterator itr = mFightTeamList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}
	return itr->second;
}

unsigned int AdventureFightEndInfo::getStepAdditionTeamListSize()
{
	return mStepAdditionItemList.size();
}

AdventureFightEndInfo::StepAdditionItem* AdventureFightEndInfo::getStepAdditionTeamByIndex( unsigned int index )
{
	if(index>=mStepAdditionItemList.size())
		return NULL;
	StepAdditionItemMap::iterator itr = mStepAdditionItemList.begin();
	while(index>0)
	{
		++itr;
		--index;
	}
	return itr->second;
}


SkillInfo* ViewTeamBattleInfoItem::getAndCreatSkillInfo( unsigned int id )
{
	SkillInfo* info = NULL;
	if(mSkillInfoList.find(id) != mSkillInfoList.end())
		info = mSkillInfoList.find(id)->second;
	else
	{
		info = new SkillInfo();
		info->id = id;
		mSkillInfoList.insert(SkillInfoList::value_type(id,info));
	}
	return info;
}

SkillInfo * ViewTeamBattleInfoItem::getSkillInfoByID( unsigned int id )
{
	if(mSkillInfoList.find(id) != mSkillInfoList.end())
		return mSkillInfoList.find(id)->second;

	return NULL;
}

UserEquipInfo* ViewTeamBattleInfoItem::getAndCreatUserEquipInfo( unsigned int id )
{
	UserEquipInfo* info = NULL;
	if(mUserEquipInfoList.find(id) != mUserEquipInfoList.end())
		info = mUserEquipInfoList.find(id)->second;
	else
	{
		info = new UserEquipInfo();
		info->id = id;
		mUserEquipInfoList.insert(UserEquipInfoList::value_type(id,info));
	}
	return info;
}

UserEquipInfo * ViewTeamBattleInfoItem::getUserEquipInfoByID( unsigned int id )
{
	if(mUserEquipInfoList.find(id) != mUserEquipInfoList.end())
		return mUserEquipInfoList.find(id)->second;

	return NULL;
}

UserBattleInfo* ViewTeamBattleInfoItem::getAndCreatUserBattleInfo( unsigned int id )
{
	UserBattleInfo* info = NULL;
	if(mUserBattleArray.find(id) != mUserBattleArray.end())
		info = mUserBattleArray.find(id)->second;
	else
	{
		info = new UserBattleInfo();
		info->id = id;
		mUserBattleArray.insert(UserBattleInfoList::value_type(id,info));
	}
	return info;
}

UserBattleInfo* ViewTeamBattleInfoItem::getUserBattleInfo( unsigned int position )
{
	for(UserBattleInfoList::iterator itr = mUserBattleArray.begin();
		itr != mUserBattleArray.end(); ++itr)
	{
		if(itr->second->position == position)
			return itr->second;
	}
	return NULL;
}

UserBattleInfoListIterator ViewTeamBattleInfoItem::getUserBattleIterator( void )
{
	return UserBattleInfoListIterator(mUserBattleArray.begin(), mUserBattleArray.end());
}

UserBattleInfo* ViewTeamBattleInfoItem::getUserBattleInfoByDisciple( unsigned int discipleID )
{
	for(UserBattleInfoList::iterator itr = mUserBattleArray.begin();
		itr != mUserBattleArray.end(); ++itr)
	{
		if(itr->second->discipleId == discipleID)
			return itr->second;
	}
	return NULL;
}

TitleInfo* ViewTeamBattleInfoItem::getAndCreatTitleInfo( unsigned int id )
{
	TitleInfo* info = NULL;
	if(mTitleInfoList.find(id) != mTitleInfoList.end())
		info = mTitleInfoList.find(id)->second;
	else
	{
		info = new TitleInfo();
		info->id = id;
		info->itemid = 0;
		mTitleInfoList.insert(TitleInfoList::value_type(id,info));
	}
	return info;
}

TitleInfo * ViewTeamBattleInfoItem::getTitleInfoByID( unsigned int id )
{
	if(mTitleInfoList.find(id) != mTitleInfoList.end())
		return mTitleInfoList.find(id)->second;

	return NULL;
}

TitleInfoListIterator ViewTeamBattleInfoItem::getTitleIterator( void )
{
	return TitleInfoListIterator(mTitleInfoList.begin(), mTitleInfoList.end());
}

UserDiscipleInfo* ViewTeamBattleInfoItem::getAndCreatDiscipleInfo( unsigned int id )
{
	UserDiscipleInfo* info = NULL;
	if(mUserDiscipleInfoList.find(id) != mUserDiscipleInfoList.end())
		info = mUserDiscipleInfoList.find(id)->second;
	else
	{
		info = new UserDiscipleInfo();
		info->id = id;
		mUserDiscipleInfoList.insert(UserDiscipleInfoList::value_type(id,info));
	}
	return info;
}

UserDiscipleInfo * ViewTeamBattleInfoItem::getUserDiscipleInfoByID( unsigned int id )
{
	if(mUserDiscipleInfoList.find(id) != mUserDiscipleInfoList.end())
		return mUserDiscipleInfoList.find(id)->second;

	return NULL;
}

void ViewTeamBattleInfoItem::clearTeamBattleInfoItem()
{
	SkillInfoList::iterator itSkill=mSkillInfoList.begin();
	while(itSkill!=mSkillInfoList.end())
	{
		if(itSkill->second)
		{
			delete itSkill->second;
		}
		itSkill++;
	}
	mSkillInfoList.clear();

	TitleInfoList::iterator itTitle=mTitleInfoList.begin();
	while(itTitle!=mTitleInfoList.end())
	{
		if(itTitle->second)
		{
			delete itTitle->second;
		}
		itTitle++;
	}
	mTitleInfoList.clear();

	UserEquipInfoList::iterator itEquip=mUserEquipInfoList.begin();
	while(itEquip!=mUserEquipInfoList.end())
	{
		if(itEquip->second)
		{
			delete itEquip->second;
		}
		itEquip++;
	}
	mUserEquipInfoList.clear();

	UserDiscipleInfoList::iterator itDisciple=mUserDiscipleInfoList.begin();
	while(itDisciple!=mUserDiscipleInfoList.end())
	{
		if(itDisciple->second)
		{
			delete itDisciple->second;
		}
		itDisciple++;
	}
	mUserDiscipleInfoList.clear();

	UserBattleInfoList::iterator itBattle=mUserBattleArray.begin();
	while(itBattle!=mUserBattleArray.end())
	{
		if(itBattle->second)
		{
			delete itBattle->second;
		}
		itBattle++;
	}
	mUserBattleArray.clear();
}