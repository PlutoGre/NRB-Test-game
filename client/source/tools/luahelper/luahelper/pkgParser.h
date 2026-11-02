#pragma once

#include <string>
#include <list>
#include <map>

struct pkgClass
{
	std::string classname;
	std::string content;
	std::list<std::string> funcions;
};

struct pkgReturn
{
	std::string indexString;
	pkgClass* classPointer;
};
class pkgParser
{
public:
	pkgParser(void);
	~pkgParser(void);

	void openfile(std::string filename);

	void createData();

	std::list<pkgReturn>& search(const std::string& inputstring);
	std::list<pkgReturn>& getLastSearch(){return mSearchReturn;}

	std::string getKeyword(const std::string& inputstring);
	void clear();
private:
	std::map<std::string,pkgClass*> mPkgClasses;

	int _parseClass(char* buff);
	void _parseSingleClass(const std::string& classString);
	void _parseFunctionsAndItems(pkgClass& pkgclass, std::list<std::string> &flags);
	std::string _getClassname(const std::string& classname);

	std::map<std::string,pkgClass*> mAllData;
	std::list<pkgReturn> mSearchReturn;
	std::string mLastSearchStr;
};

