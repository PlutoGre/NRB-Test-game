#pragma once
#include <string>
#include <set>

typedef enum {
	NotReachable = 0,
	ReachableViaWiFi,
	ReachableViaWWAN
} NetworkStatus;

class libOSListener
{
public:
	virtual void onInputboxEnter(const std::string& content){}
	virtual void onMessageboxEnter(int tag){}
};

class libOS
{
public:
	long avalibleMemory();
	void rmdir(const char* path);

	const std::string& generateSerial();

	void showInputbox(bool multiline);
	void showMessagebox(const std::string& msg, int tag = 0);

	void openURL(const std::string& url);
	void emailTo(const std::string& mailto, const std::string & cc , const std::string& title, const std::string & body);

	void setWaiting(bool);

	long long getFreeSpace();

	NetworkStatus getNetWork();
	
	void clearNotification();
    void addNotification(const std::string& msg, int secondsdelay,bool daily = false);
	
	const std::string getDeviceID();
	const std::string getPlatformInfo();
private:
	std::set<libOSListener*> mListeners;
	static libOS *m_sInstance;;
public:
	static libOS* getInstance()
	{
		if(!m_sInstance)
		{
			m_sInstance = new libOS;
		}
		return m_sInstance;
	}


	void registerListener(libOSListener* listerner)
	{
		mListeners.insert(listerner);
	}
	void removeListener(libOSListener* listener)
	{
		mListeners.erase(listener);
	}
	void _boardcastInputBoxOK(const std::string& content)
	{
		std::set<libOSListener*> listeners;
		listeners.insert(mListeners.begin(),mListeners.end());

		std::set<libOSListener*>::iterator it = listeners.begin();
		for(;it!=listeners.end();++it)
		{
			(*it)->onInputboxEnter(content);
		}
	}
	void _boardcastMessageboxOK(int tag)
	{
		std::set<libOSListener*> listeners;
		listeners.insert(mListeners.begin(),mListeners.end());

		std::set<libOSListener*>::iterator it = listeners.begin();
		for(;it!=listeners.end();++it)
		{
			(*it)->onMessageboxEnter(tag);
		}
	}
};

