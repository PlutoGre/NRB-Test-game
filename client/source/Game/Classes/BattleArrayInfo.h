#pragma once
#include "Singleton.h"

class UserBattleInfo;
class BattleArrayInfo : public Singleton<BattleArrayInfo>
{
public:
	unsigned int getBattleInfoValue(unsigned int id,bool isViewTeam=false);
	unsigned int getBattleInfoValue(const UserBattleInfo* info,bool isViewTeam=false);
	unsigned int getBattleInfoTotalValue(bool isViewTeam=false);
	unsigned int getBattleEnableSize();
	unsigned int getNextUnlockLevel();
};