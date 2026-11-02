
#include "stdafx.h"

#include "AdventureBase.h"
USING_NS_CC;
AdventureBase::AdventureBase( unsigned int id )
	:mID(id)
{
	mIsInitData=false;
}

AdventureBase::~AdventureBase()
{
	mContentContainer->release();
}

void AdventureBase::init(void)
{
	mContentContainer = CCBContainer::create();
	mContentContainer->loadCcbiFile(getCcbiFileName());
	mContentContainer->setListener(this,getContentID());
	mContentContainer->retain();
	refreshContent();
}

cocos2d::CCObject * AdventureBase::getObjectByVariableName(const std::string& variableName)
{
	cocos2d::CCObject* obj = getContainerNode()->getVariable(variableName);
	if(!obj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s%s","Failed to find variable:",variableName.c_str());
		CCMessageBox(errMsg,"error");
	}
	return obj;
}