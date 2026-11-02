#pragma once
#include "Singleton.h"

#include <string>
#include <list>

namespace cocos2d
{
	class CCSprite;
};
class ArenaOpponentInfo;
class ArenaRewardItem;

class ArenaOpponent
{
public:
	ArenaOpponent(unsigned int id);
	unsigned int	id();
	const std::string& name();
	unsigned int	level();
	unsigned int	arenaRank();
	unsigned int	rewardScore();
	unsigned int	arenaType();
	unsigned int	dicipleItemID1();
	unsigned int	dicipleItemID2();
	unsigned int	dicipleItemID3();

	cocos2d::CCSprite* getFunctionPic_Normal();
	cocos2d::CCSprite* getFunctionPic_Selected();
	std::string getFunctionName();

	bool isDefeatBy();
	bool canChallenge();

private:
	const ArenaOpponentInfo*		mArenaOpponentInfo;
};

typedef bool (*ArenaOpponentFilter)(unsigned int);
typedef bool (*ArenaOpponentSort)(unsigned int, unsigned int);

bool ArenaOpponentFilter_Type1(unsigned int id);
bool ArenaOpponentFilter_Type2_3(unsigned int id);
bool ArenaOpponentFilter_Type4(unsigned int id);
bool ArenaOpponentFilter_default(unsigned int id);
bool ArenaOpponentSort_default(unsigned int id1, unsigned int id2);

class ArenaOpponentDateCollect : public Singleton<ArenaOpponentDateCollect>
{
public:
	typedef std::list<unsigned int> ArenaOpponentIDCollect;
	ArenaOpponentIDCollect getArenaOpponentCollection(ArenaOpponentFilter sFilter=ArenaOpponentFilter_default, ArenaOpponentSort sSort=ArenaOpponentSort_default);
};


class ArenaReward
{
public:
	ArenaReward(unsigned int itemid);
	unsigned int				itemID();
	unsigned int				type();
	std::string					title();
	unsigned int				needScore();
	unsigned int				rewardNum();
	std::string					describe();
	std::string					iconPic();

	bool						isEnable();
	std::string					getFunctionName();

private:
	const ArenaRewardItem*		mArenaRewardItem;
};

typedef bool (*ArenaRewardFilter)(unsigned int);
typedef bool (*ArenaRewardSort)(unsigned int, unsigned int);

bool ArenaRewardFilter_default(unsigned int id);
bool ArenaRewardSort_default(unsigned int id1, unsigned int id2);

class ArenaRewardDateCollect : public Singleton<ArenaRewardDateCollect>
{
public:
	typedef std::list<unsigned int> ArenaRewardIDCollect;
	ArenaRewardIDCollect getArenaRewardCollection(ArenaRewardFilter sFilter=ArenaRewardFilter_default, ArenaRewardSort sSort=ArenaRewardSort_default);
};