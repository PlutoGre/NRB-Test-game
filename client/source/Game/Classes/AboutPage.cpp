
#include "stdafx.h"

#include "AboutPage.h"
#include "GameMessages.h"
REGISTER_PAGE("AboutPage",AboutPage);
USING_NS_CC;


AboutPage::AboutPage(void)
{
}


AboutPage::~AboutPage(void)
{
}

void AboutPage::Enter(MainFrame*)
{
	initContent();
}

void AboutPage::initContent()
{
	cocos2d::extension::CCScrollView* scrollContent=dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));

	if(scrollContent)
	{
		cocos2d::CCNode* node = cocos2d::CCNode::create();
		CCSize size = CCSize(0,0);

		AboutTableManager::AboutListIterator itr=AboutTableManager::Get()->getAboutIterator();
		int i=0;
		while(itr.hasMoreElements())
		{
			const AboutItem* item=itr.getNext();
			CCBContainer* content = CCBManager::Get()->createAndLoad("AboutContent.ccbi");
			CCLabelBMFont* itemName=dynamic_cast<CCLabelBMFont*>(content->getVariable(("mName")));
			if(itemName)
			{
				itemName->setString(item->name.c_str());
			}

			CCLabelBMFont* itemDes=dynamic_cast<CCLabelBMFont*>(content->getVariable(("mDes")));
			if(itemDes)
			{
				itemDes->setString(item->describe.c_str());
			}

			CCSprite* face= dynamic_cast<CCSprite*>(content->getVariable("mIcon"));
			if(face)
			{
				//CCTexture2D*tex = CCTextureCache::sharedTextureCache()->addImage(item->iconPic.c_str());
				face->setTexture(item->iconPic.c_str());
			}

			node->addChild(content);
			content->setPosition(ccp((scrollContent->getViewSize().width-content->getContentSize().width)*0.5,content->getContentSize().height*i));
			size.width = content->getContentSize().width;
			size.height += content->getContentSize().height;
			++i;
		}
		node->setContentSize(size);
		scrollContent->setContainer(node);
		scrollContent->setContentSize(size);

		if(size.height<scrollContent->getViewSize().height)
		{
			scrollContent->setContentOffset(ccp(0,scrollContent->maxContainerOffset().y));
		}
		else
		{
			scrollContent->setContentOffset(ccp(0,scrollContent->minContainerOffset().y));
		}
	}
}

void AboutPage::Exit( MainFrame* frame)
{

}

void AboutPage::Execute( MainFrame* )
{

}

void AboutPage::load( void )
{
	loadCcbiFile("AboutPage.ccbi");
}


void AboutPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	MsgMainFrameChangePage msg;
	msg.pageName="SettingsPage";
	MessageManager::Get()->sendMessage(&msg);
}

void AboutPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{

}
