
#include "stdafx.h"

#include "RestrictedWord.h"
#include "TableReader.h"
#include "GameMaths.h"

RestrictedWord::RestrictedWord(void)
{
}


RestrictedWord::~RestrictedWord(void)
{
}

class ResWordReader : public TableReader
{
	std::list<std::string>& limitlist;
public:
	ResWordReader(std::list<std::string>& _list):limitlist(_list){}

	virtual void readline(std::stringstream& _stream) 
	{
		int id;
		std::string word;
		_stream
			>>id
			>>word;
		if(id<10000 && !word.empty())
			limitlist.push_back(word);
	}

};
void RestrictedWord::init( const std::string& filename )
{
	ResWordReader reader(mRestrictList);
	reader.parse(filename,1);
}


bool RestrictedWord::isStringOK( const std::string& str )
{
	const char* strChar = str.c_str();
	for(int i=0;i<str.length();++i)
	{
		unsigned char cha = (*strChar);
		if((cha&0x80) == 0)
		{
			bool isOK = false;
			if(cha>='0' && cha<='9')//number
				isOK = true;
			if(cha>='a' && cha<='z')//number
				isOK = true;
			if(cha>='A' && cha<='Z')//
				isOK = true;
			if(cha=='.')//dot
				isOK = true;

			if(isOK == false)
				return false;
		}
		strChar++;
	}
	std::list<std::string>::iterator it = mRestrictList.begin();
	for(;it!=mRestrictList.end();++it)
	{
		std::map<int, std::string> strMap;
		GameMaths::readStringToMap(*it,strMap);
		const char* start = str.c_str();
		size_t offset = 0;
		std::map<int, std::string>::iterator chaIt = strMap.begin();
		for(;chaIt!=strMap.end();++chaIt)
		{
			std::string& cha = chaIt->second;
			while(offset<str.length() && strncmp(start,cha.c_str(),cha.length())!=0)
			{
				start++;
				offset++;
			}
			if(offset>=str.length())
				break;
		}

		if(offset<str.length())
			return false;
	}
	return true;

}
