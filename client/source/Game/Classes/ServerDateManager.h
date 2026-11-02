#pragma once
#include "Singleton.h"
#include "IteratorWrapper.h"
#include "DataTableManager.h"
#include "Language.h"
#include "ResManager.h"

#include <string>
#include <map>
#include <list>
#include <set>

class	UserBasicInfo
{
public:
	std::string		name;
	unsigned int	level;
	unsigned int	exp;
	unsigned int	power;
	unsigned int	poweraddtime;
	unsigned int	maxpower;
	unsigned int	vitality;
	unsigned int	vitalityaddtim;
	unsigned int	maxvitality;
	unsigned int	viplevel;
	unsigned int	rechargenum;
	int64_t			silvercoins;
	unsigned int	goldcoins;
	unsigned int	tutorialstep;
	unsigned int	servertime;
	unsigned int	totalTimeStamp;
	unsigned int	titleTotalNum;
	unsigned int	todayAddedPowerNum;
	unsigned int    todayBuyPowerNum;//add by xinghui
	unsigned int	todayAddedVitalityNum;
	unsigned int    todayBuyVitalityNum;//add by xinghui
	std::string		dropStatus;
	unsigned int	unReadCount;
	unsigned int	readTime;
	std::string		actStatus;
};
class MarketInfo
{
public:

	bool hasVipGift(int vipGift);;
	void clearVipGift();;
	void insertVipGift(int vipGift);;

	unsigned int	servertime;
	int				msgGotTime;
	int				recuritTenCount;
	int				recurittime1;
	int				recurittime2;
	int				recurittime3;
	int				recuritCost1;
	int				recuritCost2;
	int				recuritCost3;
	bool			firstPay;
	std::set<int>	gotVipGifts;
	bool			firstbaili;
	bool			firstwanli;
};
struct UserSettingInfo
{
	unsigned int music;
	unsigned int sound;
	unsigned int chat;
};

struct UserToolInfo
{
	unsigned int id;
	unsigned int itemid;
	unsigned int count;

	UserToolInfo()
	{
		memset(this,0,sizeof(*this));
	}
};
typedef std::map<unsigned int, UserToolInfo* > UserToolInfoList;
typedef ConstMapIterator<UserToolInfoList> UserToolInfoListIterator;

class UserDiscipleInfo
{
public:
	unsigned int id;//sever id
	unsigned int itemid;//resource id
	unsigned int level;
	unsigned int exp;
	unsigned int health;
	unsigned int attack;
	unsigned int defence;
	unsigned int rp;
	unsigned int upgradelevel;
	unsigned int potentiality;
	unsigned int skill1;
	unsigned int battleid;
	unsigned int preLevel;

	UserDiscipleInfo()
	{
		memset(this,0,sizeof(*this));
	}
};
typedef std::map<unsigned int, UserDiscipleInfo* > UserDiscipleInfoList;//<server id, pointer>
typedef ConstMapIterator<UserDiscipleInfoList> UserDiscipleInfoListIterator;
typedef std::map<unsigned int,unsigned int> UserItemKeyIdMap;

class UserSoulInfo
{
public:
	unsigned int id;//server id
	unsigned int itemid;// resource id
	unsigned int count;
};
typedef std::map<unsigned int, UserSoulInfo* > UserSoulInfoList;//<server id, pointer>
typedef ConstMapIterator<UserSoulInfoList> UserSoulInfoListIterator;

const unsigned int MaxBattleDiscipleNum = 8;

class SkillInfo
{
public:
	unsigned int	id;
	unsigned int	itemid;
	unsigned int	level;
	float			quantity;
	unsigned int	consume;
	bool			isBornSkill;
};
typedef std::map<unsigned int, SkillInfo* > SkillInfoList;
typedef ConstMapIterator<SkillInfoList> SkillInfoListIterator;

class UserStoneInfo{
public:
	unsigned int color;
	unsigned int status;
	unsigned int stoneid;
};

class UserEquipInfo
{
public:
	void insertPropertyInfo(int index, int value);;
	unsigned int getPorpertyInfoSize();;
	int getPropertyInfoByKey(int key);
	void setPropertyInfoByKey(int key, int value);
	int getPropertyInfoByIndex(int index);
	void setPropertyInfoByIndex(int index, int value);
	void setStoneInfoByPosition(int index,int value);

	unsigned int id;
	unsigned int itemid;
	unsigned int level;
	unsigned int refinexp;
	unsigned int refinelevel;
	std::map<unsigned int, unsigned int> propertyInfo;
	std::map<unsigned int, UserStoneInfo* > stoneMap;
	unsigned int bufftype;
	std::string buffvalue;
};
typedef std::map<unsigned int, UserEquipInfo* > UserEquipInfoList;
typedef ConstMapIterator<UserEquipInfoList> UserEquipInfoListIterator;

class TitleInfo
{
public:
	unsigned int id;
	unsigned int itemid;
	unsigned int startime;
};
typedef std::map<unsigned int, TitleInfo* > TitleInfoList;
typedef ConstMapIterator<TitleInfoList> TitleInfoListIterator;

struct UserFriendsInfo 
{
	unsigned int id;
	std::string name;
	unsigned int level;
	unsigned int type;
};
typedef std::map<unsigned int,UserFriendsInfo* > UserFriendsInfoList;
typedef ConstMapIterator<UserFriendsInfoList > UserFriendsInfoListIterator;
typedef std::list<UserFriendsInfoList::iterator > UserFriendsInfoIterList;
typedef std::map<unsigned int,unsigned int > UserFriendsAttentionMap;

struct UserChatInfo
{
	unsigned int id;
	unsigned int uId;
	unsigned int vipLevel;
	std::string name;
	std::string msg;
	unsigned int eventTime;
	unsigned int msgType;
};

#define MAX_BATTLE_ARRAY 8
class UserBattleInfo
{
public:
	UserBattleInfo()
	{
		id = 0;
		discipleId = 0;
		attack = 0;
		defence = 0;
		health = 0;
		rp = 0;
		skill1 = 0;
		skill2 = 0;
		equip1 = 0;
		equip2 = 0;
		equip3 = 0;
		position = 0;
	}

	void insertFateInfos(int index, int value);;
	unsigned int getFateInfosSize();
	bool hasFateInfo(int key);
	int getFateInfosByKey(int key);
	void setFateInfosByKey(int key, int value);
	int getFateInfosByIndex(int index);
	void setFateInfosByIndex(int index, int value);
	void clearFateInfos();

