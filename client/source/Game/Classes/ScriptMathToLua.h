#pragma once

class ScriptMathToLua 
{
public:
	ScriptMathToLua(void);
	~ScriptMathToLua(void);

	static bool compareSilverCoins(unsigned int _coins);
	static void modifySilverCoins(long long _coins);
};