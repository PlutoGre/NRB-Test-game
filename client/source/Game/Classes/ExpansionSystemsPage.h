#pragma once
#include "AdventureBase.h"
#include "PacketManager.h"
#include "json/json.h"

/** 运宝及养成的两端共用页面，使用原游戏画布与网络通道。 */
class ExpansionSystemsPage : public AdventureBase, public PacketHandler, public cocos2d::CCObject {
public:
    ExpansionSystemsPage(unsigned int id, int mode);
    virtual ~ExpansionSystemsPage();
    virtual void init();
    virtual void refreshContent();
    virtual void loadData();
    virtual void updatePage(bool selected, float dt=0);
    virtual void exitPage();
    virtual void onMenuItemAction(const std::string&,cocos2d::CCObject*,int) { }
    virtual void onReceivePacket(int opcode,const std::string& value);
    virtual void onSendPacketFailed(int) { failure(); }
    virtual void onConnectFailed(std::string,int) { failure(); }
    virtual void onTimeout(int) { failure(); }
    virtual void onPacketError(int) { failure(); }
    virtual Handler_Type getHandleType() { return Scripty_Handler; }
protected:
    virtual const std::string& getCcbiFileName() const;
private:
    std::string mMode,mOwner,mKey,mInfo,mMessage;
    Json::Value mState,mPending,mConfirm,mActions;
    bool mSelected,mBusy,mRegistered;
    float mWait,mPoll;
    int mTarget,mChoice,mOffset;
    cocos2d::CCMenu* mMenu;
    void send(const Json::Value& request);
    void info();
    void savePending();
    void failure();
    void click(cocos2d::CCObject* sender);
    static std::string nonce();
};
