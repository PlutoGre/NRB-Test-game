
#include "stdafx.h"

#include "DataTableManager.h"
#include "StringConverter.h"
#include "math.h"

//-----------------------------------------------------------------------------------------------
//Quality
//-----------------------------------------------------------------------------------------------
cocos2d::ccColor3B getFrameColor(unsigned int quality)
{
	std::string color = VaribleManager::Get()->getSetting("FrameColor_Quality" + StringConverter::toString(quality));
	return StringConverter::parseColor3B(color);
}

cocos2d::CCSprite* getFrameNormalSpirte(unsigned int quality, std::string ext/*=""*/)
{
	cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("FrameNormal_ImageFile" + ext).c_str());
	sprite->setColor(getFrameColor(quality));
	return sprite;
}

cocos2d::CCSprite* getFrameSelectedSpirte(unsigned int quality, std::string ext/*=""*/)
{
	cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("FrameSelected_ImageFile" + ext).c_str());
	sprite->setColor(getFrameColor(quality));
	return sprite;
}

cocos2d::CCSprite* getFrameDisabledSpirte(unsigned int quality, std::string ext/*=""*/)
{
	cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("FrameDisabled_ImageFile" + ext).c_str());
	sprite->setColor(getFrameColor(quality));
	return sprite;
}
cocos2d::CCSprite* getContentUnselectedSpirte(std::string ext/*=""*/)
{
	cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("ContentUnselected_ImageFile_"+ ext).c_str());
	return sprite;
}
cocos2d::CCSprite* getContentSelectedSpirte(std::string ext/*=""*/)
{
	cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(VaribleManager::Get()->getSetting("ContentSelected_ImageFile_"+ ext).c_str());
	return sprite;
}

std::string getQualityImageFile(unsigned int quality)
{
	return VaribleManager::Get()->getSetting("ImageFileQuality" + StringConverter::toString(quality));
}


std::string getQualityFrameNormalImageFile()
{
	return VaribleManager::Get()->getSetting("FrameNormal_ImageFile");
}

std::string getQualityFrameSelectImageFile()
{
	return VaribleManager::Get()->getSetting("FrameSelected_ImageFile");
}
std::string getVipLevelImageFile(unsigned int level)
{
	return VaribleManager::Get()->getSetting("Vip_Level_" + StringConverter::toString(level));
}
std::string getTeamDefaultPic(void)
{
	return VaribleManager::Get()->getSetting("DefaultIcon_ImageFile_100");
}
std::string getTeamLockPic(void)
{
	return VaribleManager::Get()->getSetting("DefaultIcon_ImageFile_TeamUnlock");
}

//-----------------------------------------------------------------------------------------------
//YuanfenTableManager
//-----------------------------------------------------------------------------------------------
void YuanfenTableManager::init( const std::string& filename )
{
	YuanfenList::iterator itr = mYuanfenList.begin();
	while(itr != mYuanfenList.end())
	{
		delete itr->second;
		itr->second =NULL;
		++itr;
	}
	mYuanfenList.clear();

	parse(filename, 1);
}

//-----------------------------------------------------------------------------------------------
void YuanfenTableManager::readline( std::stringstream& _stream )
{
	YuanfenItem* data = new YuanfenItem;
	data->readline(_stream);
	mYuanfenList.insert(YuanfenList::value_type(data->itemID, data));
}

//-----------------------------------------------------------------------------------------------
const YuanfenItem * YuanfenTableManager::getYuanfenItemByID( unsigned int id )
{
	if(mYuanfenList.find(id) != mYuanfenList.end())
		return mYuanfenList.find(id)->second;

	return NULL;
}

//-----------------------------------------------------------------------------------------------
const YuanfenItem * YuanfenTableManager::getYuanfenItemByName( const std::string& name )
{
	YuanfenList::iterator itr = mYuanfenList.begin();
	while(itr != mYuanfenList.end())
	{
		if(itr->second->name == name)
			return itr->second;
	}

	return NULL;
}

YuanfenTableManager* YuanfenTableManager::getInstance()
{
	return YuanfenTableManager::Get();
}

//-----------------------------------------------------------------------------------------------
//DiscipleTableManager
//-----------------------------------------------------------------------------------------------
void DiscipleTableManager::init( const std::string& filename )
{
	DiscipleList::iterator itr = mDiscipleList.begin();
	while(itr != mDiscipleList.end())
	{
		delete itr->second;
		itr->second =NULL;
		++itr;
	}
	mDiscipleList.clear();

	parse(filename, 1);
}

//-----------------------------------------------------------------------------------------------
void DiscipleTableManager::readline( std::stringstream& _stream )
{
	DiscipleItem* data = new DiscipleItem;
	data->readline(_stream);
	mDiscipleList.insert(DiscipleList::value_type(data->itemID, data));
}

//-----------------------------------------------------------------------------------------------
const DiscipleItem * DiscipleTableManager::getDiscipleItemByID( unsigned int id )
{
	if(mDiscipleList.find(id) != mDiscipleList.end())
		return mDiscipleList.find(id)->second;

	return NULL;
}

//-----------------------------------------------------------------------------------------------
const DiscipleItem * DiscipleTableManager::getDiscipleItemByName( const std::string& name )
{
	DiscipleList::iterator itr = mDiscipleList.begin();
	while(itr != mDiscipleList.end())
	{
		if(itr->second->name == name)
			return itr->second;
	}

	return NULL;
}

const std::string DiscipleTableManager::getYuanfenDescribe( unsigned int id )
{
	std::string describe("");
	if(mDiscipleList.find(id) != mDiscipleList.end())
	{
		DiscipleList::iterator itr = mDiscipleList.find(id);
		DiscipleItem* item = itr->second; 
		if(item)
		{
			for(int i = 0; i<MAX_YUANFEN_NUM; ++i)
			{
				const YuanfenItem * yuanfen = YuanfenTableManager::Get()->getYuanfenItemByID(item->yuanfens[i]);
				if(yuanfen)
					describe += yuanfen->describe + "\n";
			}
		}
	}
	return describe;
}

unsigned int DiscipleTableManager::getRandomDiscipleId()
{
	DiscipleList::iterator it=mDiscipleList.begin();
	int _index=rand()%getTotalNum();
	int i=0;
	while(i<=_index&&it!=mDiscipleList.end())
	{
		it++;
		i++;
	}
	return it->second->itemID;
}

DiscipleTableManager* DiscipleTableManager::getInstance()
{
	return DiscipleTableManager::Get();
}

