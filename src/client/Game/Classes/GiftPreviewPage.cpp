
#include "stdafx.h"

#include "GiftPreviewPage.h"
#include "StringConverter.h"
#include "GameMessages.h"

REGISTER_PAGE("GiftPreviewPage",GiftPreviewPage);

GiftPreviewPage::GiftPreviewPage(void)
{
}


GiftPreviewPage::~GiftPreviewPage(void)
{
}

void GiftPreviewPage::Enter( MainFrame* )
{
	updateContent();
}

void GiftPreviewPage::Execute( MainFrame* )
{

}

void GiftPreviewPage::Exit( MainFrame* )
{

}

void GiftPreviewPage::load( void )
{
	loadCcbiFile("PackPreivew.ccbi");
}

void GiftPreviewPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onClose" || itemName=="onConfirm")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "GiftPreviewPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void GiftPreviewPage::setContent( const PACKCONTENT_LIST& list )
{
	PACKCONTENT_LIST::const_iterator it = list.begin();
	mList.clear();
	mList.assign(it,list.end());

	if(getLoaded())
	{
		updateContent();
	}
}

void GiftPreviewPage::updateContent()
{
	CCB_FUNC(this,"mPackPageMsg",CCLabelBMFont,setString(Language::Get()->getString("@PackPreviewMsgView").c_str()));
	CCB_FUNC(this,"mTitle",CCLabelBMFont,setString(Language::Get()->getString("@PackPreviewTitleView").c_str()));
	CCObject* obj = getVariable("mContent");
	cocos2d::extension::CCScrollView* view = obj?dynamic_cast<cocos2d::extension::CCScrollView*>(obj):0;
	if(view)
	{
		CCNode * node = CCNode::create();
		int totalCount = mList.size();
		int count = 0;
		PACKCONTENT_LIST::iterator it = mList.begin();
		CCSize size(0,0);
		for(;it!=mList.end();++it,++count)
		{
			CCBContainer* ccb = CCBManager::Get()->createAndLoad("PackPreviewContent.ccbi");
			ccb->setListener(NULL);
			CCB_FUNC(ccb,"mPropName",CCLabelBMFont,setString(it->name.c_str()));
			CCB_FUNC(ccb,"mNumber",CCLabelBMFont,setString(StringConverter::toString(it->number).c_str()));
			CCB_FUNC(ccb,"mPropPic",CCSprite,setTexture(it->icon.c_str()));
			//CCRect rect(0,0,tex->getPixelsWide(),tex->getPixelsHigh());
			//CCB_FUNC(ccb,"mPropPic",CCSprite,setTextureRect(rect));
			CCObject* buttonObj=ccb->getVariable("mFrameBack");
			if(!buttonObj)
			{
				char errMsg[256];
				sprintf(errMsg,"%s","Failed to find variable:mFrameBack");
				CCMessageBox(errMsg,"error");
			}
			cocos2d::CCMenuItemImage* propFrame=dynamic_cast<cocos2d::CCMenuItemImage* >(buttonObj);
			if(propFrame)
			{
				propFrame->setNormalImage(::getFrameNormalSpirte(it->quality>0?it->quality:4));
				propFrame->setSelectedImage(::getFrameSelectedSpirte(it->quality>0?it->quality:4));
			}

			size.width = ccb->getContentSize().width;
			size.height+=ccb->getContentSize().height;
			ccb->setPositionX(0);
			ccb->setPositionY(ccb->getContentSize().height*(totalCount-count-1));
			node->addChild(ccb);
		}
		node->setContentSize(size);
		view->setContentSize(size);
		view->setContainer(node);
		//view->setContentOffset(ccp(0,view->minContainerOffset().y));
	}
	
}

void GiftPreviewPage::setTitle( const std::string& title )
{
	if(title!=mTitle)
	{
		mTitle = title;
		if(getLoaded())
			CCB_FUNC(this,"mTitle",CCLabelBMFont,setString(Language::Get()->getString("@PackPreviewMsg").c_str()));
	}
}

void GiftPreviewPage::addContent( std::string name, std::string icon, int number, int quality )
{
	GiftPreviewPage* page = dynamic_cast<GiftPreviewPage*>(CCBManager::Get()->getPage("GiftPreviewPage"));
	PackContent content(name,icon,number,quality);
	page->mList.push_back(content);
}

void GiftPreviewPage::showPage(const std::string& title)
{
	GiftPreviewPage* page = dynamic_cast<GiftPreviewPage*>(CCBManager::Get()->getPage("GiftPreviewPage"));
	
	page->setTitle(title);

	if(page->getLoaded())
	{
		page->updateContent();
	}

	MsgMainFramePushPage msg;
	msg.pageName = "GiftPreviewPage";
	MessageManager::Get()->sendMessage(&msg);
}
