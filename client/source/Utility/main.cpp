// encryption_aes.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <io.h>
#include <bitset>
#include <utility>
#include <string>
#include <fstream>
#include <iostream>
#include <list>
#include <json/json.h>

#include "zlib/zlib.h"
#include "AES.h"
#include "GameMaths.h"
#include "AsyncSocket.h"
#include "PacketManager.h"
#include "GamePackets.h"
#include "pressureTest.h"

using namespace std;
#define CC_BREAK_IF(cond) if(cond) break

unsigned char* _getFileData(const char* pszFileName, const char* pszMode, unsigned long* pSize)
{
	unsigned char* pBuffer = NULL;
	
	*pSize = 0;
	do 
	{
		// read the file from hardware
		FILE *fp = fopen(pszFileName, pszMode);
		CC_BREAK_IF(!fp);

		fseek(fp,0,SEEK_END);
		*pSize = ftell(fp);
		fseek(fp,0,SEEK_SET);
		pBuffer = new unsigned char[*pSize];
		*pSize = fread(pBuffer,sizeof(unsigned char), *pSize,fp);
		fclose(fp);
	} while (0);

	return pBuffer;
}

#define ARG_OPERATION (argv[1])
#define ARG_OUTFILE (argv[3])
#define ARG_INFILE (argv[2])
#define ARG_PARA (argv[4])
int checkFile(int argc, char* argv[])
{
	ifstream is(ARG_INFILE, ios::in | ios::binary);
	if(!is)
	{
		cerr << "InputFileNotFoundException" << endl;
		return 2;
	}

	is.close();
	return 0;
}
int encrypt(int argc, char* argv[])
{
	if(argc != 5)
		return 1;

	if(checkFile(argc,argv)!=0)
		return 2;
	
	if(!strlen(ARG_PARA)==16)
	{
		cerr << "Key must be 16 byte(128bit)" << endl;
		return 2;
	}

	AES aes;

	unsigned long size = 0;
	unsigned char* dataIn = _getFileData(ARG_INFILE,"rb",&size);
	unsigned char* dataOut = new unsigned char[size+2];
	dataOut[0]=0xef;
	dataOut[1]=0xfe;
	aes.Encrypt(dataIn,size,dataOut+2,(const unsigned char*)(ARG_PARA));

	FILE* fp;
	fp = fopen(ARG_OUTFILE,"wb");
	fwrite(dataOut,1, size+2,fp);
	fclose(fp); 
	delete[] dataOut; 
	delete[] dataIn;
	return 0;
}

int decrypt(int argc, char* argv[])
{

	if(argc != 5)
		return 1;
	if(checkFile(argc,argv)!=0)
		return 2;


	if(!strlen(ARG_PARA)==16)
	{
		cerr << "Key must be 16 byte(128bit)" << endl;
		return 2;
	}

	AES aes;
	unsigned long size = 0;
	unsigned char* dataIn = _getFileData(ARG_INFILE,"rb",&size);
	if(dataIn[0]!=0xef || dataIn[1]!=0xfe)
		printf("error file head!");
	size-=2;
	dataIn;
	unsigned char* data3 = new unsigned char[size];
	aes.Decrypt(dataIn+2,size,data3,(const unsigned char*)(ARG_PARA));

	FILE* fp;
	fp = fopen(ARG_OUTFILE,"wb");
	fwrite(data3,1,size,fp);
	fclose(fp);
	delete[] data3;
	delete[] dataIn;

}
class MyPacketHandler: public PacketHandler
{
public:
	virtual void onReceivePacket(const int opcode, const ::google::protobuf::Message* packet)
	{
		printf("received......opcode:%d\nMessage:\n\n",opcode);
		printf(packet->DebugString().c_str());
	}
	virtual void onSendPacketFailed(const int opcode)
	{
		printf("send packet failed! opcode:%d",opcode);
	}
	virtual void onConnectFailed(std::string ip, int port)
	{
		printf("connection failed! ip:%s port:%d",ip.c_str(),port);
	}
	virtual void onTimeout(const int opcode){};
	virtual void onPacketError(const int opcode){};
};

