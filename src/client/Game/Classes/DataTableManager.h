#pragma once
#include "Singleton.h"
#include "TableReader.h"
#include "IteratorWrapper.h"
#include "inifile.h"
#include "ResManager.h"
#include "cocos2d.h"

#include <string>
#include <map>
#include <utility>

const std::string CloseButton_String("@CloseButton");

const std::string ChangeSkill_String("@ChangeSkill");
const std::string ChooseSkill_String("@ChooseSkill");
const std::string UpGradeSkill_String("@SkillUpGrade");

const std::string ChooseDisciple_String("@ChooseDisciple");
const std::string ChangeDisciple_String("@ChangeDisciple");
const std::string RecruitDisciple_String("@RecruitDisciple");
const std::string UpgradeDisciple_String("@UpgradeDisciple");
const std::string TransferExp_String("@TransferExp");

const std::string ChangeEquip_String("@ChangeEquip");
const std::string ChooseEquip_String("@ChooseEquip");
const std::string UpGradeEquip_String("@EquipUpGrade");

// quality		品质
// 	1 			甲
// 	2 			乙
// 	3 			丙
// 	4 			丁
//const std::string empty("");

cocos2d::ccColor3B getFrameColor(unsigned int quality);
cocos2d::CCSprite* getFrameNormalSpirte(unsigned int quality, std::string ext="");
cocos2d::CCSprite* getFrameSelectedSpirte(unsigned int quality, std::string ext="");
cocos2d::CCSprite* getFrameDisabledSpirte(unsigned int quality, std::string ext="");
cocos2d::CCSprite* getContentUnselectedSpirte(std::string ext="");
cocos2d::CCSprite* getContentSelectedSpirte(std::string ext="");

std::string getQualityImageFile(unsigned int quality);

std::string getQualityFrameNormalImageFile(void);
std::string getQualityFrameSelectImageFile(void);

std::string getVipLevelImageFile(unsigned int level);
std::string getTeamDefaultPic(void);
std::string getTeamLockPic(void);

class VaribleManager
	: public ConfigFile
	,public Singleton<VaribleManager>
{
public:
	VaribleManager() {reload();};
	static VaribleManager* getInstance();
    void reload(){load("Varible.txt");};
};
// YUANFEN::type	触发条件		机缘前半句文案生成规则
// 	1				与他人同阵营	与XXX齐上阵，
// 	2				佩戴指定技能	装备XXX（技能），
// 	3				佩戴指定装备	装备YYYY(装备)，
// 
// 	YUANFEN::upgradeType	提升属性	机缘后半句文案生成规则
// 	1						血			生命提高N%
// 	2						攻			攻击提高N%
// 	3						防			防御提高N%
// 	4						内			内力提高N%
class YuanfenItem
{
public:
	
	unsigned int	itemID;
	std::string		name;
	unsigned int	type;
	std::string		TriggerCondition;
	unsigned int	upgradeType;
	unsigned int	upgradeRate;
	std::string		describe;

private:
	void readline(std::stringstream& _stream)
	{
		_stream
			>>itemID
			>>name
			>>type
			>>TriggerCondition
			>>upgradeType
			>>upgradeRate
			>>describe;
	}

	friend class YuanfenTableManager;
};

class YuanfenTableManager
	: public TableReader
	,public Singleton<YuanfenTableManager>
{
public:
	typedef std::map<unsigned int, YuanfenItem* > YuanfenList;
	typedef ConstMapIterator<YuanfenList> YuanfenListIterator;

	YuanfenListIterator getYuanfenIterator(void) 
	{
		return YuanfenListIterator(mYuanfenList.begin(), mYuanfenList.end());
	}

	void init(const std::string& filename);
	const YuanfenItem * getYuanfenItemByID(unsigned int id);
	const YuanfenItem * getYuanfenItemByName(const std::string& name);

	inline unsigned int getTotalNum() {return mYuanfenList.size();};

	static YuanfenTableManager* getInstance();

private:
	virtual void readline(std::stringstream& _stream);

	YuanfenList mYuanfenList;
};

#define MAX_YUANFEN_NUM 6
class DiscipleItem
{
public:
	unsigned int	itemID;
	std::string		name;
	unsigned int	quality;
	unsigned int	type;
	std::string		describe;
	unsigned int	bornSkillItemID;
	unsigned int	initHealth;
	unsigned int	initAttack;
	unsigned int	initDefence;
	unsigned int	initPower;
	unsigned int	initExp;
	unsigned int	yuanfens[MAX_YUANFEN_NUM];
	bool			showEnable;
	std::string		iconPic;
	std::string		bodyPic;
	std::string		namePic;
	std::string		backPic;
	std::string		battlePic;
	std::string		soundUrl;
private:
	void readline(std::stringstream& _stream)
	{
		_stream
			>>itemID
			>>name
			>>quality
			>>type
			>>describe
			>>bornSkillItemID
			>>initHealth
			>>initAttack
			>>initDefence
			>>initPower
			>>initExp
			>>yuanfens[0]
			>>yuanfens[1]
			>>yuanfens[2]
			>>yuanfens[3]
			>>yuanfens[4]
			>>yuanfens[5]
			>>showEnable
			>>iconPic
			>>bodyPic
			>>namePic
			>>backPic
			>>battlePic
			>>soundUrl;
	}

	friend class DiscipleTableManager;
};

