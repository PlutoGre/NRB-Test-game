#pragma once

#include "Singleton.h"
#include <string>
#include <map>
#include <list>
#include "BasePage.h"


class CCBManager :public Singleton<CCBManager>
{
public:
	CCBManager();
	~CCBManager(void);
	/*
	*/
	bool registerPage(const std::string& name, BasePage* page);
	/*
	*/
	void unRegisterPage(const std::string& name, BasePage* page);
	/*
	*/
	BasePage* getPage(const std::string& name);

	CCBContainer* createAndLoad(const std::string & ccbfile);
	//

	void purgeCachedData(void);

private:
	typedef std::map<std::string,BasePage*> PAGE_MAP;
	PAGE_MAP mPages;
	typedef std::list<CCBContainer*> CCBLIST;
	typedef std::map<std::string, CCBLIST > CCBMAP;
	CCBMAP mCCBPool;
};


#define REGISTER_PAGE(_NAME_,_CLASS_) \
	_CLASS_ *__page_##_CLASS_ = _CLASS_::create(); \
	bool __ret_##_CLASS_ = CCBManager::Get()->registerPage(_NAME_,__page_##_CLASS_);

void registerScriptPage(const std::string& ccbfile);