void protoNet(int opcode)
{
	std::string ip("127.0.0.1");
	int port = 25523;
	PacketManager::Get()->init(ip.c_str(),port);
	//PacketManager::Get()->init("223.203.216.6",25523);// 		

	PacketHandler* handler = new MyPacketHandler;

	PacketManager::Get()->regisiterPacketHandler(OPCODE_PLAYER_LOGINRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_USER_BASIC_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_USER_DISCIPLE_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_USER_EQUIP_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_MARKET_RECRUIT_DISCIPLERET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_UPDATE_CAREERRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_SHOP_ITEM_LISTRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_SHOP_PURCHASE_RETURN_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_SEND_FRIEND_MESSAGERET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_USER_TOOL_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_USER_SOUL_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_TRAIN_DISCIPLERET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_DEAL_TRAIN_STATUSRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_USE_TOOLRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_SWALLOW_DISCIPLERET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_BOARDCAST_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_CHAT_SEND_MSGRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_DISPOSE_USER_FRIENDS_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_DISPOSE_USER_SYSMSG_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_USER_FRIENDS_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_USER_SYSMSG_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_CHATRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_CHANGE_SETTINGRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_CONTINUE_LOGINRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_REWARD_CONTINUE_LOGINRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_USER_SKILL_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_UPGRADE_EQUIPRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_UPGRADE_DISCIPLERET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_UPGRADE_SKILLRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_RECRUIT_DISCIPLERET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_ADD_POWERRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_ADVENTURE_POWERINFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_ADVENTURE_BOUNDLESSHOLEINFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_ADD_ADVENTURE_BOUNDLESSHOLEINFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_UPGRADE_ROLERET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_FIGHTEND_BATTLERET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_FIGHTEND_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_FIGHTEND_ADD_YEST_ATTR_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_FIGHTEND_ADD_TEMP_ATTR_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_FIGHTEND_REWARD_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_FIGHTEND_RANK_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_USER_BATTLE_ARRAY_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_ADD_ADVENTURE_TEACHEXPRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_ADVENTURE_TEACHERINFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_HANDBOOK_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_SKILL_BOOK_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_PIECE_SKILL_BOOKRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_WORLD_BOSS_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_SKILL_BOOK_LOOT_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_SNATCH_SKILL_BOOK_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_TEAM_BUFF_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_UPGRADE_TEAM_BUFF_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_BUY_TOOLSRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_RESET_CAREER_COUNTRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_ADVENTURE_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_ADDPOWER_CAREERRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_ADD_ARENA_NUMRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_CHANE_NAMERET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_ADVENTURE_CONTINUELOGIN_GET_INFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_ADVENTURE_CONTINUELOGIN_REWARDRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_CDKEY_REWARDRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_RECORDREADMSGTIMERET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GUIDE_BUY_TOOLRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GUIDE_RECORDRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_ADD_INVITEKEYRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_USER_INVITESTATUSRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_GET_INVITE_REWARDRET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_UPGRADE_STONEINFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_INLAID_STONEINFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_REMOVE_STONEINFORET_S,handler);
	PacketManager::Get()->regisiterPacketHandler(OPCODE_PUNCH_STONEINFORET_S,handler);
	int _opcode;
	printf("input:\ninstruction:\tn means the opcode to send\n\t\t-1 means disconnet and connect again\n\t\t-n mean send packet continuely\n\t\t-100 means exit\n");
	printf("command:");
	scanf("%d",&_opcode);
	::google::protobuf::Message* msg = PacketManager::Get()->_getDefaultMessage(_opcode);
	bool testClose = true;
	bool bExit = false;
	while(!bExit)
	{
		
		if(_opcode == -1)
		{
			PacketManager::Get()->disconnect();
			PacketManager::Get()->init(ip.c_str(),port);
			msg = 0;
		}
		else if(_opcode<-1)
		{
			int count = -_opcode;
			if(count>10)count = 10;
			::google::protobuf::Message* msgs[10] = {0};
			int opcodes[10] = {0};
			for(int i=0;i<count;++i)
			{
				printf("\nopcode:\n");
				scanf("%d",&opcodes[i]);
				msgs[i] = PacketManager::Get()->_getDefaultMessage(opcodes[i]);
			}
			
			for(int i=0;i<count;++i)
			{
				if(msgs[i] && opcodes[i]>0)
				{
					printf("send......\n%s",msgs[i]->DebugString().c_str());
					PacketManager::Get()->sendPakcet(opcodes[i],msgs[i]);
				}
			}
		}
		else if(_opcode == -100)
		{
			bExit = true;
		}
		else
		{
			msg = PacketManager::Get()->_getDefaultMessage(_opcode);

			if(msg)
			{
				printf("send......\n%s",msg->DebugString().c_str());
				PacketManager::Get()->sendPakcet(_opcode,msg);
			}
		}

		for(int i=0;i<50;++i)
		{
			PacketManager::Get()->update(0.1f);
			Sleep(100);
		}

		printf("command:");
		scanf("%d",&_opcode);
		
	}
	PacketManager::Get()->removePacketHandler(handler);
	PacketManager::Get()->disconnect();
	
};

