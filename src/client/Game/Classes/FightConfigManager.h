#pragma once
#include "Singleton.h"
#include <map>
#include <set>
#include <string>

#include "cocos2d.h"

struct ReviseCoordinates
{
	unsigned int position;
	float coordinates;
};
typedef std::map<unsigned int,ReviseCoordinates* > ReviseList;
struct FightPoint
{
	unsigned int battleNum;
	unsigned int newline;
	float startY;
	float remainY;//percentage
	float stopY;//percentage
	float reviseStopX;
	ReviseList reviseStopY;
	float intervalY;
	float adjustX;
	float adjustY;
	float adjustStopY;//percentage
};
class FightConfigManager: public Singleton<FightConfigManager>
{
public:
	void init(const std::string& filename);
	const FightPoint* getFightPointById(unsigned int id);
	const float getReviseCoordinatesByBattle(unsigned int id,int postion);
	const float getEntranceDurationTime(){ return mEntranceDurationTime;};
	const float getQueueUpDurationTime(){ return mQueueUpDurationTime;};
	const float getIntervalDelayTime(){ return mIntervalDelayTime;};
	const float getEntranceDelayTime(){ return mEntranceDelayTime;};
	const float getQueueDelayTime(){ return mQueueDelayTime;};
	const float getResultAllTime(){ return mResultAllTime;};
	const float getResultLVUpBefore(){ return mResultLVUpBefore;};
	const float getResultLVUpAfter(){ return mResultAllTime-mResultLVUpBefore;};
	const float getResultExpFast(){ return mResultExpFast;};
	const float getResultHeadDeplay(){ return mResultHeadDeplay;};
	const float getThicoEntranceDurationTime(){ return mThicoEntranceDurationTime;};
	const float getThicoOffSiteWidth(){ return mThicoOffSiteWidth;};
	float getRoundStartLoadTime(){ return mRoundStartLoadTime;};

	enum ACTION
	{
		BIG_SINGLE,
		BIG_MULTIPLE,
		SMALL,
		BUFF,
		ATTACK,
		DEFENSE,
		ROUND_START,
		ROUND_OVER,
		SHAKE,
		SKILL_MAX,
	};
	enum TYPE
	{
		DURATION,
		NEXT_DELAY,
		DEFENSE_DELAY,//to play shake animation

		ENTER_DURATION,
		EXIT_START,
		EXIT_DURATION,
		DAMAGE_TIME,//damage time when play damage animation
		TYPE_MAX,
	};

	enum COLORPARA_TYPE
	{
		CT_BUFF_ATTACK,
		CT_BUFF_DEFENSE,
		CT_BUFF_WILL,
		CT_NORMAL_ATTACK,
		CT_SMALL_SKILL_PER1,
		CT_SMALL_SKILL_PER2,
		CT_BIG_SINGLE,
		CT_BIG_MULTIPLE,

		CT_MAX,
	};

	enum COLORPARA_ATTR
	{
		CA_COLOR_R,
		CA_COLOR_G,
		CA_COLOR_B,
		CA_GLOW_DELAY,
		CA_GLOW_DURATION,
		CA_GLOW_RESTORE,
		CA_FLASH_DELAY,
		CA_FLASH_DURATION,
		CA_BLEEDING,
		CA_PAREPARE,
		CA_MAX,
	};
	float getSkillTime(ACTION skill,TYPE type){return mSkillTime[skill][type];}
	float getFlashTime(COLORPARA_TYPE type, COLORPARA_ATTR attri){return mFlashColor[type][attri];}
	cocos2d::CCPoint getSkillBigSingleSelfPosition(){return mSkilBigSingleSelfPosition;}
	cocos2d::CCPoint getSkillBigSingleEnemyfPosition(){return mSkilBigSingleEnemyfPosition;}
	cocos2d::CCPoint getSkillBigMulitpleAttackerPosition(){return mSkilBigMulitpleAttackerPosition;}
	float getSkillBigMulitpleDefenderDistance(){return mSkillBigMulitpleDefenderDistance;}
	const std::string& getDefaultBogyPic(){return mDefaultBogyPic;}
private:
	typedef std::map<unsigned int,FightPoint* > FightPointList;
	FightPointList mFightPointList;
	float mRoundStartLoadTime;
	float mEntranceDurationTime;
	float mQueueUpDurationTime;
	float mIntervalDelayTime;
	float mEntranceDelayTime;
	float mQueueDelayTime;
	float mResultAllTime;
	float mResultLVUpBefore;
	float mResultExpFast;
	float mResultHeadDeplay;
	float mThicoEntranceDurationTime;
	float mThicoOffSiteWidth;
	float mSkillTime[SKILL_MAX][TYPE_MAX];
	float mFlashColor[CT_MAX][CA_MAX];

	cocos2d::CCPoint mSkilBigSingleSelfPosition;
	cocos2d::CCPoint mSkilBigSingleEnemyfPosition;
	cocos2d::CCPoint mSkilBigMulitpleAttackerPosition;
	float mSkillBigMulitpleDefenderDistance;
	std::string mDefaultBogyPic;
};

#define GET_FLASH_TIME(type,attri) \
	FightConfigManager::Get()->getFlashTime(FightConfigManager:: type, FightConfigManager::attri);

