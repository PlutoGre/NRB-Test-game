
#include "stdafx.h"

#include "SoundManager.h"
#include "GameMessages.h"
#include "BlackBoard.h"

#include "SimpleAudioEngine.h"
#include "DataTableManager.h"
#define generalmusic "general.mp3"
#define battlemusic "battle.mp3"
#define clicksound "click.mp3"

SoundManager::SoundManager(void)
	:mMusicOn(false),mEffectOn(true)
{
}


SoundManager::~SoundManager(void)
{
}

void SoundManager::playLoadingMusic()
{
	std::string stepKey = "SetttingsMusic";
	bool _localMusic=cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey(stepKey.c_str(),1)==1?true:false;
	if(_localMusic)
	{
		mPlayMusic=VaribleManager::Get()->getSetting("LoadingMusic");
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic(VaribleManager::Get()->getSetting("LoadingMusic").c_str(), true);
	}
}
void SoundManager::playGeneralMusic()
{
    if(mMusicOn && mPlayMusic!=generalmusic)
	{
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic(generalmusic, true);
	}
	mPlayMusic = generalmusic;
}
void SoundManager::playFightMusic()
{
    if(mMusicOn && mPlayMusic != battlemusic)
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic(battlemusic, true);
	mPlayMusic = battlemusic;
}

void SoundManager::playMusic( std::string music )
{
	if(mMusicOn)
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic(music.c_str(), true);
	mPlayMusic = music;
}

void SoundManager::onReceiveMassage( const GameMessage * message )
{
	if(mEffectOn)
	{
		if(!BlackBoard::Get()->isSamSungi9100Audio)
		{
			CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect( clicksound , false);
		}
	}
}

void SoundManager::init()
{
    MessageManager::Get()->regisiterMessageHandler(MSG_BUTTON_PRESSED, this);
    
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(battlemusic);
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(generalmusic);
	if(!BlackBoard::Get()->isSamSungi9100Audio)
	{
		CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(clicksound);
	}
	
}

void SoundManager::setMusicOn( bool isOn)
{
	if(mMusicOn != isOn)
	{
		mMusicOn = isOn;
		if(isOn)
		{
			if(mPauseMusic == mPlayMusic)
				CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
			else
				CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic(mPlayMusic.c_str(), true);
		}
		else
		{
			CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
			mPauseMusic = mPlayMusic;
		}
		std::string stepKey = "SetttingsMusic";
		cocos2d::CCUserDefault::sharedUserDefault()->setIntegerForKey(stepKey.c_str(),mMusicOn?1:0);
		cocos2d::CCUserDefault::sharedUserDefault()->flush();
	}
}

void SoundManager::setEffectOn( bool isOn)
{
	mEffectOn = isOn;
	if(!isOn)
	{
		if(!BlackBoard::Get()->isSamSungi9100Audio)
		{
			CocosDenshion::SimpleAudioEngine::sharedEngine()->stopAllEffects();
		}
	}
	std::string stepKey = "SetttingsEffect";
	cocos2d::CCUserDefault::sharedUserDefault()->setIntegerForKey(stepKey.c_str(),mEffectOn?1:0);
	cocos2d::CCUserDefault::sharedUserDefault()->flush();
}

int SoundManager::playEffect( std::string effect )
{
	if(mEffectOn)
	{
		//CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect( effect.c_str());
		if(!BlackBoard::Get()->isSamSungi9100Audio)
		{
			return CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect( effect.c_str() , false);
		}
		else
		{
			return -1;
		}
	}
	return -1;
}

void SoundManager::appGotoBackground()
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
	if(!BlackBoard::Get()->isSamSungi9100Audio)
	{
		CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseAllEffects();
	}
}	

void SoundManager::appResumeBackground()
{
	if(mMusicOn)
		CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
	if(mEffectOn)
	{
		if(!BlackBoard::Get()->isSamSungi9100Audio)
		{
			CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeAllEffects();
		}
	}
}

SoundManager* SoundManager::getInstance()
{
	return SoundManager::Get();
}

void SoundManager::stopAllEffect()
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->stopAllEffects();
}

void SoundManager::stopMusic()
{
	mPlayMusic = "";
	CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
}
