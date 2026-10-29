#pragma once
#include <string>
#include <list>
#include "Singleton.h"

class RestrictedWord : public Singleton<RestrictedWord>
{
private:
	std::string mFilename;
	std::list<std::string> mRestrictList;

public:
	RestrictedWord(void);
	~RestrictedWord(void);

	static RestrictedWord* getInstance(){return RestrictedWord::Get();}

	void init(const std::string& filename);
	void reload(){init(mFilename);}

	bool isStringOK(const std::string& str);
};

