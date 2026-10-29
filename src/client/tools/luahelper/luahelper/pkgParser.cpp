#include "StdAfx.h"
#include "pkgParser.h"

#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <algorithm>

using namespace std;


pkgParser::pkgParser(void)
{
	clear();
}


pkgParser::~pkgParser(void)
{
}

void pkgParser::openfile( std::string filename )
{
// 	ifstream readf(filename.c_str()); //打开文件
// 	if(!readf.is_open())  //判断是否打开成功
// 	{
// 		return;
// 	}
// 	stack<std::string> sta;
// 	
// 	while(!readf.eof())
// 	{
// 		char buf[1024];
// 		memset(buf,0,1024);
// 		readf>>buf;
// 		std::string bufStr(buf);
// 		sta.push(bufStr);  //将每行入栈
// 	}

	FILE * fp = fopen(filename.c_str(),"rb");
	fseek(fp, 0L, SEEK_END);
	int filelength = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	char * p = (char*)malloc(filelength); 
	fread(p, 1, filelength, fp);

	int startPos = 0;
	while(startPos<filelength)
	{
		startPos+=_parseClass(p+startPos);
	}
	free(p);
	fclose(fp);
}

int pkgParser::_parseClass( char* buff )
{
	std::string buffStr(buff);
	int classCount = 0;

	size_t endBuf = buffStr.find_first_of("{}");
	do 
	{
		if(endBuf!=std::string::npos)
		{
			if(endBuf!=std::string::npos && buffStr[endBuf]=='{')
			{
				classCount++;
				endBuf = buffStr.find_first_of("{}",endBuf+1);
			}
			if(endBuf!=std::string::npos && buffStr[endBuf]=='}')
			{
				classCount--;
				if(classCount>0)
					endBuf = buffStr.find_first_of("{}",endBuf+1);
			}
		}
	} while (classCount > 0 && endBuf!=std::string::npos);
	
	
	if(endBuf!=std::string::npos)
	{
		std::string singleclass = buffStr.substr(0,endBuf+1);
		_parseSingleClass(singleclass);

		return endBuf+1;
	}
	_parseSingleClass(buffStr);
	return buffStr.length();
}

void pkgParser::_parseSingleClass( const std::string& classString )
{
	std::list<std::string> flags;
	size_t startoffset = 0;
	size_t endoffset = 0;
	while(endoffset<classString.length())
	{
		startoffset = classString.find_first_not_of("\n\r",startoffset);
		endoffset = classString.find_first_of("\n\r",startoffset);
		if(startoffset < string::npos && endoffset < string::npos)
		{
			flags.push_back(classString.substr(startoffset,endoffset-startoffset));
		}
		else if(startoffset < string::npos)
		{
			flags.push_back(classString.substr(startoffset,classString.length()-startoffset));
		}
		startoffset = endoffset+1;
	}

	std::list<std::string> globeflags;
	while(!flags.empty() && !strstr(flags.front().c_str(),"class") && !strstr(flags.front().c_str(),"struct") && !strstr(flags.front().c_str(),"message "))
	{
		globeflags.push_back(flags.front());
		flags.pop_front();
	}
	
	//mPkgClasses[""] = new pkgClass;
	mPkgClasses[""]->classname = "";
	mPkgClasses[""]->content = "";
	_parseFunctionsAndItems(*mPkgClasses[""],globeflags);

	if(!flags.empty())
	{

		pkgClass* theClass = new pkgClass;
		theClass->content = classString;

		theClass->classname = _getClassname(flags.front());

		while(!flags.empty() && strstr(flags.front().c_str(),"{")==0)flags.pop_front();//:public xxxclass


		_parseFunctionsAndItems(*theClass,flags);

		mPkgClasses.insert(std::make_pair(theClass->classname,theClass));
	}
}

void pkgParser::_parseFunctionsAndItems( pkgClass& theClass, std::list<std::string> &flags )
{
	while(!flags.empty())
	{
		std::string line = flags.front();
		flags.pop_front();

		size_t flag = line.find_first_of('/');
		if(flag!=string::npos && flag<line.length()-1 && '/' == line[flag+1])
			line = line.substr(0,flag);

		flag = line.find_first_of('(');
		if(flag!=string::npos)
		{
			size_t flagend = line.find_first_of(')');
			if(flagend!=string::npos)
				line = line.substr(0,flagend+1)+";";
		}

		size_t endflag = line.find_first_of(';');
		if(endflag==string::npos)continue;
		line = line.substr(0,endflag+1);

		if(line.length()<3)continue;

		string::size_type pos = 0;  
		string::size_type srcLen = line.size();  
		
		pos=line.find_first_of('\t', pos);   
		while ((pos != string::npos))  
		{  
			line.replace(pos, 1,1, ' ');  
			pos=line.find_first_of('\t', pos+1);  
		}  
		theClass.funcions.push_back(line);
		
	}
}