struct FileInfo
{
	std::string name;
	int crc;
	int size;
	time_t last_write_time;
};

void getFiles(const std::string& rootpath,const std::string& subpath, std::list<FileInfo* > & filelist )
{
	_finddata_t file;
	std::string findfiles = rootpath;
	findfiles.append("/");
	findfiles.append(subpath);
	findfiles.append("/*.*");
	long lf;
	if((lf = _findfirst(findfiles.c_str(), &file))==-1l)//_findfirst返回的是long型; long __cdecl _findfirst(const char *, struct _finddata_t *)
		cout<<"Path not found\n";
	else
	{
		//cout<<"\nfile list:\n";
		while( _findnext( lf, &file ) == 0 )//int __cdecl _findnext(long, struct _finddata_t *);如果找到下个文件的名字成功的话就返回0,否则返回-1
		{
			//cout<<file.name;
			switch (file.attrib)
			{
			case _A_SUBDIR:
				//cout<<" subdir";
				if(strcmp(file.name,"..")!=0 && strcmp(file.name,".svn")!=0)
				{
					std::string thesubpath = subpath+"/"+file.name;
					getFiles(rootpath,thesubpath,filelist);
				}
				break;
			case _A_NORMAL:
			case _A_RDONLY:
			case _A_HIDDEN:
			case _A_SYSTEM:
			default:
				{
					//cout<<" file";
					if (strcmp(file.name,".svn") == 0)
						continue;

					std::string thefile = subpath+"/";
					thefile.append(file.name);
					unsigned long size = 0;
					unsigned char* dataIn = _getFileData((rootpath+"/"+thefile).c_str(),"rb",&size);
					unsigned short crcvalue = GameMaths::GetCRC16(dataIn,size);
					FileInfo *fileinfo = new FileInfo;
					fileinfo->name = thefile;
					fileinfo->crc = crcvalue;
					fileinfo->size = size;
					fileinfo->last_write_time = file.time_write;
					filelist.push_back(fileinfo);
					delete[] dataIn;
					break;
				}
			}
			//cout<<endl;
		}
	}
	_findclose(lf);
}

void subFileList_allSame(std::list<FileInfo* >  &modifyList, const std::list<FileInfo* >& sublist)
{
	std::list<FileInfo* > listmod;
	modifyList.swap(listmod);
	std::list<FileInfo* >::iterator it = listmod.begin();
	for(;it!=listmod.end();++it)
	{
		bool foundIt = false;
		std::list<FileInfo* >::const_iterator its = sublist.begin();
		for(;its!=sublist.end();++its)
		{
			if((*its)->name == (*it)->name &&
				(*its)->crc == (*it)->crc &&
				(*its)->size == (*it)->size && 
				(*its)->last_write_time == (*it)->last_write_time)//最后修改时间不一致，
			{
				foundIt = true;
				break;
			}
		}
		if(!foundIt)
		{
			modifyList.push_back(*it);
		}
	}
}


void subFileList_sameName(std::list<FileInfo* >  &modifyList_ori, const std::list<FileInfo* >& sublist)
{
	std::list<FileInfo* > listmod;
	modifyList_ori.swap(listmod);
	std::list<FileInfo* >::iterator it = listmod.begin();
	for(;it!=listmod.end();++it)
	{
		bool foundIt = false;
		std::list<FileInfo* >::const_iterator its = sublist.begin();
		for(;its!=sublist.end();++its)
		{
			if((*its)->name == (*it)->name)
			{
				foundIt = true;
				break;
			}
		}
		if(!foundIt)
		{
			modifyList_ori.push_back(*it);
		}
	}
}