	unsigned int id;
	unsigned int discipleId;
	unsigned int attack;
	unsigned int defence;
	unsigned int health;
	unsigned int rp;
	unsigned int skill1;
	unsigned int skill2;
	unsigned int equip1;
	unsigned int equip2;
	unsigned int equip3;
	unsigned int position;
	std::map<unsigned int, unsigned int> fateInfos;
};
typedef std::map<unsigned int, UserBattleInfo* > UserBattleInfoList;
typedef ConstMapIterator<UserBattleInfoList> UserBattleInfoListIterator;

// class CompetitorCharacter
// {
// public:
// 	CompetitorCharacter()
// 	{
// 		discipleItemId	= 0;
// 		discipleLevel	= 0;
// 		attack			= 0;
// 		defence			= 0;
// 		health			= 0;
// 		rp				= 0;
// 		skill1ItemId	= 0;
// 		skill2ItemId	= 0;
// 		skill3ItemId	= 0;
// 		equip1ItemId	= 0;
// 		equip2ItemId	= 0;
// 		equip3ItemId	= 0;
// 		skill1Level		= 0;
// 		skill2Level		= 0;
// 		skill3Level		= 0;
// 		equip1Level		= 0;
// 		equip2Level		= 0;
// 		equip3Level		= 0;
// 	}
// 	
// 	unsigned int discipleItemId;
// 	unsigned int discipleLevel;
// 	unsigned int CharacterValue;
// 	unsigned int attack;
// 	unsigned int defence;
// 	unsigned int health;
// 	unsigned int rp;
// 	unsigned int skill1ItemId;
// 	unsigned int skill2ItemId;
// 	unsigned int skill3ItemId;
// 	unsigned int equip1ItemId;
// 	unsigned int equip2ItemId;
// 	unsigned int equip3ItemId;
// 	unsigned int skill1Level;
// 	unsigned int skill2Level;
// 	unsigned int skill3Level;
// 	unsigned int equip1Level;
// 	unsigned int equip2Level;
// 	unsigned int equip3Level;
// 	std::map<unsigned int, unsigned int> fateInfos;
// };
// 
// class Competitor
// {
// 	unsigned int id;
// 	unsigned int type; //操作类型：1—查看，2—挑战，3—反击，4—无操作
// 	std::string name;
// 	unsigned int level;
// 	unsigned int ranking;
// 	unsigned int integral; //该位置积分
// 	unsigned int achievements; //气势
// 	unsigned int achievementsLevel[6]; //气势等级
// 
// 	CompetitorCharacter competitorCharacter[MaxBattleDiscipleNum];
// };


typedef std::map<unsigned int,UserChatInfo* > UserChatInfoList;
typedef ConstMapIterator<UserChatInfoList > UserChatInfoListIterator;

struct ShopItem
{
	std::string productID;
	std::string productName;
	float productPrice;
	float productOrignalPrice;
	int productCount;
};
typedef std::map<unsigned int,ShopItem*> ShopItemList;

struct TrainDiscipleInfo
{
	struct TrainStatus
	{
		unsigned int pro;
		unsigned int num;
	};
	unsigned int id;
	TrainStatus minus;
	TrainStatus add;
};

class BoardCastInfo 
{
public:
	void addParam(int param);
	unsigned int getParamsSize();;
	int getParamByIndex(int index);;
	void clearParams();

	unsigned int id;
	unsigned int itemType;
	std::string name;
	typedef std::vector<unsigned int > ParamsVec;
	ParamsVec params;
};
typedef std::map<unsigned int,BoardCastInfo* > BoardCastInfoMap;
typedef ConstMapIterator<BoardCastInfoMap > BoardCastInfoMapIterator;


class ContinueLoginInfo 
{
public:
	struct ContinueReward
	{
		unsigned int id;
		unsigned int itemType;
		unsigned int itemId;
		unsigned int quantity;
	};

	unsigned int getContinueRewardMapSize();
	ContinueReward* getContinueRewardByIndex(unsigned int index);
	bool hasContinueReward(unsigned int key);
	ContinueReward* getContinueRewardByKey(unsigned int key);
	void insertContinueReward(unsigned int key, ContinueReward* info);

	unsigned int count;
	typedef std::map<unsigned int,ContinueReward* > ContinueRewardMap;
	ContinueRewardMap continueReward;
};

struct AdventureItemInfo 
{
	unsigned int id;
	unsigned int adventureId;
	unsigned int itemId;
	unsigned int count;
	unsigned int beginTime;
	unsigned int endTime;
	unsigned int version;
};

typedef std::map<unsigned int,AdventureItemInfo* > AdventureItemInfoMap;
typedef ConstMapIterator<AdventureItemInfoMap > AdventureItemInfoMapIterator;

class SkillBookInfo
{
public:
	struct SkillBookItem
	{
		unsigned int partId;
		unsigned int partCount;
	};

	unsigned int getSkillBookItemMapSize();
	SkillBookItem* getContinueRewardByIndex(unsigned int index);
	bool hasSkillBookItem(unsigned int key);
	SkillBookItem* getSkillBookItemByKey(unsigned int key);
	void insertSkillBookItem(unsigned int key, SkillBookItem* info);

	typedef std::map<unsigned int,SkillBookItem* > SkillBookItemMap;
	typedef ConstMapIterator<SkillBookItemMap > SkillBookItemMapIterator;
	SkillBookItemMap mSkillBookItemList;
};

typedef std::map<unsigned int,SkillBookInfo* > SkillBookInfoMap;
typedef ConstMapIterator<SkillBookInfoMap > SkillBookInfoMapIterator;

struct SkillBookLootInfo
{
	struct PlayerTeamItem
	{
		unsigned int id;
		unsigned int memberId;

		PlayerTeamItem()
			:id(0),memberId(0)
		{

		}
	};
	unsigned int id;
	unsigned int playerId;
	std::string  playerName;
	unsigned int playerLevel;
	unsigned int isNpc;
	typedef std::map<unsigned int,PlayerTeamItem* > PlayerTeamItemMap;
	typedef ConstMapIterator<PlayerTeamItemMap > PlayerTeamItemMapIterator;
	PlayerTeamItemMap mPlayerTeamList;
	PlayerTeamItem* addPlayerTeamItemById(unsigned int id)
	{
		PlayerTeamItem* info = NULL;
		if(mPlayerTeamList.find(id) != mPlayerTeamList.end())
			info = mPlayerTeamList.find(id)->second;
		else
		{
			info = new PlayerTeamItem();
			info->id = id;
			mPlayerTeamList.insert(PlayerTeamItemMap::value_type(id,info));
		}
		return info;
	}
	PlayerTeamItemMapIterator getPlayerTeamItemMapIterator(void)
	{
		return PlayerTeamItemMapIterator(mPlayerTeamList.begin(),mPlayerTeamList.end());
	}
};