std::list<pkgReturn>& pkgParser::search( const std::string& inputstring )
{
	bool restartSearch = true;
	if(mLastSearchStr!="" && strstr(inputstring.c_str(),mLastSearchStr.c_str()))
	{
		restartSearch = false;
	}

	std::list<std::string> flags;
	size_t startoffset = 0;
	size_t endoffset = 0;
	while(endoffset<inputstring.length())
	{
		endoffset = inputstring.find_first_of(" ",startoffset);
		std::string key = inputstring.substr(startoffset,endoffset-startoffset);
		std::transform(key.begin(),key.end(),key.begin(),tolower);

		flags.push_back(key);

		if(endoffset < string::npos)
		{
			startoffset = endoffset+1;	
		}
	}

	if(/*restartSearch*/true)
	{
		mSearchReturn.clear();
		std::map<std::string,pkgClass*>::iterator it = mAllData.begin();
		for(;it!=mAllData.end();++it)
		{
			bool isOK = true;
			std::list<std::string>::iterator itkey = flags.begin();
			for(;itkey!=flags.end();++itkey)
			{
				std::string targetStr = it->first;
				std::transform(targetStr.begin(),targetStr.end(),targetStr.begin(),tolower);

				if(!strstr(targetStr.c_str(),itkey->c_str()))
				{
					isOK = false;
					break;
				}
			}
			if(isOK)
			{
				std::string classcontent = it->second->content;
				pkgReturn ret;
				ret.indexString = it->first;
				ret.classPointer = it->second;
				mSearchReturn.push_back(ret);
			}
		}
	}
	return mSearchReturn;
}

void pkgParser::createData()
{
	mAllData.clear();

	std::map<std::string,pkgClass*>::iterator it = mPkgClasses.begin();
	for(;it!=mPkgClasses.end();++it)
	{
		std::string classname = it->second->classname;
		mAllData.insert(std::make_pair(classname,it->second));
		
		std::list<std::string>::iterator it2 = it->second->funcions.begin();
		for(;it2!=it->second->funcions.end();++it2)
		{
			std::string funcname = it->second->classname+":"+*it2;
			mAllData.insert(std::make_pair(funcname,it->second));
		}
	}
}

std::string pkgParser::_getClassname( const std::string& classname )
{
	size_t startoffset = 0;
	size_t endoffset = 0;
	while(endoffset<classname.length())
	{
		startoffset = classname.find_first_not_of("\n\r\t ",startoffset);
		endoffset = classname.find_first_of("\n\r\t ",startoffset);
		if(startoffset < string::npos && endoffset < string::npos)
		{
			if(classname.substr(startoffset,endoffset-startoffset) == "class")
			{
				startoffset = endoffset+1;
				endoffset = classname.find_first_of("\n\r\t ",startoffset);
				if(endoffset>startoffset)
				{
					return classname.substr(startoffset,endoffset-startoffset);
				}
			}
			if(classname.substr(startoffset,endoffset-startoffset) == "struct")
			{
				startoffset = endoffset+1;
				endoffset = classname.find_first_of("\n\r\t ",startoffset);
				if(endoffset>startoffset)
				{
					return classname.substr(startoffset,endoffset-startoffset);
				}
			}
			if(classname.substr(startoffset,endoffset-startoffset) == "message")
			{
				startoffset = endoffset+1;
				endoffset = classname.find_first_of("\n\r\t ",startoffset);
				if(endoffset>startoffset)
				{
					return classname.substr(startoffset,endoffset-startoffset);
				}
			}
		}
		startoffset = endoffset+1;
	}
	return "";
}

void pkgParser::clear()
{
	mPkgClasses.clear();
	mAllData.clear();
	mSearchReturn.clear();
	mLastSearchStr = "";
	pkgClass *globe = new pkgClass;
	mPkgClasses.insert(std::make_pair("",globe));
}

std::string pkgParser::getKeyword( const std::string& inputstring )
{
	std::string instr = inputstring;

	size_t endpos = instr.find_first_of('(');
	if(endpos!=string::npos)
	{
		instr = instr.substr(0,endpos);
		
		endpos = instr.find_last_not_of(" \t");
		size_t startpos = instr.find_last_of(" \t",endpos);
		if(startpos == string::npos)
			startpos = 0;

		return instr.substr(startpos+1,endpos-startpos)+"()";
	}
	else
	{
		size_t startpos = instr.find_last_of(" \t");
		if(startpos == string::npos)
			startpos = 0;

		endpos = instr.find_first_of(';',startpos+1);
		if(endpos!=string::npos)
			return instr.substr(startpos+1,endpos-startpos-1);
	}
	
	

	return "";

}
