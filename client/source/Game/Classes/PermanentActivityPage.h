#pragma once
#include "AdventureBase.h"
#include "PacketManager.h"
#include "json/json.h"
#include <vector>

/** 两端共享的永久活动页面，复用 APK 原界面；所有扣费和抽取结果来自游戏服。 */
class PermanentActivityPage : public AdventureBase, public PacketHandler, public cocos2d::CCObject
{
public:
    PermanentActivityPage(unsigned int id, bool wheel);
    virtual ~PermanentActivityPage();
    virtual void init();
    virtual void refreshContent();
    virtual CCBContainer* getContainerNode() { return mViewport ? mViewport : mContentContainer; }
    virtual void loadData();
    virtual void updatePage(bool selected, float dt = 0.0f);
    virtual void exitPage();
    virtual void onMenuItemAction(const std::string& action, cocos2d::CCObject* sender, int tag);
    virtual void onReceivePacket(int opcode, const std::string& value);
    virtual void onSendPacketFailed(int opcode);
    virtual void onConnectFailed(std::string ip, int port);
    virtual void onTimeout(int opcode);
    virtual void onPacketError(int opcode);
    virtual Handler_Type getHandleType() { return Scripty_Handler; }

protected:
    virtual const std::string& getCcbiFileName() const;

private:
    bool mWheel, mSelected, mBusy, mRegistered;
    float mWaitTime, mAnimationTime, mPollTime;
    std::string mActivity, mOwner, mInfoId, mPendingKey;
    Json::Value mState, mPending, mResult;
    cocos2d::CCLayerColor* mModal;
    CCBContainer* mViewport;
    enum { CLOSE = 1, MAIL = 2, RETRY = 3, SHOP = 4, BUY = 100, CLAIM = 200 };

    void text(const std::string& variable, const std::string& value);
    void visible(const std::string& variable, bool value);
    void controls();
    void sendInfo();
    void send(const Json::Value& request);
    void mutate(const std::string& action, int id = 0, int count = 1);
    void persistPending();
    void networkFailure();
    void beginResult(const Json::Value& result, const std::string& action);
    void showResult();
    void showPool();
    void showShop();
    void showMilestones();
    void showHelp();
    void modal(const std::string& title, const std::vector<std::string>& rows, int buttonType = 0);
    void closeModal();
    void onModalButton(cocos2d::CCObject* sender);
    static std::string requestId();
};
