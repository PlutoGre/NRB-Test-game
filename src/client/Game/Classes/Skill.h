#pragma once
#include "Singleton.h"
#include "cocos2d.h"

#include <string>
#include <list>

class SkillInfo;
class SkillItem;

class Skill
{
public:
	Skill(unsigned int id, bool isItemID = false,bool isViewTeam=false);
	bool mIsViewTeam;
	unsigned int	id();
	unsigned int	itemID();
	const std::string& name();
	unsigned int	quality();
	unsigned int	type();
	std::string describe();
	unsigned int	initQuantity();
	unsigned int	initGrowthRate();
	std::string iconPic();
	std::string bodyPic();
	std::string namePic();
	std::string quantityStr();

	unsigned int	level();
	float quantity();
	unsigned int	consume();
	bool			isBornSkill();

	cocos2d::ccColor3B getFrameColor();
	cocos2d::CCSprite* getFrameNormalSpirte(std::string ext="");
	cocos2d::CCSprite* getFrameSelectedSpirte(std::string ext="");
	cocos2d::CCSprite* getFrameDisabledSpirte(std::string ext="");
	std::string getQualityImageFile();

	std::string getSkillAtkTypePic();

	bool isSkillCarriedBy();
	std::string getSkillCarriedBy();

	unsigned int getSkillInitValue();//初始身价
	unsigned int getSkillValue();//身价
	std::string getSkillAtkType();
	unsigned int getQualityFactor();//品质系数

	unsigned int getConsumeExp();//能提供的经验值
	unsigned int getUpgradeExp();//升级所需经验

	float getUpgradeRatio(unsigned int consume);//升级比例系数
	float desplayUpgradeRatio(unsigned int consume);//升级显示比例系数

	std::string getCurUpgradeRatioString();//升级显示比例系数
	
	bool allowUpgrade();

	float getConsumeRateValue();
	unsigned int getConsumeMoneyNeeded();

	bool isHavInfo();
	bool isHavItem();
private:
	const SkillInfo*		mSkillInfo;
	const SkillItem*		mSkillItem;
};

typedef bool (*SkillFilter)(unsigned int);
typedef bool (*SkillSort)(unsigned int, unsigned int);

bool skillFilter_default(unsigned int id);
bool SkillSort_default(unsigned int id1, unsigned int id2);

class SkillDateCollect : public Singleton<SkillDateCollect>
{
public:
	typedef std::list<unsigned int> SkillIDCollect;
	SkillIDCollect getSkillCollection(SkillFilter sFilter=skillFilter_default, SkillSort sSort=SkillSort_default);
};