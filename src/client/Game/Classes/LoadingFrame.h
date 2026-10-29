#pragma once


#include "cocos2d.h"
#include "cocos-ext.h"
#include "StateMachine.h"
#include "GamePrecedure.h"
#include "MessageManager.h"
#include "CCBManager.h"
#include "CCBContainer.h"
#include "GamePackets.h"
#include "lib91.h"
#include "libOS.h"
#include "PackageLogic.h"
#include "BulletinBoardPage.h"
// class MainPage;
// class TeamPage;
//#define Macro_AndroidTestJiHuoMa


using namespace cocos2d;

class LoadingFrame
	: public BasePage
	, public State<GamePrecedure>
	, public MessageHandler
	, public CCBContainer::CCBContainerListener
	, public PacketHandlerGeneral
    , public platformListener
	, public libOSListener
    , public BulletinBoardPageListener
{
private:
	enum CHILD_TAG
	{
		TAG_CONTENT,
	};
	cocos2d::CCScene* mScene;
    bool mLogined;
    bool m91checked;
	int mSelectedSeverID;
	void showSevers(bool);
    bool mIsUpdateDown;
public:
	LoadingFrame(void);
	~LoadingFrame(void);
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);

	CREATE_FUNC(LoadingFrame);

	virtual void load(void);

	//
	CCScene* getRootSceneNode();

	//this will execute when the state is entered
	virtual void Enter(GamePrecedure*);

	//this is the states normal update function
	virtual void Execute(GamePrecedure*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(GamePrecedure*);

	virtual void onReceiveMassage(const GameMessage * message);
	
	virtual void onReceivePacket(const int opcode, const ::google::protobuf::Message* packet);
	//virtual void onSendPacketFailed(const int opcode);
	virtual void onConnectFailed(std::string ip, int port);
private:
	virtual int _getOpcode() {return OPCODE_PLAYER_LOGINRET_S;};
public:
    virtual void onLogin(libPlatform*, bool success, const std::string& log);
    virtual void onPlatformLogout(libPlatform*);
    virtual void onUpdate(libPlatform*,bool ok, std::string msg);
	virtual void onMessageboxEnter(int tag);
    virtual void onBuyinfoSent(libPlatform*, bool success, const BUYINFO&, const std::string& log){};
	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag);
    
    virtual void onStartLoad(){};
    virtual void onFinishLoad(){};
    virtual void onFailLoadWithError(const std::string error){};
    virtual void onBtnAction();
    virtual void onLoadingTimeOut();
    
    virtual bool isUpdateDown(){return mIsUpdateDown;};
private:
	void _updateLocalSeverId();
	void showUpdateChecking();
	void showUpdateCheckingFailed();
	void showUpdateFileChecking();
	void showUpdateUpdating();
	void showPersent(float persentage,std::string sizeTip="");
	void showUpdateFailed();
	void showUpdateDone();
	void showEnter();
	void showEnterAndLoginingIn();

	void updateSeverName();
	void updateLoginName();
	
	int getDefaultSeverID();
	
};

