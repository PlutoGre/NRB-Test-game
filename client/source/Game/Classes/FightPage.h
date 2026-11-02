#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "MainFrame.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "CustomPage.h"
#include "PackageLogic.h"

#include "UserBattle.pb.h"
#define MaxBattleNumLine 5
enum FightType
{
	FightTypeAnyThing = -1,
	CareerBattle=0,
	ArenaBattle=1,
	FightEndBattle=2,
	WorldBoss=3,
	SkillBook=4,
};
struct ViewTeamStats
{
	unsigned int id;
	unsigned int viewType;
	unsigned int npcStatus;
};
class FightPage
	: public CustomPage
	, public State<MainFrame>
	, public CCBContainer::CCBContainerListener
	, public PacketHandlerGeneral
{
public:
	FightPage(void);
	~FightPage(void);
	
	CREATE_FUNC(FightPage);
	virtual PAGE_TYPE getPageType() {return PT_COVER;}

	//this will execute when the state is entered
	virtual void Enter(MainFrame*);

	//this is the states normal update function
	virtual void Execute(MainFrame*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(MainFrame*);

	virtual void load(void);

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	virtual void onReceivePacket(const int opcode, const ::google::protobuf::Message* packet);
private:
	virtual int _getOpcode() {return OPCODE_USER_BATTLERET_S;};
public:
	typedef std::vector<int > VecIntList;
	typedef std::map<unsigned int,CCBContainer* > CCBMAP; 
	CCBMAP mBattleList;
	static FightPage* getInstance();
	void fight();
	void setMessage(const OPUserBattleRet* msg,bool isShowThreeEvaluation=false,bool isHideSkip = false);
	void setMessage(char* msg, int length ,bool isShowThreeEvaluation=false,bool isHideSkip = false);
	void setFightType(unsigned int fightType){ mFightType=fightType;};
	void setViewTeamStatus(unsigned int id,unsigned int viewType,unsigned int npcStatus){ mViewTeamStats.id=id;mViewTeamStats.viewType=viewType;mViewTeamStats.npcStatus=npcStatus;};
	const OPUserBattleRet* getMessage(){return mBattleMsg;}
	void nextRound();
	bool showViewTeam(){ return mFightType==ArenaBattle||mFightType==SkillBook;};
	bool IsWorldBoss(){ return mFightType==WorldBoss;};
	ViewTeamStats getViewTeamStats(){ return mViewTeamStats;};
	unsigned int mCurrRound;
	enum TEAM_ROTATION
	{
		TEAM_SELF = 0,
		TEAM_ENEMY = 1,
		TEAM_COUNT = 2,
	};

	static float caculateXunit(int id, int count);

	static float calcXunit(int id,int count,bool isNewline);
	static bool isNewline(int id,int count);
	static int calcMaxBattle(int count);
	static bool isHavNewline(int maxPostion);
	static int calcCurrPosition(int position,int maxPosition,bool isNewline);
	static OPUserBattleRet * receiveMessage();

	int setLifeMax(int pos,int life);
	int setLiftMaxByPos(int pos,int life,bool isSelf);
	int getLifeMax(int pos);
	int getLifeMaxByPos(int pos,bool isSelf);
	void removeLifeMaxByDeadPos(VecIntList _selfList,VecIntList _enemyList);
	bool getHideSkip();
	bool mIsShowThreeEvaluation;
	bool mIsReplay;
private:
	
	friend class InstructionStepCustomAttack;
	void _removeMaxLifeList();
	std::map<int,int> mLifes;
	VecIntList mSelfLifeList;
	VecIntList mEnemyLifeList;
	ViewTeamStats mViewTeamStats;
	StateMachine<FightPage> * mStateMachine;
// 	enum STATES
// 	{
// 		STATE_ROUND,
// 		STATE_FINAL,
// 		STATE_RESULT,
// 	};
	typedef std::list<State<FightPage>* > STATEMAP;
	STATEMAP mStates;
	bool mCurrentStateDone;
	bool mFighting;
	OPUserBattleRet* mBattleMsg;
	unsigned int mFightType;
	bool mSkip;
	bool mHideSkip;
	bool mShowSkip;
};

