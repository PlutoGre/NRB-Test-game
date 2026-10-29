
#include "stdafx.h"

#include "FightCardSprite.h"
#include "GamePrecedure.h"

FightCardSprite::FightCardSprite(void)
	:currentLifeValue(1.0f),displayLifeValue(1.0f),flashTypeValue(0)
{
	addColorValue.a = addColorValue.r = addColorValue.g = addColorValue.b =0.f;

	addColorStart = addColorValue;
	addColorTarget = addColorValue;
	addColorRate = addColorValue;
	addColorTime = 0;
	addColorFullTime = 0;

	addColorDelay = 0;
	addColorRestoreTime = 0;

	//variables for damaging
	damageTimeWaitLeft = 0;
	damageTimeWorkingLeft = 0;
	flashType = 0;
}


FightCardSprite::~FightCardSprite(void)
{
}

void FightCardSprite::initUniforms()
{
	currentLife = glGetUniformLocation( getShaderProgram()->getProgram(), "currentLife");
	displayLife = glGetUniformLocation( getShaderProgram()->getProgram(), "displayLife");
	addColor = glGetUniformLocation( getShaderProgram()->getProgram(), "addColor");
	flashType = glGetUniformLocation( getShaderProgram()->getProgram(), "flashType");
}

void FightCardSprite::setUniforms()
{
	tick(GamePrecedure::Get()->getFrameTime());

	getShaderProgram()->setUniformLocationWith1f(currentLife, currentLifeValue);
	getShaderProgram()->setUniformLocationWith1f(displayLife, displayLifeValue);
	getShaderProgram()->setUniformLocationWith4f(addColor,addColorValue.r,addColorValue.g,addColorValue.b,addColorValue.a);
	getShaderProgram()->setUniformLocationWith1i(flashType, flashTypeValue);
}

FightCardSprite* FightCardSprite::create( const char* filename )
{

	FightCardSprite* pRet = new FightCardSprite();
	if (pRet && pRet->initWithFile(filename))
	{
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}

	return pRet;
}


void FightCardSprite::tick( float fDelta )
{
	if(lifeChangeTime<lifeChangeFullTime)
	{
		float dt = fDelta;
		lifeChangeTime+=fDelta;
		if(lifeChangeTime>lifeChangeFullTime)
		{
			dt -= (lifeChangeTime - lifeChangeFullTime);
			lifeChangeTime = lifeChangeFullTime;
		}
		displayLifeValue = currentLifeValue + lifeChangeRate*(lifeChangeFullTime - lifeChangeTime)*(lifeChangeFullTime - lifeChangeTime);
	}

	if(addColorDelay>0)
	{
		addColorDelay-=fDelta;
	}
	if(addColorDelay<=0 )
	{
		if(addColorTime<addColorFullTime && addColorFullTime>0)
		{
			float dt = fDelta;
			addColorTime+=dt;
			if(addColorTime>addColorFullTime)
			{
				addColorTime = addColorFullTime;
			}
			float rate = addColorTime/addColorFullTime;
			addColorValue.r = addColorStart.r*(1-rate) + addColorTarget.r*rate;
			addColorValue.g = addColorStart.g*(1-rate) + addColorTarget.g*rate;
			addColorValue.b = addColorStart.b*(1-rate) + addColorTarget.b*rate;
			addColorValue.a = addColorStart.a*(1-rate) + addColorTarget.a*rate;
		}
		else if(addColorRestoreTime>0)
		{
			cocos2d::ccColor4F c4f = {0,0,0,0};
			setAddColorTo(c4f,addColorRestoreTime);
		}
	}

	if(damageTimeWaitLeft > 0)
	{
		 damageTimeWaitLeft-=fDelta;
	}
	if(damageTimeWaitLeft<=0 && damageTimeWorkingLeft > 0)
	{
		damageTimeWorkingLeft-=fDelta;
		if(damageTimeWorkingLeft<=0)
		{
			flashTypeValue = 0;
		}
		else
		{
			flashTypeValue = ((flashTypeValue==1)?2:1);
		}
	}
	else
		flashTypeValue = 0;
}


void FightCardSprite::setLifeTo( float life, float dt /*= 0*/ )
{
	currentLifeValue = life;
	if(dt<=0)
	{
		displayLifeValue = currentLifeValue;
		lifeChangeTime = lifeChangeFullTime = 0;
	}
	else
	{
		lifeChangeRate = (displayLifeValue - currentLifeValue)/(dt*dt);
		lifeChangeTime = 0;
		lifeChangeFullTime = dt;
	}
}

void FightCardSprite::setAddColorTo( cocos2d::ccColor4F color, float dt /*= 0*/, float delay , float resotreTime)
{
	addColorDelay = delay;
	addColorTarget = color;
	addColorRestoreTime = resotreTime;

	if(dt<=0)
	{
		addColorValue = color;
		addColorFullTime = 0;
		addColorTime = 0;
		addColorStart = color;
		addColorTarget = color;
	}
	else
	{
		addColorTime = 0;
		addColorFullTime = dt;
		addColorStart = addColorValue;
		addColorTarget = color;
	}
}

char* FightCardSprite::getShaderfile()
{
    static char retstr[32] = "fightcard.sha";
	return retstr;
}

void FightCardSprite::setDamageFlash( float delay, float duration )
{
	damageTimeWaitLeft = delay;
	damageTimeWorkingLeft = duration;
}
