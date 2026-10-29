
#include "stdafx.h"

#include "BattleArrayInfo.h"
#include "ServerDateManager.h"
#include "Equip.h"
#include "Skill.h"
#include "Disciple.h"

unsigned int BattleArrayInfo::getBattleInfoValue( unsigned int id,bool isViewTeam )
{
	if(isViewTeam)
	{
		const UserBattleInfo* info = ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserBattleInfo(id);
		return getBattleInfoValue(info,isViewTeam);
	}
	else
	{
		const UserBattleInfo* info = ServerDateManager::Get()->getUserBattleInfo(id);
		return getBattleInfoValue(info,isViewTeam);
	}
}

unsigned int BattleArrayInfo::getBattleInfoValue( const UserBattleInfo* info,bool isViewTeam )
{
	unsigned int totalValue = 0;
	if(info->discipleId)
	{
		Disciple disciple(info->discipleId,false,isViewTeam);
		Skill skill(disciple.skill1(),false,isViewTeam);
		totalValue += disciple.getDiscipleValue();
		totalValue += skill.getSkillValue();
	}
	if (info->skill1)
	{
		Skill skill(info->skill1,false,isViewTeam);
		totalValue += skill.getSkillValue();
	}
	if (info->skill2)
	{
		Skill skill(info->skill2,false,isViewTeam);
		totalValue += skill.getSkillValue();
	}
	if (info->equip1)
	{
		Equip equip(info->equip1,false,isViewTeam);
		totalValue += equip.getEquipValue();
	}
	if (info->equip2)
	{
		Equip equip(info->equip2,false,isViewTeam);
		totalValue += equip.getEquipValue();
	}
	if (info->equip3)
	{
		Equip equip(info->equip3,false,isViewTeam);
		totalValue += equip.getEquipValue();
	}
	return totalValue;
}

unsigned int BattleArrayInfo::getBattleInfoTotalValue(bool isViewTeam)
{
	unsigned int totalValue = 0;
	if(isViewTeam)
	{
		UserBattleInfoListIterator itr = ServerDateManager::Get()->getViewTeamBattleInfoItem().getUserBattleIterator();
		while (itr.hasMoreElements())
		{
			totalValue += getBattleInfoValue(itr.getNext(),isViewTeam);
		}
	}
	else
	{
		UserBattleInfoListIterator itr = ServerDateManager::Get()->getUserBattleIterator();
		while (itr.hasMoreElements())
		{
			totalValue += getBattleInfoValue(itr.getNext(),isViewTeam);
		}
	}
	return totalValue;
}
unsigned int BattleArrayInfo::getBattleEnableSize()
{
	unsigned int level = ServerDateManager::Get()->getUserBasicInfo().level;
	unsigned int count = 2;
	if(level>999999)
	{
		return 2;
	}
	if (level >= 26) {
		count = 8;
	}
	else if(level >= 21){
		count = 7;
	}
	else if (level >= 16) {
		count = 6;
	}
	else if (level >= 11) {
		count = 5;
	}
	else if (level >= 5) {
		count = 4;
	}
	else if (level >= 2) {
		count = 3;
	}

	return count;

}
unsigned int BattleArrayInfo::getNextUnlockLevel()
{
	unsigned int level = ServerDateManager::Get()->getUserBasicInfo().level;
	if(level>999999)
	{
		return 2;
	}
	if (level >= 26) {
		return 0;
	}
	else if(level >= 21){
		return 26;
	}
	else if (level >= 16) {
		return 21;
	}
	else if (level >= 11) {
		return 16;
	}
	else if (level >= 5) {
		return 11;
	}
	else if (level >= 2) {
		return 5;
	}

	return 2;

}

