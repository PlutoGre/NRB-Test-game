#pragma once
#include "Singleton.h"
#include "GamePackets.h"

class GetPropManager:public Singleton<GetPropManager>
{
public:
	GetPropManager(void);
	~GetPropManager(void);
	enum TYPE
	{
		TOOL,
		TOOL_USE,
		EQUIPMENT,
		SKILL,
		SOUL,
		SKILLBOOK,
		SKILLPIECE,
		DISCIPLE,
	};
	static GetPropManager* getInstance(){return GetPropManager::Get();}
	void popUpPage(TYPE _type, int _id, int _count,int skillId=-1);
	void popUpResultPage(OPUserBattleRet* mBattleMsg);

	void gotTool(const OPUserToolInfoRet_ToolInfo& info);
	void gotEquipment(const OPUserEquipInfoRet_EquipInfo& info);
	void gotSkill(const OPUserSkillInfoRet_SkillInfo& info );
	void gotSoul(const OPUserSoulInfoRet_SoulInfo& info);
	void gotDisciple(const OPUserDiscipleInfoRet_DiscipleInfo& info);
	void gotSkillBook(const OPGetSkillBookInfoRet_SkillBookPartItem& info);
};

