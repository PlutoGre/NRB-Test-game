
#ifndef _BASE_PAGE_
#define _BASE_PAGE_

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CCBContainer.h"


class BasePage
	: public CCBContainer
{
public:
	virtual void load(void) = 0;
	virtual void preload(void){}
	virtual bool getPreloadDone(void){return true;}
	virtual void unload(void){CCBContainer::unload();}

	bool init(){return true;}
};

#endif//_BASE_PAGE_