void _parseUpdateFile( const std::string& severfile ,std::list<FileInfo* >  &modifyList)
{
	Json::Value root;
	Json::Reader jreader;
	Json::Value data;
	unsigned long filesize;
	char* pBuffer = (char*)_getFileData(severfile.c_str(),"rt",&filesize);

	if(!pBuffer)
	{
		printf("FAILED to get Update file!!");
		return;
	}

	jreader.parse(pBuffer,data,false);
	if(	data["version"].empty() || 
		data["version"].asInt()!=1 ||
		data["severVersion"].empty() ||
		data["files"].empty() ||
		!data["files"].isArray())
	{
		printf("FAILED to get Update file!!");
		return;
	}
	{


		Json::Value files = data["files"];
		for(int i=0;i<files.size();++i)
		{
			if(	!files[i]["c"].empty() &&
				!files[i]["f"].empty() &&
				!files[i]["s"].empty())
			{
				FileInfo* fileatt = new FileInfo;
				fileatt->name = files[i]["f"].asString();
				fileatt->crc = files[i]["c"].asInt();
				fileatt->size = files[i]["s"].asInt();

				modifyList.push_back(fileatt);
			}
		}
	}
}
void createVersonFile(
	const std::string& versonfile, 
	const std::string& rootdir, 
	const std::string& fileExt = "", 
	const std::string& comparePath = "", 
	const std::string& _version = "", 
	const std::string& lastVersionFile = "")
{
	std::list<FileInfo* >  orilist;
	if(lastVersionFile!="")
		_parseUpdateFile(lastVersionFile,orilist);

	std::list<FileInfo* >  filelist;
	getFiles(rootdir,"",filelist);

	std::list<FileInfo* >  filelistCmp;
	if(comparePath!="")
	{
		getFiles(comparePath,"",filelistCmp);
		subFileList_allSame(filelist,filelistCmp);
	}

	subFileList_sameName(orilist,filelist);

	std::list<FileInfo* >::iterator itcpy = filelist.begin();
	for(;itcpy!=filelist.end();++itcpy)
	{
		orilist.push_back(*itcpy);
	}

	std::string version = _version;
	if(version == "")
	{
		char versonstr[512];
		printf("Please input the target verson:");
		scanf("%s",versonstr);
		version = versonstr;
	}

	Json::Value fileroot;
	fileroot["version"] = 1;
	fileroot["severVersion"] = version.c_str();
	Json::Value files;

	int iTotalByteSize = 0;

	std::list<FileInfo*>::iterator it = filelist.begin();
	for(;it!=filelist.end();++it)
	{
		std::string ext = fileExt;
		if(!fileExt.empty())
		{
			if(fileExt.find_first_of('.') != 0)
				ext = std::string(".")+fileExt;
			int extpos = (*it)->name.find_last_of('.');
			if((*it)->name=="version.cfg")
			{
				continue;
			}
			if((*it)->name=="/version.cfg")
			{
				continue;
			}

			if(extpos!=-1)
			{
				std::string oriExt = (*it)->name.substr(extpos,(*it)->name.length());
				transform(oriExt.begin(), oriExt.end(), oriExt.begin(), (int (*)(int))tolower);
				transform(ext.begin(), ext.end(), ext.begin(), (int (*)(int))tolower);
				if(ext == oriExt)
					continue;
				if(oriExt == ".dll")
					continue;
				if(oriExt == ".exe")
					continue;
				if(oriExt == ".bat")
					continue;
				if(oriExt == ".log")
					continue;
				if(oriExt == ".pdb")
					continue;
				if(oriExt == ".cfg")
				{
					//version_ios.cfg
					//version_android.cfg
					//version_360.cfg
					std::string strTmp = (*it)->name;
					transform(strTmp.begin(), strTmp.end(), strTmp.begin(), (int (*)(int))tolower);
					if (strTmp.find("version_") == 0 || strTmp.find("/version_") == 0)
					{
						continue;
					}
				}
				if(oriExt == ".lua")
					continue;
			}
		}
		Json::Value unitFile;
		unitFile["f"] = (*it)->name;
		unitFile["c"] = (*it)->crc;
		unitFile["s"] = (*it)->size;
		files.append(unitFile);

		iTotalByteSize += (*it)->size;
	}
	fileroot["files"] = files;
	fileroot["totalByteSize"] = iTotalByteSize;
	
	Json::StyledWriter writer;
	std::string outstr = writer.write(fileroot);
	FILE* fp;
	fp = fopen(versonfile.c_str(),"wb");
	fwrite(outstr.c_str(),1, outstr.size(),fp);
	fclose(fp); 
}


