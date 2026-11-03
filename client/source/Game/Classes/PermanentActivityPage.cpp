#include "stdafx.h"
#include "PermanentActivityPage.h"
#include "GamePrecedure.h"
#include "GameMessages.h"
#include "MessageManager.h"
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <sstream>

USING_NS_CC;
USING_NS_CC_EXT;

namespace {
const int RequestOpcode = 48001, ResponseOpcode = 48002;
std::string number(int value) { return StringConverter::toString(value); }
std::string prizeText(const Json::Value& prize) { return prize["name"].asString() + " ×" + number(prize["count"].asInt()); }
CCLabelTTF* label(const std::string& value, float size, float width = 0) {
    return width > 0 ? CCLabelTTF::create(value.c_str(), "Arial", size, CCSizeMake(width, 0), kCCTextAlignmentLeft)
                     : CCLabelTTF::create(value.c_str(), "Arial", size);
}
}

PermanentActivityPage::PermanentActivityPage(unsigned int id, bool wheel)
    : AdventureBase(id), mWheel(wheel), mSelected(false), mBusy(false), mRegistered(false),
      mWaitTime(0), mAnimationTime(0), mPollTime(0), mActivity(wheel ? "fortuneWheel" : "allBlue"), mModal(NULL), mViewport(NULL)
{
    mOwner = GamePrecedure::Get()->getUin();
    mPendingKey = "PermanentActivity_" + number(GamePrecedure::Get()->getServerID()) + "_" + mOwner + "_" + mActivity;
    std::string saved = CCUserDefault::sharedUserDefault()->getStringForKey(mPendingKey.c_str(), "");
    if (!saved.empty()) {
        Json::Reader reader;
        if (!reader.parse(saved, mPending, false) || !mPending.isObject()) mPending = Json::Value();
    }
    mIsInitData = true;
}

PermanentActivityPage::~PermanentActivityPage() { exitPage(); }

const std::string& PermanentActivityPage::getCcbiFileName() const {
    static const std::string blue = "ported/activities/v1/ccbi/AllBluePage.ccbi";
    static const std::string wheel = "ported/activities/v1/ccbi/PeronaTurntable.ccbi";
    return mWheel ? wheel : blue;
}

void PermanentActivityPage::init() {
    // 旧引擎的图片查找只保留 basename，移植图集已另加前缀，不能覆盖原有界面帧。
    unsigned long size = 0;
    CCFileUtils* files = CCFileUtils::sharedFileUtils();
    unsigned char* data = files->getFileData(files->fullPathForFilename("ported/activities/v1/atlases.txt").c_str(), "rb", &size);
    if (data) {
        std::istringstream lines(std::string(reinterpret_cast<char*>(data), size)); std::string path;
        while (std::getline(lines, path)) if (!path.empty()) CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(path.c_str());
        delete[] data;
    }
    AdventureBase::init();
    // 新旧 CCB 的设计尺寸不同，保留内部坐标和动画，将整页适配到当前活动视口。
    BasePage* adventure = CCBManager::Get()->getPage("AdventurePage");
    CCScrollView* view = adventure ? dynamic_cast<CCScrollView*>(adventure->getVariable("mContent")) : NULL;
    if (view && mContentContainer->getContentSize().width > 0 && mContentContainer->getContentSize().height > 0) {
        const CCSize target = view->getViewSize(), original = mContentContainer->getContentSize();
        float scale = std::min(target.width / original.width, target.height / original.height);
        mViewport = CCBContainer::create(); mViewport->setContentSize(target);
        mContentContainer->setScale(scale);
        mContentContainer->setPosition(ccp((target.width - original.width * scale) / 2, (target.height - original.height * scale) / 2));
        mViewport->addChild(mContentContainer);
    }
}

void PermanentActivityPage::text(const std::string& variable, const std::string& value) {
    CCObject* object = mContentContainer->getVariable(variable);
    if (CCLabelBMFont* target = dynamic_cast<CCLabelBMFont*>(object)) target->setString(value.c_str());
    else if (CCLabelTTF* target = dynamic_cast<CCLabelTTF*>(object)) target->setString(value.c_str());
}

