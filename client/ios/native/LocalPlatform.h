#pragma once
#include "libPlatform.h"

// Self-hosted accounts share the same server UID on Android, iOS and Web.
class LocalPlatform : public libPlatform {
    std::string uid, token, username;
    bool loggedIn;
public:
    LocalPlatform() : loggedIn(false) {}
    void init(bool privateLogin = false);
    void updateApp();
    void login();
    void logout();
    void switchUsers();
    void acceptAccount(const std::string& identity, const std::string& name, const std::string& session);
    void clearAccount() { loggedIn = false; uid.clear(); token.clear(); username.clear(); }
    void buyGoods(BUYINFO& info);
    bool getLogined() { return loggedIn; }
    const std::string& loginUin() { return uid; }
    const std::string& sessionID() { return token; }
    const std::string& nickName() { return username; }
    const std::string getClientChannel() { return "IOS_LOCAL"; }
    void notifyEnterGame() {}
    std::string getPlatformMoneyName() { return ""; }
};
