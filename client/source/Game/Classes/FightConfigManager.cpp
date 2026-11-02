
#include "stdafx.h"

#include "FightConfigManager.h"
#include "json/json.h"
#include "cocos2d.h"
#include "TableReader.h"
#include "StringConverter.h"
#include "GamePlatform.h"
USING_NS_CC;

#define GET_TIMING_DATA(var,name,asFunc,defaultValue) \
	if(!timing[name].empty()){ \
	var = timing[name]. asFunc;} \
	else{var=defaultValue;}

#define GET_FLASH_DATA(name,defaultValue,var) \
	if(!colorflash[name].empty()){ \
	var = colorflash[name].asDouble();} \
	else{var=defaultValue;}

void FightConfigManager::init(const std::string& filename)
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

		if(!data["entranceDurationTime"].empty())
		{
			mEntranceDurationTime=(float)data["entranceDurationTime"].asDouble();
		}
		else
		{
			mEntranceDurationTime=1.0f;
		}
		if(!data["queueUpDurationTime"].empty())
		{
			mQueueUpDurationTime=(float)data["queueUpDurationTime"].asDouble();
		}
		else
		{
			mQueueUpDurationTime=1.0f;
		}
		if(!data["intervalDelayTime"].empty())
		{
			mIntervalDelayTime=(float)data["intervalDelayTime"].asDouble();
		}
		else
		{
			mIntervalDelayTime=1.0f;
		}
		if(!data["entranceDelayTime"].empty())
		{
			mEntranceDelayTime=(float)data["entranceDelayTime"].asDouble();
		}
		else
		{
			mEntranceDelayTime=1.0f;
		}
		if(!data["queueDelayTime"].empty())
		{
			mQueueDelayTime=(float)data["queueDelayTime"].asDouble();
		}
		else
		{
			mQueueDelayTime=1.0f;
		}
		if(!data["resultAllTime"].empty())
		{
			mResultAllTime=(float)data["resultAllTime"].asDouble();
		}
		else
		{
			mResultAllTime=3.0f;
		}
		if(!data["resultLVUpBefore"].empty())
		{
			mResultLVUpBefore=(float)data["resultLVUpBefore"].asDouble();
		}
		else
		{
			mResultLVUpBefore=1.5f;
		}
		if(!data["resultExpFast"].empty())
		{
			mResultExpFast=(float)data["resultExpFast"].asDouble();
		}
		else
		{
			mResultExpFast=0.75f;
		}
		
		if(!data["resultHeadDeplay"].empty())
		{
			mResultHeadDeplay=(float)data["resultHeadDeplay"].asDouble();
		}
		else
		{
			mResultHeadDeplay=2.0f;
		}

		if(!data["thicoEntranceDurationTime"].empty())
		{
			mThicoEntranceDurationTime=(float)data["thicoEntranceDurationTime"].asDouble();
		}
		else
		{
			mThicoEntranceDurationTime=1.0f;
		}

		if(!data["thicoOffSiteWidth"].empty())
		{
			mThicoOffSiteWidth=(float)data["thicoOffSiteWidth"].asDouble();
		}
		else
		{
			mThicoOffSiteWidth=1.0f;
		}

		Json::Value timing = data["timing"];
		if(!timing.empty())
		{
			GET_TIMING_DATA(mRoundStartLoadTime,"roundStartLoadTime",asDouble(),1.0f);
			
			GET_TIMING_DATA(mSkillTime[SMALL][DURATION],"skillSmallDuration",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[SMALL][NEXT_DELAY],"skillSmallNextDelay",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[SMALL][DEFENSE_DELAY],"skillSmallDefenseDelay",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[SMALL][DAMAGE_TIME],"skillSmallDamageTime",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[BIG_SINGLE][DURATION],"skillBigSingleDuration",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[BIG_SINGLE][NEXT_DELAY],"skillBigSingleNextDelay",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[BIG_SINGLE][DEFENSE_DELAY],"skillBigSingleDefenseDelay",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[BIG_MULTIPLE][DURATION],"skillBigMultipleDuration",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[BIG_MULTIPLE][NEXT_DELAY],"skillBigMultipleNextDelay",asDouble(),1.0f);

			GET_TIMING_DATA(mSkillTime[BUFF][DURATION],"buffDuration",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[BUFF][NEXT_DELAY],"buffNextDelay",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[ATTACK][DURATION],"attackDuration",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[ATTACK][NEXT_DELAY],"attackNextDelay",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[DEFENSE][DURATION],"defenseDuration",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[DEFENSE][NEXT_DELAY],"defenseNextDelay",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[ROUND_START][DURATION],"roundStartDuration",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[ROUND_START][NEXT_DELAY],"roundStartNextDelay",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[ROUND_OVER][DURATION],"roundOverDuration",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[ROUND_OVER][NEXT_DELAY],"roundOverNextDelay",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[SHAKE][DURATION],"shakeDuration",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[SHAKE][NEXT_DELAY],"shakeNextDelay",asDouble(),1.0f);
			
			GET_TIMING_DATA(mDefaultBogyPic,"defaultBodyPic",asString(),"person/poster_char_xiangjishi.png");

			GET_TIMING_DATA(mSkillTime[BIG_SINGLE][ENTER_DURATION],"skillBigSingleEnterDuration",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[BIG_SINGLE][EXIT_START],"skillBigSingleExitStartTime",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[BIG_SINGLE][EXIT_DURATION],"skillBigSingleExitDuration",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[BIG_SINGLE][DAMAGE_TIME],"skillBigSingleDamageTime",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[BIG_MULTIPLE][ENTER_DURATION],"skillBigMultipleEnterDuration",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[BIG_MULTIPLE][EXIT_START],"skillBigMultipleExitStartTime",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[BIG_MULTIPLE][EXIT_DURATION],"skillBigMultipleExitDuration",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillTime[BIG_MULTIPLE][DAMAGE_TIME],"skillBigMultipleDamageTime",asDouble(),1.0f);

			GET_TIMING_DATA(mSkilBigSingleSelfPosition.x,"skillBigSingleSelfPositionX",asDouble(),1.0f);
			GET_TIMING_DATA(mSkilBigSingleSelfPosition.y,"skillBigSingleSelfPositionY",asDouble(),1.0f);
			GET_TIMING_DATA(mSkilBigSingleEnemyfPosition.x,"skillBigSingleEnemyPositionX",asDouble(),1.0f);
			GET_TIMING_DATA(mSkilBigSingleEnemyfPosition.y,"skillBigSingleEnemyPositionY",asDouble(),1.0f);
			GET_TIMING_DATA(mSkilBigMulitpleAttackerPosition.x,"skillBigMultipleAttackerPositionX",asDouble(),1.0f);
			GET_TIMING_DATA(mSkilBigMulitpleAttackerPosition.y,"skillBigMultipleAttackerPositionY",asDouble(),1.0f);
			GET_TIMING_DATA(mSkillBigMulitpleDefenderDistance,"skillBigMultipleDefenderDistance",asDouble(),1.0f);

		}
		Json::Value colorflash = data["colorFlash"];
		if(!colorflash.empty())
		{
			GET_FLASH_DATA("buffAttackColorR",0,mFlashColor[CT_BUFF_ATTACK][CA_COLOR_R]);
			GET_FLASH_DATA("buffAttackColorG",0.79,mFlashColor[CT_BUFF_ATTACK][CA_COLOR_G]);
			GET_FLASH_DATA("buffAttackColorB",0.39,mFlashColor[CT_BUFF_ATTACK][CA_COLOR_B]);
			GET_FLASH_DATA("buffAttackDelay",0,mFlashColor[CT_BUFF_ATTACK][CA_GLOW_DELAY]);
			GET_FLASH_DATA("buffAttackDuration",0.3,mFlashColor[CT_BUFF_ATTACK][CA_GLOW_DURATION]);
			GET_FLASH_DATA("buffAttackRestoreDuration",0.3,mFlashColor[CT_BUFF_ATTACK][CA_GLOW_RESTORE]);

			GET_FLASH_DATA("buffDefenseColorR",0,mFlashColor[CT_BUFF_DEFENSE][CA_COLOR_R]);
			GET_FLASH_DATA("buffDefenseColorG",0.18,mFlashColor[CT_BUFF_DEFENSE][CA_COLOR_G]);
			GET_FLASH_DATA("buffDefenseColorB",0.60,mFlashColor[CT_BUFF_DEFENSE][CA_COLOR_B]);
			GET_FLASH_DATA("buffDefenseDelay",0,mFlashColor[CT_BUFF_DEFENSE][CA_GLOW_DELAY]);
			GET_FLASH_DATA("buffDefenseDuration",0.3,mFlashColor[CT_BUFF_DEFENSE][CA_GLOW_DURATION]);
			GET_FLASH_DATA("buffDefenseRestoreDuration",0.3,mFlashColor[CT_BUFF_DEFENSE][CA_GLOW_RESTORE]);

			GET_FLASH_DATA("buffWillColorR",0.36,mFlashColor[CT_BUFF_WILL][CA_COLOR_R]);
			GET_FLASH_DATA("buffWillColorG",0.36,mFlashColor[CT_BUFF_WILL][CA_COLOR_G]);
			GET_FLASH_DATA("buffWillColorB",0,mFlashColor[CT_BUFF_WILL][CA_COLOR_B]);
			GET_FLASH_DATA("buffWillDelay",0,mFlashColor[CT_BUFF_WILL][CA_GLOW_DELAY]);
			GET_FLASH_DATA("buffWillDuration",0.3,mFlashColor[CT_BUFF_WILL][CA_GLOW_DURATION]);
			GET_FLASH_DATA("buffWillRestoreDuration",0.3,mFlashColor[CT_BUFF_WILL][CA_GLOW_RESTORE]);

			GET_FLASH_DATA("normalDefenseFlashDelay",0.6,mFlashColor[CT_NORMAL_ATTACK][CA_FLASH_DELAY]);
			GET_FLASH_DATA("normalDefenseFlashDuration",0.1,mFlashColor[CT_NORMAL_ATTACK][CA_FLASH_DURATION]);

			GET_FLASH_DATA("smallSkillGlow1ColorR",0.68,mFlashColor[CT_SMALL_SKILL_PER1][CA_COLOR_R]);
			GET_FLASH_DATA("smallSkillGlow1ColorG",0.71,mFlashColor[CT_SMALL_SKILL_PER1][CA_COLOR_G]);
			GET_FLASH_DATA("smallSkillGlow1ColorB",0.33,mFlashColor[CT_SMALL_SKILL_PER1][CA_COLOR_B]);
			GET_FLASH_DATA("smallSkillGlow1Duration",0.3,mFlashColor[CT_SMALL_SKILL_PER1][CA_GLOW_DURATION]);

			GET_FLASH_DATA("smallSkillGlow2ColorR",0.61,mFlashColor[CT_SMALL_SKILL_PER2][CA_COLOR_R]);
			GET_FLASH_DATA("smallSkillGlow2ColorG",0.26,mFlashColor[CT_SMALL_SKILL_PER2][CA_COLOR_G]);
			GET_FLASH_DATA("smallSkillGlow2ColorB",0,mFlashColor[CT_SMALL_SKILL_PER2][CA_COLOR_B]);
			GET_FLASH_DATA("smallSkillGlow2Duration",0.3,mFlashColor[CT_SMALL_SKILL_PER2][CA_GLOW_DURATION]);

			GET_FLASH_DATA("smallSkillDamageFlashDelay",1.15,mFlashColor[CT_SMALL_SKILL_PER1][CA_FLASH_DELAY]);
			GET_FLASH_DATA("smallSkillDamageFlashDuration",0.2,mFlashColor[CT_SMALL_SKILL_PER1][CA_FLASH_DURATION]);
			GET_FLASH_DATA("smallSkillDamageFlashDelay",1.15,mFlashColor[CT_SMALL_SKILL_PER2][CA_FLASH_DELAY]);
			GET_FLASH_DATA("smallSkillDamageFlashDuration",0.2,mFlashColor[CT_SMALL_SKILL_PER2][CA_FLASH_DURATION]);

			GET_FLASH_DATA("bigSkillSingleDamageFlshDelay",2.83,mFlashColor[CT_BIG_SINGLE][CA_FLASH_DELAY]);
			GET_FLASH_DATA("bigSkillSingleDamageFlashDuration",1.0,mFlashColor[CT_BIG_SINGLE][CA_FLASH_DURATION]);

			GET_FLASH_DATA("bigSkillSingleGlowColorR",0.82,mFlashColor[CT_BIG_SINGLE][CA_COLOR_R]);
			GET_FLASH_DATA("bigSkillSingleGlowColorG",0.51,mFlashColor[CT_BIG_SINGLE][CA_COLOR_G]);
			GET_FLASH_DATA("bigSkillSingleGlowColorB",0.0,mFlashColor[CT_BIG_SINGLE][CA_COLOR_B]);
			GET_FLASH_DATA("bigSkillSingleDelay",1.9,mFlashColor[CT_BIG_SINGLE][CA_GLOW_DELAY]);
			GET_FLASH_DATA("bigSkillSingleDuration",1.0,mFlashColor[CT_BIG_SINGLE][CA_GLOW_DURATION]);
			GET_FLASH_DATA("bigSkillSingleRestore",0.1,mFlashColor[CT_BIG_SINGLE][CA_GLOW_RESTORE]);


			GET_FLASH_DATA("bigSkillMultipleDamageFlshDelay",2.83,mFlashColor[CT_BIG_MULTIPLE][CA_FLASH_DELAY]);
			GET_FLASH_DATA("bigSkillMultipleDamageFlashDuration",0.5,mFlashColor[CT_BIG_MULTIPLE][CA_FLASH_DURATION]);

			GET_FLASH_DATA("bigSkillMultipleGlowColorR",0.82,mFlashColor[CT_BIG_MULTIPLE][CA_COLOR_R]);
			GET_FLASH_DATA("bigSkillMultipleGlowColorG",0.51,mFlashColor[CT_BIG_MULTIPLE][CA_COLOR_G]);
			GET_FLASH_DATA("bigSkillMultipleGlowColorB",0.0,mFlashColor[CT_BIG_MULTIPLE][CA_COLOR_B]);
			GET_FLASH_DATA("bigSkillMultipleDelay",1.9,mFlashColor[CT_BIG_MULTIPLE][CA_GLOW_DELAY]);
			GET_FLASH_DATA("bigSkillMultipleDuration",1.0,mFlashColor[CT_BIG_MULTIPLE][CA_GLOW_DURATION]);
			GET_FLASH_DATA("bigSkillMultipleRestore",0.1,mFlashColor[CT_BIG_MULTIPLE][CA_GLOW_RESTORE]);

			GET_FLASH_DATA("normalBleedingTime",0.5,mFlashColor[CT_NORMAL_ATTACK][CA_BLEEDING]);
			GET_FLASH_DATA("smallSkillBleedingTime",0.5,mFlashColor[CT_SMALL_SKILL_PER1][CA_BLEEDING]);
			GET_FLASH_DATA("smallSkillBleedingTime",0.5,mFlashColor[CT_SMALL_SKILL_PER2][CA_BLEEDING]);
			GET_FLASH_DATA("bigSkillSingleBleedingTime",0.5,mFlashColor[CT_BIG_SINGLE][CA_BLEEDING]);
			GET_FLASH_DATA("bigSkillMultipleBleedingTime",0.5,mFlashColor[CT_BIG_MULTIPLE][CA_BLEEDING]);

			GET_FLASH_DATA("bigSkillMultiplePrepare",0.5,mFlashColor[CT_BIG_MULTIPLE][CA_PAREPARE]);
			
		}
		if(!data["position"].empty())
		{
			Json::Value postitionlist = data["position"];
			if(!postitionlist.empty() && postitionlist.isArray())
			{
				for(int i = 0;i < postitionlist.size();++i)
				{
					FightPoint * fightPoint  = new FightPoint;
					Json::Value unit = postitionlist[i];
					fightPoint->battleNum = unit["id"].asUInt();
					fightPoint->newline = unit["newline"].asUInt();
					fightPoint->startY = (float)unit["startY"].asDouble();
					fightPoint->remainY = (float)unit["remainY"].asDouble();
					fightPoint->stopY = (float)unit["stopY"].asDouble();
					fightPoint->reviseStopX =(float) unit["reviseStopX"].asDouble();
					fightPoint->intervalY =(float) unit["intervalY"].asDouble();
					if(!unit["nextline"].empty())
					{
						Json::Value unitNextline=unit["nextline"];
						fightPoint->adjustX=(float)unitNextline["adjustX"].asDouble();
						fightPoint->adjustY=(float)unitNextline["adjustY"].asDouble();
						fightPoint->adjustStopY=(float)unitNextline["adjustStopY"].asDouble();
					}
					if(!unit["reviseStopY"].empty())
					{
						Json::Value unitReviseStopY=unit["reviseStopY"];
						for(int j=1;j<=fightPoint->newline;++j)
						{
							char key[128];
							sprintf(key,"%d",j);
							if(!unitReviseStopY[key].empty())
							{
								ReviseCoordinates* reviseCoordinates= new ReviseCoordinates;
								reviseCoordinates->position=j;
								reviseCoordinates->coordinates=(float)unitReviseStopY[key].asDouble();
								fightPoint->reviseStopY.insert(std::make_pair(j,reviseCoordinates));
							}
						}
					}
					mFightPointList.insert(std::make_pair(fightPoint->battleNum,fightPoint));
				}
			}
		}
	}
	CC_SAFE_DELETE_ARRAY(pBuffer);
}


const FightPoint* FightConfigManager::getFightPointById(unsigned int id)
{
	FightPointList::iterator it = mFightPointList.find(id);
	if(it == mFightPointList.end())
		return 0;
	return it->second;
}

const float FightConfigManager::getReviseCoordinatesByBattle(unsigned int id,int postion)
{
	float ret=0.0f;
	const FightPoint* _fightPoint=getFightPointById(id);
	if(_fightPoint)
	{
		ReviseList::const_iterator it=_fightPoint->reviseStopY.find(postion);
		if(it == _fightPoint->reviseStopY.end())
		{
			ret=0.0f;
		}
		else
		{
			ret=it->second->coordinates;
		}
	}
	return ret;
}

