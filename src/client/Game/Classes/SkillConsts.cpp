
#include "stdafx.h"

#include "SkillConsts.h"
#include "cocos2d.h"
#include "json/json.h"
#include "Language.h"
#include "GamePlatform.h"

void SkillConsts::init( const std::string& filename )
{
	Json::Reader jreader;
	Json::Value data;
	unsigned long filesize;
	char* pBuffer = (char*)getFileData(
		cocos2d::CCFileUtils::sharedFileUtils()->fullPathForFilename(filename.c_str()).c_str(),
		"rt",&filesize);

	if(!pBuffer)
	{
		char msg[256];
		sprintf(msg,"Failed open file: %s !!",filename.c_str());
		cocos2d::CCMessageBox(msg,"ERROR!");
	}
	else
	{
		jreader.parse(pBuffer,data,false);
		CC_SAFE_DELETE_ARRAY(pBuffer);

		if(data["version"].asInt()==1)
		{
			Json::Value skillList = data["skills"];
			if(!skillList.empty() && skillList.isArray())
			{
				for(int i = 0;i < skillList.size();++i)
				{
					SKILL * skill  = new SKILL;
					Json::Value unit = skillList[i];
					skill->id = unit["id"].asUInt();
					skill->name = Language::Get()->getString(unit["name"].asString());
					skill->stars = unit["stars"].asInt();
					
					mSkillList.insert(std::make_pair(skill->id,skill));
				}
			}

		}
	}
	CC_SAFE_DELETE_ARRAY(pBuffer);
}

void SkillConsts::cleanUp()
{
	SKILL_LIST::iterator itCha = mSkillList.begin();
	for(;itCha!=mSkillList.end();++itCha)
	{
		delete itCha->second;
	}
	mSkillList.clear();
}

SKILL* SkillConsts::getSkill( int id )
{
	SKILL_LIST::iterator it = mSkillList.find(id);
	if(it == mSkillList.end())
		return 0;

	return it->second;
}