//////////////////////////////////////////////////////////////////////////

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
}

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

//////////////////////////////////////////////////////////////////////////
bool changeElToLuaAndSave(const std::string& filePath, const std::string& destPath)
{
	unsigned long filesize = 0;
	unsigned char* filebuf = _getFileData(filePath.c_str(), "rb", &filesize);

	if(filebuf)
	{
		unsigned long outBufferSize = 0;
		unsigned char* outBuffer = 0;
		if(decBuffer(filesize,filebuf,outBufferSize,outBuffer))
		{
			FILE* fp = fopen(destPath.c_str(), "wb"); //
			size_t return_size = fwrite(outBuffer, 1, outBufferSize, fp);  
			fclose(fp);

			delete[] outBuffer;
			delete[] filebuf;

			return true;
		}
		delete[] outBuffer;
		delete[] filebuf;
	}

	return false;
}

bool changeLuaToElAndSave(const std::string& filePath, const std::string& destPathDir, FileInfo* pInfo, std::string& destPath)
{

	unsigned long filesize = 0;
	unsigned char* filebuf = _getFileData(filePath.c_str(), "rb", &filesize);

	if(filebuf)
	{
		unsigned long outBufferSize = 0;
		unsigned char* outBuffer = 0;
		if(encBuffer(filesize,filebuf,outBufferSize,outBuffer))
		{
			std::string::size_type pos1 = filePath.find_last_of("/");
			std::string::size_type pos2 = filePath.find_last_of(".");

			std::string newfilepath = destPathDir + 
				filePath.substr(pos1, pos2-pos1) + ".el";

			FILE* fp = fopen(newfilepath.c_str(), "wb"); //
			size_t return_size = fwrite(outBuffer, 1, outBufferSize, fp);  
			fclose(fp);
			

			{
				unsigned short crcvalue = GameMaths::GetCRC16(outBuffer,outBufferSize);
				
				char szTemp[512] = {0};
				strcpy(szTemp, pInfo->name.c_str());
				int iLen = pInfo->name.length();
				szTemp[iLen-1] = 0;
				szTemp[iLen-2] = 'l';
				szTemp[iLen-3] = 'e';
				
				pInfo->name = szTemp;
				pInfo->crc = crcvalue;
				pInfo->size = outBufferSize;
			}

			delete[] outBuffer;
			delete[] filebuf;

			destPath = newfilepath;
			return true;
		}
		delete[] outBuffer;
		delete[] filebuf;
	}
	

	return false;
}

bool decryptTxtAndSave(const std::string& filePath, const std::string& destPath)
{
	unsigned long size = 0;
	unsigned char* dataIn = _getFileData(filePath.c_str(), "rb", &size);

	if (dataIn)
	{
		{
			unsigned long outBufferSize = 0;
			unsigned char* outBuffer = 0;
			if(decBuffer(size-2,dataIn+2,outBufferSize,outBuffer))//
			{
				FILE* fp = fopen(destPath.c_str(), "wb"); //
				size_t return_size = fwrite(outBuffer, 1, outBufferSize, fp);  
				fclose(fp);

				delete[] outBuffer;
				delete[] dataIn;

				return true;
			}
			delete[] outBuffer;
			delete[] dataIn;
		}
	}

	return false;
}

