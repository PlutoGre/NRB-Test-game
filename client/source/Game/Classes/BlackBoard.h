#pragma once
#include "Singleton.h"
#include <vector>
#include <string>
#include <map>
#include "IteratorWrapper.h"

class BlackBoard : public Singleton<BlackBoard>
{
public:
	BlackBoard()
		: DisciplePageTab(1)
		, TransferExp_Receiver(0)
		, TransferExp_Provider(0)
		, TrainingBolusID(0)
		, TransferBeforeBlood(0)
		, TransferBeforeAttack(0)
		, TransferBeforeDefense(0)
		, TransferBeforeRP(0)
		, TransferBeforeLV(0)
		, TransferBeforeExp(0)
		, TransferBeforePotential(0)
		, TrainDisciple(0)
		, TrainType(0)
		, ShowDisciple(0)
		, ShowSoul(0)
		, NeedChangeDisciple(false)
		, NeedChooseDisciple(false)
	    , UpgradeSoul(0)
	    , UpgradeDiscipleBeforePotential(0)
		, isUpgradeDisciple(false)
		, jumpDisciplePageName("")
		, ShowSkill(0)
		, ShowSkillItem(0)
		, NeedChangeSkill(false)
		, NeedChooseSkill(false)
		, UpgradeSkill(0)
		, SkillUpgradeBeforeLV(0)
		, SkillUpgradeBeforeQuantity("")
		, jumpSkillPageName("")
		, jumpEquipPageName("")
		,SkillBookId(0)
		,SkillBookSkillId(0)
		,SnatchSkillBookRet(false)
		,SnatchSkillBookPlayerId(0)
		,WorldBossRankType(0)
		,WorldBossId(1)
		,CurrTBBoldness(0)
		,NeedRefreshTeamBuffPage(false)
		,UseToolId(0)
		,UseToolCount(0)
		,CareerAttackChapterId(0)
		,CareerAttackCustomsId(0)
		,TeamShowIndex(0)
		,MarketBuyItemId(0)
		,ConsumeTrainningBolus(0)
		,PreTitleId(0)
		,UpdateTitleType(0)
		,ToAdventruePageType(0)
		,loseArenaData(0)
		, JustViewSkill(false)
		,IsTimeoutResponse(false)
		,fortuneComeFlag(0)
		,fortuneComeSum(0)
		,worldbossRankFlag(0)
		,worldbossFinalFight(0)
		,showFightGuide(true)
		,continueOpenBoxNum(0)
		,quickEnhanceTimes(0)
		,currentStone(0)
		,currentStoneMId(0)
		,stonePosition(0)
		,isSamSungi9100Audio(false)
		,currStonePage(0)
	{
		resetSkillConsume();
		mVaribleMap.clear();
	}

	inline void setSkillBookInfo(unsigned int bookId,unsigned int id) {SkillBookId = bookId;SkillBookSkillId=id;}
	inline unsigned int getSkillBookId(void) const {return SkillBookId;}
	inline unsigned int getSkillBookSkillId(void) const {return SkillBookSkillId;}
	inline void resetSkillBookInfo(void) {SkillBookId = 0;SkillBookSkillId = 0;}

	inline void setSnatchSkillBookRet(bool isSnatch){ SnatchSkillBookRet=isSnatch;}
	inline bool getSnatchSkillBookRet(){ return SnatchSkillBookRet;};

	inline void setSnatchSkillBookPlayerId(unsigned int playerId){ SnatchSkillBookPlayerId=playerId;}
	inline unsigned int getSnatchSkillBookPlayerId(){ return SnatchSkillBookPlayerId;};

	inline void setWorldBossRankType(unsigned int type){ WorldBossRankType=type;}
	inline unsigned int getWorldBossRankType(){ return WorldBossRankType;};

	inline void setWorldBossId(unsigned int id){ WorldBossId=id;}
	inline unsigned int getWorldBossId(){ return WorldBossId;}

	inline void setUseToolId(unsigned int id){ UseToolId=id;}
	inline unsigned int getUseToolId(){ return UseToolId;}
	inline void setUseToolCount(unsigned int count){ UseToolCount=count;}
	inline unsigned int getUseToolCount(){ return UseToolCount;}

	inline void setCareerAttackChapterId(unsigned int chapterId){ CareerAttackChapterId=chapterId;}
	inline unsigned int getCareerAttackChapterId(){ return CareerAttackChapterId;}
	inline void setCareerAttackCustomsId(unsigned int cusId){ CareerAttackCustomsId=cusId;}
	inline unsigned int getCareerAttackCustomsId(){ return CareerAttackCustomsId;}

