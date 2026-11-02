#include "libPlatform.h"
#include "libOS.h"
#include "cocos2d.h"
#include <emscripten.h>

class WebPlatform : public libPlatform {
    std::string uid;
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
    void login() {
        char* value = reinterpret_cast<char*>(EM_ASM_PTR({ return stringToNewUTF8(window.NRB.identity || ''); }));
        uid = value; free(value); loggedIn = !uid.empty();
        cocos2d::CCLog("[WebPlatform] Browser guest login %s", loggedIn ? "ready" : "unavailable");
        _boardcastLoginResult(loggedIn, loggedIn ? "Browser identity ready" : "Reload the page to establish a session");
    }
    void logout() { loggedIn = false; _boardcastPlatformLogout(); }
    void switchUsers() { login(); }
    void buyGoods(BUYINFO& info) {
        _boardcastBuyinfoSent(false, info, "Web payments are not configured");
        libOS::getInstance()->showMessagebox("网页测试客户端未接入充值，请使用游戏管理工具发放测试道具。");
    }
    bool getLogined() { return loggedIn; }
    const std::string& loginUin() { return uid; }
    const std::string& nickName() { return uid; }
    const std::string getClientChannel() { return "WEB_LOCAL"; }
    void notifyEnterGame() {}
    std::string getPlatformMoneyName() { return ""; }
};
void initializeWebPlatform() {
    libPlatformManager::getInstance()->registerPlatform("WebPlatform", new WebPlatform);
    libPlatformManager::getInstance()->setPlatform("WebPlatform");
}
