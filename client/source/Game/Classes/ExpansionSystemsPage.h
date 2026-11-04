#pragma once
#include "AdventureBase.h"
#include "PacketManager.h"
#include "json/json.h"

/** 运宝与养成的共用界面：参考版美术、分页选择、成长预览及可核对的提交。 */
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
    std::string mMode,mOwner,mKey,mInfo,mMessage,mOverlay;
    Json::Value mState,mPending,mConfirm,mActions;
    bool mSelected,mBusy,mRegistered;
    float mWait,mPoll;
    int mTarget,mChoice,mOffset,mTab;
    bool mResetScroll;
    cocos2d::extension::CCScrollView* mScroll;
    // 所有按钮都映射到服务器原始操作；本地页签和弹层不会直接修改角色数据。
    void button(cocos2d::CCNode* parent,const std::string& label,const Json::Value& action,
                float x,float y,float width,float height,bool active=true);
    void renderBody(cocos2d::CCNode* parent,float height);
    void renderOverlay(cocos2d::CCNode* parent,float height);
    void send(const Json::Value& request);
    void info();
    void savePending();
    void failure();
    void click(cocos2d::CCObject* sender);
    static std::string nonce();
};
