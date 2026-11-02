/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

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
#include "CCFileUtilsAndroid.h"
#include "support/zip_support/ZipUtils.h"
#include "platform/CCCommon.h"
#include "jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h"

using namespace std;

NS_CC_BEGIN

// record the zip on the resource path
static ZipFile *s_pZipFile = NULL;

CCFileUtils* CCFileUtils::sharedFileUtils()
{
    if (s_sharedFileUtils == NULL)
    {
        s_sharedFileUtils = new CCFileUtilsAndroid();
        s_sharedFileUtils->init();
        std::string resourcePath = getApkPath();
        s_pZipFile = new ZipFile(resourcePath, "assets/");
    }
    return s_sharedFileUtils;
}

CCFileUtilsAndroid::CCFileUtilsAndroid()
{
}

CCFileUtilsAndroid::~CCFileUtilsAndroid()
{
    CC_SAFE_DELETE(s_pZipFile);
}

bool CCFileUtilsAndroid::init()
{
	m_strAppExternalStoragePath = std::string(getAppExternalStoragePath()) + "/";
	m_strAppExternalStorageResourcesPath = std::string(getAppExternalStoragePath()) + "/Assets/";
	//m_searchPathArray.push_back(m_strAppExternalStorageResourcesPath);
	//m_strDefaultResRootPath = m_strAppExternalStorageResourcesPath;
	//不再这里动searchpath了，只到真正从assets zip查文件前，额外先组合一个外部存储资源路径查raw文件
	m_strDefaultResRootPath = "assets/";
	m_strApkAssetsZipPathPrefix = "assets/";
	//
	bool bResult = false;
    bResult = CCFileUtils::init();
    //
    //m_searchPathArray.push_back(m_strApkAssetsZipPathPrefix);
    //
    return bResult;
}

bool CCFileUtilsAndroid::isFileExist(const std::string& strFilePath)
{
    if (0 == strFilePath.length())
    {
        return false;
    }

    bool bFound = false;
    
    // Check whether file exists in apk.
    if (strFilePath[0] != '/')
    {
    	//--begin xinzheng 2013-05-18
    	//先尝试补成外部存储资源路径看下
    	std::string strFullPath = m_strAppExternalStoragePath + strFilePath;//不需要添加PathPrefix，自带了assets/
		CCLOGINFO("CCFileUtilsAndroid::isFileExist 1: %s", strFullPath.c_str());
    	FILE *fp = fopen(strFullPath.c_str(), "r");
		if(fp)
		{
			CCLOGINFO("found CCFileUtilsAndroid::isFileExist 1: %s", strFullPath.c_str());
			bFound = true;
			fclose(fp);
			return bFound;
		}
    	//--end xinzheng 2013-05-18

        std::string strPath = strFilePath;
        //if (strPath.find(m_strDefaultResRootPath) != 0)
        if (strPath.find(m_strApkAssetsZipPathPrefix) != 0)
        {// Didn't find "assets/" at the beginning of the path, adding it.
            //strPath.insert(0, m_strDefaultResRootPath);
        	strPath.insert(0, m_strApkAssetsZipPathPrefix);
        }
		CCLOGINFO("CCFileUtilsAndroid::isFileExist 2: %s", strPath.c_str());
        if (s_pZipFile->fileExists(strPath))
        {
			CCLOGINFO("found CCFileUtilsAndroid::isFileExist 2: %s", strPath.c_str());
            bFound = true;
        } 
    }
    else
    {
		CCLOGINFO("CCFileUtilsAndroid::isFileExist 3: %s", strFilePath.c_str());
        FILE *fp = fopen(strFilePath.c_str(), "r");
        if(fp)
        {
			CCLOGINFO("found CCFileUtilsAndroid::isFileExist 3: %s", strFilePath.c_str());
            bFound = true;
            fclose(fp);
        }
    }
    return bFound;
}

