#pragma once
#include "libPlatform.h"

// iOS development channel: persistent per-install identity, no obsolete vendor SDK.
class LocalPlatform : public libPlatform {
    std::string uid;
    bool loggedIn;
public:
    LocalPlatform() : loggedIn(false) {}
    void init(bool privateLogin = false);
    void updateApp();
    void login();
    void logout();
    void switchUsers();
    void buyGoods(BUYINFO& info);
    bool getLogined() { return loggedIn; }
    const std::string& loginUin() { return uid; }
    const std::string& nickName() { return uid; }
    const std::string getClientChannel() { return "IOS_LOCAL"; }
    void notifyEnterGame() {}
    std::string getPlatformMoneyName() { return ""; }
};