	inline void addSkillConsume(unsigned int id) 
	{
		SkillConsume[SkillConsumeCount] = id;
		SkillConsumeCount++;
	}
	inline void resetSkillConsume(void) 
	{
		SkillConsume[0] = 0;
		SkillConsume[1] = 0;
		SkillConsume[2] = 0;
		SkillConsume[3] = 0;
		SkillConsume[4] = 0;
		SkillConsumeCount = 0;
	}
	inline std::vector<unsigned int> getSkillConsume(void)
	{
		std::vector<unsigned int> skills;
		for(unsigned int i = 0; i<SkillConsumeCount; ++i)
		{
			skills.push_back(SkillConsume[i]);
		}
		return skills;
	}
	inline unsigned int getSkillConsume(unsigned int index)
	{
		if(index < SkillConsumeCount)
			return SkillConsume[index];
		return 0;
	}
	inline bool hasSkillConsume(void) { return SkillConsumeCount >0;}
	inline unsigned int getSkillConsumeNum(void) {return SkillConsumeCount;}
	inline bool isConsumedSkill(unsigned int id) 
	{
		for(unsigned int i = 0; i<SkillConsumeCount; ++i)
		{
			if(SkillConsume[i] == id)
				return true;
		}
		return false;
	}

	inline void addUpTBDisciple(unsigned int id,int count,unsigned int boldnessCount)
	{
		if(upTBDiscipleList.find(id)!=upTBDiscipleList.end())
		{
			upTBDiscipleList.find(id)->second+=count;
			if(upTBDiscipleList.find(id)->second<=0)
			{
				upTBDiscipleList.erase(upTBDiscipleList.find(id));
			}
		}
		else
		{
			if(count>0)
			{
				upTBDiscipleList.insert(UpTBConsumeMemberMap::value_type(id,count));
			}
		}
		CurrTBBoldness+=boldnessCount;
	}

	inline void addUpTBSoul(unsigned int id,int count,unsigned int boldnessCount)
	{
		if(upTBSoulList.find(id)!=upTBSoulList.end())
		{
			upTBSoulList.find(id)->second+=count;
			if(upTBSoulList.find(id)->second<=0)
			{
				upTBSoulList.erase(upTBSoulList.find(id));
			}
		}
		else
		{
			if(count>0)
			{
				upTBSoulList.insert(UpTBConsumeMemberMap::value_type(id,count));
			}
		}
		CurrTBBoldness+=boldnessCount;
	}

	inline unsigned int getUpTBDiscipleById(unsigned int id)
	{
		if(upTBDiscipleList.find(id)!=upTBDiscipleList.end())
		{
			return upTBDiscipleList.find(id)->second;

		}
		return 0;
	}

	inline unsigned int getUpTBSoulById(unsigned int id)
	{
		if(upTBSoulList.find(id)!=upTBSoulList.end())
		{
			return upTBSoulList.find(id)->second;

		}
		return 0;
	}

	inline void clearUpTBCounsumeInfo()
	{
		UpTBConsumeMemberMap::iterator itm=upTBDiscipleList.begin();
		while(itm!=upTBDiscipleList.end())
		{
			itm->second=0;
			++itm;
		}
		upTBDiscipleList.clear();

		UpTBConsumeMemberMap::iterator its=upTBSoulList.begin();
		while(its!=upTBSoulList.end())
		{
			its->second=0;
			++its;
		}
		upTBSoulList.clear();
		CurrTBBoldness=0;
	}

	inline unsigned int getCurrCashBoldness()
	{
		return CurrTBBoldness;
	}

	inline void setNeedRefreshTeamBuffPage(bool _refresh)
	{
		NeedRefreshTeamBuffPage=_refresh;
	}

	inline bool getNeedRefreshTeamBuffPage(){ return NeedRefreshTeamBuffPage;};

	unsigned int DisciplePageTab;
	unsigned int TransferExp_Receiver;
	unsigned int TransferExp_Provider;
	unsigned int TrainingBolusID;
	unsigned int TransferBeforeBlood;
	unsigned int TransferBeforeAttack;
	unsigned int TransferBeforeDefense;
	unsigned int TransferBeforeRP;
	unsigned int TransferBeforeLV;
	unsigned int TransferBeforeExp;
	unsigned int TransferBeforePotential;
	unsigned int TrainDisciple;
	unsigned int TrainType;
	unsigned int ShowDisciple;
	unsigned int ShowSoul;

	bool NeedChangeDisciple;
	bool NeedChooseDisciple;
	unsigned int UpgradeSoul;
	unsigned int UpgradeDiscipleBeforePotential;
	bool isUpgradeDisciple;
	std::string jumpDisciplePageName;

