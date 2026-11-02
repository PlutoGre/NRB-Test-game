#pragma once
#include "platform/CCCommon.h"
#include "platform/CCApplicationProtocol.h"
#include <string>
NS_CC_BEGIN
class CCApplication : public CCApplicationProtocol {
public:
    CCApplication();
    virtual ~CCApplication();
    static CCApplication* sharedApplication();
    int run();
    void setAnimationInterval(double interval);
    ccLanguageType getCurrentLanguage();
    TargetPlatform getTargetPlatform();
    virtual void applicationWillGoToExit() {}
    void setResourceRootPath(const std::string& path) { root = path; }
    const std::string& getResourceRootPath() { return root; }
private:
    std::string root;
    static CCApplication* instance;
};
NS_CC_END