void PermanentActivityPage::visible(const std::string& variable, bool value) {
    if (CCNode* node = dynamic_cast<CCNode*>(mContentContainer->getVariable(variable))) node->setVisible(value);
}

void PermanentActivityPage::refreshContent() {
    const bool ready = mState.isObject();
    const int free = ready ? mState["freeRemaining"].asInt() : 0;
    if (!mWheel) {
        text("mRemainingTimeText", "永久开放"); text("mRemainingTime", "永久开放");
        text("mHaveDiamond", "钻石"); text("mHaveIntegral", "积分");
        text("mHaveDiamondNum", ready ? number(mState["goldcoins"].asInt()) : "--");
        text("mHaveIntegralNum", ready ? number(mState["points"].asInt()) : "--");
        text("mConsumeNum1", ready ? number(free > 0 ? 0 : mState["singleCost"].asInt()) : "--");
        text("mConsumeNum2", ready ? number(mState["tenCost"].asInt()) : "--");
        text("mExploreOneText", free > 0 ? "免费探索" : "探索一次"); text("mExploreBestText", "探索十次");
        text("mTreasureRoomText", "宝库"); text("mRechargeText", "积分商店");
        text("mGoodsContent", "奖励通过传书领取");
        text("mCurrentCondition", ready ? "今日剩余免费 " + number(free) + " 次" : "正在读取活动…");
    } else {
        text("mCountdown", "永久开放"); text("mTimeCountdown", "永久开放");
        int collected = 0;
        if (ready) for (int i = 0; i < 8; i++) if ((mState["mask"].asInt() & (1 << i)) != 0) ++collected;
        text("mPoints", ready ? number(mState["points"].asInt()) : "--");
        text("mTotalStarsNum", ready ? number(collected) : "--");
        text("mFreeNumTex", free > 0 ? "今日免费次数" : "本次消耗钻石");
        text("mFreeNum", ready ? number(free > 0 ? free : mState["singleCost"].asInt()) : "--");
        text("mResetCostNum", ready ? number(mState["resetCost"].asInt()) : "--");
        text("mResetCostTex", "重置消耗"); text("mUlTimesReset", "重置转盘");
        text("mResidueDegree", "集齐本轮后可免费重置"); text("mProbability", "查看奖池概率");
        text("mDrawSuccessTex", "奖励已发送至传书"); text("mDrawFailTex", "重复位置获得保底奖励");
        visible("mDrawGoldBanner", free <= 0); visible("mResetGoldBanner", true);
        visible("mSuccessNode", false); visible("mFailNode", false); visible("mAni", false);
        if (ready) for (int i = 0; i < 8; i++) {
            std::string suffix = number(i + 1);
            const Json::Value& prize = mState["pool"][i];
            text("mRewardName" + suffix, prizeText(prize));
            visible("mPicBG" + suffix, (mState["mask"].asInt() & 1 << i) != 0);
            visible("mStar" + suffix, (mState["mask"].asInt() & 1 << i) != 0);
            if (CCSprite* icon = dynamic_cast<CCSprite*>(mContentContainer->getVariable("mRewardPic" + suffix))) {
                const CCSize previous = icon->boundingBox().size;
                std::string file = prize["icon"].asString();
                CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(file.c_str());
                CCTexture2D* texture = frame ? NULL : CCTextureCache::sharedTextureCache()->addImage(file.c_str());
                if (frame) {
                    icon->setDisplayFrame(frame);
                    icon->setScale(std::min(previous.width / icon->getContentSize().width, previous.height / icon->getContentSize().height));
                } else if (texture) {
                    icon->setTexture(texture); icon->setTextureRect(CCRectMake(0, 0, texture->getContentSize().width, texture->getContentSize().height));
                    icon->setScale(std::min(previous.width / texture->getContentSize().width, previous.height / texture->getContentSize().height));
                }
            }
        }
    }
    controls();
}