//-----------------------------------------------------------------------------------------------
//EquipTableManager
//-----------------------------------------------------------------------------------------------
// const std::string& getEquipAtkTypeImageFile( unsigned int type )
// {
// 	{
// 		switch(type)
// 		{
// 		case 1:
// 			return Equip1_ImageFile;
// 		case 2:
// 			return Equip2_ImageFile;
// 		case 3:
// 			return Equip3_ImageFile;
// 		default:
// 			{
// 				return empty;
// 			}
// 		}
// 	}
// }

void EquipTableManager::init( const std::string& filename )
{
	EquipList::iterator itr = mEquipList.begin();
	while(itr != mEquipList.end())
	{
		delete itr->second;
		itr->second =NULL;
		++itr;
	}
	mEquipList.clear();

	parse(filename ,1);
}

//-----------------------------------------------------------------------------------------------
void EquipTableManager::readline( std::stringstream& _stream )
{
	EquipItem* data = new EquipItem;
	data->readline(_stream);
	mEquipList.insert(EquipList::value_type(data->itemID, data));
}

//-----------------------------------------------------------------------------------------------
const EquipItem * EquipTableManager::getEquipItemByID( unsigned int id )
{
	if(mEquipList.find(id) != mEquipList.end())
		return mEquipList.find(id)->second;

	return NULL;
}

//-----------------------------------------------------------------------------------------------
const EquipItem * EquipTableManager::getEquipItemByName( const std::string& name )
{
	EquipList::iterator itr = mEquipList.begin();
	while(itr != mEquipList.end())
	{
		if(itr->second->name == name)
			return itr->second;
	}

	return NULL;
}

unsigned int EquipTableManager::getRandomEquipId()
{
	EquipList::iterator it=mEquipList.begin();
	int _index=rand()%getTotalNum();
	int i=0;
	while(i<=_index&&it!=mEquipList.end())
	{
		it++;
		i++;
	}
	return it->second->itemID;
}

DiscipleTableManager* EquipTableManager::getInstance()
{
	return DiscipleTableManager::Get();
}

//-----------------------------------------------------------------------------------------------
//EquipLevelParamManager
//-----------------------------------------------------------------------------------------------
void EquipLevelParamManager::init( const std::string& filename )
{
	EquipLevelParamList::iterator itr = mEquipLevelParamList.begin();
	while(itr != mEquipLevelParamList.end())
	{
		delete itr->second;
		itr->second =NULL;
		++itr;
	}
	mEquipLevelParamList.clear();

	parse(filename, 1);
}

//-----------------------------------------------------------------------------------------------
void EquipLevelParamManager::readline( std::stringstream& _stream )
{
	EquipLevelParam* data = new EquipLevelParam;
	data->readline(_stream);
	mEquipLevelParamList.insert(EquipLevelParamList::value_type(data->index, data));
}

//-----------------------------------------------------------------------------------------------
const float EquipLevelParamManager::getParamByLevel( unsigned int level )
{
	unsigned int index = ceilf((float)level / 3);
	EquipLevelParamList::iterator itr = mEquipLevelParamList.find(index);
	if(itr == mEquipLevelParamList.end())
		return 0;

	return itr->second->param;
}

EquipLevelParamManager* EquipLevelParamManager::getInstance()
{
	return EquipLevelParamManager::Get();
}

//-----------------------------------------------------------------------------------------------
//SkillTableManager
//-----------------------------------------------------------------------------------------------

// const std::string& getSkillAtkTypeImageFile( unsigned int type )
// {
// 	{
// 		switch(type)
// 		{
// 		case 1:
// 			return Skill1_ImageFile;
// 		case 2:
// 			return Skill2_ImageFile;
// 		case 3:
// 			return Skill3_ImageFile;
// 		case 4:
// 			return Skill4_ImageFile;
// 		case 5:
// 			return Skill5_ImageFile;
// 		case 6:
// 			return Skill6_ImageFile;
// 		default:
// 			{
// 				return empty;
// 			}
// 		}
// 	}
// }

void SkillTableManager::init( const std::string& filename )
{
	SkillList::iterator itr = mSkillList.begin();
	while(itr != mSkillList.end())
	{
		delete itr->second;
		itr->second =NULL;
		++itr;
	}
	mSkillList.clear();

	parse(filename,1);
}

//-----------------------------------------------------------------------------------------------
void SkillTableManager::readline( std::stringstream& _stream )
{
	SkillItem* data = new SkillItem;
	data->readline(_stream);
	mSkillList.insert(SkillList::value_type(data->itemID, data));
}

//-----------------------------------------------------------------------------------------------
const SkillItem * SkillTableManager::getSkillItemByID( unsigned int id )
{
	if(mSkillList.find(id) != mSkillList.end())
		return mSkillList.find(id)->second;

	return NULL;
}

//-----------------------------------------------------------------------------------------------
const SkillItem * SkillTableManager::getSkillItemByName( const std::string& name )
{
	SkillList::iterator itr = mSkillList.begin();
	while(itr != mSkillList.end())
	{
		if(itr->second->name == name)
			return itr->second;
	}

	return NULL;
}

unsigned int SkillTableManager::getRandomSkillId()
{
	SkillList::iterator it=mSkillList.begin();
	int _index=rand()%getTotalNum();
	int i=0;
	while(i<=_index&&it!=mSkillList.end())
	{
		it++;
		i++;
	}
	return it->second->itemID;
}

SkillTableManager* SkillTableManager::getInstance()
{
	return SkillTableManager::Get();
}

//-----------------------------------------------------------------------------------------------
//SkillBookTableManager
//-----------------------------------------------------------------------------------------------

void SkillBookTableManager::init( const std::string& filename )
{
	SkillBookTableMap::iterator itr = mSkillBookList.begin();
	while(itr != mSkillBookList.end())
	{
		SkillBookTable::SkillBookList::iterator it=itr->second->mSkillBookItemList.begin();
		while(it!=itr->second->mSkillBookItemList.end())
		{
			delete it->second;
			it->second=NULL;
			++it;
		}
		itr->second->mSkillBookItemList.clear();
		delete itr->second;
		itr->second =NULL;
		++itr;
	}
	mSkillBookList.clear();

	parse(filename,1);
}

