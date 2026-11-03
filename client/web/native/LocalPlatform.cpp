#include "libPlatform.h"
#include "libOS.h"
#include "cocos2d.h"
#include <emscripten.h>

class WebPlatform : public libPlatform {
    std::string uid, token, username;
    bool loggedIn;
public:
    WebPlatform() : loggedIn(false) {}
    void init(bool) {
        // LoadingFrame must finish building its scene before receiving onInit.
        emscripten_async_call([](void* self) {
            WebPlatform* platform = static_cast<WebPlatform*>(self);
            platform->_boardcastInitDone(true, "Browser local channel ready"); platform->updateApp();
        }, this, 0);
    }
    void updateApp() { _boardcastUpdateCheckDone(true, "Same-origin game resource updates"); }
    void login() { EM_ASM({ window.NRB.loginPlatform(Module, false); }); }
    void accept(const char* identity) {
        uid = identity ? identity : "";
        char* value = reinterpret_cast<char*>(EM_ASM_PTR({ return stringToNewUTF8(window.NRB.accountToken || ''); }));
        token = value; free(value);
        value = reinterpret_cast<char*>(EM_ASM_PTR({ return stringToNewUTF8(window.NRB.accountName || ''); }));
        username = value; free(value);
        loggedIn = !uid.empty() && !token.empty();
        cocos2d::CCLog("[WebPlatform] Account authentication %s", loggedIn ? "ready" : "cancelled");
        _boardcastLoginResult(loggedIn, loggedIn ? "Account verified" : "Login required");
    }
    void logout() {
        loggedIn = false; uid.clear(); token.clear(); username.clear();
        EM_ASM({ window.NRB.logoutPlatform(); });
        _boardcastPlatformLogout();
    }
    void switchUsers() {
        loggedIn = false; uid.clear(); token.clear(); username.clear();
        EM_ASM({ window.NRB.loginPlatform(Module, true); });
    }
    void buyGoods(BUYINFO& info) {
        _boardcastBuyinfoSent(false, info, "Web payments are not configured");
        libOS::getInstance()->showMessagebox("网页测试客户端未接入充值，请使用游戏管理工具发放测试道具。");
    }
    bool getLogined() { return loggedIn; }
    const std::string& loginUin() { return uid; }
    const std::string& sessionID() { return token; }
    const std::string& nickName() { return username; }
    const std::string getClientChannel() { return "WEB_LOCAL"; }
    void notifyEnterGame() {}
    std::string getPlatformMoneyName() { return ""; }
};
static WebPlatform* webPlatform;
extern "C" EMSCRIPTEN_KEEPALIVE void nrb_account_result(const char* uid) {
    if (webPlatform) webPlatform->accept(uid);
}
void initializeWebPlatform() {
    webPlatform = new WebPlatform;
    libPlatformManager::getInstance()->registerPlatform("WebPlatform", webPlatform);
    libPlatformManager::getInstance()->setPlatform("WebPlatform");
}