void PermanentActivityPage::controls() {
    bool enable = mSelected && !mBusy && mAnimationTime <= 0 && mModal == NULL && mState.isObject() && mPending.isNull();
    const char* actions[] = {"onExploreOne", "onExploreBest", "onTreasureRoom", "onRecharge", "onHelp", "onDraw", "onReset", "onShop", "onCheck"};
    for (unsigned int i = 0; i < sizeof(actions) / sizeof(actions[0]); i++)
        if (CCMenuItem* button = mContentContainer->getMenuItemByAction(actions[i])) button->setEnabled(enable);
    for (int i = 1; i <= 8; i++) {
        const std::string action = mWheel ? "onRewardBtn" + number(i) : "onIslandPic" + number(i);
        if (CCMenuItem* button = mContentContainer->getMenuItemByAction(action)) button->setEnabled(enable);
    }
    if (enable && mWheel && mState["mask"].asInt() == 255)
        if (CCMenuItem* button = mContentContainer->getMenuItemByAction("onDraw")) button->setEnabled(false);
}

void PermanentActivityPage::loadData() {
    mSelected = true;
    if (!mRegistered) { PacketManager::Get()->regisiterPacketHandler(ResponseOpcode, this); mRegistered = true; }
    sendInfo();
}

void PermanentActivityPage::updatePage(bool selected, float dt) {
    if (!selected) { if (mSelected) exitPage(); return; }
    if (!mSelected) { loadData(); return; }
    mPollTime += dt;
    if (mPollTime >= 60 && !mBusy && mAnimationTime <= 0 && mPending.isNull() && mModal == NULL) { mPollTime = 0; sendInfo(); }
    if (mBusy) {
        mWaitTime += dt;
        if (mWaitTime >= 20) networkFailure();
    }
    if (mAnimationTime > 0) {
        mAnimationTime -= dt;
        if (mAnimationTime <= 0) { mAnimationTime = 0; showResult(); }
    }
}

void PermanentActivityPage::exitPage() {
    mSelected = false; mBusy = false; mAnimationTime = 0; mPollTime = 0;
    if (mRegistered) { PacketManager::Get()->removePacketHandler(ResponseOpcode, this); mRegistered = false; }
    if (mContentContainer) {
        if (CCNode* target = dynamic_cast<CCNode*>(mContentContainer->getVariable("mTurnTableNode"))) target->stopAllActions();
        closeModal(); controls();
    }
}

std::string PermanentActivityPage::requestId() {
    static unsigned int sequence = 0;
    char value[33];
    sprintf(value, "%08x%08x%08x%08x", static_cast<unsigned int>(time(NULL)), static_cast<unsigned int>(clock()),
            static_cast<unsigned int>(rand()), ++sequence);
    return value;
}

void PermanentActivityPage::sendInfo() {
    if (!mSelected || mBusy) return;
    Json::Value request; request["activity"] = mActivity; request["action"] = "info";
    mInfoId = requestId(); request["requestId"] = mInfoId; send(request);
}

void PermanentActivityPage::send(const Json::Value& request) {
    if (mOwner != GamePrecedure::Get()->getUin()) return;
    std::string data = Json::FastWriter().write(request);
    mBusy = true; mWaitTime = 0; controls();
    PacketManager::Get()->sendPakcet(RequestOpcode, &data[0], data.size(), false);
}

void PermanentActivityPage::persistPending() {
    const std::string value = mPending.isNull() ? "" : Json::FastWriter().write(mPending);
    CCUserDefault::sharedUserDefault()->setStringForKey(mPendingKey.c_str(), value); CCUserDefault::sharedUserDefault()->flush();
}

void PermanentActivityPage::mutate(const std::string& action, int id, int count) {
    if (!mSelected || mBusy || !mPending.isNull() || !mState.isObject()) return;
    closeModal();
    mPending = Json::Value(); mPending["activity"] = mActivity; mPending["action"] = action;
    mPending["requestId"] = requestId(); mPending["revision"] = mState["revision"];
    mPending["configRevision"] = mState["configRevision"]; mPending["count"] = count; mPending["id"] = id;
    // 先记录用户已经确认的操作，再发送；退出、断网或重启后只能用原编号核对结果。
    persistPending(); send(mPending);
}