class DiscipleTableManager
	: public TableReader
	,public Singleton<DiscipleTableManager>
{
public:
	typedef std::map<unsigned int, DiscipleItem* > DiscipleList;
	typedef ConstMapIterator<DiscipleList> DiscipleListIterator;

	DiscipleListIterator getDiscipleIterator(void) 
	{
		return DiscipleListIterator(mDiscipleList.begin(), mDiscipleList.end());
	}
	DiscipleList getDiscipleList(void){

		return mDiscipleList;
	}

	void init(const std::string& filename);
	const DiscipleItem * getDiscipleItemByID(unsigned int id);
	const DiscipleItem * getDiscipleItemByName(const std::string& name);

	const std::string getYuanfenDescribe(unsigned int id);

	inline unsigned int getTotalNum() {return mDiscipleList.size();};
	unsigned int getRandomDiscipleId();

	static DiscipleTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);

	DiscipleList mDiscipleList;
}; 

// EquipItem::Type	装备类别
// 	1 				武器
// 	2 				防具
// 	3 				饰品
// const std::string Equip1_ImageFile("career/cash.png");
// const std::string Equip2_ImageFile("career/cash.png");
// const std::string Equip3_ImageFile("career/cash.png");
// const std::string& getEquipAtkTypeImageFile(unsigned int type);

const unsigned int EquipQualityNum = 4;
const unsigned int EquipTypeNum = 3;
const unsigned int EquipUpgradeMoneyConsumeFactor[EquipQualityNum][EquipTypeNum] = 
{
	{90,	48,	120},
	{72,	36,	96},
	{54,	24,	72},
	{36,	12,	48},
};

const float EquipQualityFactor[EquipQualityNum] = {5.25f, 4.41f, 3.21f, 1.63f};

class EquipItem
{
public:
	unsigned int		itemID;
	std::string		name;
	unsigned int	quality;
	unsigned int	type;
	std::string		describe;
	unsigned int	initQuantity;
	float			initGrowthRate;
	std::string		iconPic;
	std::string		bodyPic;
	std::string		namePic;

private:
	void readline(std::stringstream& _stream)
	{
		_stream
			>>itemID
			>>name
			>>type
			>>quality
			>>describe
			>>initQuantity
			>>initGrowthRate
			>>iconPic
			>>bodyPic
			>>namePic;
	}

	friend class EquipTableManager;
};

class EquipTableManager
	: public TableReader
	,public Singleton<EquipTableManager>
{
public:
	typedef std::map<unsigned int, EquipItem* > EquipList;
	typedef ConstMapIterator<EquipList> EquipListIterator;

	EquipListIterator getEquipIterator(void) 
	{
		return EquipListIterator(mEquipList.begin(), mEquipList.end());
	}

	void init(const std::string& filename);
	const EquipItem * getEquipItemByID(unsigned int id);
	const EquipItem * getEquipItemByName(const std::string& name);

	inline unsigned int getTotalNum() {return mEquipList.size();};
	unsigned int getRandomEquipId();

	static DiscipleTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);

	EquipList mEquipList;
};

class EquipLevelParam
{
public:
	unsigned int	index;
	float			param;

private:
	void readline(std::stringstream& _stream)
	{
		_stream
			>>index
			>>param;
	}

	friend class EquipLevelParamManager;
};

class EquipLevelParamManager
	: public TableReader
	,public Singleton<EquipLevelParamManager>
{
public:
	typedef std::map<unsigned int, EquipLevelParam* > EquipLevelParamList;
	typedef ConstMapIterator<EquipLevelParamList> EquipLevelParamListIterator;

	EquipLevelParamListIterator getToolIterator(void) 
	{
		return EquipLevelParamListIterator(mEquipLevelParamList.begin(), mEquipLevelParamList.end());
	}

	void init(const std::string& filename);
	const float getParamByLevel(unsigned int level);

	inline unsigned int getTotalNum() {return mEquipLevelParamList.size();};

	static EquipLevelParamManager* getInstance();

private:
	virtual void readline(std::stringstream& _stream);

	EquipLevelParamList mEquipLevelParamList;
};


// SkillItem::type	技能类型
// 	1 				单体攻击
// 	2 				群体攻击
// 	3 				血量BUFF
// 	4 				攻击BUFF
// 	5 				防御BUFF
// 	6 				内力BUFF
// const std::string Skill1_ImageFile("career/cash.png");
// const std::string Skill2_ImageFile("career/cash.png");
// const std::string Skill3_ImageFile("career/cash.png");
// const std::string Skill4_ImageFile("career/cash.png");
// const std::string Skill5_ImageFile("career/cash.png");
// const std::string Skill6_ImageFile("career/cash.png");
// const std::string& getSkillAtkTypeImageFile(unsigned int type);
class SkillItem
{
public:
	typedef std::map<std::string, std::pair<std::string, std::string> > REPLACE_SPRITE_MAP;
	REPLACE_SPRITE_MAP mReplaceMap;
	enum
	{
		TYPE_SINGLE = 1,
		TYPE_MULTIPLE = 2,
		TYPE_BUFF_ATTACK = 4,
		TYPE_BUFF_DEFENSE = 5,
		TYPE_BUFF_POWER = 6
	};
	enum
	{
		QUALITY_BEST = 1,
		QUALITY_GOOD = 2,
		QUALITY_NORMAL = 3,
		QUALITY_BAD = 4
	};
	unsigned int		itemID;
	std::string		name;
	unsigned int	quality;
	unsigned int	type;
	std::string		describe;
	float	initQuantity;
	float	initGrowthRate;
	std::string		iconPic;
	std::string		bodyPic;
	std::string		namePic;
	//std::string		backPic;
	float			duration;
	std::string		ccbFile;
	std::string		timeLine;
	std::string		soundUrl;

private:
	void readline(std::stringstream& _stream);