bool encryptTxtAndSave(const std::string& filePath, const std::string& destPathDir, FileInfo* pInfo, std::string& destPath)
{
	
	unsigned long size = 0;
	unsigned char* dataIn = _getFileData(filePath.c_str(), "rb", &size);
	
	if (dataIn)
	{
		std::string::size_type pos1 = filePath.find_last_of("/");
		std::string::size_type pos2 = filePath.find_last_of(".");

		std::string newfilepath = destPathDir + 
			filePath.substr(pos1);

		//
		if (false)//几百Kb的太耗时
		{
			AES aes;

			unsigned char* dataOut = new unsigned char[size+2];
			dataOut[0]=0xef;
			dataOut[1]=0xfe;
			aes.Encrypt(dataIn, size, dataOut+2, (AES::byte*)"youai123youai123");

			FILE* fp;
			fp = fopen(newfilepath.c_str(), "wb");
			fwrite(dataOut,1, size+2,fp);
			fclose(fp); 

			delete[] dataOut; 
			delete[] dataIn;
		}
		
		{
			unsigned long outBufferSize = 0;
			unsigned char* outBuffer = 0;
			if(encBuffer(size,dataIn,outBufferSize,outBuffer))//先压缩再加密
			{
				FILE* fp = fopen(newfilepath.c_str(), "wb"); //

				unsigned char head[2];
				head[0] = 0xef;
				head[1] = 0xfe;

				fwrite(head, 1, 2, fp);

				size_t return_size = fwrite(outBuffer, 1, outBufferSize, fp);  
				fclose(fp);

				{
					{
						unsigned char* szTemp = new unsigned char[outBufferSize+5];
						szTemp[0] = 0xef;
						szTemp[1] = 0xfe;
						szTemp[outBufferSize+2] = 0;
						szTemp[outBufferSize+3] = 0;
						szTemp[outBufferSize+4] = 0;
						memcpy(szTemp+2, outBuffer, outBufferSize);

						unsigned short crcvalue = GameMaths::GetCRC16(szTemp,outBufferSize+2);

						pInfo->crc = crcvalue;
						pInfo->size = outBufferSize+2;

						delete[] szTemp;
					}
				}

				delete[] outBuffer;
				delete[] dataIn;

				destPath = newfilepath;
				return true;
			}
			delete[] outBuffer;
			delete[] dataIn;
		}
	}

	return false;
}