void PermanentActivityPage::onReceivePacket(int opcode, const std::string& value) {
    if (!mSelected || opcode != ResponseOpcode || mOwner != GamePrecedure::Get()->getUin()) return;
    Json::Value reply; Json::Reader reader;
    if (!reader.parse(value, reply, false) || reply["activity"].asString() != mActivity) return;
    std::string id = reply["requestId"].asString();
    bool pending = !mPending.isNull() && id == mPending["requestId"].asString();
    if (!pending && id != mInfoId) return;
    mBusy = false; mWaitTime = 0;
    if (!reply.get("ok", false).asBool()) {
        if (pending && !reply.get("uncertain", false).asBool()) { mPending = Json::Value(); persistPending(); }
        mState = Json::Value();
        std::vector<std::string> rows; rows.push_back(reply.get("message", "活动暂时无法连接").asString());
        modal("活动提示", rows, RETRY); return;
    }
    if (reply["owner"].asString() != mOwner || !reply["pool"].isArray() || reply["pool"].size() != 8) { networkFailure(); return; }
    mState = reply;
    ServerDateManager::Get()->getUserBasicInfo().goldcoins = reply["goldcoins"].asUInt();
    if (pending) {
        // 原操作确认后，忽略重试期间较早发出的信息查询响应。
        mInfoId.clear(); closeModal();
        mPending = Json::Value(); persistPending();
        refreshContent(); beginResult(reply["result"], reply["action"].asString());
    } else {
        refreshContent();
        if (!mPending.isNull()) { send(mPending); return; }
    }
    MsgMainFrameRefreshPage message; message.pageName = "AdventurePage";
    MessageManager::Get()->sendMessage(&message);
}

void PermanentActivityPage::beginResult(const Json::Value& result, const std::string& action) {
    mResult = result;
    if ((action == "spin" || action == "explore") && !result.get("replayed", false).asBool() && result["rewards"].size() > 0) {
        mAnimationTime = 1.7f; controls();
        int slot = result["rewards"][0u]["slot"].asInt();
        if (mWheel) {
            if (CCNode* target = dynamic_cast<CCNode*>(mContentContainer->getVariable("mTurnTableNode"))) {
                target->stopAllActions();
                float current = fmodf(target->getRotation(), 360.0f);
                // 原界面按顺时针排列八个奖格，指针的中心偏移也需保留。
                const float angles[] = {338, 292, 248, 202, 158, 112, 68, 24};
                target->runAction(CCEaseSineOut::create(CCRotateBy::create(1.6f, 1080.0f + angles[slot % 8] - current)));
            }
        } else {
            const int islands[] = {2, 1, 3, 4};
            mContentContainer->runAnimation("0" + number(islands[slot % 4]));
        }
    } else showResult();
}

void PermanentActivityPage::showResult() {
    std::vector<std::string> rows;
    const Json::Value& prizes = mResult["rewards"];
    for (unsigned int i = 0; i < prizes.size(); i++) {
        std::string line = prizeText(prizes[i]);
        if (prizes[i].get("duplicate", false).asBool()) line += "（重复位置保底）";
        rows.push_back(line);
    }
    if (rows.empty()) rows.push_back(mWheel ? "转盘已重置，本轮奖励可以重新收集。" : "暂无探索或兑换奖励。");
    else rows.push_back("奖励已发送至传书，领取后进入背包。");
    modal("活动结果", rows, prizes.empty() ? 0 : MAIL);
}

void PermanentActivityPage::showPool() {
    std::vector<std::string> rows;
    for (unsigned int i = 0; i < mState["pool"].size(); i++) {
        const Json::Value& item = mState["pool"][i]; char chance[32]; sprintf(chance, "  %.2f%%", item["probability"].asDouble());
        rows.push_back(prizeText(item) + chance);
    }
    modal("固定奖池", rows);
}

void PermanentActivityPage::showShop() {
    std::vector<std::string> rows;
    for (unsigned int i = 0; i < mState["shop"].size(); i++) {
        const Json::Value& item = mState["shop"][i];
        rows.push_back(prizeText(item) + "\n" + number(item["points"].asInt()) + " 积分");
    }
    modal("积分商店 · 当前 " + number(mState["points"].asInt()), rows, BUY);
}

