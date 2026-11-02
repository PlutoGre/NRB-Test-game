#include "libOS.h"
#include <windows.h>
#include "inputBox.h"

wchar_t* UTF8_to_UNICODE(const char *utf8_string, int length)
{
	int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, utf8_string, length, NULL, 0);   
	wchar_t* wszString = new wchar_t[wcsLen + 1];
	::MultiByteToWideChar(CP_UTF8, NULL, utf8_string, length, wszString, wcsLen);
	wszString[wcsLen] = '\0';
	return wszString;
}

int _enc_unicode_to_utf8_one(wchar_t unic, std::string& outstr)  
{  

	if ( unic <= 0x0000007F )  
	{  
		// * U-00000000 - U-0000007F:  0xxxxxxx  
		outstr.push_back(unic & 0x7F);  
		return 1;  
	}  
	else if ( unic >= 0x00000080 && unic <= 0x000007FF )  
	{  
		// * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx  
		outstr.push_back(((unic >> 6) & 0x1F) | 0xC0); 
		outstr.push_back((unic & 0x3F) | 0x80);  
		return 2;  
	}  
	else if ( unic >= 0x00000800 && unic <= 0x0000FFFF )  
	{  
		// * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx  
		outstr.push_back( ((unic >> 12) & 0x0F) | 0xE0);  
		outstr.push_back(((unic >>  6) & 0x3F) | 0x80);  
		outstr.push_back( (unic & 0x3F) | 0x80);  
		return 3;  
	}  
	else if ( unic >= 0x00010000 && unic <= 0x001FFFFF )  
	{  
		// * U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  
		outstr.push_back( ((unic >> 18) & 0x07) | 0xF0); 
		outstr.push_back( ((unic >> 12) & 0x3F) | 0x80);
		outstr.push_back( ((unic >>  6) & 0x3F) | 0x80);
		outstr.push_back( (unic & 0x3F) | 0x80);
		return 4;  
	}  
	else if ( unic >= 0x00200000 && unic <= 0x03FFFFFF )  
	{  
		// * U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  

		outstr.push_back( ((unic >> 24) & 0x03) | 0xF8); 
		outstr.push_back( ((unic >> 18) & 0x3F) | 0x80);
		outstr.push_back( ((unic >> 12) & 0x3F) | 0x80);
		outstr.push_back( ((unic >>  6) & 0x3F) | 0x80);  
		outstr.push_back( (unic & 0x3F) | 0x80);  
		return 5;  
	}  
	else if ( unic >= 0x04000000 && unic <= 0x7FFFFFFF )  
	{  
		// * U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
		outstr.push_back( ((unic >> 30) & 0x01) | 0xFC);
		outstr.push_back( ((unic >> 24) & 0x3F) | 0x80); 
		outstr.push_back( ((unic >> 18) & 0x3F) | 0x80);
		outstr.push_back( ((unic >> 12) & 0x3F) | 0x80);  
		outstr.push_back( ((unic >>  6) & 0x3F) | 0x80);  
		outstr.push_back( (unic & 0x3F) | 0x80); 
		return 6;  
	}  

	return 0;  
}


NetworkStatus libOS::getNetWork()
{
	return ReachableViaWWAN;
}
const std::string& libOS::generateSerial()
{
	static std::string ret("defaultSerial");
	return ret;
}


void libOS::showInputbox(bool mulitline)
{
	wchar_t* ret = _InputBox(L"input");
	int length = lstrlen(ret);
	std::string outStr("");
	for(int i=0;i<length;++i)
	{
		_enc_unicode_to_utf8_one(ret[i],outStr);
	}
	_boardcastInputBoxOK(outStr);

}

void libOS::openURL( const std::string& url )
{
	MessageBox(0,L"open a url!!",L"url",MB_OK);
}

void libOS::setWaiting( bool )
{

}

void libOS::emailTo( const std::string& mailto,const std::string & cc ,  const std::string& title, const std::string & body )
{
	std::string out = "mailto:"+mailto+",title:"+title+"\nbody:\n"+body;
	MessageBox(0,L"send a mail",L"send email",MB_OK);
}

void libOS::showMessagebox( const std::string& msg, int tag /*= 0*/ )
{
	wchar_t* out = UTF8_to_UNICODE(msg.c_str(),msg.length());
	MessageBox(NULL, out, L"", MB_OK);
	_boardcastMessageboxOK(tag);
	delete[] out;
}

long libOS::avalibleMemory()
{
	return 256;
}

void libOS::rmdir( const char* path )
{
	std::string removepath(path);
	if(removepath.find_last_of("\\/")+1 == removepath.length())
		removepath = removepath.substr(0,removepath.find_last_of("\\/"));
	SHFILEOPSTRUCTA FileOp;
	FileOp.fFlags = FOF_NOCONFIRMATION;
	FileOp.hNameMappings = NULL;
	FileOp.hwnd = NULL;
	FileOp.lpszProgressTitle = NULL;
	FileOp.pFrom = removepath.c_str();
	FileOp.pTo = NULL;
	FileOp.wFunc = FO_DELETE;
	SHFileOperationA(&FileOp);
}


long long libOS::getFreeSpace()
{
	return -1;
}

const std::string libOS::getDeviceID()
{
	return "win32Device";
}

const std::string libOS::getPlatformInfo()
{
	return "win32PlatformInfo";
}

void libOS::addNotification( const std::string& msg, int secondsdelay,bool daily /*= false*/ )
{

}

void libOS::clearNotification()
{

}


libOS * libOS::m_sInstance = 0;