typedef std::map<unsigned int,SkillBookLootInfo* > SkillBookLootInfoMap;
typedef ConstMapIterator<SkillBookLootInfoMap > SkillBookLootInfoMapIterator;

struct HandbookPersonInfo
{
public:
	unsigned int id;
	unsigned int userid;
	unsigned int userstatus;

};
typedef std::map<unsigned int, HandbookPersonInfo* > HandbookPersonInfoList;
typedef ConstMapIterator<HandbookPersonInfoList > HandbookPersonInfoIterator;

struct HandbookEquipmentInfo
{
public:
	unsigned int id;
	unsigned int equipmentid;
	unsigned int equipmentstatus;

};
typedef std::map<unsigned int, HandbookEquipmentInfo* > HandbookEquipmentInfoList;
typedef ConstMapIterator<HandbookEquipmentInfoList > HandbookEquipmentInfoIterator;

struct HandbookSkillInfo
{
public:
	unsigned int id;
	unsigned int skillid;
	unsigned int skillstatus;

};
typedef std::map<unsigned int, HandbookSkillInfo* > HandbookSkillInfoList;
typedef ConstMapIterator<HandbookSkillInfoList > HandbookSkillInfoIterator;
/*
typedef std::map<unsigned int,unsigned int> UserItemKeyIdMap;
*/


struct UserAdventureDataInfo
{
	struct AddPowerInfo
	{
		unsigned int amStatus;
		unsigned int pmStatus;
	};
	struct BoundlessHoleInfo
	{
		unsigned int accumulateCount;
		unsigned int currCount;
		unsigned int todayStatus;
	};
	
	AddPowerInfo addPowerInfo;
	BoundlessHoleInfo boundlessHoldeInfo;
	std::string mContinueLoginInfo;
	std::string mInviteKey;
	std::string mInviteStatus;
	std::string mInviteRewardStatus;
};
struct AdventureTeacherInfo
{
	unsigned int id;
	unsigned int teacherId;
	unsigned int serverTime;
	unsigned int endTime;
	unsigned int exp;
	float totalTimeStamp;
};
typedef std::map<unsigned int,AdventureTeacherInfo* > AdventureTeacherInfoMap;

struct DiscipleExpAddedInfo
{
	unsigned int id;
	unsigned int discipleId;
	unsigned int level;
	unsigned int exp;
	unsigned int expAdded;
	unsigned int extraExpAdded;
	unsigned int preLevel;
};
typedef std::map<unsigned int,DiscipleExpAddedInfo* > DiscipleExpAddedInfoMap;
typedef ConstMapIterator<DiscipleExpAddedInfoMap > DiscipleExpAddedInfoMapIterator;

class AdventureFightEndInfo
{
public:
	

	enum PanelType
	{
		MainPanel=1,
		TeamPanel=2,
		StepAddAttr=3,
		RewardPanel=4,
		YestAddAttrPanel=5,
	};
	enum RewardType 
	{
		NONE = 0,
		GOLD = 1,
		SILVER = 2,
		TOOLS =3,
	};
	enum Addition_Type
	{
		HP=1,
		ATTATCK=2,
		DEFENSE=3,
		WILL=4,
	};
	struct TotalAdditionStatInfo
	{
		unsigned int hp;
		unsigned int attack;
		unsigned int defense;
		unsigned int will;
	};
	struct PassBarrierInfo
	{
		unsigned int attackCount;
		unsigned int surplusAttackCount;
		unsigned int currBarrier;
		unsigned int currObtainStar;
		unsigned int currSurplusStar;
		unsigned int maxPassBarrier;
		unsigned int maxObtainStar;
		unsigned int stepObtainStar;
		unsigned int exceedObtainStar;
		unsigned int addition;
		TotalAdditionStatInfo todayAddition;
	};
	struct FightTeamItem
	{
		unsigned int id;
		unsigned int memberId;
		unsigned int selfMember;
		unsigned int opponentMember;
		unsigned int rewardKeyId;
	};
	struct StepAdditionItem
	{
		unsigned int id;
		unsigned int type;
		unsigned int percentage;
		unsigned int requireStar;
	};
	struct RewardItem
	{
		unsigned int id;
		unsigned int itemId;
		unsigned int itemType;
		unsigned int count;
	};

	unsigned int getFightTeamListSize();
	FightTeamItem* getFightTeamByIndex(unsigned int index);

	unsigned int getStepAdditionTeamListSize();
	StepAdditionItem* getStepAdditionTeamByIndex(unsigned int index);

	PassBarrierInfo mYestFightInfo;
	PassBarrierInfo mTodayFightInfo;
	unsigned int mPanelType;
	unsigned int mCalculateTodayRanking;
	unsigned int mContinueInRankAmount;
	unsigned int mIsNewStatus;
	typedef std::map<unsigned int,FightTeamItem* > FightTeamInfoMap;
	typedef ConstMapIterator<FightTeamInfoMap > FightTeamInfoMapIterator;
	FightTeamInfoMap mFightTeamList;

	typedef std::map<unsigned int,StepAdditionItem* > StepAdditionItemMap;
	typedef ConstMapIterator<StepAdditionItemMap > StepAdditionItemMapIterator;
	StepAdditionItemMap mStepAdditionItemList;

	typedef std::map<unsigned int,RewardItem* > RewardItemMap;
	typedef ConstMapIterator<RewardItemMap > RewardItemMapIterator;
	RewardItemMap mRewardItemList;
};

struct FightEndRankInfo
{
	struct RankItem
	{
		unsigned int id;
		unsigned int playerId;
		std::string playerName;
		unsigned int level;
		unsigned int passBattle;
		unsigned int obtainStar;
		unsigned int sussessionCount;
	};
	enum Fight_Rank_Type
	{
		FIVE_BATTLE=1,
		SIX_BATTLE=2,
		SEVEN_BATTLE=3,
		EIGHT_BATTLE=4,
	};
	typedef std::map<unsigned int,RankItem* > RankItemMap;
	typedef ConstMapIterator<RankItemMap > RankItemMapIterator; 
	RankItemMap mFiveRankList;
	RankItemMap mSixRankList;
	RankItemMap mSevenRankList;
	RankItemMap mEightRankList;
	unsigned int mPlayerId;
};