	unsigned int ShowSkill;
	unsigned int ShowSkillItem;
	bool NeedChangeSkill;
	bool NeedChooseSkill;
	unsigned int UpgradeSkill;
	unsigned int SkillConsume[5];
	unsigned int SkillConsumeCount;
	unsigned int SkillUpgradeBeforeLV;
	std::string SkillUpgradeBeforeQuantity;
	std::string jumpSkillPageName;
	bool JustViewSkill;
	bool IsTimeoutResponse;

	unsigned int ShowEquip;
	bool NeedChangeEquip;
	bool NeedChooseEquip;
	unsigned int UpgradeEquip;
	unsigned int EquipBeforeSilverCoins;
	unsigned int EquipBeforeLevel;
	unsigned int EquipBeforeQuantity;
	std::string jumpEquipPageName;


	unsigned int CurBattlePosition;
	unsigned int CurChangeSkillPosition;
	unsigned int CurChangeEquipID;
	unsigned int CurChangeEquipType;
	unsigned int CurChangeDiscipleID;

	unsigned int SkillBookId;
	unsigned int SkillBookSkillId;
	bool SnatchSkillBookRet;
	unsigned int SnatchSkillBookPlayerId;
	unsigned int ShowTitle;

	unsigned int WorldBossRankType;
	unsigned int WorldBossId;

	unsigned int UseToolId;
	unsigned int UseToolCount;

	unsigned int CareerAttackChapterId;
	unsigned int CareerAttackCustomsId;

	unsigned int TeamShowIndex;

	unsigned int MarketBuyItemId;

	unsigned int ConsumeTrainningBolus;

	unsigned int PreTitleId;
	unsigned int UpdateTitleType;

	unsigned int ToAdventruePageType;

	unsigned int loseArenaData;

	unsigned int fortuneComeFlag;
	unsigned int fortuneComeSum;

	unsigned int worldbossRankFlag;

	unsigned int worldbossFinalFight;

	bool showFightGuide;

	unsigned int continueOpenBoxNum;

	unsigned int quickEnhanceTimes;

	bool isSamSungi9100Audio;

	unsigned int stonePosition;
	unsigned int currentStone;
	unsigned int currentStoneMId;
	unsigned int currStonePage;

	typedef std::map<unsigned int,int> UpTBConsumeMemberMap;
	typedef ConstMapIterator<UpTBConsumeMemberMap> UpTBConsumeMemberMapIterator;
	UpTBConsumeMemberMap upTBDiscipleList;
	UpTBConsumeMemberMap upTBSoulList;
	unsigned int CurrTBBoldness;
	bool NeedRefreshTeamBuffPage;

	UpTBConsumeMemberMapIterator getUpTBConsumeDiscipleMapIterator()
	{
		return UpTBConsumeMemberMapIterator(upTBDiscipleList.begin(),upTBDiscipleList.end());
	};
	UpTBConsumeMemberMapIterator getUpTBConsumeSoulMapIterator()
	{
		return UpTBConsumeMemberMapIterator(upTBSoulList.begin(),upTBSoulList.end());
	};

	bool hasVarible(const std::string& key);

	bool addVarible(const std::string& key, const std::string& value);
	bool addVarible(const std::string& key, bool value);
	bool addVarible(const std::string& key, unsigned int value);

	void delVarible(const std::string& key);

	bool setVarible(const std::string& key, const std::string& value);
	bool setVarible(const std::string& key, bool value);
	bool setVarible(const std::string& key, unsigned int value);

	std::string getVarible(const std::string& key);
	unsigned int getVaribleAsUint(const std::string& key);
	bool getVaribleAsBool(const std::string& key);
	
	std::string fetchVarible(const std::string& key);
	unsigned int fetchVaribleAsUint(const std::string& key);
	bool fetchVaribleAsBool(const std::string& key);

	void clearMap();

	static BlackBoard* getInstance();

private:
		std::map<std::string,std::string> mVaribleMap;
};

#define HAS_V(key)			BlackBoard::Get()->hasVarible(key)
#define ADD_V(key, value)	BlackBoard::Get()->addVarible(key, value)
#define SET_V(key, value)	BlackBoard::Get()->setVarible(key, value)
#define GET_V_S(key)		BlackBoard::Get()->getVarible(key)
#define GET_V_UINT(key)		BlackBoard::Get()->getVaribleAsUint(key)
#define GET_V_B(key)		BlackBoard::Get()->getVaribleAsBool(key)
#define FETCH_V_S(key)		BlackBoard::Get()->fetchVarible(key)
#define FETCH_V_UINT(key)	BlackBoard::Get()->fetchVaribleAsUint(key)
#define FETCH_V_B(key)		BlackBoard::Get()->fetchVaribleAsBool(key)
#define DEL_V(key)			BlackBoard::Get()->delVarible(key)

#define ADD_OR_SET_V(key, value)	if(HAS_V(key)) SET_V(key, value); else ADD_V(key, value)