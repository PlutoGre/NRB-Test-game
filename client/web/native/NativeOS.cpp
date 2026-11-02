#include "libOS.h"
#include "BulletinBoardPage.h"
#include "cocos2d.h"
#include <emscripten.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>

libOS* libOS::m_sInstance = 0;
long libOS::avalibleMemory() { return EM_ASM_INT({ return Math.min(2147483647, HEAPU8.length); }); }
long long libOS::getFreeSpace() { return 256LL * 1024 * 1024; } // Browser quota cannot be queried synchronously.
NetworkStatus libOS::getNetWork() { return EM_ASM_INT({ return navigator.onLine ? 1 : 0; }) ? ReachableViaWiFi : NotReachable; }
const std::string libOS::getDeviceID() {
    char* value = reinterpret_cast<char*>(EM_ASM_PTR({ return stringToNewUTF8(window.NRB.identity || ''); }));
    std::string result(value); free(value); return result;
}
const std::string libOS::getPlatformInfo() { return "WebAssembly"; }
const std::string& libOS::generateSerial() {
    static std::string value; static unsigned long counter = 0;
    std::ostringstream stream; stream << getDeviceID() << "_" << emscripten_get_now() << "_" << ++counter;
    value = stream.str(); return value;
}
void libOS::rmdir(const char* path) {
    std::string target(path ? path : "");
    // The game may remove an outdated resource cache. Never touch bundled assets.
    if (target.compare(0, 9, "/persist/") != 0 || target.find("..") != std::string::npos) return;
    DIR* directory = opendir(target.c_str());
    if (!directory) { remove(target.c_str()); return; }
    while (dirent* entry = readdir(directory)) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
        std::string child = target + "/" + entry->d_name;
        struct stat info;
        if (stat(child.c_str(), &info) == 0 && S_ISDIR(info.st_mode)) rmdir(child.c_str()); else remove(child.c_str());
    }
    closedir(directory); ::rmdir(target.c_str());
}
void libOS::showInputbox(bool multiline) { EM_ASM({ window.NRB.input(Module, $0); }, multiline); }
void libOS::showMessagebox(const std::string& text, int tag) { EM_ASM({ window.NRB.message(UTF8ToString($0), () => Module._nrb_message($1)); }, text.c_str(), tag); }
void libOS::openURL(const std::string& url) { EM_ASM({ window.NRB.openURL(UTF8ToString($0)); }, url.c_str()); }
void libOS::emailTo(const std::string& to, const std::string& cc, const std::string& title, const std::string& body) {
    EM_ASM({ location.href = 'mailto:' + encodeURIComponent(UTF8ToString($0)) + '?cc=' + encodeURIComponent(UTF8ToString($1))
        + '&subject=' + encodeURIComponent(UTF8ToString($2)) + '&body=' + encodeURIComponent(UTF8ToString($3)); }, to.c_str(), cc.c_str(), title.c_str(), body.c_str());
}
void libOS::setWaiting(bool waiting) { EM_ASM({ window.NRB.waiting(!!$0); }, waiting); }
void libOS::clearNotification() {}
void libOS::addNotification(const std::string&, int, bool) { cocos2d::CCLog("[WebOS] Native scheduled notifications are unavailable in this browser channel"); }
extern "C" EMSCRIPTEN_KEEPALIVE void nrb_message(int tag) { libOS::getInstance()->_boardcastMessageboxOK(tag); }
extern "C" EMSCRIPTEN_KEEPALIVE void nrb_input(const char* text) { libOS::getInstance()->_boardcastInputBoxOK(text); }

BulletinBoardPage* BulletinBoardPage::mInstance = 0;
static BulletinBoardPageListener* boardListener = 0;
static int boardTimeout = 15;
void BulletinBoardPage::init(float, float, float, float, BulletinBoardPageListener* listener) { close(); boardListener = listener; }
void BulletinBoardPage::show(const std::string& url) {
    if (boardListener) boardListener->onStartLoad();
    EM_ASM({ window.NRB.board(Module, UTF8ToString($0), $1); }, url.c_str(), boardTimeout);
}
void BulletinBoardPage::close() { boardListener = 0; EM_ASM({ window.NRB.closeBoard(); }); }
void BulletinBoardPage::webKitSetLoadingTimeOut(int seconds) { boardTimeout = seconds < 1 ? 1 : seconds; }
extern "C" EMSCRIPTEN_KEEPALIVE void nrb_board(int event) {
    BulletinBoardPageListener* listener = boardListener;
    if (!listener) return;
    if (event == 0) { listener->onFinishLoad(); return; }
    BulletinBoardPage::getInstance()->close();
    if (event == 1) listener->onBtnAction();
    else if (event == 2) listener->onLoadingTimeOut();
    else listener->onFailLoadWithError("Announcement URL blocked or unavailable");
}
