#pragma once

#include "Singleton.h"
#include "StateMachine.h"
#include "CCScheduler.h"
#include "cocos2d.h"
#include <map>
#include <google/protobuf/message.h>
#include "PacketManager.h"

class LoadingFrame;
class MainFrame;

class GamePrecedure : 
	public cocos2d::CCObject,
	public PacketHandler,
	public Singleton<GamePrecedure>
{
protected:
	LoadingFrame *mLoadingScene;
	MainFrame *mMainFrame;

	StateMachine<GamePrecedure> *m_pStateMachine;
	//RythemTouchState * m_pTouchState;

	cocos2d::CCScheduler* m_pScheduler;
	float mFrameTime;
	float mTotalTime;
    bool mInLoadingScene;
	bool gotoMainScene;

	int mServerID;
	std::string mUin;

	float mHeartbeatTime;
    bool mInitialized;
	int mServerTime;

	bool mReadTables;

	bool mHasAlertServerUpdating;

	
	struct REQUEST_PACKAGE
	{
		int opcode;
		int opcoderet;
		::google::protobuf::Message* message;
	};
	std::map<int,REQUEST_PACKAGE> mRequestPackages;
	void _requestPackage(int opcode, int opcoderet, ::google::protobuf::Message* message);
	void _gotPackage(int opcoderet);
	void _failedPakcage(int opcode, bool isSendFaild);
public:
	GamePrecedure()
		:mLoadingScene(0)
		,mMainFrame(0)
		,mFrameTime(0)
		,mTotalTime(0)
		,m_pScheduler(0)
		,m_pStateMachine(0)
        ,mInLoadingScene(true)
		,gotoMainScene(false)
		,mHeartbeatTime(0)
        ,mInitialized(false)
		,mServerTime(0)
		,mHasAlertServerUpdating(false)
		//,m_pTouchState(0)
	{
		
	}

	~GamePrecedure();

	static GamePrecedure* getInstance(){return GamePrecedure::Get();}

	void init();

	//--begin xinzheng 2013-6-3
	void unInit();//clear memory
	//--end

	void enterLoading();
	void preEnterMainMenu();
	void readTables();
	void requestPackages();
	void reRequestPackages();

	void enterMainMenu();
	void enterGame();
	void enterInstruction(int id = -1);//-1 means start from the first
	void exitGame();
	void update(float dt);

    bool isInLoadingSceneAndNeedExit();
    bool isInLoadingScene(){return mInLoadingScene;}
	float getFrameTime();
	float getTotalTime();
    
	void enterBackGround();
    void enterForeGround();

	void setServerID(int serverID){mServerID = serverID;}
	int getServerID(){return mServerID;}

	int getServerTime(){ return mServerTime;};
	void setServerTime(int serverTime){ mServerTime=serverTime;};
	void setUin(const std::string& uin){mUin = uin;}
	const std::string& getUin(){return mUin;}

	bool hasAlertServerUpdating(){return mHasAlertServerUpdating;}
	void setAlertServerUpdating(bool hasAlert){mHasAlertServerUpdating = hasAlert;}

	//--begin xinzheng 2013-5-23
	//引擎层给游戏截图，返回png文件存储绝对路径
	const std::string gameSnapshot();
	//--end

	virtual void onReceivePacket(const int opcode, const ::google::protobuf::Message* packet){_gotPackage(opcode);}
	virtual void onSendPacketFailed(const int opcode){_failedPakcage(opcode,true);}
	virtual void onConnectFailed(std::string ip, int port){}
	virtual void onTimeout(const int opcode){_failedPakcage(opcode,true);}
	virtual void onPacketError(const int opcode){_failedPakcage(opcode,true);_failedPakcage(opcode,false);}
};