	friend class SkillTableManager;
};

class SkillTableManager
	: public TableReader
	,public Singleton<SkillTableManager>
{
public:
	typedef std::map<unsigned int, SkillItem* > SkillList;
	typedef ConstMapIterator<SkillList> SkillListIterator;

	SkillListIterator getSkillIterator(void) 
	{
		return SkillListIterator(mSkillList.begin(), mSkillList.end());
	}

	void init(const std::string& filename);
	const SkillItem * getSkillItemByID(unsigned int id);
	const SkillItem * getSkillItemByName(const std::string& name);

	//const std::string getSkillQualityFramePic();
	//const unsigned int getSkillValue();

	inline unsigned int getTotalNum() {return mSkillList.size();};
	unsigned int getRandomSkillId();
	
	static SkillTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);

	SkillList mSkillList;
};

class SkillBookTable
{
public:
	class SkillBookItem
	{
	public:
		unsigned int		itemID;
		unsigned int	skillId;
		std::string		name;
		std::string		iconPic;

	private:
		void readline(std::stringstream& _stream)
		{
			_stream
				>>itemID
				>>skillId
				>>name
				>>iconPic;
		}

		friend class SkillBookTableManager;
	};
	typedef std::map<unsigned int, SkillBookItem* > SkillBookList;
	typedef ConstMapIterator<SkillBookList> SkillBookListIterator;
	SkillBookList mSkillBookItemList;
	inline unsigned int getSkillBookItemSize(){return mSkillBookItemList.size();};
	friend class SkillBookTableManager;
	friend class SkillBookPage;
};

class SkillBookTableManager
	: public TableReader
	,public Singleton<SkillBookTableManager>
{
public:
	
	typedef std::map<unsigned int,SkillBookTable* > SkillBookTableMap;
	typedef ConstMapIterator<SkillBookTableMap > SkillBookInfoMapIterator;

	SkillBookTable::SkillBookListIterator getSkillBookTableIterator(unsigned int id) 
	{
		return SkillBookTable::SkillBookListIterator(mSkillBookList.find(id)->second->mSkillBookItemList.begin(),mSkillBookList.find(id)->second->mSkillBookItemList.end());
	}

	void init(const std::string& filename);
	const SkillBookTable::SkillBookItem * getSkillBookItemByID(unsigned int skillId,unsigned int id);

	inline unsigned int getTotalNum(){return mSkillBookList.size();};

	SkillBookTable* getSkillTableBySkillId(unsigned int skillId)
	{
		if(mSkillBookList.find(skillId)!=mSkillBookList.end())
		{
			return mSkillBookList.find(skillId)->second;
		}
		return NULL;
	}

	static SkillBookTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);

	SkillBookTableMap mSkillBookList;
};

class ToolItem
{
public:
	unsigned int		itemID;
	std::string		name;
	std::string		describe;
	unsigned int	price;
	unsigned int	priceType;
	unsigned int	operationType;
	std::string		jumpPage;
	std::string		buttonString;
	std::string		iconPic;
	std::string		bodyPic;
	std::string		namePic;
	unsigned int	originPrice;
	int				pageID;
	int				buyCountLimit;
	std::string		includeStr;
	ResInfoMap		includeOther;
	unsigned int	isStone;
	unsigned int	showInBag;

private:
	void readline(std::stringstream& _stream);

	friend class ToolTableManager;
};

class ToolTableManager
	: public TableReader
	,public Singleton<ToolTableManager>
{
public:
	typedef std::map<unsigned int, ToolItem* > ToolList;
	typedef ConstMapIterator<ToolList> ToolListIterator;

	ToolListIterator getToolIterator(void) 
	{
		return ToolListIterator(mToolList.begin(), mToolList.end());
	}

	void init(const std::string& filename);
	void initIncludeOther();
	const ToolItem * getToolItemByID(unsigned int id);
	const ToolItem * getToolItemByName(const std::string& name);

	inline unsigned int getTotalNum() {return mToolList.size();};

	static ToolTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);

	ToolList mToolList;
};

class DisciplesLevelParam
{
public:
	unsigned int	index;
	float			param;

private:
	void readline(std::stringstream& _stream)
	{
		_stream
			>>index
			>>param;
	}

	friend class DisciplesLevelParamManager;
};

class DisciplesLevelParamManager
	: public TableReader
	,public Singleton<DisciplesLevelParamManager>
{
public:
	typedef std::map<unsigned int, DisciplesLevelParam* > DisciplesLevelParamList;
	typedef ConstMapIterator<DisciplesLevelParamList> DisciplesLevelParamListIterator;

	DisciplesLevelParamListIterator getToolIterator(void) 
	{
		return DisciplesLevelParamListIterator(mDisciplesLevelParamList.begin(), mDisciplesLevelParamList.end());
	}

	void init(const std::string& filename);
	const float getParamByLevel(unsigned int level);
	const float getTotalParamByLevel(unsigned int level);

	inline unsigned int getTotalNum() {return mDisciplesLevelParamList.size();};

	static DisciplesLevelParamManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);

	DisciplesLevelParamList mDisciplesLevelParamList;
};