//-----------------------------------------------------------------------------------------------
void SkillBookTableManager::readline( std::stringstream& _stream )
{
	SkillBookTable::SkillBookItem* data = new SkillBookTable::SkillBookItem;
	data->readline(_stream);
	SkillBookTable* skillTable=NULL;
	if(mSkillBookList.find(data->skillId)==mSkillBookList.end())
	{
		skillTable=new SkillBookTable();
		mSkillBookList.insert(SkillBookTableMap::value_type(data->skillId,skillTable));
	}
	else
	{
		skillTable=mSkillBookList.find(data->skillId)->second;
	}
	skillTable->mSkillBookItemList.insert(SkillBookTable::SkillBookList::value_type(data->itemID, data));
}

//-----------------------------------------------------------------------------------------------
const SkillBookTable::SkillBookItem* SkillBookTableManager::getSkillBookItemByID(unsigned int skillId,unsigned int id )
{
	if(mSkillBookList.find(skillId)!=mSkillBookList.end())
	{
		if(mSkillBookList.find(skillId)->second->mSkillBookItemList.find(id)!=mSkillBookList.find(skillId)->second->mSkillBookItemList.end())
		{
			return mSkillBookList.find(skillId)->second->mSkillBookItemList.find(id)->second;
		}
		return NULL;
	}
	return NULL;
}

SkillBookTableManager* SkillBookTableManager::getInstance()
{
	return SkillBookTableManager::Get();
}

//-----------------------------------------------------------------------------------------------
//ToolTableManager
//-----------------------------------------------------------------------------------------------
void ToolTableManager::init( const std::string& filename )
{
	ToolList::iterator itr = mToolList.begin();
	while(itr != mToolList.end())
	{
		delete itr->second;
		itr->second =NULL;
		++itr;
	}
	mToolList.clear();

	parse(filename, 1);
}

//-----------------------------------------------------------------------------------------------
void ToolTableManager::readline( std::stringstream& _stream )
{
	ToolItem* data = new ToolItem;
	data->readline(_stream);
	mToolList.insert(ToolList::value_type(data->itemID, data));
}

//-----------------------------------------------------------------------------------------------
const ToolItem * ToolTableManager::getToolItemByID( unsigned int id )
{
	if(mToolList.find(id) != mToolList.end())
		return mToolList.find(id)->second;

	return NULL;
}

//-----------------------------------------------------------------------------------------------
const ToolItem * ToolTableManager::getToolItemByName( const std::string& name )
{
	ToolList::iterator itr = mToolList.begin();
	while(itr != mToolList.end())
	{
		if(itr->second->name == name)
			return itr->second;
	}

	return NULL;
}
void ToolTableManager::initIncludeOther()
{
	ToolList::iterator it = mToolList.begin();
	for(;it!=mToolList.end();it++)
	{
		try
		{
			if(it->second->includeStr!="none")
			{
				it->second->includeOther = ResManager::Get()->getResInfoListByResBagStr(it->second->includeStr);
			}
		}catch(...)
		{
			cocos2d::CCMessageBox("Tool includeOther string is not match the right fromat!","Error");
		}
	}
	
}

ToolTableManager* ToolTableManager::getInstance()
{
	return ToolTableManager::Get();
}

//-----------------------------------------------------------------------------------------------
//DisciplesLevelParamManager
//-----------------------------------------------------------------------------------------------
void DisciplesLevelParamManager::init( const std::string& filename )
{
	DisciplesLevelParamList::iterator itr = mDisciplesLevelParamList.begin();
	while(itr != mDisciplesLevelParamList.end())
	{
		delete itr->second;
		itr->second =NULL;
		++itr;
	}
	mDisciplesLevelParamList.clear();

	parse(filename, 1);
}

//-----------------------------------------------------------------------------------------------
void DisciplesLevelParamManager::readline( std::stringstream& _stream )
{
	DisciplesLevelParam* data = new DisciplesLevelParam;
	data->readline(_stream);
	mDisciplesLevelParamList.insert(DisciplesLevelParamList::value_type(data->index, data));
}

//-----------------------------------------------------------------------------------------------
const float DisciplesLevelParamManager::getParamByLevel( unsigned int level )
{
	return mDisciplesLevelParamList[ level ]->param;
}

//-----------------------------------------------------------------------------------------------
const float DisciplesLevelParamManager::getTotalParamByLevel( unsigned int level )
{
	float total = 0;
	for(unsigned int i = 1; i < level; ++i)
	{
		total += mDisciplesLevelParamList[ i ]->param;
	}
	return total;
}

DisciplesLevelParamManager* DisciplesLevelParamManager::getInstance()
{
	return DisciplesLevelParamManager::Get();
}

//-----------------------------------------------------------------------------------------------
//HelpTableManager
//-----------------------------------------------------------------------------------------------
void HelpTableManager::init( const std::string& filename )
{
	HelpList::iterator itr = mHelpList.begin();
	while(itr != mHelpList.end())
	{
		delete itr->second;
		itr->second =NULL;
		++itr;
	}
	mHelpList.clear();

	parse(filename, 1);
}

//-----------------------------------------------------------------------------------------------
void HelpTableManager::readline( std::stringstream& _stream )
{
	HelpItem* data = new HelpItem;
	data->readline(_stream);
	mHelpList.insert(HelpList::value_type(data->itemID, data));
}

//-----------------------------------------------------------------------------------------------
const HelpItem * HelpTableManager::getHelpItemByID( unsigned int id )
{
	if(mHelpList.find(id) != mHelpList.end())
		return mHelpList.find(id)->second;

	return NULL;
}

HelpTableManager* HelpTableManager::getInstance()
{
	return HelpTableManager::Get();
}

//-----------------------------------------------------------------------------------------------
//VIPPrivilegeTableManager
//-----------------------------------------------------------------------------------------------
void VIPPrivilegeTableManager::init(const std::string& filename){
	VIPPrivilegeList::iterator itr = mVIPPrivilegeList.begin();
	while(itr != mVIPPrivilegeList.end()){
		delete itr->second;
		itr->second = NULL;
		++itr;
	}
	mVIPPrivilegeList.clear();

	parse(filename, 1);
}
//-----------------------------------------------------------------------------------------------
void VIPPrivilegeTableManager::readline(std::stringstream& _stream){
	VIPPrivilegeItem* data = new VIPPrivilegeItem;
	data->readline(_stream);
	mVIPPrivilegeList.insert(VIPPrivilegeList::value_type(data->itemID,data));
}
//-----------------------------------------------------------------------------------------------
const VIPPrivilegeItem * VIPPrivilegeTableManager::getVIPPrivilegeItemByID(unsigned int id){
	if(mVIPPrivilegeList.find(id) != mVIPPrivilegeList.end()){
		return mVIPPrivilegeList.find(id)->second;
	}
	return NULL;
}
//-----------------------------------------------------------------------------------------------
const VIPPrivilegeItem * VIPPrivilegeTableManager::getVIPPrivilegeDescribeItemByID(unsigned int id,int describeIndex){
	if(mVIPPrivilegeList.find(id) != mVIPPrivilegeList.end()){
		return mVIPPrivilegeList.find(id)->second;
	}
	return NULL;
}