struct AdventureLevelUpRewardInfo{

	unsigned int id ;
	unsigned int needLevel ;
	unsigned int rechargenum ;
	unsigned int goldcoins ;
	unsigned int upVipLevel ;
	unsigned int rewardStatus ;
	unsigned int size;

};

typedef std::map<unsigned int, AdventureLevelUpRewardInfo* > AdventureLevelUpRewardInfoList;
typedef ConstMapIterator<AdventureLevelUpRewardInfoList > AdventureLevelUpRewardInfoIterator;

struct WorldBossInfo
{
	unsigned int bossId;
	unsigned int serverTime;
	unsigned int beginTime;
	unsigned int bossLevel;
	unsigned int killedTime;
	unsigned int sustainedTime;
	unsigned int totalTimeStamp;
};
typedef std::map<unsigned int,WorldBossInfo* > WorldBossInfoMap;

struct WorldBossRankInfo
{
	unsigned int id;
	unsigned int playerId;
	std::string name;
	unsigned int level;
	unsigned int damage;
	unsigned int rankType;
};
typedef std::map<unsigned int,WorldBossRankInfo* > WorldBossRankInfoMap;
typedef ConstMapIterator<WorldBossRankInfoMap > WorldBossRankInfoMapIterator;

struct TeamBuffInfo
{

	unsigned int teamId;
	unsigned int teamLevel;
	unsigned int currBoldness;
};

typedef std::map<unsigned int,TeamBuffInfo* > TeamBuffInfoMap;
typedef ConstMapIterator<TeamBuffInfoMap > TeamBuffInfoMapIterator;

struct RewardData
{
	unsigned int id;
	unsigned int itemId;
	unsigned int itemType;
	unsigned int count;
};
typedef std::map<unsigned int,RewardData* > RewardDataMap;
typedef ConstMapIterator<RewardDataMap> RewardDataMapIterator;
typedef std::list<unsigned int> ResIDCollect;
typedef bool (*ResSort)(unsigned int, unsigned int);
bool resSort(unsigned int, unsigned int);


class UserSysMsgInfo 
{
public:
	unsigned int getRewardDataMapSize();;

	unsigned int id;
	unsigned int relationId;
	unsigned int uId;
	std::string name;
	std::string info;
	std::string content;
	unsigned int sysMsgType;
	unsigned int fightRet;
	unsigned int oprType;
	unsigned int eventTime;
	unsigned int addEnemyType;
	RewardDataMap rewardDataMap;
};

typedef std::map<unsigned int,UserSysMsgInfo* > UserSysMsgEntityMap;
typedef ConstMapIterator<UserSysMsgEntityMap > UserSysMsgEntityListIterator;
typedef std::list<UserSysMsgEntityMap::iterator > UserSysMsgEntityList;

class	UserArenaInfo
{
public:
	bool hasReward(unsigned int key);
	unsigned int				arenaRank;
	unsigned int				arenaScore;
	unsigned int				arenaNum;
	unsigned int				rewardScore;
	unsigned int				rewardRefreshTime;
	std::map<unsigned int, unsigned int>	rewardIDs;
	std::map<unsigned int, unsigned int>	gotRewardIDs;
};

class ArenaOpponentInfo
{
public:
	unsigned int	id;
	std::string		name;
	unsigned int	level;
	unsigned int	arenaRank;
	unsigned int	rewardScore;
	unsigned int	arenaType;
	unsigned int	dicipleItemID1;
	unsigned int	dicipleItemID2;
	unsigned int	dicipleItemID3;
};
typedef std::map<unsigned int, ArenaOpponentInfo* > ArenaOpponentInfoList;
typedef ConstMapIterator<ArenaOpponentInfoList> ArenaOpponentInfoListIterator;


class WorldBossItem
{
public:
	WorldBossItem(){
		bossLevel = 0;
		startTime = "12:30";
		finished = 1;
		bossTotalBlood = 10000;
		bossLeftBlood = 10000;
		attackNum = 0;
		damageBlood = 0;
		reliveTime = 0;
		lastTime = 0;
		useFireTime = 0;
	}
	unsigned int	servertime;
	unsigned int	bossLevel;
	std::string		startTime;
	unsigned int	finished;
	unsigned int	bossTotalBlood;
	unsigned int	bossLeftBlood;
	unsigned int	attackNum;
	unsigned int	damageBlood;
	unsigned int	reliveTime;
	unsigned int	lastTime;
	unsigned int	useFireTime;
};

class WorldBossBattleItem{
public:
	unsigned int	id;
	unsigned int	rank;
	std::string		name;
	unsigned int	level;
	unsigned int	damageBlood;
	unsigned int	battleId;
	unsigned int	playerId;
	bool	isFinal;
};
typedef std::map<unsigned int,WorldBossBattleItem* > WorldBossBattleMap;
typedef ConstMapIterator<WorldBossBattleMap> WorldBossBattleMapIterator;

class WorldBossDamageItem{
public:
	unsigned int	id;
	std::string		name;
	unsigned int	attackNum;
	unsigned int	damageBlood;
	unsigned int	discipleId;
};
typedef std::map<unsigned int,WorldBossDamageItem* > WorldBossDamageMap;
typedef ConstMapIterator<WorldBossDamageMap> WorldBossDamageMapIterator;

class ChargeRebateItem{
public:
	enum ChargeType
	{
		CHARGE=1,
		REBATE=2,
	};
	unsigned int	id;
	unsigned int	servertime;
	unsigned int	chargeType;
	unsigned int	recurittime;
	unsigned int	minRecharge;
	unsigned int	maxRecharge;
	float	refund;
	unsigned int	rechargeInfact;
	unsigned int	rebateBeginTime;
	unsigned int	rebateEndTime;
	unsigned int	getRewardToday;
	float	exchangeRate;
	unsigned int	getRewardStat;
	unsigned int	userGoldCoins;
	unsigned int	refundGoldCoins;
	unsigned int	rebateDuringDays;
};

class FortuneComeItem{
public:
	unsigned int	servertime ;
	unsigned int	lastDays ;
	unsigned int	chanceNum ;
	unsigned int	recurittime ;
	unsigned int	needGold ;
	unsigned int	userGold ;
	unsigned int	rewardGold ;
	unsigned int	rewardStat ;
	unsigned int	costGold ;
};