class HelpItem
{
public:
	unsigned int		itemID;
	std::string		iconPic;
	std::string		name;
	std::string		describe;

private:
	void readline(std::stringstream& _stream)
	{
		_stream
			>>itemID
			>>name
			>>describe
			>>iconPic;
	}

	friend class HelpTableManager;
};

class HelpTableManager
	: public TableReader
	,public Singleton<HelpTableManager>
{
public:
	typedef std::map<unsigned int, HelpItem* > HelpList;
	typedef ConstMapIterator<HelpList> HelpListIterator;

	HelpListIterator getHelpIterator(void) 
	{
		return HelpListIterator(mHelpList.begin(), mHelpList.end());
	}

	void init(const std::string& filename);
	const HelpItem * getHelpItemByID(unsigned int id);

	inline unsigned int getTotalNum() {return mHelpList.size();};

	static HelpTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);
	HelpList mHelpList;
};

#define MAX_VIP_DESCIBE_NUM 9
class VIPPrivilegeItem{
public:
	unsigned int		itemID;
	std::string		iconPic;
	std::string		name;
	unsigned int	rechangeNum;
	unsigned int	describeNum;
	std::string		describe[MAX_VIP_DESCIBE_NUM];
private:
	void readline(std::stringstream& _stream){
		_stream
			>>itemID
			>>name
			>>iconPic
			>>rechangeNum
			>>describeNum
			>>describe[0]
			>>describe[1]
			>>describe[2]
			>>describe[3]
			>>describe[4]
			>>describe[5]
			>>describe[6]
			>>describe[7]
			>>describe[8];
	}
	friend class VIPPrivilegeTableManager;
};

class VIPPrivilegeTableManager
	: public TableReader
	,public Singleton<VIPPrivilegeTableManager>
{
public:
	typedef std::map<unsigned int,VIPPrivilegeItem* > VIPPrivilegeList;
	typedef ConstMapIterator<VIPPrivilegeList> VIPPrivilegeListIterator;

	VIPPrivilegeListIterator getVIPPrivilegeIterator(void){
		return VIPPrivilegeListIterator(mVIPPrivilegeList.begin(),mVIPPrivilegeList.end());
	}
	void init (const std::string& filename);
	const VIPPrivilegeItem * getVIPPrivilegeItemByID(unsigned int id);
	const VIPPrivilegeItem * getVIPPrivilegeDescribeItemByID(unsigned int id,int describeIndex);

	inline unsigned int getTotalNum(){return mVIPPrivilegeList.size();};

	static VIPPrivilegeTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);
	VIPPrivilegeList mVIPPrivilegeList;

};

class AboutItem
{
public:
	unsigned int		itemID;
	std::string		iconPic;
	std::string		name;
	std::string		describe;

private:
	void readline(std::stringstream& _stream)
	{
		_stream
			>>itemID
			>>name
			>>describe
			>>iconPic;
	}

	friend class AboutTableManager;
};

class AboutTableManager
	: public TableReader
	,public Singleton<AboutTableManager>
{
public:
	typedef std::map<unsigned int, AboutItem* > AboutList;
	typedef ConstMapIterator<AboutList> AboutListIterator;

	AboutListIterator getAboutIterator(void) 
	{
		return AboutListIterator(mAboutList.begin(), mAboutList.end());
	}

	void init(const std::string& filename);
	const AboutItem * getAboutItemByID(unsigned int id);

	inline unsigned int getTotalNum() {return mAboutList.size();};

	static AboutTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);
	AboutList mAboutList;
};

class ContinueLoginItem
{
public:
	unsigned int	id;
	unsigned int	poolIndex;
	unsigned int	itemType;
	unsigned int    itemId;
	unsigned int	quantity;
	unsigned int	rate;
private:
	void readline(std::stringstream& _stream)
	{
		_stream
			>>id
			>>poolIndex
			>>itemType
			>>itemId
			>>quantity
			>>rate;
	}

	friend class ContinueLoginTableManager;
};

class ContinueLoginTableManager
	: public TableReader
	,public Singleton<ContinueLoginTableManager>
{
public:
	typedef std::map<unsigned int, ContinueLoginItem* > ContinueLoginList;
	typedef ConstMapIterator<ContinueLoginList> ContinueListIterator;

	ContinueListIterator getContinueLoginInfoIterator(void) 
	{
		return ContinueListIterator(mContinueLoginList.begin(), mContinueLoginList.end());
	}

	void init(const std::string& filename);
	const ContinueLoginItem * getContinueLoginItemByID(unsigned int id);
	inline unsigned int getTotalNum() {return mContinueLoginList.size();};

	static ContinueLoginTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);
	ContinueLoginList mContinueLoginList;
};

class AdventureItem
{
public:
	unsigned int	itemID;
	std::string		name;
	unsigned int	adventureType;
	std::string		iconPic;
	std::string		ccbi;
	unsigned int	checkVersion;

private:
	void readline(std::stringstream& _stream)
	{
		_stream
			>>itemID
			>>name
			>>adventureType
			>>iconPic
			>>ccbi
			>>checkVersion;
	}

	friend class AdventureTableManager;
};