bool CCFileUtilsAndroid::isAbsolutePath(const std::string& strPath)
{
    // On Android, there are two situations for full path.
    // 1) Files in APK, e.g. assets/path/path/file.png
    // 2) Files not in APK, e.g. /data/data/org.cocos2dx.hellocpp/cache/path/path/file.png, or /sdcard/path/path/file.png.
    // So these two situations need to be checked on Android.
    //if (strPath[0] == '/' || strPath.find(m_strDefaultResRootPath) == 0)
	if (strPath[0] == '/' || strPath.find(m_strApkAssetsZipPathPrefix) == 0)
    {
        return true;
    }
	CCLOGINFO("CCFileUtilsAndroid::isAbsolutePath : %s", strPath.c_str());
    return false;
}


unsigned char* CCFileUtilsAndroid::getFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize)
{    
    unsigned char * pData = 0;

    if ((! pszFileName) || (! pszMode) || 0 == strlen(pszFileName))
    {
        return 0;
    }
	CCLOGINFO("CCFileUtilsAndroid::getFileData: %s", pszFileName);
    if (pszFileName[0] != '/')
    {
    	//--begin xinzheng 2013-05-18
		//先尝试补成外部存储资源路径看下
		std::string strFullPath = m_strAppExternalStoragePath + pszFileName;//不需要添加PathPrefix，自带了assets/
		CCLOGINFO("GETTING FILE RELATIVE DATA 1: %s", strFullPath.c_str());
		FILE *fp = fopen(strFullPath.c_str(), pszMode);
		if(fp)
		{
	        do
	        {
	            // read rrom other path than user set it
		        //CCLOG("GETTING FILE ABSOLUTE DATA: %s", pszFileName);
	            //FILE *fp = fopen(pszFileName, pszMode);
	            //CC_BREAK_IF(!fp);

	            unsigned long size;
	            fseek(fp,0,SEEK_END);
	            size = ftell(fp);
	            fseek(fp,0,SEEK_SET);
	            pData = new unsigned char[size];
	            size = fread(pData,sizeof(unsigned char), size,fp);
	            fclose(fp);

	            if (pSize)
	            {
	                *pSize = size;
	            }
	        } while (0);
			CCLOGINFO("got CCFileUtilsAndroid::getFileData 1: %s", strFullPath.c_str());
		}
		else
		{
			//--end xinzheng 2013-05-18
			//CCLOG("GETTING FILE RELATIVE DATA: %s", pszFileName);
			string fullPath = fullPathForFilename(pszFileName);
			//CCLOG("GETTING FILE RELATIVE DATA 2: %s", fullPath.c_str());
			pData = s_pZipFile->getFileData(fullPath.c_str(), pSize);
			CCLOGINFO("got CCFileUtilsAndroid::getFileData 2: %s", fullPath.c_str());
		}
    }
    else
    {
        do 
        {
            // read rrom other path than user set it CCLOGINFO
	        CCLOGINFO("GETTING FILE ABSOLUTE DATA: %s	%s", pszFileName, pszMode);
            FILE *fp = fopen(pszFileName, pszMode);
            CC_BREAK_IF(!fp);

            unsigned long size;
            fseek(fp,0,SEEK_END);
            size = ftell(fp);
            fseek(fp,0,SEEK_SET);
            pData = new unsigned char[size];
            size = fread(pData,sizeof(unsigned char), size,fp);
            fclose(fp);

            if (pSize)
            {
                *pSize = size;
            }           
			CCLOGINFO("got CCFileUtilsAndroid::getFileData 3: %s", pszFileName);
        } while (0);    
    }

    if (! pData)
    {
        std::string msg = "Get data from file(";
        msg.append(pszFileName).append(") failed!");
        CCLOG(msg.c_str());
    }

    return pData;
}

string CCFileUtilsAndroid::getWritablePath()
{

	//--begin xinzheng 2013-5-20
	return getAppExternalStoragePath();
	//--end
    // Fix for Nexus 10 (Android 4.2 multi-user environment)
    // the path is retrieved through Java Context.getCacheDir() method
    string dir("");
    string tmp = getFileDirectoryJNI();

    if (tmp.length() > 0)
    {
        dir.append(tmp).append("/");

        return dir;
    }
    else
    {
        return "";
    }
}

NS_CC_END