VIPPrivilegeTableManager* VIPPrivilegeTableManager::getInstance()
{
	return VIPPrivilegeTableManager::Get();
}

//-----------------------------------------------------------------------------------------------
//AboutTableManager
//-----------------------------------------------------------------------------------------------
void AboutTableManager::init( const std::string& filename )
{
	AboutList::iterator itr = mAboutList.begin();
	while(itr != mAboutList.end())
	{
		delete itr->second;
		itr->second =NULL;
		++itr;
	}
	mAboutList.clear();

	parse(filename, 1);
}

//-----------------------------------------------------------------------------------------------
void AboutTableManager::readline( std::stringstream& _stream )
{
	AboutItem* data = new AboutItem;
	data->readline(_stream);
	mAboutList.insert(AboutList::value_type(data->itemID, data));
}

//-----------------------------------------------------------------------------------------------
const AboutItem * AboutTableManager::getAboutItemByID( unsigned int id )
{
	if(mAboutList.find(id) != mAboutList.end())
		return mAboutList.find(id)->second;

	return NULL;
}

AboutTableManager* AboutTableManager::getInstance()
{
	return AboutTableManager::Get();
}

//-----------------------------------------------------------------------------------------------
//ContinueLoginTableManager
//-----------------------------------------------------------------------------------------------
void ContinueLoginTableManager::init( const std::string& filename )
{
	ContinueLoginList::iterator itr = mContinueLoginList.begin();
	while(itr != mContinueLoginList.end())
	{
		delete itr->second;
		itr->second =NULL;
		++itr;
	}
	mContinueLoginList.clear();

	parse(filename, 1);
}

//-----------------------------------------------------------------------------------------------
void ContinueLoginTableManager::readline( std::stringstream& _stream )
{
	ContinueLoginItem* data = new ContinueLoginItem;
	data->readline(_stream);
	mContinueLoginList.insert(ContinueLoginList::value_type(data->id, data));
}

//-----------------------------------------------------------------------------------------------
const ContinueLoginItem * ContinueLoginTableManager::getContinueLoginItemByID( unsigned int id )
{
	if(mContinueLoginList.find(id) != mContinueLoginList.end())
		return mContinueLoginList.find(id)->second;
	return NULL;
}

ContinueLoginTableManager* ContinueLoginTableManager::getInstance()
{
	return ContinueLoginTableManager::Get();
}

//-----------------------------------------------------------------------------------------------
//AdventureTableManager
//-----------------------------------------------------------------------------------------------
void AdventureTableManager::init( const std::string& filename )
{
	AdventureList::iterator itr = mAdventureList.begin();
	while(itr != mAdventureList.end())
	{
		delete itr->second;
		itr->second =NULL;
		++itr;
	}
	mAdventureList.clear();

	parse(filename, 1);
}

//-----------------------------------------------------------------------------------------------
void AdventureTableManager::readline( std::stringstream& _stream )
{
	AdventureItem* data = new AdventureItem;
	data->readline(_stream);
	mAdventureList.insert(AdventureList::value_type(data->itemID, data));
}

//-----------------------------------------------------------------------------------------------
const AdventureItem * AdventureTableManager::getAdventureItemByID( unsigned int id )
{
	if(mAdventureList.find(id) != mAdventureList.end())
		return mAdventureList.find(id)->second;

	return NULL;
}

AdventureTableManager* AdventureTableManager::getInstance()
{
	return AdventureTableManager::Get();
}

void SkillItem::readline( std::stringstream& _stream )
{
	std::string		replaceResource;
	_stream
		>>itemID
		>>name
		>>quality
		>>type
		>>describe
		>>initQuantity
		>>initGrowthRate
		>>iconPic
		>>bodyPic
		>>namePic
		//>>backPic
		>>duration
		>>ccbFile
		>>timeLine
		>>replaceResource
		>>soundUrl;
		
	if(!replaceResource.empty())
	{
		size_t  starter = replaceResource.find_first_of('{');
		size_t	ender = replaceResource.find_first_of('}');
		while(starter!=std::string::npos && ender!=std::string::npos && ender>starter+1)
		{
			do 
			{
				std::string sub = replaceResource.substr(starter+1,ender-starter-1);

				size_t colon = sub.find_first_of(":");
				if(colon==std::string::npos || sub.length()<=colon+1)
					break;

				std::string variable = sub.substr(0,colon);
				std::string texture = "";
				std::string plist = "";

				size_t flagAt = sub.find_first_of('@');
				if(flagAt == std::string::npos)
				{
					texture = sub.substr(colon+1,sub.length()-colon-1); 
				}
				else if(flagAt+1<sub.length() && flagAt>colon+1)
				{
					texture = sub.substr(colon+1,flagAt-colon-1);
					plist = sub.substr(flagAt+1,sub.length()-flagAt-1);
				}

				mReplaceMap.insert(std::make_pair(variable, std::make_pair(texture,plist)));

			} while (false);

			starter = replaceResource.find_first_of('{',ender);
			ender =  replaceResource.find_first_of('}',ender+1);
		}
	}
}