class AdventureTableManager
	: public TableReader
	,public Singleton<AdventureTableManager>
{
public:
	typedef std::map<unsigned int, AdventureItem* > AdventureList;
	typedef ConstMapIterator<AdventureList> AdventureListIterator;

	AdventureListIterator getAdventureIterator(void) 
	{
		return AdventureListIterator(mAdventureList.begin(), mAdventureList.end());
	}

	void init(const std::string& filename);
	const AdventureItem * getAdventureItemByID(unsigned int id);

	inline unsigned int getTotalNum() {return mAdventureList.size();};

	static AdventureTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);
	AdventureList mAdventureList;
};
class AdventureTeacherItem
{
public:
	unsigned int		itemID;
	std::string		name;
	std::string		iconPic;
	unsigned int needPropNum;
	unsigned int propID;
	

private:
	void readline(std::stringstream& _stream)
	{
		_stream
			>>itemID
			>>name
			>>iconPic
			>>needPropNum
			>>propID;
	}

	friend class AdventureTeacherTableManager;
};
class AdventureTeacherTableManager
	: public TableReader
	,public Singleton<AdventureTeacherTableManager>
{
public:
	typedef std::map<unsigned int, AdventureTeacherItem* > AdventureTeacherList;
	typedef ConstMapIterator<AdventureTeacherList> AdventureTeacherListIterator;

	AdventureTeacherListIterator getAdventureIterator(void)
	{
		return AdventureTeacherListIterator(mAdventureTeacherList.begin(), mAdventureTeacherList.end());
	}

	void init(const std::string& filename);
	const AdventureTeacherItem * getAdventureTeacherByID(unsigned int id);

	inline unsigned int getTotalNum() {return mAdventureTeacherList.size();};

	static AdventureTeacherTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);
	AdventureTeacherList mAdventureTeacherList;
};

class PlayerGradeItem
{
public:
	unsigned int ID;
	unsigned int canDiscipleNum;
	unsigned int maxPower;
	unsigned int maxVitality;
	unsigned int goldReward;
	unsigned int silverReward;
	unsigned int exp;
	unsigned int levelSilverCoins;
private:
	void readLine(std::stringstream& _stream)
	{
		_stream>>ID>>canDiscipleNum>>maxPower>>maxVitality>>goldReward>>silverReward>>exp>>levelSilverCoins;
	}
	friend class PlayerGradeTableManager;
};

class PlayerGradeTableManager
	:public TableReader
	, public Singleton<PlayerGradeTableManager>
{
public:
	typedef std::map<unsigned int, PlayerGradeItem*> GradeItemList;
	typedef ConstMapIterator<GradeItemList> GradeItemListIterator;

	GradeItemListIterator getGradeItemIterator(void)
	{
		return GradeItemListIterator(mGradeItemList.begin(), mGradeItemList.end());
	}

	void init(const std::string& filename);
	const PlayerGradeItem* getGradeItemByID(unsigned int id);

	inline unsigned int getTotalNum(){return mGradeItemList.size();};

	static PlayerGradeTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);
	GradeItemList mGradeItemList;
};


class TitleItem
{
public:
	unsigned int	itemID;
	std::string		name;
	unsigned int	buffType;
	unsigned int	time;
	std::string		levelupCondition;
	std::string		describe;
	std::string		iconPic;
	std::string		namePic;
	unsigned int	quantity;
private:
	void readLine(std::stringstream& _stream)
	{
		_stream
			>>itemID
			>>name
			>>buffType
			>>time
			>>levelupCondition
			>>describe
			>>iconPic
			>>namePic
			>>quantity;
	}
	friend class TitleTableManager;
};

class TitleTableManager
	:public TableReader
	, public Singleton<TitleTableManager>
{
public:
	typedef std::map<unsigned int, TitleItem*> TitleItemList;
	typedef ConstMapIterator<TitleItemList> TitleItemListIterator;

	TitleItemListIterator getTitleItemIterator(void)
	{
		return TitleItemListIterator(mTitleItemList.begin(), mTitleItemList.end());
	}

	void init(const std::string& filename);
	const TitleItem* getTitleItemByID(unsigned int id);

	inline unsigned int getTotalNum(){return mTitleItemList.size();};

	static TitleTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);
	TitleItemList mTitleItemList;
};

class TeamBuffBoldnessItem
{
public:
	unsigned int		quality;
	unsigned int	soulBoldness;
	unsigned int	discipleBoldness;
private:
	void readLine(std::stringstream& _stream)
	{
		_stream
			>>quality			
			>>soulBoldness
			>>discipleBoldness;
	}
	friend class TeamBuffBoldnessTableManager;
};

class TeamBuffBoldnessTableManager
	:public TableReader
	, public Singleton<TeamBuffBoldnessTableManager>
{
public:
	typedef std::map<unsigned int, TeamBuffBoldnessItem*> TeamBuffBoldnessItemList;
	typedef ConstMapIterator<TeamBuffBoldnessItemList> TeamBuffBoldnessItemListIterator;

	TeamBuffBoldnessItemListIterator getTeamBuffBoldnessItemListIterator(void)
	{
		return TeamBuffBoldnessItemListIterator(mTeamBuffBoldnessList.begin(), mTeamBuffBoldnessList.end());
	}

	void init(const std::string& filename);
	const TeamBuffBoldnessItem* getTeamBuffBoldnessItemByQuality(unsigned int quality);

	inline unsigned int getTotalNum(){return mTeamBuffBoldnessList.size();};

	static TeamBuffBoldnessTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);
	TeamBuffBoldnessItemList mTeamBuffBoldnessList;
};