class ViewTeamBattleInfoItem
{
public:
	UserBattleInfoList		mUserBattleArray;
	UserDiscipleInfoList	mUserDiscipleInfoList;
	UserEquipInfoList		mUserEquipInfoList;
	SkillInfoList			mSkillInfoList;
	TitleInfoList			mTitleInfoList;
	std::string				mPlayerName;
	unsigned int			mPlayerLevel;
	unsigned int			mPlayerTitleNum;
	//SkillInfo
	SkillInfo* getAndCreatSkillInfo(unsigned int id);
	SkillInfo * getSkillInfoByID(unsigned int id);
	//EquipInfo
	UserEquipInfo* getAndCreatUserEquipInfo(unsigned int id);
	UserEquipInfo * getUserEquipInfoByID(unsigned int id);
	//UserBattleInfo
	UserBattleInfo* getAndCreatUserBattleInfo(unsigned int id);
	UserBattleInfo* getUserBattleInfo(unsigned int position);
	UserBattleInfoListIterator getUserBattleIterator( void );
	UserBattleInfo* getUserBattleInfoByDisciple( unsigned int discipleID );
	unsigned int getBattleArraySize(){ return mUserBattleArray.size();};
	//TitleInfo
	TitleInfo* getAndCreatTitleInfo(unsigned int id);
	TitleInfo * getTitleInfoByID(unsigned int id);
	TitleInfoListIterator getTitleIterator();
	//DiscipleInfo
	UserDiscipleInfo* getAndCreatDiscipleInfo(unsigned int id);
	UserDiscipleInfo * getUserDiscipleInfoByID(unsigned int id);
	void clearTeamBattleInfoItem(void);
};

class ServerDateManager : public Singleton<ServerDateManager>
{
public:
	ServerDateManager();
	static ServerDateManager* getInstance();

	//SeverID
	int getSeverID(){return mSeverID;}
	void setSeverID(int id){mSeverID = id;}
	int mSeverID;

	//UserBasicInfo
	void getBasicNextTime();
	unsigned int getNextLevelExp();
	float getUserExpPercent();
	const std::string getUserExpPercentStr();
	UserBasicInfo& getUserBasicInfo() {return mUserBasicInfo;};
	void updateBasicSyncTime();
	void resetBasicNextSyncTime();
	void creatAdventureLevelUpGetRewardInfo(unsigned int rechargeNum,unsigned int goldCoins);
	UserBasicInfo mUserBasicInfo;
	
	//UserSettingInfo
	UserSettingInfo& getUserSettingInfo() {return mSettingInfo;};
	UserSettingInfo mSettingInfo;

	//DiscipleInfo
	UserDiscipleInfo* getAndCreatDiscipleInfo(unsigned int id);
	UserDiscipleInfo * getUserDiscipleInfoByID(unsigned int id);
	UserDiscipleInfo * getUserDiscipleInfoByItemID(unsigned int itemId);
	UserDiscipleInfo * getUserDiscipleInfoByIndex(unsigned int index);
	inline unsigned int getDiscipleInfoTotalNum() {return mUserDiscipleInfoList.size();};
	void removeDiscipleInfoById(unsigned int id);
	void cleareDiscipleInfo() { mUserDiscipleInfoList.clear();}
	UserDiscipleInfoListIterator getDiscipleIterator(void);
	UserDiscipleInfoList mUserDiscipleInfoList;

	TrainDiscipleInfo& getTrainDiscipleInfo() {return mTrainDiscipleInfo;};
	TrainDiscipleInfo mTrainDiscipleInfo;

	//ToolInfo
	UserToolInfo* getAndCreatToolInfo(unsigned int id);
	UserToolInfo * getUserToolInfoByID(unsigned int id);
	UserToolInfo * getUserToolInfoByItemId(unsigned int itemid);
	UserToolInfo * getUserToolInfoByIndex(unsigned int index);
	inline unsigned int getToolInfoTotalNum() {return mUserToolInfoList.size();};
	UserToolInfoListIterator getToolIterator(void);
	void creatUserToolKeyMapByToolItem(const UserToolInfo* userToolInfo);
	UserToolInfoList mUserToolInfoList;
	UserItemKeyIdMap mUserToolItemKeyList;

	//SoulInfo
	UserSoulInfo* getAndCreatSoulInfo(unsigned int id);
	UserSoulInfo * getUserSoulInfoByDiscipleID(unsigned int id);
	UserSoulInfo * getUserSoulInfoByID(unsigned int id);
	UserSoulInfo * getUserSoulInfoByIndex(unsigned int index);
	inline unsigned int getSoulInfoTotalNum() {return mUserSoulInfoList.size();};
	UserSoulInfoListIterator getSoulIterator(void);
	void increaseSoulInfoById(unsigned int id,int count);
	UserSoulInfoList mUserSoulInfoList;

	//SkillInfo
	SkillInfo* getAndCreatSkillInfo(unsigned int id);
	SkillInfo * getSkillInfoByID(unsigned int id);
	SkillInfo * getSkillInfoByIndex(unsigned int index);
	inline unsigned int getSkillInfoTotalNum() {return mSkillInfoList.size();};
	SkillInfoListIterator getSkillIterator(void);
	void removeSkillInfoById(unsigned int id);
	void clearSkillInfo() { mSkillInfoList.clear();}
	SkillInfoList mSkillInfoList;

	//EquipInfo
	UserEquipInfo* getAndCreatUserEquipInfo(unsigned int id);
	UserEquipInfo * getUserEquipInfoByID(unsigned int id);
	UserEquipInfo * getEquipInfoByIndex(unsigned int index);
	inline unsigned int getUserEquipInfoTotalNum() {return mUserEquipInfoList.size();};
	UserEquipInfoListIterator getUserEquipIterator(void);
	void removeEquipInfoById(unsigned int id);
	UserEquipInfoList mUserEquipInfoList;

	//UserBattleInfo
	UserBattleInfo* getAndCreatUserBattleInfo(unsigned int id);
	UserBattleInfo* getUserBattleInfo(unsigned int position);
	UserBattleInfo * getUserBattleInfoByIndex(unsigned int index);
	UserBattleInfo* getUserBattleInfoByDisciple(unsigned int discipleID);
	unsigned int getBattleArraySize(){ return mUserBattleArray.size();};
	UserBattleInfoListIterator getUserBattleIterator(void);
	unsigned int getUserBattleSize(void);
	UserBattleInfoList mUserBattleArray;
	
