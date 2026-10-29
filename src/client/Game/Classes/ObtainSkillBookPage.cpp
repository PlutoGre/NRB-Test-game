
#include "stdafx.h"

#include "ObtainSkillBookPage.h"
REGISTER_PAGE("ObtainSkillBookPage",ObtainSkillBookPage);
USING_NS_CC;


cocos2d::CCObject * ObtainSkillBookPage::getObjectByVariableName(const std::string& variableName)
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


ObtainSkillBookPage::ObtainSkillBookPage(void)
{
	mTitle=NULL;
	mSummary=NULL;
	mNumber1=NULL;
	mNumber=NULL;
	mDescription=NULL;
	mPopMsg=NULL;
	mNote1=NULL;
	mFunctionLabel1=NULL;
	mFunctionLabel2=NULL;
	mFrame=NULL;
	mPic=NULL;
}

ObtainSkillBookPage::~ObtainSkillBookPage(void)
{
	mTitle=NULL;
	mSummary=NULL;
	mNumber1=NULL;
	//mNumber=NULL;
	mDescription=NULL;
	//mPopMsg=NULL;
	mNote1=NULL;
	mFunctionLabel1=NULL;
	mFunctionLabel2=NULL;
	mFrame=NULL;
	mPic=NULL;
}


void ObtainSkillBookPage::Enter( MainFrame* frame)
{
	mSkillItem=SkillTableManager::Get()->getSkillItemByID(BlackBoard::Get()->getSkillBookSkillId());
	mSkillBookItem=SkillBookTableManager::Get()->getSkillBookItemByID(BlackBoard::Get()->getSkillBookSkillId(),BlackBoard::Get()->getSkillBookId());
	if(mSkillItem&&mSkillBookItem)
	{
		mSkillBookData=ServerDateManager::Get()->getSkillBookItemBySkillIdAndPartId(mSkillItem->itemID,mSkillBookItem->itemID);
		if(mSummary)
		{
			std::list<std::string> replaceList;
			replaceList.push_back(mSkillBookItem->name);
			replaceList.push_back(mSkillItem->name);
			std::string outstr;
			int lines;
			int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("SkillBook_Describe"));
			GameMaths::stringAutoReturn(GameMaths::replaceStringWithStringList(Language::Get()->getString("@SkillBookSummary"),&replaceList),outstr,width,lines);
			mSummary->setString(outstr.c_str());
		}
		if(mSkillBookData)
		{
			if(mNumber1)
			{
				mNumber1->setString(StringConverter::toString(mSkillBookData->partCount).c_str());
			}
// 			if(mNumber)
// 			{
// 				mNumber->setString(StringConverter::toString(mSkillBookData->partCount).c_str());
// 			}
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
			replaceList.push_back(StringConverter::toString(SkillBookTableManager::Get()->getSkillTableBySkillId(BlackBoard::Get()->getSkillBookSkillId())->getSkillBookItemSize()));
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
		mFunctionLabel1->setString(Language::Get()->getString("@Close").c_str());
	}
	if(mFunctionLabel2)
	{
		mFunctionLabel2->setString(Language::Get()->getString("@GotoFeedPage").c_str());
	}
}


void ObtainSkillBookPage::Exit( MainFrame* frame)
{
}

void ObtainSkillBookPage::Execute( MainFrame* )
{

}

void ObtainSkillBookPage::load( void )
{
	loadCcbiFile("PopUpSkillBook.ccbi");
	mTitle=dynamic_cast<cocos2d::CCLabelBMFont* >(getObjectByVariableName("mTitle"));
	mSummary=dynamic_cast<cocos2d::CCLabelBMFont* >(getObjectByVariableName("mSummary"));
	mNumber1=dynamic_cast<cocos2d::CCLabelBMFont* >(getObjectByVariableName("mNumber1"));
	//mNumber=dynamic_cast<cocos2d::CCLabelBMFont* >(getObjectByVariableName("mNumber"));
	mName=dynamic_cast<cocos2d::CCLabelBMFont* >(getObjectByVariableName("mName"));
	mDescription=dynamic_cast<cocos2d::CCLabelBMFont* >(getObjectByVariableName("mDescription"));
	//mPopMsg=dynamic_cast<cocos2d::CCLabelBMFont* >(getObjectByVariableName("mPopMsg"));
	mNote1=dynamic_cast<cocos2d::CCLabelBMFont* >(getObjectByVariableName("mNote1"));
	mFunctionLabel1=dynamic_cast<cocos2d::CCLabelBMFont* >(getObjectByVariableName("mFunctionLable1"));
	mFunctionLabel2=dynamic_cast<cocos2d::CCLabelBMFont* >(getObjectByVariableName("mFunctionLable2"));
	mFrame=dynamic_cast<cocos2d::CCMenuItemImage* >(getObjectByVariableName("mFrame"));
	mPic=dynamic_cast<cocos2d::CCSprite* >(getObjectByVariableName("mPic"));
}

void ObtainSkillBookPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{

}

void ObtainSkillBookPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onClose")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "ObtainSkillBookPage";
		MessageManager::Get()->sendMessage(&msg);
	}
	else if(itemName=="onConfirm")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "ObtainSkillBookPage";
		MessageManager::Get()->sendMessage(&msg);
	}

	MsgMainFramePopPage msg;
	msg.pageName = "ObtainSkillBookPage";
	MessageManager::Get()->sendMessage(&msg);
}

void ObtainSkillBookPage::onReceiveMassage( const GameMessage * message )
{

}