
#include "stdafx.h"

#include "GamePlatform.h"
#include <cstdio>

#ifdef _WINDOWS
#include <direct.h>
#include "zlib/zlib.h"
#else
#include<sys/types.h>
#include<sys/stat.h>
//#include<fcntl.h>
//#include <unistd.h>
#include "zlib.h"
#endif
#include "GameMaths.h"

#include <string>
#include "cocos2d.h"
#include "AES.h"

void saveFileInPath( const std::string& fileInPath, const char* mode, const unsigned char* data, unsigned long size )
{
	FILE* fp = fopen(fileInPath.c_str(), "wb"); //
	int directoryDepth = 50;

	int pathstart = 0;
	int totalLength = fileInPath.length();
	while(!fp && --directoryDepth>0 && pathstart!=std::string::npos && pathstart<totalLength)
	{
		int pathend = fileInPath.find_first_of("\\/",pathstart);
		pathstart = pathend+1;
		if(pathend!=std::string::npos)
		{
			std::string subFilename = fileInPath.substr(0,pathend);
#ifdef _WINDOWS
			mkdir(subFilename.c_str());
#else
			mkdir(subFilename.c_str(), S_IRWXU);
#endif
		}
		fp = fopen(fileInPath.c_str(), mode); 
	}
	size_t return_size = fwrite(data, 1,size, fp);  
	fclose(fp);
}

const char gamekey[]="youaiABC";
const int blocksize = 1024*8;
bool decBuffer(unsigned long inSize , unsigned char* inBuffer, unsigned long& outSize, unsigned char*& outBuffer)
{

	unsigned char* decbuffer = new unsigned char[inBuffer[0]*blocksize];
	outSize = inBuffer[0]*blocksize;
	if(decbuffer)
	{
		int keyflag=0;
		int keylength = strlen(gamekey);
		for(int i=0;i<inSize;++i)
		{
			*(inBuffer+i)=(*(inBuffer+i)^gamekey[keyflag]);
			keyflag = (keyflag+1)%keylength;
		}
		int ret = uncompress(decbuffer,&outSize,inBuffer+1,inSize-1);

		outBuffer = new unsigned char[outSize];
		if(ret == Z_OK && outBuffer!=0)
		{
			memcpy(outBuffer,decbuffer,outSize);
			delete[] decbuffer;
			return true;
		}

		delete[] decbuffer;
	}
	return false;
}

unsigned char* getFileData( const char* pszFileName, const char* pszMode, unsigned long * pSize ,unsigned short* crc)
{
	unsigned char* buffer = cocos2d::CCFileUtils::sharedFileUtils()->getFileData(pszFileName,pszMode,pSize);
	if(buffer && buffer[0]==0xef && buffer[1]==0xfe)
	{
		if(strcmp(pszMode,"rb")!=0 )
		{
			delete[] buffer;
			buffer = cocos2d::CCFileUtils::sharedFileUtils()->getFileData(pszFileName,"rb",pSize);
		}
		/*
		if (false)//几百Kb的 太耗时了
		{
			AES aes;
			unsigned char* dataOut = new unsigned char[*pSize-1];
			dataOut[*pSize-2]=0;

			aes.Decrypt(buffer+2,*pSize-2,dataOut,(AES::byte*)"youai123youai123");
			delete[] buffer;

			return dataOut;
		}
		*/
		{

			unsigned long outBufferSize = 0;
			unsigned char* outBuffer = 0;
			if(crc!=0)
			{
				*crc = GameMaths::GetCRC16((unsigned char*)buffer,*pSize);
			}
			if(decBuffer(*pSize-2,buffer+2,outBufferSize,outBuffer))
			{
				delete []buffer;

				*pSize = outBufferSize;
				return outBuffer;
			}
			delete []buffer;
			buffer = NULL;
		}
		return buffer;
	}
	if(crc!=0)
	{
		*crc = GameMaths::GetCRC16((unsigned char*)buffer,*pSize);
	}
	return buffer;
}
