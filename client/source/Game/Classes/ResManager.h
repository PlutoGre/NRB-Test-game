#pragma once
#include <string>
#include <map>
#include "IteratorWrapper.h"
#include "Singleton.h"

enum ResDataTypeEnum
{
	USER_PROPERTY_TYPE = 10,
	USER_PROPERTY = 11001,
	USER_PROPERTY_GOLD_COINS = 1001,
	USER_PROPERTY_SILVER_COINS = 1002,
	USER_PROPERTY_EXP = 1003,
	USER_LEVEL_PROPERTY = 12001,
	USER_PROPERTY_LEVEL_SILVER_COINS = 2002,
	DISCIPLE_PROPERTY=21001,
	DISCIPLE_TYPE=31,
	A_DISCIPLE=31001,
	B_DISCIPLE=31002,
	C_DISCIPLE=31003,
	D_DISCIPLE=31004,
	SOUL_TYPE=32,
	A_SOUL=32001,
	B_SOUL=32002,
	C_SOUL=32003,
	D_SOUL=32004,
	SKILL_TYPE=41,
	A_SKILL=41001,
	B_SKILL=41002,
	C_SKILL=41003,
	D_SKILL=41004,
	SKILL_PIECE_TYPE=42,
	SKILL_PIECE=42000,
	A_SKILL_PIECE=42001,
	B_SKILL_PIECE=42002,
	C_SKILL_PIECE=42003,
	D_SKILL_PIECE=42004,
	EQUIP_TYPE=50,
	A_WEAPON=51001,
	B_WEAPON=51002,
	C_WEAPON=51003,
	D_WEAPON=51004,
	A_ARMOR=52001,
	B_ARMOR=52002,
	C_ARMOR=52003,
	D_ARMOR=52004,
	A_ACCESSORIES=53001,
	B_ACCESSORIES=53002,
	C_ACCESSORIES=53003,
	D_ACCESSORIES=53004,
	TOOLS_TYPE=60,
	USER_TOOLS=61001,
	INTERFACE_TYPE=70,
	USER_INTERFACE=71001,
	WOOD_TREASURE=3001001,
	COPPER_TREASURE=3001002,
	SILVER_TREASURE=3001003,
	GOLD_TREASURE=3001004,
	WOOD_KEY=3002001,
	COPPER_KEY=3002002,
	SILVER_KEY=3002003,
	GOLD_KEY=3002004,
	HAIXIANFAN=2501001,
	JISUZHEN=2502001,
	TIAOZHANSHU=2503001,
	GIFTBOXPAGE=2,
	YINBEI=1101002,
	PUNCHHOLE=5002001,
	REMOVESTONE=5002002,
	WINE_TYPE = 70,
	WINE_LANGMU = 1501001,
	WINE_PUTAO = 1501002,

};
#define VIP_MAX_LEVEL 13
struct ResInfo
{
	unsigned int id;
	std::string name;
	std::string describe;
	unsigned int quality;
	std::string icon;
	unsigned int count;
	unsigned int itemId;
	unsigned int type;
	unsigned int partId;
};
typedef std::map<unsigned int,ResInfo* > ResInfoMap;
typedef ConstMapIterator<ResInfoMap > ResInfoMapIterator;
typedef std::map<unsigned int,unsigned int > ResMap;
class ResManager : public Singleton<ResManager>
{
public:
	 ResManager()
	{
		mResList.insert(ResMap::value_type(USER_PROPERTY,USER_PROPERTY_TYPE));
		mResList.insert(ResMap::value_type(USER_LEVEL_PROPERTY,USER_PROPERTY_TYPE));
		mResList.insert(ResMap::value_type(A_DISCIPLE,DISCIPLE_TYPE));
		mResList.insert(ResMap::value_type(B_DISCIPLE,DISCIPLE_TYPE));
		mResList.insert(ResMap::value_type(C_DISCIPLE,DISCIPLE_TYPE));
		mResList.insert(ResMap::value_type(D_DISCIPLE,DISCIPLE_TYPE));
		mResList.insert(ResMap::value_type(A_SOUL,SOUL_TYPE));
		mResList.insert(ResMap::value_type(B_SOUL,SOUL_TYPE));
		mResList.insert(ResMap::value_type(C_SOUL,SOUL_TYPE));
		mResList.insert(ResMap::value_type(D_SOUL,SOUL_TYPE));
		mResList.insert(ResMap::value_type(A_SKILL,SKILL_TYPE));
		mResList.insert(ResMap::value_type(B_SKILL,SKILL_TYPE));
		mResList.insert(ResMap::value_type(C_SKILL,SKILL_TYPE));
		mResList.insert(ResMap::value_type(D_SKILL,SKILL_TYPE));
		mResList.insert(ResMap::value_type(SKILL_PIECE,SKILL_PIECE_TYPE));
		mResList.insert(ResMap::value_type(A_SKILL_PIECE,SKILL_PIECE_TYPE));
		mResList.insert(ResMap::value_type(A_SKILL_PIECE,SKILL_PIECE_TYPE));
		mResList.insert(ResMap::value_type(A_SKILL_PIECE,SKILL_PIECE_TYPE));
		mResList.insert(ResMap::value_type(A_SKILL_PIECE,SKILL_PIECE_TYPE));
		mResList.insert(ResMap::value_type(A_WEAPON,EQUIP_TYPE));
		mResList.insert(ResMap::value_type(B_WEAPON,EQUIP_TYPE));
		mResList.insert(ResMap::value_type(C_WEAPON,EQUIP_TYPE));
		mResList.insert(ResMap::value_type(D_WEAPON,EQUIP_TYPE));
		mResList.insert(ResMap::value_type(A_ARMOR,EQUIP_TYPE));
		mResList.insert(ResMap::value_type(B_ARMOR,EQUIP_TYPE));
		mResList.insert(ResMap::value_type(C_ARMOR,EQUIP_TYPE));
		mResList.insert(ResMap::value_type(D_ARMOR,EQUIP_TYPE));
		mResList.insert(ResMap::value_type(A_ACCESSORIES,EQUIP_TYPE));
		mResList.insert(ResMap::value_type(B_ACCESSORIES,EQUIP_TYPE));
		mResList.insert(ResMap::value_type(C_ACCESSORIES,EQUIP_TYPE));
		mResList.insert(ResMap::value_type(D_ACCESSORIES,EQUIP_TYPE));
		mResList.insert(ResMap::value_type(USER_TOOLS,TOOLS_TYPE));
		mResList.insert(ResMap::value_type(USER_INTERFACE,INTERFACE_TYPE));
		mResList.insert(ResMap::value_type(WINE_LANGMU, WINE_TYPE));
		mResList.insert(ResMap::value_type(WINE_PUTAO, WINE_TYPE));
		mPairList.insert(ResMap::value_type(WOOD_TREASURE,WOOD_KEY));
		mPairList.insert(ResMap::value_type(COPPER_TREASURE,COPPER_KEY));
		mPairList.insert(ResMap::value_type(SILVER_TREASURE,SILVER_KEY));
		mPairList.insert(ResMap::value_type(GOLD_TREASURE,GOLD_KEY));
		mPairList.insert(ResMap::value_type(WOOD_KEY,WOOD_TREASURE));
		mPairList.insert(ResMap::value_type(COPPER_KEY,COPPER_TREASURE));
		mPairList.insert(ResMap::value_type(SILVER_KEY,SILVER_TREASURE));
		mPairList.insert(ResMap::value_type(GOLD_KEY,GOLD_TREASURE));
		mVipDailyLimitList.insert(ResMap::value_type(0,1));
		mVipDailyLimitList.insert(ResMap::value_type(1,1));
		mVipDailyLimitList.insert(ResMap::value_type(2,2));
		mVipDailyLimitList.insert(ResMap::value_type(3,3));
		mVipDailyLimitList.insert(ResMap::value_type(4,3));
		mVipDailyLimitList.insert(ResMap::value_type(5,5));
		mVipDailyLimitList.insert(ResMap::value_type(6,5));
		mVipDailyLimitList.insert(ResMap::value_type(7,15));
		mVipDailyLimitList.insert(ResMap::value_type(8,15));
		mVipDailyLimitList.insert(ResMap::value_type(9,25));
		mVipDailyLimitList.insert(ResMap::value_type(10,25));
		mVipDailyLimitList.insert(ResMap::value_type(11,25));
		mVipDailyLimitList.insert(ResMap::value_type(12,25));
		mVipDailyLimitList.insert(ResMap::value_type(13,25));
	}
	static ResManager* getInstance(){return ResManager::Get();}
	ResInfo* getResInfoByTypeAndId(unsigned int type,unsigned int Id,unsigned int count,unsigned int partId=0);
	void setResInfo(ResInfo* info);
	ResInfoMap getResInfoListByResBagStr(const std::string& ResBagStr);
	//method for lua iterator
	int getResInfoListByResBagStrCount(const std::string& ResBagStr);
	ResInfo*  getResInfoListByResBagStrByIndex(const std::string& ResBagStr,int index);

	const unsigned int getTreasurePairType(unsigned int treasureId);
	const unsigned int getResMainType(unsigned int resId);
	const unsigned int getVipDailyLimit(unsigned int vipLevel);
	const unsigned int getNextDailyLimitVipLevel(unsigned int vipLevel);
	void useToolByIDAndCount(unsigned int toolId,unsigned int toolItemId,unsigned int toolCount);
private:
	ResMap mResList;
	ResMap mPairList;
	ResMap mVipDailyLimitList;
};