	//TitleInfo
	TitleInfo* getAndCreatTitleInfo(unsigned int id);
	TitleInfo * getTitleInfoByID(unsigned int id);
	TitleInfo * getTitleInfoByIndex(unsigned int index);
	inline unsigned int getTitleInfoTotalNum() {return mTitleInfoList.size();};
	TitleInfoListIterator getTitleIterator(void);
	TitleInfoList mTitleInfoList;
	
	//ArenaInfo
	UserArenaInfo& getUserArenaInfo() {return mUserArenaInfo;};
	UserArenaInfo mUserArenaInfo;

	ArenaOpponentInfo* getAndCreatArenaOpponentInfo(unsigned int id);
	ArenaOpponentInfo * getArenaOpponentInfoByID(unsigned int id);
	ArenaOpponentInfo * getArenaOpponentInfoByIndex(unsigned int index);
	inline unsigned int getArenaOpponentInfoTotalNum() {return mArenaOpponentInfoList.size();};
	ArenaOpponentInfoListIterator getArenaOpponentIterator(void);
	void clearAllArenaOpponentInfoList();
	ArenaOpponentInfoList mArenaOpponentInfoList;

	//UserSysMsgInfo
	UserSysMsgInfo* getAndCreatSysMsgEntity(unsigned int id);
	UserSysMsgInfo * getUserSysMsgEntityByID(unsigned int id);
	UserSysMsgInfo * getUserSysMsgEntityByIndex(unsigned int index);
	inline unsigned int getUserSysMsgEntityListTotalNum() {return mSysMsgList.size();};
	UserSysMsgEntityListIterator getSysMsgEntityIterator(void);
	static bool cmpSysMsgEntity(const UserSysMsgEntityMap::iterator a,const UserSysMsgEntityMap::iterator b);
	UserSysMsgEntityList* cloneSysMsgEntityList(const unsigned int tabIndex);
	UserSysMsgEntityMap mSysMsgList;

	RewardData* createSysMsgRewardById(unsigned int id,unsigned int rewardId);
	RewardData * getSysMsgRewardByIndex(unsigned int id,unsigned int rewardIndex);
	unsigned int getSysMsgRewardTotalNum(unsigned int id);;
	RewardDataMapIterator getSysMsgRewardMapIteratorById(unsigned int id);
	void clearSysMsgRewardMap(unsigned int msgId);
	
	//UserFriendsInfo
	UserFriendsInfo* getAndCreatFriendInfo(unsigned int id);
	UserFriendsInfo* getAndCreatInviteFriendInfo(unsigned int id);
	UserFriendsInfo * getUserFriendsInfoByID(unsigned int id,unsigned int nodeType);
	UserFriendsInfo * getUserFriendsInfoIndex(unsigned int index,unsigned int nodeType);
	inline unsigned int getUserFriendsInfoTotalNum(unsigned int nodeType) {if(nodeType==1){return mFriendsEntityList.size();} else if(nodeType==2){return mInviteEntityList.size();} else{ return 0;}};
	UserFriendsInfoListIterator getFriendsInfoIterator(unsigned int nodeType);
	static bool cmpUserFriendsInfo(const UserFriendsInfoList::iterator a,const UserFriendsInfoList::iterator b);
	UserFriendsInfoIterList* cloneUserFriendsInfoByType(const unsigned int nodeType,const unsigned int tabIndex);
	UserFriendsInfoIterList* cloneUserFriendsInfoByList(UserFriendsInfoList* entityList,const unsigned int tabIndex,const unsigned int nodeType);
	void delUserFriendsInfoById(unsigned int id,unsigned int nodeType);
	void clearInviteFriendList(void);
	UserFriendsInfoList mFriendsEntityList;
	UserFriendsInfoList mInviteEntityList;
	
	//UserChatInfo
	UserChatInfo* getAndCreatChatInfo(unsigned int id);
	UserChatInfo * getUserChatInfoByID(unsigned int id);
	UserChatInfo * getUserChatInfoByIndex(unsigned int index);
	inline unsigned int getChatInfoTotalNum() {return mUserChatInfoList.size();};
	void clearChatInfo();
	UserChatInfoListIterator getChatIterator(void);
	UserChatInfoList mUserChatInfoList;
	
	//BoardCastInfo
	BoardCastInfo* getAndCreatBoardCastInfoEntity(unsigned int id);
	BoardCastInfo * getBoardCastInfoByID(unsigned int id);
	BoardCastInfo * getBoardCastInfoByIndex(unsigned int index);
	inline unsigned int getBoardCastInfoListTotalNum() {return mBoardCastInfoList.size();};
	BoardCastInfoMapIterator getBoardCastInfoIterator(void);
	BoardCastInfoMap mBoardCastInfoList;
	
	//ContinueLoginInfo
	ContinueLoginInfo& getContinueLoginInfo();;
	ContinueLoginInfo::ContinueReward* getContinueRewardByIndex(unsigned int index);
	unsigned int getContinueLoginInfoMaxIndex();
	ContinueLoginInfo::ContinueReward* _getContinueReward(unsigned int index);
	ContinueLoginInfo mContinueLoginInfo;
	
	//AdventureItemInfo
	AdventureItemInfo* getAndCreatAdventureItemInfo(unsigned int id);
	AdventureItemInfo * getAdventureItemInfoByID(unsigned int id);
	AdventureItemInfo * getAdventureItemInfoByIndex(unsigned int index);
	inline unsigned int getAdventureItemInfoMapTotalNum() {return mAdventureItemInfoList.size();};
	AdventureItemInfoMapIterator getAdventureItemInfoMapIterator(void);
	void removeAdventureItemInfoById(const unsigned int id);
	inline unsigned int getAdventureMaxIdNext(){return mAdventureMaxId+1;};
	void resetAdventureMaxId(){mAdventureMaxId=0;};
	
	bool minusSkillBookCountById(unsigned int id);
	unsigned int getAdventureSkillBookPieceCount(unsigned int skillId);
	AdventureItemInfoMap mAdventureItemInfoList;
	unsigned int mAdventureMaxId;

	UserAdventureDataInfo& getUserAdventureDataInfo(){ return mAdventureDataInfo;};
	UserAdventureDataInfo mAdventureDataInfo;
	
