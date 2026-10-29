#pragma once
#include "StateMachine.h"
#include "FightPage.h"
#include "FightConfigManager.h"
#include "MessageManager.h"
#define WorldBossAttack 30000
class FightTask;
class FightCardSprite;
class FightPage_roundState 
	: public State<FightPage>
	, public MessageHandler
{
public:
	FightPage_roundState(const OPUserBattleRet_Round&, int roundCount, bool hasNext, bool hadPrevious);
	~FightPage_roundState(void);

	//this will execute when the state is entered
	virtual void Enter(FightPage*);

	//this is the states normal update function
	virtual void Execute(FightPage*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(FightPage*);

	virtual void onReceiveMassage(const GameMessage * message);

	struct SLOTMEMBER
	{
		enum {
			TAG_BG = 100,
			TAG_HEAD = 101,
		};
		SLOTMEMBER():battle(0),life(0),maxlife(0){}
		CCBContainer* battle;
		//CCBContainer* buff;
		//CCBContainer* headface;
		//CCBContainer* lifeBar;
		int life;
		int maxlife;
		bool isNewline;
		int position;
		inline int currBattlePosition(){return (position+2)>>1;}//(position +2)/2
		int resourceID;
		CCPoint mEntrancePoint;
		CCPoint mStopPoint;
		int *plife;
	};

private:
	const OPUserBattleRet_Round& mMessage;
	const FightPoint* mFightPoint;
	bool  mIsHavNewline;
	int mBattleCount;
	int mMaxNewLineSize;
	int mRoundCount;
	bool mHasNext;
	bool mHasPrevious;
	bool mSkillLoaded;
	typedef std::list<FightTask*> TASKLIST;
	TASKLIST mTaskList;

	float mCurrentActionTime;
	
	typedef std::map<int, SLOTMEMBER> SLOTLIST;
	SLOTLIST mSlots;

	void _createSlots(FightPage*);
	void _destroySlots(FightPage*);
	void _createTasks(FightPage*);

	int _getOpponentID(int id);

	CCPoint getStartPosition(FightPage* page,CCBContainer* _battle,CCNode* _member,bool isSelf,bool _isHavNewline,bool _isNewline);

	CCPoint getEntrancePosition(FightPage* page,CCBContainer* _battle,CCNode* _member,bool _isSelf,bool _isHavNewline,bool _isNewline);

	CCPoint getQueuePosition(FightPage* page,CCBContainer* _battle,CCNode* _member,int _currBattle,bool _isSelf,bool _isHavNewline,bool _isNewline);

	enum SKILL_TYPE
	{
		ST_SKILL_SINGLE = 1,
		ST_SKILL_MULTIPLE = 2,
		ST_BUFF_ATTACK = 4,
		ST_BUFF_DEFENSE = 5,
		ST_BUFF_POWER = 6,
		ST_BUFF = ST_BUFF_ATTACK&ST_BUFF_DEFENSE&ST_BUFF_DEFENSE,//100&101&110 = 100(0x4)
	};

	bool mSkipShown;
	float mSkipTime;
	bool mfirstEnter;
};