class TeamBuffItem
{
public:
	enum AdditionType
	{
		HP=1,
		ATTACK=2,
		DEFENSE=3,
		WILL=4,
	};
	unsigned int	buffType;
	std::string		name;
	unsigned int	unlockLevel;
	unsigned int	initLevel;
	unsigned int	addition;
	std::string		iconPic;
	std::string		namePic;
private:
	void readLine(std::stringstream& _stream)
	{
		_stream
			>>buffType
			>>name
			>>unlockLevel
			>>initLevel
			>>addition
			>>iconPic
			>>namePic;
	}
	friend class TeamBuffTableManager;
};

class TeamBuffTableManager
	:public TableReader
	, public Singleton<TeamBuffTableManager>
{
public:
	typedef std::map<unsigned int, TeamBuffItem*> TeamBuffItemList;
	typedef ConstMapIterator<TeamBuffItemList> TeamBuffItemListIterator;

	TeamBuffItemListIterator getTeamBuffItemListIterator(void)
	{
		return TeamBuffItemListIterator(mTeamBuffList.begin(), mTeamBuffList.end());
	}

	void init(const std::string& filename);
	const TeamBuffItem* getTeamBuffItemByID(unsigned int id);
	unsigned int getAdditionValueByIdAndLevel(unsigned int id,unsigned int level);
	inline unsigned int getTotalNum(){return mTeamBuffList.size();};
	unsigned int getBoldnessByIdAndLevel(unsigned int id,unsigned int level);

	static TeamBuffTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);
	TeamBuffItemList mTeamBuffList;
};

class AnnouncementItem
{
public:
	unsigned int itemId;
	std::string idName;
	std::string title;
	long int startTime;
	long int endTime;
	std::string timeMsg;
	std::string msg;
	unsigned int isHaveButton;
	std::string buttonText;
	std::string buttonGoPage;
	int btnType;
	unsigned int adventureId;
	unsigned int changeOrPush;

private:
	void readLine(std::stringstream& _stream)
	{
		_stream>>itemId>>idName>>title>>startTime>>endTime>>timeMsg>>msg>>isHaveButton>>buttonText>>buttonGoPage>>btnType>>adventureId>>changeOrPush;
	}

	friend class AnnouncementTableManager;
};

class AnnouncementTableManager
	: public TableReader
	, public Singleton<AnnouncementTableManager>
{
public:
	typedef std::map<unsigned int, AnnouncementItem*> AnnouncementItemList;
	typedef ConstMapIterator<AnnouncementItemList> AnnouncementItemListIterator;

	AnnouncementItemListIterator getAnnouncementItemListIterator(void)
	{
		return AnnouncementItemListIterator(mAnnouncementItemList.begin(), mAnnouncementItemList.end());
	}

	void init(const std::string& fileName);

	const AnnouncementItem* getAnnouncementItemById(unsigned int id);

	AnnouncementItemList getAnnouncementItemList(){return mAnnouncementItemList;};

	inline unsigned int getTotalNum(){return mAnnouncementItemList.size();};

	static AnnouncementTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);
	AnnouncementItemList mAnnouncementItemList;
};

class UserPropertyItem
{
public:
	unsigned int	itemID;
	std::string	name;
	std::string	describe;
	std::string	iconPic;

private:
	void readline(std::stringstream& _stream)
	{
		_stream
			>>itemID
			>>name
			>>describe
			>>iconPic;
	}

	friend class UserPropertyTableManager;
};
class UserPropertyTableManager
	: public TableReader
	,public Singleton<UserPropertyTableManager>
{
public:
	typedef std::map<unsigned int, UserPropertyItem* > UserPropertyList;
	typedef ConstMapIterator<UserPropertyList> UserPropertyListIterator;

	UserPropertyListIterator getUserPropertyListIterator(void) 
	{
		return UserPropertyListIterator(mUserPropertyList.begin(), mUserPropertyList.end());
	}

	void init(const std::string& filename);
	const UserPropertyItem * getUserPropertyItemByID(unsigned int id);

	inline unsigned int getTotalNum() {return mUserPropertyList.size();};

	static UserPropertyTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);
	UserPropertyList mUserPropertyList;
};


class NewActivePromptItem
{
public:
	unsigned int	itemID;
	unsigned int	type;
	int	level;
	std::string	ccbiPath;
	std::string	pageName;
	std::string	dropType;
	int	dropId;

private:
	void readline(std::stringstream& _stream)
	{
		_stream
			>>itemID
			>>type
			>>level
			>>ccbiPath
			>>pageName
			>>dropType
			>>dropId;
	}

	friend class NewActivePromptTableManager;
};
class NewActivePromptTableManager
	: public TableReader
	,public Singleton<NewActivePromptTableManager>
{
public:
	typedef std::map<unsigned int, NewActivePromptItem* > NewActivePromptList;
	typedef ConstMapIterator<NewActivePromptList> NewActivePromptItemListIterator;

	NewActivePromptItemListIterator getNewActivePromptListIterator(void) 
	{
		return NewActivePromptItemListIterator(mNewActivePromptList.begin(), mNewActivePromptList.end());
	}

	NewActivePromptList * getNewAcrivePromptItemByLevel(unsigned int id,unsigned int type = 1);
	NewActivePromptList * getNewAcrivePromptItemByDropId(unsigned int id,unsigned int type = 2);
	void init(const std::string& filename);
	const NewActivePromptItem * getNewActivePromptItemByID(unsigned int id);

	inline unsigned int getTotalNum() {return mNewActivePromptList.size();};

	static NewActivePromptTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);
	NewActivePromptList mNewActivePromptList;
};


