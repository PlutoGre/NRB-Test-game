#pragma once

#include "Singleton.h"
#include <string>
#include <map>

struct SKILL
{
	int id;
	int stars;
	std::string name;
};

class SkillConsts : public Singleton<SkillConsts>
{
private:
	typedef std::map<unsigned int,SKILL* > SKILL_LIST;
	SKILL_LIST mSkillList;

public:

	void init(const std::string& filename);
	void cleanUp();

	SKILL* getSkill(int id);
};

