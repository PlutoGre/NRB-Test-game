#pragma once

#include "cocos2d.h"
#include "StateMachine.h"
#include "GamePrecedure.h"

#include "LoadingUnit.h"
#include "CurlDownload.h"

#include "lib91.h"

class LoadingScene: public State<GamePrecedure>, public CurlDownload::DownloadListener, public platformListener
{
public:
	enum TAGS
	{
		TAGS_LAYER,
		TAGS_PROGRESS,
	};
	LoadingScene(void);
	~LoadingScene(void);

	//this will execute when the state is entered
	virtual void Enter(GamePrecedure*);

	//this is the states normal update function
	virtual void Execute(GamePrecedure*);

	//this will execute when the state is exited. (My word, isn't
	//life full of surprises... ;o))
	virtual void Exit(GamePrecedure*);

	virtual void downloaded(const std::string& url,const std::string& filename);

	virtual void onLogin(libPlatform*);
private:
	void _addImage(const char*);
	void _addSound(const char*);
	void _addLoadingList();

	cocos2d::CCScene* mScene;
	//std::list<LoadingUnit*> mLoadingList;

	int mLoadingCount;
	LoadingGroup::LoadingGroupData* mLoadingData;
	LoadingGroup *mLoadingGroup;

};

