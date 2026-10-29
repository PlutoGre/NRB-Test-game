#pragma once
#include "Singleton.h"
#include "cocos2d.h"

#include <string>
#include <list>

namespace cocos2d
{
	class CCSprite;
};

class UserDiscipleInfo;
class UserSoulInfo;
class DiscipleItem;

class Disciple
{
public:
	Disciple();
	Disciple(unsigned int id, bool isItemID = false,bool isViewTeam=false);

	void init(unsigned int id, bool isItemID = false,bool isViewTeam=false);
	
	bool mIsViewTeam;
	unsigned int id();
	unsigned int level();
	unsigned int exp();
	unsigned int health();
	unsigned int attack();
	unsigned int defence();
	unsigned int rp();
	unsigned int upgradelevel();
	unsigned int potentiality();
	unsigned int skill1();
	unsigned int battleid();

	unsigned int		itemID();
	std::string			name();
	unsigned int		quality();
	unsigned int		type();
	std::string			describe();
	unsigned int		bornSkillItemID();
	unsigned int		initHealth();
	unsigned int		initAttack();
	unsigned int		initDefence();
	unsigned int		initPower();
	unsigned int		initExp();
	unsigned int		yuanfens(unsigned int index);
	std::string			iconPic();
	std::string			bodyPic();
	std::string			namePic();
	std::string			backPic();

	bool isInBattle();

	unsigned int getLevelExp(); //当前等级已有经验多获得的exp

	float			getDiscipleInitValueFactor();//初始身价系数
	float			getDiscipleUpgradeValueFactor();//成长身价系数
	float			getDiscipleInitValue();//初始身价
	unsigned int	getDiscipleValue();//身价
	float			getDiscipleInitTotalProperty();//初始总属性值
	float			getDiscipleTotalProperty();//当前总属性值

	//unsigned int getTotalExp();//散功能提供的经验值
	// int getTotalExpByLevel(unsigned int level);//到level需要的总经验

	unsigned int getLevelupNeededExp();//升级所需经验值
	unsigned int getProviderExp();//散功能提供的经验值

	unsigned int getExpForLevel( unsigned int level );//到level需要的总经验
	//unsigned int getExpForCurLevel();//弟子当前级别需要的经验值

	unsigned int getExpForLevelUpgrade( unsigned int level );//level级别需要的经验

	unsigned int getUpgradeLevel(unsigned int provideExp);//获取能升到的级别

	cocos2d::ccColor3B getFrameColor();

	cocos2d::CCSprite* getFrameNormalSpirte(std::string ext="");
	cocos2d::CCSprite* getFrameSelectedSpirte(std::string ext="");
	cocos2d::CCSprite* getFrameDisabledSpirte(std::string ext="");
	std::string getQualityImageFile(bool isLarge=false);
	std::string getQualityBgImageFile();

	const std::string getYuanfenDescribe(void);

	const std::string getYuanfenName(void);

// 	std::string		getBornSkillName();
// 	std::string		getBornSkillPicPath();

	bool hasDiscipleInfo(){return (mDiscipleInfo!=0);}

	bool canUpgradeLevel();
private:
	const UserDiscipleInfo*		mDiscipleInfo;
	const DiscipleItem*			mDiscipleItem;
};

typedef bool (*DiscipleFilter)(unsigned int);
typedef bool (*DiscipleSort)(unsigned int, unsigned int);

bool DiscipleFilter_default(unsigned int id);
bool DiscipleSort_default(unsigned int id1, unsigned int id2);
bool DiscipleSort_qualityASC(unsigned int id1, unsigned int id2);
bool SoulFilter_default(unsigned int id);
bool SoulSort_default(unsigned int id1, unsigned int id2);
bool SoulSort_qualityASC(unsigned int id1, unsigned int id2);
bool SoulSort_recruitUpgrade(unsigned int id1, unsigned int id2);
class DiscipleDateCollect : public Singleton<DiscipleDateCollect>
{
public:
	typedef std::list<unsigned int> DiscipleIDCollect;
	DiscipleIDCollect getDiscipleCollection(DiscipleFilter sFilter=DiscipleFilter_default, DiscipleSort sSort=DiscipleSort_default);
	DiscipleIDCollect getSoulCollection(DiscipleFilter sFilter=SoulFilter_default, DiscipleSort sSort=SoulSort_default);
};

class Soul
{
public:
	Soul(unsigned int id);

	unsigned int id();
	unsigned int count();

	unsigned int		itemID();
	std::string			name();
	unsigned int		quality();
	unsigned int		type();
	std::string			describe();
	unsigned int		bornSkillItemID();
	unsigned int		initHealth();
	unsigned int		initAttack();
	unsigned int		initDefence();
	unsigned int		initPower();
	unsigned int		initExp();
	unsigned int		yuanfens(unsigned int index);
	std::string			iconPic();
	std::string			bodyPic();
	std::string			namePic();
	std::string			backPic();

	bool canRecruit();
	bool canUpgrade();
	unsigned int getNeededNum();
	unsigned int getGapNum();
	std::string getOprText();
	std::string getNumText();

	cocos2d::CCSprite* getFrameNormalSpirte(std::string ext="");
	cocos2d::CCSprite* getFrameSelectedSpirte(std::string ext="");
	cocos2d::CCSprite* getFrameDisabledSpirte(std::string ext="");
	std::string getQualityImageFile();

	float			getSoulInitValueFactor();
	float			getSoulInitTotalProperty();
	unsigned int	getSoulValue();//身价

	unsigned int getDiscipleID();

	const std::string getYuanfenDescribe(void);

private:
	const UserSoulInfo*			mSoulInfo;
	const DiscipleItem*			mDiscipleItem;
};