class ArenaRewardItem
{
public:
	unsigned int	itemID;
	unsigned int	type;
	std::string		title;
	unsigned int	needScore;
	unsigned int	rewardNum;
	std::string		describe;
	std::string		iconPic;

private:
	void readline(std::stringstream& _stream)
	{
		_stream
			>>itemID
			>>type
			>>title
			>>needScore
			>>rewardNum
			>>describe
			>>iconPic;
	}

	friend class ArenaRewardTableManager;
};
class ArenaRewardTableManager
	: public TableReader
	,public Singleton<ArenaRewardTableManager>
{
public:
	typedef std::map<unsigned int, ArenaRewardItem* > ArenaRewardList;
	typedef ConstMapIterator<ArenaRewardList> ArenaRewardListIterator;

	ArenaRewardListIterator getArenaListIterator(void) 
	{
		return ArenaRewardListIterator(mArenaRewardList.begin(), mArenaRewardList.end());
	}

	void init(const std::string& filename);
	const ArenaRewardItem * getArenaRewardItemByID(unsigned int id);

	inline unsigned int getTotalNum() {return mArenaRewardList.size();};

	static ArenaRewardTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);
	ArenaRewardList mArenaRewardList;
};

class FightEndRewardItem
{
public:
	unsigned int	itemID;
	unsigned int	battle5;
	unsigned int	battle6;
	unsigned int	battle7;
	unsigned int	battle8;
	std::string		rewardInfo;

private:
	void readline(std::stringstream& _stream)
	{
		_stream
			>>itemID
			>>battle5
			>>battle6
			>>battle7
			>>battle8
			>>rewardInfo;
	}

	friend class FightEndRewardTableManager;
};
class FightEndRewardTableManager
	: public TableReader
	,public Singleton<FightEndRewardTableManager>
{
public:
	typedef std::map<unsigned int, FightEndRewardItem* > FightEndRewardList;
	typedef ConstMapIterator<FightEndRewardList> FightEndRewardIterator;

	FightEndRewardIterator getFightEndRewardIterator(void) 
	{
		return FightEndRewardIterator(mFightEndRewardList.begin(), mFightEndRewardList.end());
	}

	void init(const std::string& filename);
	const FightEndRewardItem * getFightEndRewardItemByID(unsigned int id);

	inline unsigned int getTotalNum() {return mFightEndRewardList.size();};

	static FightEndRewardTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);
	FightEndRewardList mFightEndRewardList;
};

class AdventureContinueLoginItem
{
public:
	unsigned int	itemID;
	unsigned int	dayCount;
	std::string		rewardInfo;

private:
	void readline(std::stringstream& _stream)
	{
		_stream
			>>itemID
			>>dayCount
			>>rewardInfo;
	}

	friend class AdventureContinueLoginTableManager;
};
class AdventureContinueLoginTableManager
	: public TableReader
	,public Singleton<AdventureContinueLoginTableManager>
{
public:
	typedef std::map<unsigned int, AdventureContinueLoginItem* > AdventureContinueLoginList;
	typedef ConstMapIterator<AdventureContinueLoginList> AdventureContinueLoginIterator;

	AdventureContinueLoginIterator getAdventureContinueLoginIterator(void) 
	{
		return AdventureContinueLoginIterator(mAdventureContinueLoginList.begin(), mAdventureContinueLoginList.end());
	}

	void init(const std::string& filename);
	const AdventureContinueLoginItem * getAdventureContinueLoginItemByID(unsigned int id);

	inline unsigned int getTotalNum() {return mAdventureContinueLoginList.size();};

	static AdventureContinueLoginTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);
	AdventureContinueLoginList mAdventureContinueLoginList;
};

class AdventureInviteItem
{
public:
	unsigned int	itemID;
	unsigned int	requireCount;
	std::string		describe;
	std::string		rewardInfo;

private:
	void readline(std::stringstream& _stream)
	{
		_stream
			>>itemID
			>>requireCount
			>>describe
			>>rewardInfo;
	}

	friend class AdventureInviteTableManager;
};
class AdventureInviteTableManager
	: public TableReader
	,public Singleton<AdventureInviteTableManager>
{
public:
	typedef std::map<unsigned int, AdventureInviteItem* > AdventureInviteList;
	typedef ConstMapIterator<AdventureInviteList> AdventureInviteIterator;

	AdventureInviteIterator getAdventureContinueLoginIterator(void) 
	{
		return AdventureInviteIterator(mAdventureInviteList.begin(), mAdventureInviteList.end());
	}

	void init(const std::string& filename);
	const AdventureInviteItem * getAdventureInviteItemByID(unsigned int id);

	inline unsigned int getTotalNum() {return mAdventureInviteList.size();};

	static AdventureInviteTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);
	AdventureInviteList mAdventureInviteList;
};

class FortuneComeRewardItem
{
public:
	unsigned int	itemID;
	std::string		name;
	std::string		rewardGold;

private:
	void readline(std::stringstream& _stream)
	{
		_stream
			>>itemID
			>>name
			>>rewardGold;
	}

	friend class FortuneComeRewardTableManager;
};
class FortuneComeRewardTableManager
	: public TableReader
	,public Singleton<FortuneComeRewardTableManager>
{
public:
	typedef std::map<unsigned int, FortuneComeRewardItem* > FortuneComeRewardList;
	typedef ConstMapIterator<FortuneComeRewardList> FortuneComeRewardIterator;

	FortuneComeRewardIterator getFortuneComeRewardIterator(void) 
	{
		return FortuneComeRewardIterator(mFortuneComeRewardList.begin(), mFortuneComeRewardList.end());
	}

	void init(const std::string& filename);
	const FortuneComeRewardItem * getFortuneComeRewardItemByID(unsigned int id);

	inline unsigned int getTotalNum() {return mFortuneComeRewardList.size();};

	static FortuneComeRewardTableManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);
	FortuneComeRewardList mFortuneComeRewardList;
};

