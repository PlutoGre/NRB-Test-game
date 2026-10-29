#include "libPlatform.h"
#include "libOS.h"
#include <assert.h>
#include <string>

#if !defined(WIN32) && !defined(ANDROID)
#include <sys/sysctl.h>
#include <mach/mach.h>
#import <SystemConfiguration/SystemConfiguration.h>
#import <netinet/in.h>
#endif

libPlatformManager * libPlatformManager::m_sInstance = 0;

libPlatformManager* libPlatformManager::getInstance()
{
	if(!m_sInstance)m_sInstance = new libPlatformManager;
	return m_sInstance;
}


void libPlatformManager::setPlatform( std::string name )
{
	assert(mPlatforms.find(name)!=mPlatforms.end());
	m_sPlatform = mPlatforms.find(name)->second;	
}

bool libPlatformManager::registerPlatform( std::string name, libPlatform* platform )
{
	assert(mPlatforms.find(name)==mPlatforms.end());
	
	mPlatforms.insert(std::make_pair(name,platform));
	return true;
}

void libPlatform::logout()
{

}

void libPlatform::final()
{

}

void libPlatform::switchUsers()
{

}

void libPlatform::buyGoods( BUYINFO& )
{

}

void libPlatform::openBBS()
{

}

void libPlatform::userFeedBack()
{

}

void libPlatform::gamePause()
{

}


const std::string libPlatform::getPlatformInfo()
{
    return libOS::getInstance()->getPlatformInfo() + "_" + getClientChannel();
}

const std::string& libPlatform::sessionID()
{
	static std::string ret = "";
	return ret;
}

const std::string& libPlatform::nickName()
{
	static std::string ret = "";
	return ret;
}

const unsigned int libPlatform::getPlatformId()
{
	return 0u;
}