void ToolItem::readline( std::stringstream& _stream )
{
	_stream
		>>itemID
		>>name
		>>describe
		>>price
		>>priceType
		>>operationType
		>>jumpPage
		>>buttonString
		>>iconPic
		>>bodyPic
		>>originPrice
		>>pageID
		>>buyCountLimit
		>>includeStr
		>>isStone
		>>showInBag;

	/*size_t  starter = 0;//included.find_first_of('{');
	size_t	ender = included.find_first_of(',');
	while(starter!=std::string::npos && starter<included.length())
	{
		do 
		{
			if(ender == std::string::npos)
				ender = included.length();

			std::string sub = included.substr(starter,ender-starter);

			size_t colon = sub.find_first_of(':');
			if(colon==std::string::npos || sub.length()<=colon+2)
				break;

			std::string iconstr = sub.substr(0,colon);
			
			
			size_t colon2 = sub.find_first_of(':',colon+1);
			if(colon2==std::string::npos || sub.length()<=colon2+2)
				break;

			std::string statStr = sub.substr(colon+1,colon2-colon-1); 
			std::string numStr = sub.substr(colon2+1,sub.length()-colon2-1); 
			//unsigned int id = StringConverter::parseUnsignedInt(idStr.c_str(),0);
			int ct = StringConverter::parseInt(numStr.c_str(),0);
			
			if(ct>0)
			{
				INCLUDED item;
				item.count = ct;
				item.statment = statStr;
				item.icon = iconstr;
				includeOther.push_back(item);
			}

		} while (false);

		starter = ender+1;//included.find_first_of('{',ender);
		ender =  included.find_first_of(',',starter);
	}*/
}
//-----------------------------------------------------------------------------------------------
//AdventureTeacherTableManager
//-----------------------------------------------------------------------------------------------
void AdventureTeacherTableManager::init( const std::string& filename )
{
	AdventureTeacherList::iterator itr = mAdventureTeacherList.begin();
	while(itr != mAdventureTeacherList.end())
	{
		delete itr->second;
		itr->second =NULL;
		++itr;
	}
	mAdventureTeacherList.clear();

	parse(filename, 1);
}

//-----------------------------------------------------------------------------------------------
void AdventureTeacherTableManager::readline( std::stringstream& _stream )
{
	AdventureTeacherItem* data = new AdventureTeacherItem();
	data->readline(_stream);
	mAdventureTeacherList.insert(AdventureTeacherList::value_type(data->itemID, data));
}

//-----------------------------------------------------------------------------------------------
const AdventureTeacherItem * AdventureTeacherTableManager::getAdventureTeacherByID( unsigned int id )
{
	if(mAdventureTeacherList.find(id) != mAdventureTeacherList.end())
		return mAdventureTeacherList.find(id)->second;

	return NULL;
}

AdventureTeacherTableManager* AdventureTeacherTableManager::getInstance()
{
	return AdventureTeacherTableManager::Get();
}

//------------------------------------------------------------------------------------------------
//GradeTableManager
//------------------------------------------------------------------------------------------------
void PlayerGradeTableManager::init(const std::string& filename)
{
	GradeItemList::iterator it = mGradeItemList.begin();
	while (it != mGradeItemList.end())
	{
		delete it->second;
		it->second = NULL;
		++it;
	}
	mGradeItemList.clear();
	parse(filename, 1);
}

void PlayerGradeTableManager::readline(std::stringstream& _stream)
{
	PlayerGradeItem* data = new PlayerGradeItem;
	data->readLine(_stream);
	mGradeItemList.insert(GradeItemList::value_type(data->ID, data));
}

const PlayerGradeItem* PlayerGradeTableManager::getGradeItemByID(unsigned int id)
{
	if (mGradeItemList.find(id) != mGradeItemList.end())
	{
		return mGradeItemList.find(id)->second;
	}

	return NULL;
}

PlayerGradeTableManager* PlayerGradeTableManager::getInstance()
{
	return PlayerGradeTableManager::Get();
}

//------------------------------------------------------------------------------------------------
//TitleTableManager
//------------------------------------------------------------------------------------------------
void TitleTableManager::init(const std::string& filename)
{
	TitleItemList::iterator it = mTitleItemList.begin();
	while (it != mTitleItemList.end())
	{
		delete it->second;
		it->second = NULL;
		++it;
	}
	mTitleItemList.clear();
	parse(filename, 1);
}

void TitleTableManager::readline(std::stringstream& _stream)
{
	TitleItem* data = new TitleItem;
	data->readLine(_stream);
	mTitleItemList.insert(TitleItemList::value_type(data->itemID, data));
}

const TitleItem* TitleTableManager::getTitleItemByID(unsigned int id)
{
	if (mTitleItemList.find(id) != mTitleItemList.end())
	{
		return mTitleItemList.find(id)->second;
	}

	return NULL;
}

TitleTableManager* TitleTableManager::getInstance()
{
	return TitleTableManager::Get();
}

//------------------------------------------------------------------------------------------------
//TeamBuffBoldnessTableManager
//------------------------------------------------------------------------------------------------
void TeamBuffBoldnessTableManager::init(const std::string& filename)
{
	TeamBuffBoldnessItemList::iterator it = mTeamBuffBoldnessList.begin();
	while (it != mTeamBuffBoldnessList.end())
	{
		delete it->second;
		it->second = NULL;
		++it;
	}
	mTeamBuffBoldnessList.clear();
	parse(filename, 1);
}

void TeamBuffBoldnessTableManager::readline(std::stringstream& _stream)
{
	TeamBuffBoldnessItem* data = new TeamBuffBoldnessItem;
	data->readLine(_stream);
	mTeamBuffBoldnessList.insert(TeamBuffBoldnessItemList::value_type(data->quality, data));
}

const TeamBuffBoldnessItem* TeamBuffBoldnessTableManager::getTeamBuffBoldnessItemByQuality(unsigned int id)
{
	if (mTeamBuffBoldnessList.find(id) != mTeamBuffBoldnessList.end())
	{
		return mTeamBuffBoldnessList.find(id)->second;
	}

	return NULL;
}

TeamBuffBoldnessTableManager* TeamBuffBoldnessTableManager::getInstance()
{
	return TeamBuffBoldnessTableManager::Get();
}

//------------------------------------------------------------------------------------------------
//TeamBuffTableManager
//------------------------------------------------------------------------------------------------
void TeamBuffTableManager::init(const std::string& filename)
{
	TeamBuffItemList::iterator it = mTeamBuffList.begin();
	while (it != mTeamBuffList.end())
	{
		delete it->second;
		it->second = NULL;
		++it;
	}
	mTeamBuffList.clear();

	parse(filename, 1);
}

void TeamBuffTableManager::readline(std::stringstream& _stream)
{
	TeamBuffItem* data = new TeamBuffItem;
	data->readLine(_stream);
	mTeamBuffList.insert(TeamBuffItemList::value_type(data->buffType, data));
}

const TeamBuffItem* TeamBuffTableManager::getTeamBuffItemByID(unsigned int id)
{
	if (mTeamBuffList.find(id) != mTeamBuffList.end())
	{
		return mTeamBuffList.find(id)->second;
	}

	return NULL;
}