void createDiffVersionFile(
		const std::string& newDir,	//参与比较的新资源目录
		const std::string& oldDir,	//参与比较的旧资源目录
		const std::string& difDir,	//比较结果，差异文件输出目录，lua文件转el，txt、cfg加密
		/*const */std::string& difListFile,	//在difDir输出这个文件，json格式差异列表
		const std::string& verionStr,
		const std::string& excludeFileExt = "",
		const std::string& includeFileExt = ""
	)
{
	std::list<FileInfo* >  filelist;
	getFiles(newDir,"",filelist);
	
	//
	std::list<FileInfo* >  filelistCmp;
	getFiles(oldDir,"",filelistCmp);

	//
	subFileList_allSame(filelist,filelistCmp);
	
	//
	std::string decryptDir = difDir + "Decrypt";
	std::string strCmd0 = "mkdir \"" + decryptDir + "\"";
	::system(strCmd0.c_str());
	//
	//filelist now is new added and modified files
	//
	Json::Value fileroot;
	fileroot["version"] = 1;
	fileroot["severVersion"] = verionStr;
	Json::Value files;

	int iTotalByteSize = 0;
	std::string strIncludeFileExt = includeFileExt;
	std::string strExcludeFileExt = excludeFileExt;

	std::list<FileInfo*>::iterator it = filelist.begin();
	for(;it!=filelist.end();++it)
	{
		if((*it)->name=="version.cfg")
		{
			continue;
		}
		if((*it)->name=="/version.cfg")
		{
			continue;
		}
		if((*it)->name=="/.svn")
		{
			continue;
		}

		int extpos = (*it)->name.find_last_of('.');

		if(extpos!=-1)
		{
			std::string oriExt = (*it)->name.substr(extpos,(*it)->name.length());
			transform(oriExt.begin(), oriExt.end(), oriExt.begin(), (int (*)(int))tolower);

			transform(strIncludeFileExt.begin(), strIncludeFileExt.end(), strIncludeFileExt.begin(), (int (*)(int))tolower);
			transform(strExcludeFileExt.begin(), strExcludeFileExt.end(), strExcludeFileExt.begin(), (int (*)(int))tolower);
				
			if(strExcludeFileExt.find(oriExt) != std::string::npos)
				continue;

			if (!strIncludeFileExt.empty() && strIncludeFileExt.find(oriExt) == std::string::npos)
				continue;

			if(oriExt == ".dll")
				continue;
			if(oriExt == ".exe")
				continue;
			if(oriExt == ".bat")
				continue;
			if(oriExt == ".log")
				continue;
			if(oriExt == ".pdb")
				continue;
			if(oriExt == ".php")
				continue;
			if(oriExt == ".cfg")
			{
				//version_ios.cfg
				//version_android.cfg
				//version_360.cfg
				std::string strTmp = (*it)->name;
				transform(strTmp.begin(), strTmp.end(), strTmp.begin(), (int (*)(int))tolower);
				if (strTmp.find("version_") == 0 || strTmp.find("/version_") == 0)
				{
					//continue;
					std::string strCmd = "cp -urf \"" + 
						newDir + (*it)->name + "\" \"" + difDir + (*it)->name + "\"";
					::system(strCmd.c_str());
					continue;
				}
			}

			std::string destPath = difDir + (*it)->name;
			std::string destDir;
			std::string::size_type pos = destPath.find_last_of("/");
			if (pos != 0 && pos != std::string::npos)
			{
				destDir = destPath.substr(0, pos);
				static std::string stDir;
				if (destDir != stDir)
				{
					stDir = destDir;
					std::string strCmd = "mkdir \"" + destDir + "\"";
					::system(strCmd.c_str());
				}
			}

			if (oriExt == ".lua" || oriExt == ".txt" || oriExt == ".cfg")
			{
				std::string destPath11 = decryptDir + (*it)->name;
				std::string destDir11;
				std::string::size_type pos = destPath11.find_last_of("/");
				if (pos != 0 && pos != std::string::npos)
				{
					destDir11 = destPath11.substr(0, pos);
					static std::string stDir11;

					if (destDir11 != stDir11)
					{
						stDir11 = destDir11;
						std::string strCmd = "mkdir \"" + destDir11 + "\"";
						::system(strCmd.c_str());
					}
				}
			}

			//--begin
			//处理lua文件，转el
			if (oriExt == ".lua")
			{
				std::string destPath0;
				std::string tempName = (*it)->name;
				bool bret = changeLuaToElAndSave(newDir + (*it)->name, destDir, (*it), destPath0);
				if (bret)
				{
					changeElToLuaAndSave(destPath0, decryptDir+tempName);
				}
				else
				{
					std::string strCmd = "echo failed: \"" + newDir + (*it)->name + "\"";
					::system(strCmd.c_str());
					//::system("pause");
					strCmd = "cp -urf \"" + 
						newDir + (*it)->name + "\" \"" + destPath + "\"";
					::system(strCmd.c_str());
					//return;
				}
			}
			//处理txt文件，加密
			else if (oriExt == ".txt" || oriExt == ".cfg")
			{
				std::string destPath1;
				bool bret = encryptTxtAndSave(newDir + (*it)->name, destDir, (*it), destPath1);
				if (bret)
				{
					decryptTxtAndSave(destPath1, decryptDir+(*it)->name);
				}
				else
				{
					std::string strCmd = "echo failed: \"" + newDir + (*it)->name + "\"";
					::system(strCmd.c_str());
					//::system("pause");
					strCmd = "cp -urf \"" + 
						newDir + (*it)->name + "\" \"" + destPath + "\"";
					::system(strCmd.c_str());
					//return;
				}
			}
			//only copy
			else
			{
				
				std::string strCmd = "cp -urf \"" + 
					newDir + (*it)->name + "\" \"" + destPath + "\"";
				::system(strCmd.c_str());
			}
			//--end
		}

		Json::Value unitFile;
		unitFile["f"] = (*it)->name;
		unitFile["c"] = (*it)->crc;
		unitFile["s"] = (*it)->size;
		files.append(unitFile);

		iTotalByteSize += (*it)->size;
	}

	fileroot["files"] = files;
	fileroot["totalByteSize"] = iTotalByteSize;

	Json::StyledWriter writer;
	std::string outstr = writer.write(fileroot);

	if (difListFile.find(":") == std::string::npos && 
		difListFile.find(".") != 0)
	{
		difListFile = difDir + "\\" + difListFile;
	}

	FILE* fp;
	fp = fopen(difListFile.c_str(),"wb");
	fwrite(outstr.c_str(),1, outstr.size(),fp);
	fclose(fp);

	::system("pause");
}

