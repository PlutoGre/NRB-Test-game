
#include "stdafx.h"

#include "SkillBookSurprisePage.h"
#include "AdventurePage.h"
#include "PopupManager.h"
REGISTER_PAGE("SkillBookSurprisePage",SkillBookSurprisePage);
USING_NS_CC;


cocos2d::CCObject * SkillBookSurprisePage::getObjectByVariableName(const std::string& variableName)
{
	cocos2d::CCObject* obj = getVariable(variableName);
	if(!obj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s%s","Failed to find variable:",variableName.c_str());
		CCMessageBox(errMsg,"error");
	}
	return obj;
}


SkillBookSurprisePage::SkillBookSurprisePage(void)
{
	mTitle=NULL;
	mSummary=NULL;
	mNumber1=NULL;
	mDescription=NULL;
	//mPopMsg=NULL;
	mNote1=NULL;
	mFunctionLabel1=NULL;
	mFunctionLabel2=NULL;
	mFrame=NULL;
	mPic=NULL;
}

SkillBookSurprisePage::~SkillBookSurprisePage(void)
{
	mTitle=NULL;
	mSummary=NULL;
	mNumber1=NULL;
	mDescription=NULL;
	//mPopMsg=NULL;
	mNote1=NULL;
	mFunctionLabel1=NULL;
	mFunctionLabel2=NULL;
	mFrame=NULL;
	mPic=NULL;
}


void SkillBookSurprisePage::Enter( MainFrame* frame)
{
	mSkillItem=SkillTableManager::Get()->getSkillItemByID(mSkillId);
	mSkillBookItem=SkillBookTableManager::Get()->getSkillBookItemByID(mSkillId,mPartId);
	if(mSkillItem&&mSkillBookItem)
	{
		if(mSummary)
		{
			std::list<std::string> replaceList;
			replaceList.push_back(mSkillItem->name);
			replaceList.push_back(mSkillBookItem->name);
			std::string outstr;
			int lines;
			int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("SkillBook_Describe"));
			GameMaths::stringAutoReturn(GameMaths::replaceStringWithStringList(Language::Get()->getString("@SkillBookSummarySurprise"),&replaceList),outstr,width,lines);
			mSummary->setString(outstr.c_str());
		}

		if(mNumber1)
		{
			mNumber1->setString(StringConverter::toString(mCount).c_str());
		}
		if(mName)
		{
			mName->setString((mSkillItem->name).c_str());
		}
		if(mDescription)
		{
			mDescription->setString((mSkillBookItem->name).c_str());
		}
		if(mNote1)
		{
			std::list<std::string> replaceList;
			replaceList.push_back(mSkillBookItem->name);
			replaceList.push_back(mSkillItem->name);
			replaceList.push_back(StringConverter::toString(SkillBookTableManager::Get()->getSkillTableBySkillId(mSkillId)->getSkillBookItemSize()));
			std::string outstr;
			int lines;
			int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("SkillBookNote_Describe"));
			GameMaths::stringAutoReturn(GameMaths::replaceStringWithStringList(Language::Get()->getString("@SkillBookPieceNote"),&replaceList),outstr,width,lines);
			mNote1->setString(outstr.c_str());
		}
		if(mPic)
		{
			mPic->setTexture(mSkillBookItem->iconPic.c_str());
		}
		
		if(mFrame)
		{
			mFrame->setNormalImage(::getFrameNormalSpirte(1));
			mFrame->setSelectedImage(::getFrameSelectedSpirte(1));
		}
	}
	if(mTitle)
	{
		mTitle->setString(Language::Get()->getString("@ObatainSkillBookTitle").c_str());
	}
	if(mFunctionLabel1)
	{
		mFunctionLabel1->setString(Language::Get()->getString("@GotoAdventureSkillBook").c_str());
	}
	if(mFunctionLabel2)
	{
		mFunctionLabel2->setString(Language::Get()->getString("@Close").c_str());
	}
}


void SkillBookSurprisePage::Exit( MainFrame* frame)
{
}

void SkillBookSurprisePage::Execute( MainFrame* )
{

}

void SkillBookSurprisePage::load( void )
{
	loadCcbiFile("PopUpSkillBook.ccbi");
	mTitle=dynamic_cast<cocos2d::CCLabelBMFont* >(getObjectByVariableName("mTitle"));
	mSummary=dynamic_cast<cocos2d::CCLabelBMFont* >(getObjectByVariableName("mSummary"));
	mNumber1=dynamic_cast<cocos2d::CCLabelBMFont* >(getObjectByVariableName("mNumber1"));
	mName=dynamic_cast<cocos2d::CCLabelBMFont* >(getObjectByVariableName("mName"));
	mDescription=dynamic_cast<cocos2d::CCLabelBMFont* >(getObjectByVariableName("mDescription"));
	//mPopMsg=dynamic_cast<cocos2d::CCLabelBMFont* >(getObjectByVariableName("mPopMsg"));
	mNote1=dynamic_cast<cocos2d::CCLabelBMFont* >(getObjectByVariableName("mNote1"));
	mFunctionLabel1=dynamic_cast<cocos2d::CCLabelBMFont* >(getObjectByVariableName("mFunctionLable1"));
	mFunctionLabel2=dynamic_cast<cocos2d::CCLabelBMFont* >(getObjectByVariableName("mFunctionLable2"));
	mFrame=dynamic_cast<cocos2d::CCMenuItemImage* >(getObjectByVariableName("mFrame"));
	mPic=dynamic_cast<cocos2d::CCSprite* >(getObjectByVariableName("mPic"));
}

void SkillBookSurprisePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{

}

void SkillBookSurprisePage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	MsgMainFramePopPage msg;
	msg.pageName = "SkillBookSurprisePage";
	MessageManager::Get()->sendMessage(&msg);
	PopupManager::Get()->popPage();
	if(itemName=="onFunction1")
	{
		MsgMainFrameChangePage msgPage;
		msgPage.pageName = "AdventurePage";
		MessageManager::Get()->sendMessage(&msgPage);
		AdventureItemInfoMapIterator itr = ServerDateManager::Get()->getAdventureItemInfoMapIterator();
		int _indexId=0;
		while(itr.hasMoreElements())
		{
			const AdventureItemInfo* adventureItemInfo=itr.getNext(); 
			unsigned int adventureId = adventureItemInfo->adventureId;
			const AdventureItem* adventureItem=AdventureTableManager::Get()->getAdventureItemByID(adventureId);
			bool isAddItem=true;
			if(adventureItem!=NULL)
			{
				if(adventureItem->adventureType==SkillBook)
				{
					if(adventureItemInfo->itemId==mSkillItem->itemID)
					{
						_indexId=adventureItemInfo->id;
						break;
					}
				}
			}
		}
		MsgAdventurePageChange msg;
		msg.index = _indexId;
		MessageManager::Get()->sendMessage(&msg);
	}
}