	//SkillBookInfo
	SkillBookInfo* getAndCreatSkillBookInfoBySkillId(unsigned int id);
	SkillBookInfo * getSkillBookInfoByIndex(unsigned int index);
	inline unsigned int getSkillBookInfoMapTotalNum() {return mSkillBookInfoList.size();};
	SkillBookInfoMapIterator getSkillBookInfoMapIterator(void);
	SkillBookInfo::SkillBookItem* getSkillBookItemBySkillIdAndPartId(unsigned int skillId,unsigned int partId);
	SkillBookInfo::SkillBookItem* getSkillBookItemBySkillIdAndIndex(unsigned int skillId,unsigned int index);
	unsigned int getSkillBookItemMapTotalNum(unsigned int skillId);
    SkillBookInfo::SkillBookItemMapIterator getSkillBookItemMapIterator(unsigned int skillId);
	void pieceSkillBookBySkillId(unsigned int skillId);
	SkillBookInfoMap mSkillBookInfoList;
	
	void addSkillBookPieceChanceBySkillId(unsigned int skillId,unsigned int count);
	void addSkillBookPartBySkillIdAndPartId(unsigned int skillId,unsigned int partId,unsigned int count);

	//SkillBookLootInfo
	SkillBookLootInfo* getAndCreatSkillBookLootInfoById(unsigned int id);
	SkillBookLootInfo* getSkillBookLootItemById(unsigned int id);
	SkillBookLootInfo* getSkillBookLootItemByIndex(unsigned int index);
	inline unsigned int getSkillBookLootInfoMapTotalNum() {return mSkillBookLootList.size();};
	SkillBookLootInfoMapIterator getSkillBookLootMapIterator(void);
	void clearSkillBookLoot(void);
	SkillBookLootInfoMap mSkillBookLootList;
	
	//HandbookPersonInfo
	HandbookPersonInfo* getAndCreatHandbookPersonInfo(unsigned int id);
	HandbookPersonInfo * getHandbookPersonInfoByID(unsigned int id);
	HandbookPersonInfo* getHandbookPersonInfoByIndex(unsigned int index);
	inline unsigned int getHandbookPersonInfoListTotalNum() {return mHandbookPersonInfoList.size();};
	HandbookPersonInfoIterator getHandbookPersonIterator(void);
	HandbookPersonInfoList mHandbookPersonInfoList;

	//HandbookSkillInfo
	HandbookSkillInfo* getAndCreatHandbookSkillInfo(unsigned int id);
	HandbookSkillInfo * getHandbookSkillInfoByID(unsigned int id);
	HandbookSkillInfo* getHandbookSkillInfoByIndex(unsigned int index);
	inline unsigned int getHandbookSkillInfoListTotalNum() {return mHandbookSkillInfoList.size();};
	HandbookSkillInfoIterator getHandbookSkillIterator(void);
	HandbookSkillInfoList mHandbookSkillInfoList;
	
	//HandbookEquipmentInfo
	HandbookEquipmentInfo* getAndCreatHandbookEquipmentInfo(unsigned int id);
	HandbookEquipmentInfo * getHandbookEquipmentInfoByID(unsigned int id);
	HandbookEquipmentInfo* getHandbookEquipmentInfoByIndex(unsigned int index);
	inline unsigned int getHandbookEquipmentInfoListTotalNum() {return mHandbookEquipmentInfoList.size();};
	HandbookEquipmentInfoIterator getHandbookEquipmentIterator(void);
	HandbookEquipmentInfoList mHandbookEquipmentInfoList;
	
	//AdventureFightEndInfo
	AdventureFightEndInfo* getAdventureFightEndInfo() { return &mAdventureFightEndInfo;};
	void clearFightEndAllMap();

	AdventureFightEndInfo::FightTeamItem* addFightEndFightItemById(unsigned int id);
	AdventureFightEndInfo::FightTeamItem * getFightEndFightItemById(unsigned int id);
	AdventureFightEndInfo::FightTeamItem * getFightEndFightItemByIndex(unsigned int id);
	inline unsigned int getFightTeamListTotalNum() {return mAdventureFightEndInfo.mFightTeamList.size();};
	AdventureFightEndInfo::FightTeamInfoMapIterator getFightEndFightTeamInfoMapIterator(void);
	void removeFightEndAllTeamItem();
	
	AdventureFightEndInfo::StepAdditionItem* addFightEndStepAdditionItemById(unsigned int id);
	AdventureFightEndInfo::StepAdditionItem * getFightEndStepAdditionItemById(unsigned int id);
	AdventureFightEndInfo::StepAdditionItem * getFightEndStepAdditionItemByIndex(unsigned int id);
	inline unsigned int getStepAdditionItemListTotalNum() {return mAdventureFightEndInfo.mStepAdditionItemList.size();};
	AdventureFightEndInfo::StepAdditionItemMapIterator getFightEndStepAdditionItemMapIterator(void);
	void removeFightEndAllStepAdditionItem();
	
	AdventureFightEndInfo::RewardItem* addFightEndRewardItemById(unsigned int id);
	AdventureFightEndInfo::RewardItem * getFightEndRewardItemById(unsigned int id);
	AdventureFightEndInfo::RewardItem * getFightEndRewardItemByIndex(unsigned int id);
	inline unsigned int getRewardItemListTotalNum() {return mAdventureFightEndInfo.mRewardItemList.size();};
	AdventureFightEndInfo::RewardItemMapIterator getFightEndRewardItemMapIterator(void);
	void removeFightEndAllRewardItem();

	AdventureFightEndInfo mAdventureFightEndInfo;
	
	//DiscipleExpAddedInfo
	DiscipleExpAddedInfo* getAndCreateDiscipleExpAddedInfo(unsigned int id);
	DiscipleExpAddedInfo* getDiscipleExpAddedInfoByID(unsigned int id);
	DiscipleExpAddedInfo* getDiscipleExpAddedInfoByIndex(unsigned int id);
	inline unsigned int getDiscipleExpAddedInfoMapTotalNum() {return mDiscipleExpAddedInfoList.size();};
	DiscipleExpAddedInfoMapIterator getDiscipleExpAddedIterator(void);
	void removeAllDiscipleExpAddedInfo();
	DiscipleExpAddedInfoMap mDiscipleExpAddedInfoList;
	