unsigned int TeamBuffTableManager::getAdditionValueByIdAndLevel(unsigned int id,unsigned int level)
{
	const TeamBuffItem* teamItem=getTeamBuffItemByID(id);
	if(teamItem)
	{
		return teamItem->addition*level;
	}
	return 0;
}


unsigned int TeamBuffTableManager::getBoldnessByIdAndLevel(unsigned int id,unsigned int level)
{
	const TeamBuffItem* teamItem=getTeamBuffItemByID(id);
	if(teamItem)
	{
		if(teamItem->buffType==TeamBuffItem::HP)
		{
			return (level+1)*4;
		}
		if(teamItem->buffType==TeamBuffItem::ATTACK)
		{
			return (level+1)*4;
		}
		if(teamItem->buffType==TeamBuffItem::DEFENSE)
		{
			return (level+1)*4;
		}
		if(teamItem->buffType==TeamBuffItem::WILL)
		{
			return (level+1)*4;
		}
		return 0;
	}
	return 0;
}

TeamBuffTableManager* TeamBuffTableManager::getInstance()
{
	return TeamBuffTableManager::Get();
}

//------------------------------------------------------------------------------------------------
//AnnouncementTableManager
//------------------------------------------------------------------------------------------------
void AnnouncementTableManager::init(const std::string& fileName)
{
	AnnouncementItemList::iterator it = mAnnouncementItemList.begin();
	if(it != mAnnouncementItemList.end())
	{
		delete it->second;
		it->second = NULL;
		++it;
	}

	mAnnouncementItemList.clear();
	parse(fileName, 1);
}

void AnnouncementTableManager::readline(std::stringstream& _stream)
{
	AnnouncementItem* item = new AnnouncementItem;
	item->readLine(_stream);
	mAnnouncementItemList.insert(AnnouncementItemList::value_type(item->itemId, item));
}

const AnnouncementItem* AnnouncementTableManager::getAnnouncementItemById(unsigned int id)
{
	if(mAnnouncementItemList.find(id) != mAnnouncementItemList.end())
	{
		return mAnnouncementItemList.find(id)->second;
	}

	return NULL;
}

AnnouncementTableManager* AnnouncementTableManager::getInstance()
{
	return AnnouncementTableManager::Get();
}

//------------------------------------------------------------------------------------------------
//UserPropertyTableManager
//------------------------------------------------------------------------------------------------
void UserPropertyTableManager::init(const std::string& filename)
{
	UserPropertyList::iterator it = mUserPropertyList.begin();
	while (it != mUserPropertyList.end())
	{
		delete it->second;
		it->second = NULL;
		++it;
	}
	mUserPropertyList.clear();
	parse(filename, 1);
}
const UserPropertyItem * UserPropertyTableManager::getUserPropertyItemByID( unsigned int id )
{
	if(mUserPropertyList.find(id) != mUserPropertyList.end())
		return mUserPropertyList.find(id)->second;

	return NULL;
}
void UserPropertyTableManager::readline(std::stringstream& _stream)
{
	UserPropertyItem* item = new UserPropertyItem;
	item->readline(_stream);
	mUserPropertyList.insert(UserPropertyList::value_type(item->itemID, item));
}

UserPropertyTableManager* UserPropertyTableManager::getInstance()
{
	return UserPropertyTableManager::Get();
}

//------------------------------------------------------------------------------------------------
//NewActivePromptTableManager
//------------------------------------------------------------------------------------------------
void NewActivePromptTableManager::init(const std::string& filename)
{
	NewActivePromptList::iterator it = mNewActivePromptList.begin();
	while (it != mNewActivePromptList.end())
	{
		delete it->second;
		it->second = NULL;
		++it;
	}
	mNewActivePromptList.clear();
	parse(filename, 1);
}
const NewActivePromptItem * NewActivePromptTableManager::getNewActivePromptItemByID( unsigned int id )
{
	if(mNewActivePromptList.find(id) != mNewActivePromptList.end())
		return mNewActivePromptList.find(id)->second;

	return NULL;
}
void NewActivePromptTableManager::readline(std::stringstream& _stream)
{
	NewActivePromptItem* item = new NewActivePromptItem;
	item->readline(_stream);
	mNewActivePromptList.insert(NewActivePromptList::value_type(item->itemID, item));
}

NewActivePromptTableManager::NewActivePromptList * NewActivePromptTableManager::getNewAcrivePromptItemByLevel(unsigned int level,unsigned int type)
{
	NewActivePromptList::iterator iterator = mNewActivePromptList.begin();
	NewActivePromptList * list = new NewActivePromptList;
	while(iterator != mNewActivePromptList.end())
	{
		NewActivePromptItem* newActivePromptItem = dynamic_cast<NewActivePromptItem*>(iterator->second);
		if(newActivePromptItem->level == level && newActivePromptItem->type == type)
		{
			list->insert(NewActivePromptList::value_type(newActivePromptItem->itemID,newActivePromptItem));
		}
		iterator++;
	}
	return list;
}


NewActivePromptTableManager::NewActivePromptList * NewActivePromptTableManager::getNewAcrivePromptItemByDropId(unsigned int dropId,unsigned int type)
{
	NewActivePromptList::iterator iterator = mNewActivePromptList.begin();
	NewActivePromptList * list = new NewActivePromptList;
	while(iterator != mNewActivePromptList.end())
	{
		NewActivePromptItem* newActivePromptItem = dynamic_cast<NewActivePromptItem*>(iterator->second);
		if(newActivePromptItem->type == type)
		{
			list->insert(NewActivePromptList::value_type(newActivePromptItem->itemID,newActivePromptItem));
		}
		iterator++;
	}
	return list;
}

NewActivePromptTableManager* NewActivePromptTableManager::getInstance()
{
	return NewActivePromptTableManager::Get();
}

//------------------------------------------------------------------------------------------------
//ArenaRewardRewardTableManager
//------------------------------------------------------------------------------------------------
void ArenaRewardTableManager::init(const std::string& filename)
{
	ArenaRewardList::iterator it = mArenaRewardList.begin();
	while (it != mArenaRewardList.end())
	{
		delete it->second;
		it->second = NULL;
		++it;
	}
	mArenaRewardList.clear();
	parse(filename, 1);
}
const ArenaRewardItem * ArenaRewardTableManager::getArenaRewardItemByID( unsigned int id )
{
	if(mArenaRewardList.find(id) != mArenaRewardList.end())
		return mArenaRewardList.find(id)->second;

	return NULL;
}
void ArenaRewardTableManager::readline(std::stringstream& _stream)
{
	ArenaRewardItem* item = new ArenaRewardItem;
	item->readline(_stream);
	mArenaRewardList.insert(ArenaRewardList::value_type(item->itemID, item));
}