void PermanentActivityPage::showMilestones() {
    std::vector<std::string> rows;
    for (unsigned int i = 0; i < mState["milestones"].size(); i++) {
        const Json::Value& item = mState["milestones"][i];
        rows.push_back("累计 " + number(item["draws"].asInt()) + " 次：" + prizeText(item) +
                       (item["claimed"].asBool() ? "（已领取）" : ""));
    }
    modal("累计抽取 " + number(mState["draws"].asInt()) + " 次", rows, CLAIM);
}

void PermanentActivityPage::showHelp() {
    std::vector<std::string> rows;
    rows.push_back("活动永久开放，免费机会每天北京时间零点刷新。");
    rows.push_back("每次随机抽取固定奖池中的一项，概率见奖池详情。");
    rows.push_back("奖励通过传书领取，领取状态与账号角色一起保存。");
    if (mWheel) {
        rows.push_back("本轮首次命中奖励格获得该项奖励；再次命中同一格获得保底：" + prizeText(mState["duplicateReward"]));
        rows.push_back("每次抽取获得 " + number(mState["pointsPerDraw"].asInt()) + " 积分；累计奖励每档可领取一次。");
        rows.push_back("可付费重置本轮收集状态，集齐 8 格后免费重置；积分和累计次数保留。");
    } else {
        rows.push_back("每次探索获得 " + number(mState["pointsPerDraw"].asInt()) + " 积分，可在积分商店兑换。");
        rows.push_back("免费机会用于单次探索；十次探索按显示价格扣费，保留当天的免费机会。");
    }
    modal("玩法说明", rows);
}

void PermanentActivityPage::modal(const std::string& title, const std::vector<std::string>& rows, int buttonType) {
    closeModal();
    const CCSize size = mContentContainer->getContentSize();
    const float unit = size.width / 320.0f;
    mModal = CCLayerColor::create(ccc4(12, 22, 38, 248), size.width, size.height);
    mContentContainer->addChild(mModal, 1000);
    CCLabelTTF* heading = label(title, 14 * unit, size.width - 28 * unit);
    heading->setPosition(ccp(size.width / 2, size.height - 22 * unit)); mModal->addChild(heading);
    const float rowHeight = (buttonType == BUY || buttonType == CLAIM ? 48 : 37) * unit;
    const float viewHeight = std::max(30.0f * unit, size.height - 100 * unit);
    CCScrollView* scroll = CCScrollView::create(CCSizeMake(size.width - 20 * unit, viewHeight));
    scroll->setDirection(kCCScrollViewDirectionVertical); scroll->setPosition(ccp(10 * unit, 62 * unit));
    CCNode* content = CCNode::create(); const float contentHeight = std::max(viewHeight, rowHeight * rows.size());
    content->setContentSize(CCSizeMake(size.width - 20 * unit, contentHeight)); scroll->setContainer(content);
    CCMenu* menu = CCMenu::create(); menu->setPosition(CCPointZero); menu->setScrollViewChild(true); content->addChild(menu);
    for (unsigned int i = 0; i < rows.size(); i++) {
        const bool operation = buttonType == BUY || buttonType == CLAIM;
        CCLabelTTF* caption = label(rows[i], 11 * unit, size.width - (operation ? 102 : 36) * unit);
        caption->setAnchorPoint(ccp(0, 0.5f)); caption->setPosition(ccp(7 * unit, contentHeight - rowHeight * (i + 0.5f)));
        content->addChild(caption);
        if (operation) {
            CCMenuItemLabel* button = CCMenuItemLabel::create(label(buttonType == BUY ? "兑换" : "领取", 13 * unit), this, menu_selector(PermanentActivityPage::onModalButton));
            const Json::Value& item = mState[buttonType == BUY ? "shop" : "milestones"][i];
            button->setTag(buttonType + item["id"].asInt());
            button->setPosition(ccp(size.width - 57 * unit, contentHeight - rowHeight * (i + 0.5f)));
            bool enabled = buttonType == BUY ? mState["points"].asInt() >= item["points"].asInt()
                : !item["claimed"].asBool() && mState["draws"].asInt() >= item["draws"].asInt();
            button->setEnabled(enabled); button->setColor(enabled ? ccc3(100, 255, 145) : ccc3(140, 140, 140)); menu->addChild(button);
        }
    }
    scroll->setContentOffset(ccp(0, viewHeight - contentHeight)); mModal->addChild(scroll);
    CCMenu* footer = CCMenu::create(); footer->setPosition(CCPointZero); mModal->addChild(footer);
    CCMenuItemLabel* close = CCMenuItemLabel::create(label("关闭", 14 * unit), this, menu_selector(PermanentActivityPage::onModalButton));
    close->setTag(CLOSE); close->setPosition(ccp(size.width * 0.25f, 30 * unit)); footer->addChild(close);
    if (buttonType == MAIL || buttonType == RETRY) {
        CCMenuItemLabel* action = CCMenuItemLabel::create(label(buttonType == MAIL ? "前往传书" : "重试查询", 14 * unit), this, menu_selector(PermanentActivityPage::onModalButton));
        action->setTag(buttonType); action->setPosition(ccp(size.width * 0.72f, 30 * unit)); footer->addChild(action);
    } else if (buttonType == 0 && !mWheel && mState.isObject()) {
        CCMenuItemLabel* shop = CCMenuItemLabel::create(label("积分商店", 14 * unit), this, menu_selector(PermanentActivityPage::onModalButton));
        shop->setTag(SHOP); shop->setPosition(ccp(size.width * 0.72f, 30 * unit)); footer->addChild(shop);
    }
    controls();
}