	//FightEndRankInfo
	FightEndRankInfo::RankItem* getFightEndRankItemByTypeAndIndex(unsigned int type, unsigned int index);
	unsigned int getFightEndRankMapTotalNum(const unsigned int index);
	FightEndRankInfo::RankItemMapIterator getFightEndRankMapIteratorByIndex(const unsigned int index);
	FightEndRankInfo::RankItem* getFightEndRankItemByIndexAndId(const unsigned int index,const unsigned int id);
	FightEndRankInfo::RankItem* addRankItemByIndexAndId(const unsigned int index,const unsigned int id);
	void clearAllFightEndRankItem();
	void clearFightEndRankItemByIndex(const unsigned int index);
	FightEndRankInfo& getFightEndRankInfo(){ return mFightEndRankInfo;};
	FightEndRankInfo mFightEndRankInfo;

	//AdventureLevelUpRewardInfo
	AdventureLevelUpRewardInfo* getAndCreatAdventureLevelUpRewardInfo(unsigned int id);
	AdventureLevelUpRewardInfo* getAdventureLevelUpRewardInfoByID(unsigned int id);
	AdventureLevelUpRewardInfo* getAdventureLevelUpRewardInfoByIndex(unsigned int id);
	inline unsigned int getAdventureLevelUpRewardInfoListTotalNum() {return mAdventureLevelUpRewardInfoList.size();};
	AdventureLevelUpRewardInfoIterator getAdventureLevelUpRewardInfoIterator(void);
	AdventureLevelUpRewardInfoList mAdventureLevelUpRewardInfoList;
	
	inline void  setAdventureLevelUpRewardInfoSize(int size){mAdventureLevelUpRewardInfoSize = size;};
	inline int getAdventureLevelUpRewardInfoSize(){return mAdventureLevelUpRewardInfoSize;};
	int mAdventureLevelUpRewardInfoSize;

	//WorldBossInfo
	WorldBossInfo* getAndCreateWorldBossInfo(unsigned int id);
	const WorldBossInfo* getWorldBossInfoByID(const unsigned int id);
	WorldBossInfoMap mWorldBossList;

	//WorldBossRankInfo
	WorldBossRankInfo* getAndCreateWorldBossRankInfo(unsigned int index);
	WorldBossRankInfo* getWorldBossRankInfoById(const unsigned int id);
	WorldBossRankInfo* getWorldBossRankInfoByIndex(unsigned int index);
	inline unsigned int getWorldBossRankInfoMapTotalNum() {return mWorldBossRankList.size();};
	WorldBossRankInfoMapIterator getWorldBossRankInfoMapIterator(void);
	WorldBossRankInfoMap mWorldBossRankList;
	
	//TeamBuffInfo
	TeamBuffInfo* getAndCreatTeamBuffById(unsigned int id);
	TeamBuffInfo* getTeamBuffInfoById(const unsigned int id);
	TeamBuffInfo* getTeamBuffInfoByIndex(unsigned int index);
	inline unsigned int getTeamBuffInfoTotalSum(){return mTeamBuffInfoList.size();};
	TeamBuffInfoMapIterator getTeamBuffInfoMapIterator(void);
	TeamBuffInfoMap mTeamBuffInfoList;
	
	//ResInfo
	ResInfo* getAndCreateResInfo(unsigned int id);
	ResInfo* getResInfoById(unsigned int id);
	ResInfo* getResInfoByIndex(unsigned int index);
	inline unsigned int getResInfoMapTotalSum(){return mResInfoList.size();};
	ResInfoMapIterator getResInfoIterator(void);
	void clealAllResInfo();
	ResInfoMap mResInfoList;
	
	//MarketInfo
	MarketInfo& _getMarketInfo() {return mMarketInfo;};
	MarketInfo mMarketInfo;

	//ShopItem
	ShopItem* getAndCreatShopItem(unsigned int id);
	ShopItem* getShopItemIndex(unsigned int index);
	inline unsigned int getShopItemListTotalSum(){return mShopList.size();};
	ShopItemList mShopList;

	//AdventureTeacherInfo
	AdventureTeacherInfo* getAndCreateAdventureTeacherInfo(unsigned int id);
	AdventureTeacherInfo * getAdventureTeacherInfoByID(unsigned int id);
	void removeAdventureTeacherInfoById(const unsigned int id);
	void removeAdventureChildListInfo(const unsigned int id);
	AdventureTeacherInfoMap mAdventureTeacherInfoList;
	
	//RewardData
	RewardData* getAndCreateRewardData(unsigned int id);
	void rewardDataToResInfo();
	RewardDataMap mRewardDataList;
	ResIDCollect mResIDList;
	
	void sortResInfoList(ResSort rSort = resSort);
	
	
	
	//WorldBoss

	WorldBossItem mWorldBossItem;
	WorldBossItem& getWorldBossItem(){return mWorldBossItem;};

	void clearWorldBossBattle();
	WorldBossBattleItem * getAndCreateWorldBossBattleItem(unsigned int id);
	WorldBossBattleMap mWorldBossBattleList;
	WorldBossBattleItem * getWorldBossBattleItemByID(unsigned int id);
	WorldBossBattleMapIterator getWorldBossBattleIterator(void);

	/*
	void clearWorldBossLastBattle();
	WorldBossBattleItem * getAndCreateWorldBossLastBattleItem(unsigned int id);
	WorldBossBattleMap mWorldBossLastBattleList;
	WorldBossBattleItem * getWorldBossLastBattleItemByID(unsigned int id);
	WorldBossBattleMapIterator getWorldBossLastBattleIterator(void);
	*/

	void clearWorldBossDamage();
	WorldBossDamageItem * getWorldBossDamageItemByID(unsigned int id);
	WorldBossDamageMapIterator getWorldBossDamageIterator(void);
	WorldBossDamageItem * getNextWorldBossDamageItem();
	unsigned int worldBossDamageItemIndex;

	WorldBossDamageItem * getAndCreateWorldBossDamageItem(unsigned int id);
	WorldBossDamageMap mWorldBossDamageList;
	
	//ChargeRebate
	ChargeRebateItem& getChargeRebateItem(){return chargeRebateItem;};
	ChargeRebateItem  chargeRebateItem;

	//FortuneCome
	FortuneComeItem& getFortuneComeItem(){return fortuneComeItem;};
	FortuneComeItem  fortuneComeItem;

	//ShowUserBattle
	ViewTeamBattleInfoItem& getViewTeamBattleInfoItem(){return mTeamBattleInfoItem;};
	ViewTeamBattleInfoItem mTeamBattleInfoItem;
};