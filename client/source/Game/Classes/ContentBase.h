#pragma once
#include "CCBContainer.h"
#include "CCBManager.h"

class ContentBase
	: public CCBContainer::CCBContainerListener
{
public:
	ContentBase(unsigned int id)
		:mID(id)
	{

	};
	virtual ~ContentBase()
	{
		mContentContainer->release();
	};

	virtual void init(void)
	{
		mContentContainer = CCBManager::Get()->createAndLoad(getCcbiFileName());
		mContentContainer->setListener(this);
		mContentContainer->retain();
		refreshContent();
	};

	virtual void refreshContent(void) = 0;

	virtual CCBContainer* getContainerNode(){return mContentContainer;};

	unsigned int getContentID(){return mID;};

	virtual void setContentID(unsigned int id){mID = id;};

protected:
	virtual const std::string& getCcbiFileName(void) const = 0;
	unsigned int mID;
	CCBContainer* mContentContainer;
};