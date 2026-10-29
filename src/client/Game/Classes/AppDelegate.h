#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "cocos2d.h"

/**
@brief    The cocos2d Application.

The reason for implement as private inheritance is to hide some interface call by CCDirector.
*/
class  AppDelegate : private cocos2d::CCApplication
{
public:
    AppDelegate();
    virtual ~AppDelegate();

    /**
    @brief    Implement CCDirector and CCScene init code here.
    @return true    Initialize success, app continue.
    @return false   Initialize failed, app terminate.
    */
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  The function be called when the application enter background
    @param  the pointer of the application
    */
    virtual void applicationDidEnterBackground();

    /**
    @brief  The function be called when the application enter foreground
    @param  the pointer of the application
    */
    virtual void applicationWillEnterForeground();

	/*
	*/
	virtual void purgeCachedData(void);

	//--begin xinzheng 2013-6-3
	/*
		GameApp具体实现上层自己的清理
		CCDirector::end()只触发了引擎层面及内部的清理
	*/
	virtual void applicationWillGoToExit();
	//--end

};

extern void globalPurgeCachedData();

#endif // _APP_DELEGATE_H_

