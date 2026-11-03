#include "cocos2d.h"
#include "GUI/CCControlExtension/CCControlButton.h"
#include "json/json.h"
#include "CCBContainer.h"
#include <emscripten.h>
#include <sstream>
#include <typeinfo>

namespace {
std::string action(cocos2d::CCMenuItem* item) {
    for (cocos2d::CCNode* parent = item->getParent(); parent; parent = parent->getParent()) {
        CCBContainer* container = dynamic_cast<CCBContainer*>(parent);
        if (!container) continue;
        std::istringstream description(container->dumpInfo()); std::string line; bool menus = false;
        while (std::getline(description, line)) {
            if (line.find("menuitems name info") != std::string::npos) { menus = true; continue; }
            if (menus && container->getMenuItemByAction(line) == item) return line;
        }
    }
    return "";
}
// Read-only scene inspection supports real canvas clicks during browser QA.
// It never dispatches game packets or invokes a button's action directly.
Json::Value snapshot(cocos2d::CCNode* node, int& remaining, int depth) {
    Json::Value value(Json::objectValue);
    if (!node || !node->isVisible() || --remaining < 0 || depth > 40) return value;
    value["type"] = typeid(*node).name();
    value["tag"] = node->getTag();
    if (cocos2d::CCLabelBMFont* text = dynamic_cast<cocos2d::CCLabelBMFont*>(node)) value["text"] = text->getString();
    if (cocos2d::CCLabelTTF* text = dynamic_cast<cocos2d::CCLabelTTF*>(node)) value["text"] = text->getString();
    if (cocos2d::CCMenuItem* button = dynamic_cast<cocos2d::CCMenuItem*>(node)) {
        value["enabled"] = button->isEnabled(); value["action"] = action(button);
    }
    if (cocos2d::extension::CCControlButton* button = dynamic_cast<cocos2d::extension::CCControlButton*>(node)) value["enabled"] = button->isEnabled();
    cocos2d::CCSize size = node->getContentSize();
    cocos2d::CCPoint corners[] = {cocos2d::CCPoint(0, 0), cocos2d::CCPoint(size.width, 0),
        cocos2d::CCPoint(0, size.height), cocos2d::CCPoint(size.width, size.height)};
    float left = 1e9, bottom = 1e9, right = -1e9, top = -1e9;
    for (int i = 0; i < 4; ++i) {
        cocos2d::CCPoint point = node->convertToWorldSpace(corners[i]);
        left = std::min(left, point.x); right = std::max(right, point.x);
        bottom = std::min(bottom, point.y); top = std::max(top, point.y);
    }
    value["x"] = left; value["y"] = bottom; value["width"] = right - left; value["height"] = top - bottom;
    Json::Value children(Json::arrayValue);
    cocos2d::CCArray* nodes = node->getChildren();
    if (nodes) for (unsigned int index = 0; index < nodes->count() && remaining > 0; ++index) {
        cocos2d::CCNode* child = static_cast<cocos2d::CCNode*>(nodes->objectAtIndex(index));
        if (child->isVisible()) children.append(snapshot(child, remaining, depth + 1));
    }
    value["children"] = children;
    return value;
}
}

extern "C" EMSCRIPTEN_KEEPALIVE const char* nrb_scene_snapshot() {
    static std::string result;
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCSize size = director->getWinSize();
    int budget = 8000;
    Json::Value root(Json::objectValue);
    root["frames"] = static_cast<unsigned int>(director->getTotalFrames());
    root["paused"] = director->isPaused();
    root["width"] = size.width; root["height"] = size.height;
    root["scene"] = snapshot(director->getRunningScene(), budget, 0);
    result = Json::FastWriter().write(root);
    return result.c_str();
}