ArenaRewardTableManager* ArenaRewardTableManager::getInstance()
{
	return ArenaRewardTableManager::Get();
}

//------------------------------------------------------------------------------------------------
//FightEndRewardTableManager
//------------------------------------------------------------------------------------------------
void FightEndRewardTableManager::init(const std::string& filename)
{
	FightEndRewardList::iterator it = mFightEndRewardList.begin();
	while (it != mFightEndRewardList.end())
	{
		delete it->second;
		it->second = NULL;
		++it;
	}
	mFightEndRewardList.clear();
	parse(filename, 1);
}
const FightEndRewardItem * FightEndRewardTableManager::getFightEndRewardItemByID( unsigned int id )
{
	if(mFightEndRewardList.find(id) != mFightEndRewardList.end())
		return mFightEndRewardList.find(id)->second;

	return NULL;
}
void FightEndRewardTableManager::readline(std::stringstream& _stream)
{
	FightEndRewardItem* item = new FightEndRewardItem;
	item->readline(_stream);
	mFightEndRewardList.insert(FightEndRewardList::value_type(item->itemID, item));
}

FightEndRewardTableManager* FightEndRewardTableManager::getInstance()
{
	return FightEndRewardTableManager::Get();
}

//------------------------------------------------------------------------------------------------
//AdventureContinueLoginTableManager
//------------------------------------------------------------------------------------------------
void AdventureContinueLoginTableManager::init(const std::string& filename)
{
	AdventureContinueLoginList::iterator it = mAdventureContinueLoginList.begin();
	while (it != mAdventureContinueLoginList.end())
	{
		delete it->second;
		it->second = NULL;
		++it;
	}
	mAdventureContinueLoginList.clear();
	parse(filename, 1);
}
const AdventureContinueLoginItem * AdventureContinueLoginTableManager::getAdventureContinueLoginItemByID( unsigned int id )
{
	if(mAdventureContinueLoginList.find(id) != mAdventureContinueLoginList.end())
		return mAdventureContinueLoginList.find(id)->second;

	return NULL;
}
void AdventureContinueLoginTableManager::readline(std::stringstream& _stream)
{
	AdventureContinueLoginItem* item = new AdventureContinueLoginItem;
	item->readline(_stream);
	mAdventureContinueLoginList.insert(AdventureContinueLoginList::value_type(item->itemID, item));
}

AdventureContinueLoginTableManager* AdventureContinueLoginTableManager::getInstance()
{
	return AdventureContinueLoginTableManager::Get();
}

VaribleManager* VaribleManager::getInstance()
{
	return VaribleManager::Get();
}

//------------------------------------------------------------------------------------------------
//AdventureInviteTableManager
//------------------------------------------------------------------------------------------------
void AdventureInviteTableManager::init(const std::string& filename)
{
	AdventureInviteList::iterator it = mAdventureInviteList.begin();
	while (it != mAdventureInviteList.end())
	{
		delete it->second;
		it->second = NULL;
		++it;
	}
	mAdventureInviteList.clear();
	parse(filename, 1);
}
const AdventureInviteItem * AdventureInviteTableManager::getAdventureInviteItemByID( unsigned int id )
{
	if(mAdventureInviteList.find(id) != mAdventureInviteList.end())
		return mAdventureInviteList.find(id)->second;

	return NULL;
}
void AdventureInviteTableManager::readline(std::stringstream& _stream)
{
	AdventureInviteItem* item = new AdventureInviteItem;
	item->readline(_stream);
	mAdventureInviteList.insert(AdventureInviteList::value_type(item->itemID, item));
}

AdventureInviteTableManager* AdventureInviteTableManager::getInstance()
{
	return AdventureInviteTableManager::Get();
}


//------------------------------------------------------------------------------------------------
// class FortuneComeRewardTableManager
//------------------------------------------------------------------------------------------------
void FortuneComeRewardTableManager::init(const std::string& filename)
{
	FortuneComeRewardList::iterator it = mFortuneComeRewardList.begin();
	while (it != mFortuneComeRewardList.end())
	{
		delete it->second;
		it->second = NULL;
		++it;
	}
	mFortuneComeRewardList.clear();
	parse(filename, 1);
}
const FortuneComeRewardItem * FortuneComeRewardTableManager::getFortuneComeRewardItemByID( unsigned int id )
{
	if(mFortuneComeRewardList.find(id) != mFortuneComeRewardList.end())
		return mFortuneComeRewardList.find(id)->second;

	return NULL;
}
void FortuneComeRewardTableManager::readline(std::stringstream& _stream)
{
	FortuneComeRewardItem* item = new FortuneComeRewardItem;
	item->readline(_stream);
	mFortuneComeRewardList.insert(FortuneComeRewardList::value_type(item->itemID, item));
}

FortuneComeRewardTableManager* FortuneComeRewardTableManager::getInstance()
{
	return FortuneComeRewardTableManager::Get();
}

/**
class StoneTableManager
*/
void StoneTableManager::init(const std::string& filename){
	StoneItemList::iterator itr = mStoneItemList.begin();
	while( itr != mStoneItemList.end()){
		delete itr->second;
		itr->second = NULL;
		++itr;
	}
	mStoneItemList.clear();
	StoneItemList::iterator colorItr = mStoneColorItemList.begin();
	while( colorItr != mStoneColorItemList.end()){
		delete colorItr->second;
		colorItr->second = NULL;
		++colorItr;
	}
	mStoneColorItemList.clear();
	StoneItemList::iterator statusItr = mStoneStatusItemList.begin();
	while( statusItr != mStoneStatusItemList.end()){
		delete statusItr->second;
		statusItr->second = NULL;
		++statusItr;
	}
	mStoneStatusItemList.clear();
	parse(filename,1);
}
const StoneItem * StoneTableManager::getStoneItemByID(unsigned int id){
	if(mStoneItemList.find(id) != mStoneItemList.end()){
		return mStoneItemList.find(id)->second;
	}
	return NULL;
}

