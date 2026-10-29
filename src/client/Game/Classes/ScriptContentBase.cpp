
#include "stdafx.h"
#include "ScriptContentBase.h"
#include "CCLuaEngine.h"
#include "script_support/CCScriptSupport.h"
#include "Gamelua.h"

#define RUN_SCRIPT_FUN(funname) \
	if(mScriptFunHandler) \
{ \
	cocos2d::CCLuaEngine* pEngine = cocos2d::CCLuaEngine::defaultEngine(); \
	pEngine->executeEvent(mScriptFunHandler,funname,this,"ScriptContentBase"); \
} 

ScriptContentBase* ScriptContentBase::create( const std::string& fileName, int tag /*= 0*/ )
{
	ScriptContentBase *pRet = new ScriptContentBase(fileName); 
	pRet->setTag(tag);
	pRet->autorelease(); 
	return pRet; 
}

ScriptContentBase::ScriptContentBase( std::string fileName )
	:mScriptFunHandler(0)
{
	loadCcbiFile(fileName);
	this->m_pItemNode = this;
	this->retain();
	mScriptFunHandler = NULL;
}

ScriptContentBase::~ScriptContentBase()
{
	unregisterFunctionHandler();
	removeFromParent();
}

void ScriptContentBase::initItemView()
{
	RUN_SCRIPT_FUN("luaInitItemView");
}

void ScriptContentBase::refreshItemView( cocos2d::extension::CCReViSvItemData* pItem )
{
	mItemDate = pItem;
	RUN_SCRIPT_FUN("luaRefreshItemView");
}

void ScriptContentBase::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	RUN_SCRIPT_FUN(itemName.c_str());
}

void ScriptContentBase::onAnimationDone( const std::string& animationName )
{
	RUN_SCRIPT_FUN("luaOnAnimationDone");
}

void ScriptContentBase::registerFunctionHandler( int nHandler )
{
	unregisterFunctionHandler();
	mScriptFunHandler = nHandler;
}

void ScriptContentBase::unregisterFunctionHandler()
{
	if (mScriptFunHandler)
	{
		cocos2d::CCScriptEngineManager::sharedManager()->getScriptEngine()->removeScriptHandler(mScriptFunHandler);
		mScriptFunHandler = 0;
	}
}

cocos2d::extension::CCReViSvItemData* ScriptContentBase::getItemDate()
{
	return mItemDate;
}

std::string ScriptContentBase::dumpInfo()
{
	return CCBContainer::dumpInfo();
}