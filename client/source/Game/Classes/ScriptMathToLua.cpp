
#include "stdafx.h"

#include "ScriptMathToLua.h"
#include "ServerDateManager.h"
#include "StringConverter.h"
#include "DataTableManager.h"
#include "BlackBoard.h"
#include "Language.h"
#include "GameMessages.h"

USING_NS_CC;
ScriptMathToLua::ScriptMathToLua(void)
{

}

ScriptMathToLua::~ScriptMathToLua(void)
{
}


bool ScriptMathToLua::compareSilverCoins(unsigned int _coins)
{
	return ServerDateManager::Get()->getUserBasicInfo().silvercoins>_coins;
}

void ScriptMathToLua::modifySilverCoins(long long _coins)
{
	ServerDateManager::Get()->getUserBasicInfo().silvercoins=_coins;
}
