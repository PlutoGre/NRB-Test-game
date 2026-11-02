#import <Foundation/Foundation.h>
#include "CCCommon.h"
#include "libOS.h"
#include <cstdarg>
#include <cstdio>
#include <string>

namespace cocos2d {
void CCLog(const char *format, ...) {
    if (!format) return;
    char buffer[kMaxLogLen + 1] = {};
    va_list arguments;
    va_start(arguments, format);
    vsnprintf(buffer, sizeof(buffer), format, arguments);
    va_end(arguments);
    // Unified system logging remains available in Xcode and device logs.
    NSLog(@"[Cocos] %s", buffer);
}
void CCLuaLog(const char *message) { CCLog("%s", message ? message : ""); }
void CCMessageBox(const char *message, const char *title) {
    std::string text = title ? title : "";
    if (!text.empty()) text += "\n";
    if (message) text += message;
    libOS::getInstance()->showMessagebox(text);
}
}
