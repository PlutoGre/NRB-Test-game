#pragma once
#include "Singleton.h"
#include "cocos2d.h"
#include "json/json.h"

#include <string>
#include <list>

class UserEquipInfo;
class EquipItem;

class Equip
{
public:
	Equip(unsigned int id, bool isItemID = false,bool isViewTeam=false);
	bool mIsViewTeam;
	unsigned int	id();
	unsigned int	itemID();
	const std::string& name();
	unsigned int	quality();
	unsigned int	type();
	std::string describe(std::string lengthStr="");
	unsigned int	initQuantity();
	float	initGrowthRate();
	std::string iconPic();
	std::string bodyPic();
	std::string namePic();

	unsigned int	level();
	unsigned int	refinexp();
	unsigned int	refinelevel();
	unsigned int	propertyValue(unsigned int type);
	unsigned int	quantity();
	unsigned int getTotalProperty();
	unsigned int	stoneColor(unsigned int stonePosition);
	unsigned int	stoneStatus(unsigned int stonePosition);
	unsigned int	stoneId(unsigned int stonePosition);
	unsigned int	stoneNum();
	unsigned int	stoneBuffType();
	std::string		stoneBuffValue();

	cocos2d::ccColor3B getFrameColor();
	cocos2d::CCSprite* getFrameNormalSpirte(std::string ext="");
	cocos2d::CCSprite* getFrameSelectedSpirte(std::string ext="");
	cocos2d::CCSprite* getFrameDisabledSpirte(std::string ext="");
	std::string getQualityImageFile();

	std::string getEquipAtkTypePic();
	
	bool isEquipCarriedBy();
	std::string getEquipCarriedBy();

	float			getEquipInitValue();//初始身价
	float			getEquipGrowthValue();//成长身价
	unsigned int	getEquipValue();//身价

	float getQualityFactor();//品质系数

	bool allowUpgrade();
	bool allowLevelUpgrade();

	unsigned int getUpgradeMoneyConsumeFactor();

	unsigned int getUpgradeMoneyConsumeLevelFactor();

	unsigned int getConsumeMoneyNeeded();
private:
	const UserEquipInfo*	mEquipInfo;
	const EquipItem*		mEquipItem;
};

typedef bool (*EquipFilter)(unsigned int);
typedef bool (*EquipSort)(unsigned int, unsigned int);

bool EquipFilter_default(unsigned int id);
bool EquipFilter_Type1(unsigned int id);
bool EquipFilter_Type2(unsigned int id);
bool EquipFilter_Type3(unsigned int id);

bool EquipFilter_default(unsigned int id);


bool EquipSort_default(unsigned int id1, unsigned int id2);

class EquipDateCollect : public Singleton<EquipDateCollect>
{
public:
	typedef std::list<unsigned int> EquipIDCollect;
	EquipIDCollect getEquipCollection(EquipFilter sFilter=EquipFilter_default, EquipSort sSort=EquipSort_default);
};