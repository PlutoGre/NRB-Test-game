#pragma once
#include "SpriteShader.h"

class FightCardSprite : public SpriteShader
{
private:
	FightCardSprite();

	//variables for life
	float currentLifeValue;
	float displayLifeValue;

	float lifeChangeRate;
	float lifeChangeFullTime;
	float lifeChangeTime;

	//variables for color
	cocos2d::ccColor4F addColorValue;

	cocos2d::ccColor4F addColorStart;
	cocos2d::ccColor4F addColorTarget;
	cocos2d::ccColor4F addColorRate;
	float addColorTime;
	float addColorFullTime;
	float addColorDelay;
	float addColorRestoreTime;

	//variables for damaging
	float damageTimeWaitLeft;
	float damageTimeWorkingLeft;
	GLuint    flashTypeValue;

	//gl tags
	GLuint    currentLife;
	GLuint    displayLife;
	GLuint    addColor;
	GLuint    flashType;
	void tick(float fDelta);
protected:
	virtual char* getShaderfile();
	//used to get uniforms from name into handle
	virtual void initUniforms();
	//used to set uniforms value 
	virtual void setUniforms();
public:
	~FightCardSprite(void);
	static FightCardSprite* create(const char* filename);
	
	void setLifeTo(float life, float duration = 0);
	void setAddColorTo(cocos2d::ccColor4F color, float duration = 0, float delay = 0,float restoreTime = -1);
	void setDamageFlash(float delay, float duration);
};