void PermanentActivityPage::closeModal() { if (mModal) { mModal->removeFromParent(); mModal = NULL; } }

void PermanentActivityPage::onModalButton(CCObject* sender) {
    int tag = static_cast<CCNode*>(sender)->getTag();
    if (tag >= CLAIM) mutate("claim", tag - CLAIM);
    else if (tag >= BUY) mutate("exchange", tag - BUY);
    else if (tag == RETRY) { closeModal(); mBusy = false; sendInfo(); }
    else if (tag == SHOP) showShop();
    else {
        closeModal(); controls();
        if (tag == CLOSE && (!mPending.isNull() || !mState.isObject())) { sendInfo(); return; }
        if (tag == MAIL) { MsgMainFramePushPage message; message.pageName = "SysMsgPage"; MessageManager::Get()->sendMessage(&message); }
    }
}

void PermanentActivityPage::onMenuItemAction(const std::string& action, CCObject*, int) {
    if (!mSelected || mBusy || mModal || mAnimationTime > 0 || !mState.isObject() || !mPending.isNull()) return;
    if (action == "onExploreOne") mutate("explore");
    else if (action == "onExploreBest") mutate("explore", 0, 10);
    else if (action == "onDraw") mutate("spin");
    else if (action == "onReset") mutate("reset");
    else if (action == "onRecharge" || action == "onShop") showShop();
    else if (action == "onCheck") showMilestones();
    else if (action == "onHelp") showHelp();
    else if (action == "onTreasureRoom") { mResult["rewards"] = mState["lastRewards"]; showResult(); }
    else if (action.find("onIslandPic") == 0 || action.find("onRewardBtn") == 0) showPool();
}

void PermanentActivityPage::networkFailure() {
    if (!mSelected) return;
    mBusy = false; mWaitTime = 0;
    std::vector<std::string> rows; rows.push_back("连接超时或中断，请确认服务端连接后重试。");
    if (!mPending.isNull()) rows.push_back("将按原操作编号核对结果，不会重新创建一次付费操作。");
    modal("活动连接提示", rows, RETRY);
}
void PermanentActivityPage::onSendPacketFailed(int) { networkFailure(); }
void PermanentActivityPage::onConnectFailed(std::string, int) { networkFailure(); }
void PermanentActivityPage::onTimeout(int) { networkFailure(); }
void PermanentActivityPage::onPacketError(int) { networkFailure(); }