int main(int argc, char* argv[])
{
	

	const string USAGE =	"Usage:	Utility [-OPERATION]\n" \
							"	-E:	(Encrypt): Utility -E sourcefile destinationfile key\n" \
							"	-D:	(Decrypt): Utility -D sourcefile destinationfile key\n" \
							"	-C:	(CRC validate): Utility -C checkfile\n" \
							"	-N:	(Network test): Utility -N opcode" \
							"	-V:	(Verson file creation): Utility -V rootdir versonfile excludeFileExtension compareDir version"; \
							"	-P:	(Pressure test): Utility -P type(0:Login 1:PlayerSimulate 2:MassivePlayers"; 
	if(argc>=2 &&(strcmp(ARG_OPERATION, "-P") == 0 || strcmp(ARG_OPERATION, "-p") == 0))
	{
		if(strcmp(ARG_INFILE,"0")==0)
			pressureTest::Get()->testLogin();
		if(strcmp(ARG_INFILE,"1")==0)
			pressureTest::Get()->testPlay();
		if(strcmp(ARG_INFILE,"2")==0)
			pressureTest::Get()->testAmount();
	}
	else if(argc>=3 &&(strcmp(ARG_OPERATION, "-V") == 0 || strcmp(ARG_OPERATION, "-v") == 0))
	{
		if(argc == 5)
			createVersonFile(ARG_OUTFILE,ARG_INFILE);
		else if(argc == 6)
			createVersonFile(ARG_OUTFILE,ARG_INFILE,ARG_PARA);
		else if(argc == 7)
			createVersonFile(ARG_OUTFILE,ARG_INFILE,ARG_PARA,argv[5],argv[6]);
		else
			createVersonFile(ARG_OUTFILE,ARG_INFILE,ARG_PARA,argv[5],argv[6],argv[7]);
	}
	else if(argc>=3 &&(strcmp(ARG_OPERATION, "-VE") == 0 || strcmp(ARG_OPERATION, "-ve") == 0))
	{
		std::string destDiffFile = argv[5];
		std::string includeFileExt = "";
		std::string excludeFileExt = ".el";

		if (argc >= 7)
			excludeFileExt = argv[6];
		if (argc >= 8)
			includeFileExt = argv[7];

		createDiffVersionFile(argv[2],argv[3],argv[4],destDiffFile,excludeFileExt,includeFileExt);
		return 0;
	}

	if(argc>=2 &&(strcmp(ARG_OPERATION, "-N") == 0 || strcmp(ARG_OPERATION, "-n") == 0))
	{
		_gamePacketsCreatePackets();
		//if(ARG_INFILE)
		{
			//int opcode;
			//sscanf(ARG_INFILE,"%d",&opcode);
			protoNet(0);
			return 0;
		}
	}

	if(argc>=3 &&(strcmp(ARG_OPERATION, "-C") == 0 || strcmp(ARG_OPERATION, "-c") == 0))
	{
		if(checkFile(argc,argv)!=0)
			return 2;
		unsigned long size = 0;
		unsigned char* dataIn = _getFileData(ARG_INFILE,"rb",&size);
		unsigned short crcvalue = GameMaths::GetCRC16(dataIn,size);
		printf("CRC:%d",crcvalue);
		return crcvalue;
	}
	

	if(argc>=5 &&(strcmp(ARG_OPERATION, "-E") == 0 || strcmp(ARG_OPERATION, "-e") == 0))
	{
		if(encrypt(argc,argv) !=0)
		{
			cout << USAGE << endl;
			return 1;
		}
	}

	if(argc>=5 && (strcmp(ARG_OPERATION, "-D") == 0 || strcmp(ARG_OPERATION, "-d") == 0))
	{
		if(decrypt(argc,argv) !=0)
		{
			cout << USAGE << endl;
			return 1;
		}
	}

	cout << USAGE << endl;
	return 0;
}

