#pragma once
#include "Singleton.h"
#include "MessageManager.h"
#include <string>

class SoundManager
	: public Singleton<SoundManager>
	, public MessageHandler
{
public:
	SoundManager(void);
	~SoundManager(void);
	void init();
	void setMusicOn(bool);
	void setEffectOn(bool);

	void playLoadingMusic();
	void playGeneralMusic();
	void playFightMusic();
	void stopMusic();
	void playMusic(std::string music);

	int playEffect(std::string effect);
	void stopAllEffect();
	virtual void onReceiveMassage(const GameMessage * message);

	void appGotoBackground();
	void appResumeBackground();

	static SoundManager* getInstance();
private:
	bool mMusicOn;
	bool mEffectOn;
	std::string mPauseMusic;
	std::string mPlayMusic;
};

