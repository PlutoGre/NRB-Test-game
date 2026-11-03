#pragma once

#include "cocos2d.h"
#include "GUI/CCControlExtension/CCControlButton.h"
#include "json/json.h"
#include "CCBContainer.h"
#include <jni.h>
#include <algorithm>
#include <cstdio>
#include <typeinfo>
#include <sstream>

namespace nrb_scene_inspection {

inline std::string action(cocos2d::CCMenuItem* item) {
    if (!item) return "";
    for (cocos2d::CCNode* parent = item->getParent(); parent; parent = parent->getParent()) {
        CCBContainer* container = dynamic_cast<CCBContainer*>(parent);
        if (!container) continue;
        std::istringstream description(container->dumpInfo());
        std::string line;
        bool menus = false;
        while (std::getline(description, line)) {
            if (line.find("menuitems name info") != std::string::npos) { menus = true; continue; }
            if (menus && container->getMenuItemByAction(line) == item) return line;
        }
    }
    return "";
}

// Read-only UI hierarchy for adb QA. No packet dispatch, actions or game edits.
// Disabled for non-debuggable APKs, and inactive until the local marker exists.
inline Json::Value node(cocos2d::CCNode* current, int& remaining, int depth) {
    Json::Value value(Json::objectValue);
    if (!current || !current->isVisible() || --remaining < 0 || depth > 40) return value;
    value["type"] = typeid(*current).name();
    value["tag"] = current->getTag();
    if (cocos2d::CCLabelBMFont* text = dynamic_cast<cocos2d::CCLabelBMFont*>(current)) value["text"] = text->getString();
    if (cocos2d::CCLabelTTF* text = dynamic_cast<cocos2d::CCLabelTTF*>(current)) value["text"] = text->getString();
    if (cocos2d::CCMenuItem* button = dynamic_cast<cocos2d::CCMenuItem*>(current)) {
        value["enabled"] = button->isEnabled();
        value["action"] = action(button);
    }
    if (cocos2d::extension::CCControlButton* button = dynamic_cast<cocos2d::extension::CCControlButton*>(current)) value["enabled"] = button->isEnabled();
    cocos2d::CCSize size = current->getContentSize();
    cocos2d::CCPoint points[] = {cocos2d::CCPoint(0, 0), cocos2d::CCPoint(size.width, 0),
        cocos2d::CCPoint(0, size.height), cocos2d::CCPoint(size.width, size.height)};
    float left = 1e9f, bottom = 1e9f, right = -1e9f, top = -1e9f;
    for (int i = 0; i < 4; ++i) {
        cocos2d::CCPoint point = current->convertToWorldSpace(points[i]);
        left = std::min(left, point.x); right = std::max(right, point.x);
        bottom = std::min(bottom, point.y); top = std::max(top, point.y);
    }
    value["x"] = left; value["y"] = bottom;
    value["width"] = right - left; value["height"] = top - bottom;
    Json::Value children(Json::arrayValue);
    cocos2d::CCArray* nodes = current->getChildren();
    if (nodes) for (unsigned int i = 0; i < nodes->count() && remaining > 0; ++i) {
        cocos2d::CCNode* child = static_cast<cocos2d::CCNode*>(nodes->objectAtIndex(i));
        if (child->isVisible()) children.append(node(child, remaining, depth + 1));
    }
    value["children"] = children;
    return value;
}

class Observer : public cocos2d::CCObject {
public:
    void tick(float) {
        std::string directory = cocos2d::CCFileUtils::sharedFileUtils()->getWritablePath();
        if (directory.empty()) return;
        if (directory[directory.size() - 1] != '/') directory += '/';
        FILE* marker = std::fopen((directory + "nrb-scene-inspection.enabled").c_str(), "rb");
        if (!marker) return;
        std::fclose(marker);
        cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
        cocos2d::CCEGLView* view = director->getOpenGLView();
        if (!view || !director->getRunningScene()) return;
        Json::Value snapshot(Json::objectValue);
        cocos2d::CCSize size = director->getWinSize();
        cocos2d::CCSize frame = view->getFrameSize();
        cocos2d::CCRect viewport = view->getViewPortRect();
        snapshot["width"] = size.width; snapshot["height"] = size.height;
        snapshot["frameWidth"] = frame.width; snapshot["frameHeight"] = frame.height;
        snapshot["viewportX"] = viewport.origin.x; snapshot["viewportY"] = viewport.origin.y;
        snapshot["scaleX"] = view->getScaleX(); snapshot["scaleY"] = view->getScaleY();
        snapshot["frame"] = static_cast<int>(director->getTotalFrames());
        int budget = 8000;
        snapshot["scene"] = node(director->getRunningScene(), budget, 0);
        std::string data = Json::FastWriter().write(snapshot);
        std::string temporary = directory + "nrb-scene.json.writing";
        FILE* output = std::fopen(temporary.c_str(), "wb");
        if (!output) return;
        bool complete = std::fwrite(data.data(), 1, data.size(), output) == data.size();
        if (std::fclose(output) != 0) complete = false;
        if (complete) std::rename(temporary.c_str(), (directory + "nrb-scene.json").c_str());
    }
};

// Read Android's actual debuggable flag; JNI failures leave inspection disabled.
inline void start(JNIEnv* env) {
    static bool scheduled = false;
    if (scheduled) return;
    jclass helper = env->FindClass("org/cocos2dx/lib/Cocos2dxHelper");
    if (!helper) { env->ExceptionClear(); return; }
    jfieldID contextField = env->GetStaticFieldID(helper, "sContext", "Landroid/content/Context;");
    if (!contextField) { env->ExceptionClear(); env->DeleteLocalRef(helper); return; }
    jobject context = env->GetStaticObjectField(helper, contextField);
    jclass contextType = env->FindClass("android/content/Context");
    if (!context || !contextType) { env->ExceptionClear(); env->DeleteLocalRef(helper); return; }
    jmethodID getInfo = env->GetMethodID(contextType, "getApplicationInfo", "()Landroid/content/pm/ApplicationInfo;");
    jobject info = getInfo ? env->CallObjectMethod(context, getInfo) : NULL;
    jclass infoType = info ? env->GetObjectClass(info) : NULL;
    jfieldID flagsField = infoType ? env->GetFieldID(infoType, "flags", "I") : NULL;
    bool allowed = flagsField && !env->ExceptionCheck() && (env->GetIntField(info, flagsField) & 2);
    if (env->ExceptionCheck()) env->ExceptionClear();
    if (infoType) env->DeleteLocalRef(infoType);
    if (info) env->DeleteLocalRef(info);
    env->DeleteLocalRef(contextType); env->DeleteLocalRef(context); env->DeleteLocalRef(helper);
    if (!allowed) return;
    Observer* observer = new Observer();
    cocos2d::CCDirector::sharedDirector()->getScheduler()->scheduleSelector(
        (cocos2d::SEL_SCHEDULE)(&Observer::tick), observer, 1.0f, false);
    observer->release();
    scheduled = true;
}

} // namespace nrb_scene_inspection
