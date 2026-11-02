/****************************************************************************
Copyright (c) 2011 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "Cocos2dxLuaLoader.h"
#include <string>
#include <algorithm>
#include "zlib.h"

using namespace cocos2d;

extern "C"
{
	const char gamekey[]="youaiABC";
	const int blocksize = 1024*8;

	bool encBuffer(unsigned long inSize , unsigned char* inBuffer, unsigned long& outSize, unsigned char*& outBuffer)
	{
		outBuffer = new unsigned char[inSize+1];
		outSize = inSize;
		int ret = compress(outBuffer+1,&outSize,inBuffer,inSize);

		if(ret == Z_OK && outBuffer!=0)
		{
			int keyflag=0;
			int keylength = strlen(gamekey);
			for(int i=0;i<inSize;++i)
			{
				*(outBuffer+i)=(*(outBuffer+i)^gamekey[keyflag]);
				keyflag = (keyflag+1)%keylength;
			}
			outBuffer[0]=inSize/blocksize + 1;//record number of blocks on the first byte
			outSize++;
			return true;
		}
		return false;
	};
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
	};
    int cocos2dx_lua_loader(lua_State *L)
    {
        std::string filename(luaL_checkstring(L, 1));
        size_t pos = filename.rfind(".lua");
        if (pos != std::string::npos)
        {
            filename = filename.substr(0, pos);
        }
        
        pos = filename.find_first_of(".");
        while (pos != std::string::npos)
        {
            filename.replace(pos, 1, "/");
            pos = filename.find_first_of(".");
        }

		std::string encryptFilename = filename+".el";

#ifdef WIN32 
		std::string luaFilename = filename+".lua";
		
		if(CCFileUtils::sharedFileUtils()->isFileExist(CCFileUtils::sharedFileUtils()->fullPathForFilename(luaFilename.c_str())))
		{
			//if lua exist, encrypt lua to el
			luaFilename = CCFileUtils::sharedFileUtils()->fullPathForFilename(luaFilename.c_str());
			unsigned long codeBufferSize = 0;
			unsigned char* codeBuffer = CCFileUtils::sharedFileUtils()->getFileData(cocos2d::CCFileUtils::sharedFileUtils()->fullPathForFilename(luaFilename.c_str()).c_str(), 
				"rb", &codeBufferSize);

			if(codeBuffer)
			{
				unsigned long outBufferSize = 0;
				unsigned char* outBuffer = 0;
				if(encBuffer(codeBufferSize,codeBuffer,outBufferSize,outBuffer))
				{
					encryptFilename = CCFileUtils::sharedFileUtils()->fullPathFromRelativeFile(encryptFilename.c_str(),luaFilename.c_str());
					FILE* fp = fopen(encryptFilename.c_str(), "wb"); //
					size_t return_size = fwrite(outBuffer, 1,outBufferSize, fp);  
					fclose(fp);
					delete[] outBuffer;
				}
				delete[] codeBuffer;
			}
		}
		else if(CCFileUtils::sharedFileUtils()->isFileExist(CCFileUtils::sharedFileUtils()->fullPathForFilename(encryptFilename.c_str()).c_str()))
		{
			//if lua DO NOT exist, decrypt el to lua
			encryptFilename = CCFileUtils::sharedFileUtils()->fullPathForFilename(encryptFilename.c_str());
			unsigned long codeBufferSize = 0;
			unsigned char* codeBuffer = CCFileUtils::sharedFileUtils()->getFileData(encryptFilename.c_str(),"rb", &codeBufferSize);
			if(codeBuffer)
			{
				unsigned long outBufferSize = 0;
				unsigned char* outBuffer = 0;
				if(decBuffer(codeBufferSize,codeBuffer,outBufferSize,outBuffer))
				{
					luaFilename = CCFileUtils::sharedFileUtils()->fullPathFromRelativeFile(luaFilename.c_str(),encryptFilename.c_str());
					FILE* fp = fopen(luaFilename.c_str(), "wb"); //
					size_t return_size = fwrite(outBuffer, 1,outBufferSize, fp);  
					fclose(fp);
					delete[] outBuffer;
				}
				delete[] codeBuffer;
			}
		}
#endif	
		std::string usingname = filename + ".lua";
		filename.append(".el");
        
        unsigned long codeBufferSize = 0;
		
        unsigned char* codeBuffer = CCFileUtils::sharedFileUtils()->getFileData(cocos2d::CCFileUtils::sharedFileUtils()->fullPathForFilename(filename.c_str()).c_str(), 
			"rb", &codeBufferSize);
        
        if (codeBuffer)
        {
			unsigned long outBufferSize = 0;
			unsigned char* outBuffer = 0;
			if(decBuffer(codeBufferSize,codeBuffer,outBufferSize,outBuffer))
			{
				if (luaL_loadbuffer(L, (char*)outBuffer, outBufferSize, usingname.c_str()) != 0)
				{
					luaL_error(L, "error loading module %s from file %s :\n\t%s",
						lua_tostring(L, 1), filename.c_str(), lua_tostring(L, -1));
				}
				delete[] outBuffer;
			}
            delete []codeBuffer;
        }
        else
        {
            CCLog("can not get file data of %s", filename.c_str());
        }
        
        return 1;
    }
}