class StoneItem
{
public:
	unsigned int	id;
	std::string		type;
	std::string		iconPic;
	std::string		bigIconPic;
	unsigned int	color;
	unsigned int	attackBuff;
	unsigned int	defendBuff;
	unsigned int	bloodBuff;
	unsigned int	upgradeSilver;
	unsigned int	upgradeNum;
	unsigned int	needToolNum;
	unsigned int	level;
	unsigned int	nextLevelID;
	unsigned int	status;
	std::string	statusSign;
private:
	void readline(std::stringstream& _stream)
	{
		_stream
			>>id
			>>type
			>>iconPic
			>>bigIconPic
			>>color
			>>attackBuff
			>>defendBuff
			>>bloodBuff
			>>upgradeSilver
			>>upgradeNum
			>>needToolNum
			>>level
			>>nextLevelID
			>>status
			>>statusSign;
	}

	friend class StoneTableManager;
};

class StoneTableManager
	: public TableReader
	,public Singleton<StoneTableManager>
{
public:
	typedef std::map<unsigned int,StoneItem* > StoneItemList;
	typedef ConstMapIterator<StoneItemList> StoneItermIterator;
	StoneItermIterator getStoneItemIterator(void){
		return StoneItermIterator(mStoneItemList.begin(),mStoneItemList.end());
	}
	StoneItermIterator getStoneColorItemIterator(void){
		return StoneItermIterator(mStoneColorItemList.begin(),mStoneColorItemList.end());
	}
	StoneItermIterator getStoneStatusItemIterator(void){
		return StoneItermIterator(mStoneStatusItemList.begin(),mStoneStatusItemList.end());
	}
	void init(const std::string& filename);
	const StoneItem * getStoneItemByID(unsigned int id);
	const StoneItem * getStoneColorItemByID(unsigned int id);
	const StoneItem * getStoneStatusItemByID(unsigned int id);

	const StoneItem * getStoneItemBySign(std::string statusSign);
	const StoneItem * getStoneColorItemBySign(std::string statusSign);
	const StoneItem * getStoneStatusItemBySign(std::string statusSign);

	const StoneItem * getStoneItemByStatus(unsigned int status);
	const StoneItem * getStoneColorItemByStatus(unsigned int status);
	const StoneItem * getStoneStatusItemByStatus(unsigned int status);

	const StoneItem * getStoneHoleByID(unsigned int id);

	inline unsigned int getTotalNum(){return mStoneItemList.size();};

	static StoneTableManager* getInstance();

private:
	virtual void readline(std::stringstream& _stream);
	StoneItemList mStoneItemList;
	StoneItemList mStoneColorItemList;
	StoneItemList mStoneStatusItemList;
	StoneItemList mStoneHoleList;

};

class VipDataItem
{
public:
	unsigned int	itemID;
	std::string		vipLevel;
	unsigned int	friendNumLimit;
	unsigned int	enemyNumLimit;
private:
	void readline(std::stringstream& _stream)
	{
		_stream
			>>itemID
			>>vipLevel
			>>friendNumLimit
			>>enemyNumLimit;
	}

	friend class VipDataTableManager;
};
class VipDataTableManager
	: public TableReader
	,public Singleton<VipDataTableManager>
{
public:
	typedef std::map<unsigned int, VipDataItem* > VipDataList;

	void init(const std::string& filename);
	const VipDataItem * getVipDataByID(unsigned int id);

	inline unsigned int getTotalNum() {return mVipDataList.size();};
private:
	virtual void readline(std::stringstream& _stream);
	VipDataList mVipDataList;
};

class PlatformNameManager
	: public TableReader
	,public Singleton<PlatformNameManager>
{
public:
	typedef std::map<unsigned int, std::string> PlatformNameList;	

	void init(const std::string& filename);
	const std::string getPlatNameByID(unsigned int id);

	static PlatformNameManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);
	PlatformNameList mPlatformList;
};

class BBSConfigManager
	: public TableReader
	,public Singleton<BBSConfigManager>
{
public:
	typedef std::map<unsigned int, unsigned int> PlatBBSList;	

	void init(const std::string& filename);
	bool getBBSOpenStatusByID(unsigned int id);

	static BBSConfigManager* getInstance();
private:
	virtual void readline(std::stringstream& _stream);
	PlatBBSList mPlatformBBSList;
};

class ActivityPopItem
{
public:
	unsigned int	activityID;
	std::string		name;
	unsigned int	referenceID;
	std::string	describe;
private:
	void readline(std::stringstream& _stream)
	{
		_stream
			>>activityID
			>>name
			>>referenceID
			>>describe;
	}
	friend class ActivityPopTableManager;
};
class ActivityPopTableManager
	: public TableReader
	,public Singleton<ActivityPopTableManager>
{
public:
	typedef std::map<unsigned int, ActivityPopItem* > ActivityPopList;

	void init(const std::string& filename);
	const ActivityPopItem * getActivityPopDataByID(unsigned int id);

	inline unsigned int getTotalNum() {return mActivityPopList.size();};
private:
	virtual void readline(std::stringstream& _stream);
	ActivityPopList mActivityPopList;
};