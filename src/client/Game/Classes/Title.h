#pragma once
#include "Singleton.h"
#include "cocos2d.h"

#include <string>
#include <list>

class TitleInfo;
class TitleItem;

class Title
{
public:
	static unsigned int getTypeByItemID(unsigned int itemid); 
	static unsigned int getItemIDByType( unsigned int itemid, unsigned int level = 1 );
// 	enum ID_TYPE
// 	{
// 		T_ID,
// 		T_ITEMID,
// 		T_ITEMTYPE,
// 	};

	Title(unsigned int id/*, ID_TYPE type = T_ITEMTYPE*/,bool isViewTeam=false);
	bool mIsViewTeam;
	unsigned int		id();
	unsigned int		itemID();
	unsigned int		startime();

	const std::string&	name();
	unsigned int		buffType();
	unsigned int		time();
	std::string			levelupCondition();
	std::string			getNextLevelUpCondition();
	std::string			describe();
	const std::string&	iconPic();
	const std::string&	namePic();
	unsigned int		quantity();

	unsigned int		getlefttime();
	std::string	getLeftTimeString();

	cocos2d::ccColor3B getFrameColor();
	cocos2d::CCSprite* getFrameNormalSpirte(std::string ext="");
	cocos2d::CCSprite* getFrameSelectedSpirte(std::string ext="");
	cocos2d::CCSprite* getFrameDisabledSpirte(std::string ext="");

	std::string	getTitleBuffPic();

	std::string	getNextQuantityString();
	std::string	getQuantityString();

	bool				hasNextTitle();
	unsigned int		getNextTitleItemID();
	unsigned int		nextQuantity();
	unsigned int		level();
	unsigned int		type();

	bool				hasGet();

private:
	const TitleInfo*		mTitleInfo;
	const TitleItem*		mTitleItem;
	unsigned int mTitleLvOneId;
};

typedef bool (*TitleFilter)(unsigned int);
typedef bool (*TitleSort)(unsigned int, unsigned int);

bool TitleFilter_default(unsigned int id);


bool TitleSort_default(unsigned int id1, unsigned int id2);

class TitleDateCollect : public Singleton<TitleDateCollect>
{
public:
	typedef std::list<unsigned int> TitleIDCollect;
	TitleIDCollect getTitleCollection(TitleFilter sFilter=TitleFilter_default, TitleSort sSort=TitleSort_default);
};