const StoneItem * StoneTableManager::getStoneItemBySign(std::string statusSign){
	StoneItemList::iterator itr = mStoneItemList.begin();
	while(itr != mStoneItemList.end()){
		if(itr->second->statusSign == statusSign){
			return itr->second;
		}
		itr++;
	}
}
const StoneItem * StoneTableManager::getStoneItemByStatus(unsigned int status){
	StoneItemList::iterator itr = mStoneItemList.begin();
	while(itr != mStoneItemList.end()){
		if(itr->second->status == status){
			return itr->second;
		}
		itr++;
	}
}

const StoneItem * StoneTableManager::getStoneColorItemByID(unsigned int id){
	if(mStoneColorItemList.find(id) != mStoneColorItemList.end()){
		return mStoneColorItemList.find(id)->second;
	}
	return NULL;
}

const StoneItem * StoneTableManager::getStoneColorItemByStatus(unsigned int status){
	StoneItemList::iterator itr = mStoneColorItemList.begin();
	while(itr != mStoneColorItemList.end()){
		if(itr->second->status == status){
			return itr->second;
		}
		itr++;
	}
}
const StoneItem * StoneTableManager::getStoneColorItemBySign(std::string statusSign){
	StoneItemList::iterator itr = mStoneColorItemList.begin();
	while(itr != mStoneColorItemList.end()){
		if(itr->second->statusSign == statusSign){
			return itr->second;
		}
		itr++;
	}
}

const StoneItem * StoneTableManager::getStoneStatusItemByID(unsigned int id){
	if(mStoneStatusItemList.find(id) != mStoneStatusItemList.end()){
		return mStoneStatusItemList.find(id)->second;
	}
	return NULL;
}

const StoneItem * StoneTableManager::getStoneStatusItemByStatus(unsigned int status){
	StoneItemList::iterator itr = mStoneStatusItemList.begin();
	while(itr != mStoneStatusItemList.end()){
		if(itr->second->status == status){
			return itr->second;
		}
		itr++;
	}
}
const StoneItem * StoneTableManager::getStoneStatusItemBySign(std::string statusSign){
	StoneItemList::iterator itr = mStoneStatusItemList.begin();
	while(itr != mStoneStatusItemList.end()){
		std::string sign = itr->second->statusSign;
		bool bo = (itr->second->statusSign == statusSign);
		if(itr->second->statusSign == statusSign){
			return itr->second;
		}
		itr++;
	}
}

const StoneItem * StoneTableManager::getStoneHoleByID(unsigned int id){
	if(mStoneHoleList.find(id) != mStoneHoleList.end()){
		return mStoneHoleList.find(id)->second;
	}
	return NULL;
}

void StoneTableManager::readline(std::stringstream& _stream){
	StoneItem* item = new StoneItem;
	item->readline(_stream);
	
	if(item->type == "stone"){
		mStoneItemList.insert(StoneItemList::value_type(item->id,item));
	}else if(item->type == "stonecolor"){
		mStoneColorItemList.insert(StoneItemList::value_type(item->id,item));
	}else if(item->type == "stonestatus"){
		mStoneStatusItemList.insert(StoneItemList::value_type(item->id,item));
	}else if(item->type == "hole"){
		mStoneHoleList.insert(StoneItemList::value_type(item->id,item));
	}
	
}
StoneTableManager* StoneTableManager::getInstance(){
	return StoneTableManager::Get();
}

//------------------------------------------------------------------------------------------------
// class VipDataTableManager
//------------------------------------------------------------------------------------------------
void VipDataTableManager::init(const std::string& filename)
{
	VipDataList::iterator it = mVipDataList.begin();
	while (it != mVipDataList.end())
	{
		delete it->second;
		it->second = NULL;
		++it;
	}
	mVipDataList.clear();
	parse(filename, 1);
}
const VipDataItem * VipDataTableManager::getVipDataByID( unsigned int id )
{
	if(mVipDataList.find(id) != mVipDataList.end())
		return mVipDataList.find(id)->second;

	return NULL;
}
void VipDataTableManager::readline(std::stringstream& _stream)
{
	VipDataItem* item = new VipDataItem;
	item->readline(_stream);
	mVipDataList.insert(VipDataList::value_type(item->itemID, item));
}

//------------------------------------------------------------------------------------------------
// class PlatformNameManager
//------------------------------------------------------------------------------------------------
void PlatformNameManager::init(const std::string& filename)
{
	mPlatformList.clear();
	parse(filename, 1);
}

const std::string PlatformNameManager::getPlatNameByID( unsigned int id )
{
	if(mPlatformList.find(id) != mPlatformList.end())
		return mPlatformList.find(id)->second;
	return "Android_default";
}
void PlatformNameManager::readline(std::stringstream& _stream)
{
	int id;
	std::string platName;
	_stream
		>>id
		>>platName;
	mPlatformList.insert(PlatformNameList::value_type(id, platName));
}

PlatformNameManager* PlatformNameManager::getInstance()
{
	return PlatformNameManager::Get();
}

//------------------------------------------------------------------------------------------------
// class BBSConfigManager
//------------------------------------------------------------------------------------------------
void BBSConfigManager::init(const std::string& filename)
{
	mPlatformBBSList.clear();
	parse(filename, 1);
}

bool BBSConfigManager::getBBSOpenStatusByID( unsigned int id )
{
	if(mPlatformBBSList.find(id) != mPlatformBBSList.end())
	{
		if( mPlatformBBSList.find(id)->second==1)
			return true;
	}
	return false;
}
void BBSConfigManager::readline(std::stringstream& _stream)
{
	int id;
	int status;
	_stream
		>>id
		>>status;
	mPlatformBBSList.insert(PlatBBSList::value_type(id, status));
}

BBSConfigManager* BBSConfigManager::getInstance()
{
	return BBSConfigManager::Get();
}

//------------------------------------------------------------------------------------------------
// class ActivityPopTableManager
//------------------------------------------------------------------------------------------------
void ActivityPopTableManager::init(const std::string& filename)
{
	ActivityPopList::iterator it = mActivityPopList.begin();
	while (it != mActivityPopList.end())
	{
		delete it->second;
		it->second = NULL;
		++it;
	}
	mActivityPopList.clear();
	parse(filename, 1);
}
const ActivityPopItem * ActivityPopTableManager::getActivityPopDataByID( unsigned int id )
{
	if(mActivityPopList.find(id) != mActivityPopList.end())
		return mActivityPopList.find(id)->second;

	return NULL;
}
void ActivityPopTableManager::readline(std::stringstream& _stream)
{
	ActivityPopItem* item = new ActivityPopItem;
	item->readline(_stream);
	mActivityPopList.insert(ActivityPopList::value_type(item->activityID, item));
}
