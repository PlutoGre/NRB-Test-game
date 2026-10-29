
#include "stdafx.h"

#include "CCBManager.h"
#include "cocos2d.h"
#include "CCBScriptContainer.h"
USING_NS_CC;


CCBManager::CCBManager()
{

}

CCBManager::~CCBManager(void)
{
	for(PAGE_MAP::iterator it = mPages.begin();it!=mPages.end();++it)
	{
		/*
			BasePage基本都是以全局变量注册的，
			最后退出时刻了，尝试尽量清理，
			如果下面while出问题，说明BasePage间有依赖或Page与引擎有交错，可以尝试去找找，修正
		*/
		//while (!it->second->isSingleReference())
		//{
		//	it->second->release();
		//}
		it->second->release();
	}
}

bool CCBManager::registerPage( const std::string& name, BasePage* page )
{
	if(mPages.find(name)!=mPages.end())
		CCMessageBox("ERROR! Page name redefined!!","ERROR");

	page->retain();
	mPages.insert(std::make_pair(name,page));

	return true;
}

void CCBManager::unRegisterPage( const std::string& name, BasePage* page )
{
	PAGE_MAP::iterator it = mPages.find(name);
	if (it == mPages.end())
		return;
	//
	page->release();
	mPages.erase(it);
	//
}

BasePage* CCBManager::getPage( const std::string& name )
{
	PAGE_MAP::iterator it = mPages.find(name);
	if(it!=mPages.end())
		return it->second;
	else
		return NULL;
}

CCBContainer* CCBManager::createAndLoad( const std::string & ccbfile )
{
	CCBMAP::iterator mapit = mCCBPool.find(ccbfile);
	if(mapit==mCCBPool.end())
	{
		CCBContainer * ccb = CCBContainer::create();
		ccb->retain();
		ccb->loadCcbiFile(ccbfile);

		CCBLIST clist;
		clist.push_back(ccb);

		mCCBPool.insert(std::make_pair(ccbfile,clist));

		return ccb;
	}
	else
	{
		CCBLIST::iterator listit = mapit->second.begin();
		for(;listit!=mapit->second.end();++listit)
		{
			CCBContainer* ccb = *listit;
			if(ccb->isSingleReference())
			{ 
				ccb->setListener(NULL);
				return ccb;
			}
		}
		
		CCBContainer * ccb = CCBContainer::create();
		ccb->retain();
		ccb->loadCcbiFile(ccbfile);

		mapit->second.push_back(ccb);
		return ccb;
	}
}

void CCBManager::purgeCachedData( void )
{
	CCBMAP::iterator mapit = mCCBPool.begin();
	for(;mapit!=mCCBPool.end();++mapit)
	{
		CCBLIST listSwap;
		listSwap.swap(mapit->second);
		CCBLIST::iterator listit = listSwap.begin();
		for(;listit!=listSwap.end();++listit)
		{
			CCBContainer* ccb = *listit;
			if(ccb->isSingleReference())
			{
				ccb->release();
			}
			else
			{
				mapit->second.push_back(*listit);
			}
		}
	}
}


void registerScriptPage( const std::string& ccbfile )
{
	CCBScriptContainer *pRet = CCBScriptContainer::create(ccbfile);
	CCBManager::Get()->registerPage(ccbfile,pRet);
}

