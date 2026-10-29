#pragma once
#include "CCBContainer.h"
#include "CCBManager.h"
#include "cocos-ext.h"

#define LUA_GETVAR_FUNC_REPLACE(_type,_func) \
	_type* _func( char* name ){ \
	cocos2d::CCObject* obj = getVariable(name); \
	_type* node = obj?dynamic_cast<_type*>(obj):0; \
	return node;}

class ScriptContentBase
	: public CCBContainer
	, public cocos2d::extension::CCReViSvItemNodeFacade
{
public:
	ScriptContentBase(std::string fileName);
	virtual ~ScriptContentBase();

	virtual void initItemView();;
	virtual void refreshItemView(cocos2d::extension::CCReViSvItemData* pItem);;
// #ifdef _DEBUG
// 	unsigned int m_iIdx;
// #endif

	virtual void onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender);
	virtual void onAnimationDone(const std::string& animationName);

	//for lua
	static ScriptContentBase* create(const std::string& fileName, int tag = 0);

	LUA_GETVAR_FUNC_REPLACE(cocos2d::CCNode,getVarNode);
	LUA_GETVAR_FUNC_REPLACE(cocos2d::CCSprite,getVarSprite);
	LUA_GETVAR_FUNC_REPLACE(cocos2d::CCLabelBMFont,getVarLabelBMFont);
	LUA_GETVAR_FUNC_REPLACE(cocos2d::CCLabelTTF,getVarLabelTTF);
	LUA_GETVAR_FUNC_REPLACE(cocos2d::CCMenu,getVarMenu);
	LUA_GETVAR_FUNC_REPLACE(cocos2d::CCMenuItem,getVarMenuItem);
	LUA_GETVAR_FUNC_REPLACE(cocos2d::CCMenuItemSprite,getVarMenuItemSprite);
	LUA_GETVAR_FUNC_REPLACE(cocos2d::CCMenuItemImage,getVarMenuItemImage);
	LUA_GETVAR_FUNC_REPLACE(cocos2d::extension::CCScrollView,getVarScrollView);
	LUA_GETVAR_FUNC_REPLACE(cocos2d::extension::CCScale9Sprite,getVarScale9Sprite);

	void registerFunctionHandler(int nHandler);
	void unregisterFunctionHandler();

	cocos2d::extension::CCReViSvItemData* getItemDate();

	virtual std::string dumpInfo();
private:
	int mScriptFunHandler;
	cocos2d::extension::CCReViSvItemData* mItemDate;
};