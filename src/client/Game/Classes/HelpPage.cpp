
#include "stdafx.h"

#include "HelpPage.h"
#include "GameMessages.h"
#include "StringConverter.h"
#include "GameMaths.h"
REGISTER_PAGE("HelpPage",HelpPage);
USING_NS_CC;


HelpPage::HelpPage(void)
{
}


HelpPage::~HelpPage(void)
{
}

void HelpPage::Enter(MainFrame*)
{
	initContent();
}

void HelpPage::initContent()
{
	cocos2d::extension::CCScrollView* scrollContent=dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));

	if(scrollContent)
	{
		cocos2d::CCNode* node = cocos2d::CCNode::create();
		CCSize size = CCSize(0,0);

		HelpTableManager::HelpListIterator itr=HelpTableManager::Get()->getHelpIterator();
		int i=0;
		while(itr.hasMoreElements())
		{
			const HelpItem* item=itr.getNext();
			CCBContainer* content = CCBManager::Get()->createAndLoad("HelpContent.ccbi");
			CCLabelBMFont* itemName=dynamic_cast<CCLabelBMFont*>(content->getVariable(("mName")));
			if(itemName)
			{
				itemName->setString(item->name.c_str());
			}

			CCLabelBMFont* itemDes=dynamic_cast<CCLabelBMFont*>(content->getVariable(("mDes")));
			if(itemDes)
			{
				std::string describe;
				int lines;
				int width = StringConverter::parseInt(VaribleManager::Get()->getSetting("HelpDescribe"));
				GameMaths::stringAutoReturn(item->describe,describe,width,lines);
				itemDes->setString(describe.c_str());
			}

			CCSprite* face= dynamic_cast<CCSprite*>(content->getVariable("mIcon"));
			if(item->iconPic!="none")
			{
				if(face)
				{
					face->setVisible(true);
					face->setTexture(item->iconPic.c_str());
				}
			}
			else
			{
				if(face)
				{
					face->setVisible(false);
				}
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

void HelpPage::Exit( MainFrame* frame)
{

}

void HelpPage::Execute( MainFrame* )
{

}

void HelpPage::load( void )
{
	loadCcbiFile("HelpPage.ccbi");
}


void HelpPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	MsgMainFrameChangePage msg;
	msg.pageName="SettingsPage";
	MessageManager::Get()->sendMessage(&msg);
}

void HelpPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{

}
