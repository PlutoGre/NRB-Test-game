
#include "stdafx.h"

#include "SkillBookLootPage.h"
#include "Disciple.h"
#include "PopupNotEnoughPage.h"
#include "json/json.h"
#include "GamePlatform.h"
#include "FightPage.h"
REGISTER_PAGE("SkillBookLootPage",SkillBookLootPage);
USING_NS_CC;

void SkillBookLootContent::refreshContent( void )
{
	mPlayerName=dynamic_cast<CCLabelTTF* >(this->getObjectByVariableName("mEnemyName"));
	mPlayerLevel=dynamic_cast<CCLabelBMFont* >(this->getObjectByVariableName("mEnemyLV"));
	mSkillBookLootInfo=ServerDateManager::Get()->getSkillBookLootItemById(getContentID());
	if(mSkillBookLootInfo)
	{
		_refreshPage();
	}
}

void SkillBookLootContent::_refreshPage()
{
	if(mPlayerLevel)
	{
		mPlayerLevel->setString(StringConverter::toString(mSkillBookLootInfo->playerLevel).c_str());
	}
	if(mPlayerName)
	{
		mPlayerName->setString(mSkillBookLootInfo->playerName.c_str());
	}
	SkillBookLootInfo::PlayerTeamItemMapIterator it=mSkillBookLootInfo->getPlayerTeamItemMapIterator();
	for(int i=1;i<=TeamMax;++i)
	{
		char frame[128];
		sprintf(frame,"mFrame%d",i);

		char icon[128];
		sprintf(icon,"mHeadPic%d",i);

		CCMenuItemImage* frameBtn=dynamic_cast<CCMenuItemImage* >(this->getObjectByVariableName(frame));
		if(frameBtn && frameBtn->getParent())
		{
			CCMenu* menu = dynamic_cast<CCMenu*>(frameBtn->getParent());
			if(menu)
			{
				menu->setScrollViewChild(true);
			}
		}
		if(it.hasMoreElements())
		{
			unsigned int id=it.getNext()->memberId;
			if(id!=0)
			{
				CCB_FUNC(this->getContainerNode(),frame,CCMenuItemImage,setVisible(true));
				CCB_FUNC(this->getContainerNode(),icon,CCSprite,setVisible(true));
				Disciple disciple(id, true);
				CCB_FUNC(this->getContainerNode(),icon,CCSprite,setTexture(disciple.iconPic().c_str()));
				CCB_FUNC(this->getContainerNode(),frame,CCMenuItemImage,setNormalImage(disciple.getFrameNormalSpirte()));
				CCB_FUNC(this->getContainerNode(),frame,CCMenuItemImage,setSelectedImage(disciple.getFrameSelectedSpirte()));
			}
			else
			{
				CCB_FUNC(this->getContainerNode(),frame,CCMenuItemImage,setVisible(false));
				CCB_FUNC(this->getContainerNode(),icon,CCSprite,setVisible(false));
			}
		}
		else
		{
			CCB_FUNC(this->getContainerNode(),frame,CCMenuItemImage,setVisible(false));
			CCB_FUNC(this->getContainerNode(),icon,CCSprite,setVisible(false));
		}
	}
};

void SkillBookLootContent::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender, int tag )
{
	if(itemName=="onLoot")
	{
		if(ServerDateManager::Get()->getUserBasicInfo().vitality<=0)
		{
			POP_NOT_ENOUGH_BOX(VitalityNotEnough);
		}
		else
		{
			std::string _fightEnemyNameKey="fightEnemyNameKey";
			if(HAS_V(_fightEnemyNameKey))
			{
				SET_V(_fightEnemyNameKey,mSkillBookLootInfo->playerName);
			}
			else
			{
				ADD_V(_fightEnemyNameKey,mSkillBookLootInfo->playerName);
			}
			
			BlackBoard::Get()->setSnatchSkillBookPlayerId(mSkillBookLootInfo->id);
			BasePage* bp = CCBManager::Get()->getPage("FightPage");
			if(bp)
			{
				FightPage* fp = dynamic_cast<FightPage*>(bp);
				fp->setViewTeamStatus(ServerDateManager::Get()->getSkillBookLootItemById(BlackBoard::Get()->getSnatchSkillBookPlayerId())->playerId,3,ServerDateManager::Get()->getSkillBookLootItemById(BlackBoard::Get()->getSnatchSkillBookPlayerId())->isNpc==0?1:0);
			}
			OPSnatchSkillBook msg;
			msg.set_version(1);
			msg.set_playerid(ServerDateManager::Get()->getSkillBookLootItemById(BlackBoard::Get()->getSnatchSkillBookPlayerId())->playerId);
			msg.set_bookid(BlackBoard::Get()->getSkillBookId());
			msg.set_isnpc(ServerDateManager::Get()->getSkillBookLootItemById(BlackBoard::Get()->getSnatchSkillBookPlayerId())->isNpc);
			PacketManager::Get()->sendPakcet(OPCODE_SNATCH_SKILL_BOOK_INFO_C, &msg);
		}
		
	}
}

cocos2d::CCObject * SkillBookLootContent::getObjectByVariableName(const std::string& variableName)
{
	cocos2d::CCObject* obj = getContainerNode()->getVariable(variableName);
	if(!obj)
	{
		char errMsg[256];
		sprintf(errMsg,"%s%s","Failed to find variable:",variableName.c_str());
		CCMessageBox(errMsg,"error");
	}
	return obj;
}

SkillBookLootPage::SkillBookLootPage(void)
	:mExpbarInitScale(-1.0f)
{
	mLongBtnHeight = 0.0f;
	membersRootNode = NULL;
}


SkillBookLootPage::~SkillBookLootPage(void)
{
	MessageManager::Get()->removeMessageHandler(this);
	membersRootNode = 0;
	clearAllItem();
}

void SkillBookLootPage::Enter( MainFrame* frame)
{
	MessageManager::Get()->regisiterMessageHandler(MSG_SEVERINFO_UPDATE,this);
	MessageManager::Get()->regisiterMessageHandler(MSG_FIGHT_END,this);
	if(mNameMap.size()==0)
	{
		_readMap();
	}
	loadData();
}

void SkillBookLootPage::Exit( MainFrame* frame)
{
	clearAllItem();
	MessageManager::Get()->removeMessageHandler(this);
}

void SkillBookLootPage::Execute( MainFrame* )
{

}

cocos2d::CCObject* SkillBookLootPage::getObjectByVariableName(const std::string& variableName)
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

void SkillBookLootPage::loadData()
{
	OPSkillBookLoot msg;
	msg.set_version(1);
	msg.set_skillbookid(BlackBoard::Get()->getSkillBookId());
	//mSkillBookItem=SkillTableManager::Get()->getSkillItemByID(BlackBoard::Get()->getSkillBookSkillId());
	mSkillBookItem = SkillBookTableManager::Get()->getSkillBookItemByID(BlackBoard::Get()->getSkillBookSkillId(),BlackBoard::Get()->getSkillBookId());
	if(mSkillBookItem&&mSkillName)
	{
		mSkillName->setString(mSkillBookItem->name.c_str());
	}
	PacketManager::Get()->sendPakcet(OPCODE_GET_SKILL_BOOK_LOOT_INFO_C, &msg);
}

void SkillBookLootPage::load( void )
{
	loadCcbiFile("SkillBookLoot.ccbi");
	members =  dynamic_cast<cocos2d::extension::CCScrollView*>(getVariable("mContent"));
	membersRootNode = members->getContainer();
	mSkillName=dynamic_cast<CCLabelBMFont* >(this->getObjectByVariableName("mSkillSheetName"));

	CCNode* expbar=dynamic_cast<CCNode* >(getVariable("mExpBar"));
	if(expbar && mExpbarInitScale<0)
		mExpbarInitScale = expbar->getScaleX();
}

void SkillBookLootPage::buildItem( void )
{

	CCBContainer* longButton = CCBManager::Get()->createAndLoad("LongButton.ccbi");
	longButton->setListener(this,999);
	if(longButton)
	{
		CCLabelBMFont* goGetBtnLabel=dynamic_cast<CCLabelBMFont*>(longButton->getVariable(("mButtonLabel")));
		if(goGetBtnLabel)
		{
			goGetBtnLabel->setString(Language::Get()->getString("@SkillBookLootNext").c_str());
		}
		CCMenuItemImage* btn = dynamic_cast<CCMenuItemImage*>(longButton->getVariable("onFunction"));
		if(btn && btn->getParent())
		{
			CCMenu* menu = dynamic_cast<CCMenu*>(btn->getParent());
			if(menu)
			{
				menu->setScrollViewChild(true);
			}
		}
		membersRootNode->addChild(longButton);
		CCPoint pos = ccp((members->getViewSize().width-longButton->getContentSize().width)*0.5,0);
		longButton->setPosition(pos);
		mLongBtnHeight =  longButton->getContentSize().height;
		CCSize size = CCSize(longButton->getContentSize().width, mLongBtnHeight);
		members->setContentSize(size);
		members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
	}

	SkillBookLootInfoMapIterator itr = ServerDateManager::Get()->getSkillBookLootMapIterator();
	while(itr.hasMoreElements())
	{
		unsigned int id = itr.getNext()->id;
		SkillBookLootInfo* info=ServerDateManager::Get()->getAndCreatSkillBookLootInfoById(id);
		if(info->isNpc==0)
		{
			info->playerName=randomName();
		}
		SkillBookLootContent * item = new SkillBookLootContent(id);
		item->init();
		addItem(item);
	}
}

std::string SkillBookLootPage::randomName()
{
	struct cc_timeval now;
	CCTime::gettimeofdayCocos2d(&now, NULL);
	srand(now.tv_sec+now.tv_usec);

	std::string name("");
	int namecount = mNameMap.size();
	int namelength = rand()%2 +4;
	std::map<int, std::string> nameMap;
	mNameMap.swap(nameMap);

	for(int i=0;i<namelength;++i)
	{
		std::map<int, std::string>::iterator it = nameMap.end();
		while(it == nameMap.end())
		{
			it = nameMap.find(rand()%namecount);
		}
		name.append(it->second);
		mNameMap.insert(std::make_pair(it->first,it->second));
		nameMap.erase(it);
	}
	mNameMap.insert(nameMap.begin(),nameMap.end());
	return name;
}

void SkillBookLootPage::_readMap()
{
	Json::Reader jreader;
	Json::Value data;
	unsigned long filesize;
	char* pBuffer = (char*)getFileData(
		cocos2d::CCFileUtils::sharedFileUtils()->fullPathForFilename("instructions.cfg").c_str(),
		"rt",&filesize);

	if(!pBuffer)
	{
		char msg[256];
		sprintf(msg,"Failed open file: instructions.cfg");
		cocos2d::CCMessageBox(msg,"ERROR!");
	}
	else
	{
		jreader.parse(pBuffer,data,false);
		CC_SAFE_DELETE_ARRAY(pBuffer);

		if(data["version"].asInt()==1)
		{

			Json::Value steplist = data["steps"];
			if(!steplist.empty() && steplist.isArray())
			{
				for(int i = 0;i < steplist.size();++i)
				{
					Json::Value unit = steplist[i];

					if(unit["type"].asString() == "custom")
					{
						if(unit["func"].asString() == "chooseName")
						{
							std::string names = data["randomNameList"].asString();
							GameMaths::readStringToMap(names,mNameMap);
						}
					}
				}
			}

		}
	}
	CC_SAFE_DELETE_ARRAY(pBuffer);
}


void SkillBookLootPage::addItem( ContentBase* item )
{
	mContentList.push_back(item);
	membersRootNode->addChild(item->getContainerNode());
	CCPoint pos = ccp((members->getViewSize().width-item->getContainerNode()->getContentSize().width)*0.5,item->getContainerNode()->getContentSize().height*(mContentList.size()-1)+mLongBtnHeight);
	item->getContainerNode()->setPosition(pos);
	CCSize size = CCSize(item->getContainerNode()->getContentSize().width, item->getContainerNode()->getContentSize().height*mContentList.size()+mLongBtnHeight);
	members->setContentSize(size);
	members->setContentOffset(ccp(0, members->getViewSize().height - members->getContentSize().height*members->getScaleY()));
}

void SkillBookLootPage::rebuildAllItem( void )
{
	clearAllItem();
	buildItem();
}

void SkillBookLootPage::clearAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		delete (*itr);
		++itr;
	}
	ContentList list;
	mContentList.swap(list);
	if(membersRootNode)
		membersRootNode->removeAllChildren();
}

void SkillBookLootPage::refreshAllItem( void )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		(*itr)->refreshContent();
		++itr;
	}
}

void SkillBookLootPage::removeItem( ContentBase* item )
{
	ContentList::iterator itr = mContentList.begin();
	while(itr != mContentList.end())
	{
		if((*itr) == item)
		{
			membersRootNode->removeChild(item->getContainerNode());
			delete item;
			mContentList.erase(itr);
		}
		++itr;
	}
}

void SkillBookLootPage::onMenuItemAction( const std::string& itemName, cocos2d::CCObject* sender )
{
	if(itemName=="onBack")
	{
		MsgMainFramePopPage msg;
		msg.pageName = "SkillBookLootPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void SkillBookLootPage::onMenuItemAction(const std::string& itemName, cocos2d::CCObject* sender, int tag)
{
	if(tag==999)
	{
		loadData();
	}
}

void SkillBookLootPage::onReceiveMassage(const GameMessage * message)
{
	if(message->getTypeId() == MSG_SEVERINFO_UPDATE)
	{
		const MsgSeverInfoUpdate * msg = dynamic_cast<const MsgSeverInfoUpdate*>(message);
		if(msg)
		{
			if(msg->opcode==OPCODE_GET_SKILL_BOOK_LOOT_INFORET_S)
			{
				rebuildAllItem();
				refreshPage();
			}
			else if(msg->opcode==OPCODE_SNATCH_SKILL_BOOK_INFORET_S)
			{

			}
		}
	}
	else if(message->getTypeId()==MSG_FIGHT_END)
	{
		std::string key="snatchSkillBookRetKey";
		unsigned int ret=GET_V_UINT(key);
		if(ret==1)
		{
			MsgMainFramePushPage pushMsg;
			pushMsg.pageName = "ObtainSkillBookPage";
			MessageManager::Get()->sendMessage(&pushMsg);
		}
		else
		{
			if(ret==0)
			{
				POP_BOX(SnatchSkillBookFightWin);
			}
			else
			{
				POP_BOX(SnatchSkillBookFightFail);
			}
		}
		MsgMainFramePopPage msg;
		msg.pageName = "SkillBookLootPage";
		MessageManager::Get()->sendMessage(&msg);
	}
}

void SkillBookLootPage::refreshPage( void )
{
	const UserBasicInfo& userBasicInfo = ServerDateManager::Get()->getUserBasicInfo();
	CCLabelTTF* name=dynamic_cast<CCLabelTTF* >(getVariable("mName"));
	if(name)
	{
		name->setString(userBasicInfo.name.c_str());
	}
	CCLabelBMFont* level=dynamic_cast<CCLabelBMFont* >(getVariable("mLV"));
	if(level)
	{
		level->setString(StringConverter::toString(userBasicInfo.level).c_str());
	}
	CCLabelBMFont* exp = dynamic_cast<CCLabelBMFont* >(getVariable("mExp"));
	if(exp)
	{
		exp->setString(ServerDateManager::Get()->getUserExpPercentStr().c_str());
	}
	CCNode* expbar=dynamic_cast<CCNode* >(getVariable("mExpbar"));
	if(expbar)
	{
		if(mExpbarInitScale<0)
			mExpbarInitScale = expbar->getScaleX();

		float expscale = 0;
		if(ServerDateManager::Get()->getNextLevelExp()>0)
			expscale = ServerDateManager::Get()->getUserExpPercent();
		if(expscale>1.0f)expscale=1.0f;
		expbar->setScaleX(expscale*mExpbarInitScale);
	}

	CCLabelBMFont* gold = dynamic_cast<CCLabelBMFont* >(getVariable("mGold"));
	if(gold)
	{
		gold->setString(StringConverter::toString(userBasicInfo.goldcoins).c_str());
	}
	CCLabelBMFont* coin = dynamic_cast<CCLabelBMFont* >(getVariable("mCoin"));
	if(coin)
	{
		coin->setString(StringConverter::toString(userBasicInfo.silvercoins).c_str());
	}
	CCLabelBMFont* ti = dynamic_cast<CCLabelBMFont* >(getVariable("mTi"));
	if(ti)
	{
		ti->setString((StringConverter::toString(userBasicInfo.power)+"/"+StringConverter::toString(userBasicInfo.maxpower)).c_str());
	}
	CCLabelBMFont* qi = dynamic_cast<CCLabelBMFont* >(getVariable("mQi"));
	if(qi)
	{
		qi->setString((StringConverter::toString(userBasicInfo.vitality)+"/"+StringConverter::toString(userBasicInfo.maxvitality)).c_str());
	}
	CCSprite* vipLevel=dynamic_cast<CCSprite* >(getVariable("mVipPic"));
	if(vipLevel)
	{

		const std::string vipImage = ::getVipLevelImageFile(userBasicInfo.viplevel);
		if(vipImage!="")
		{
			vipLevel->setVisible(true);
			vipLevel->initWithFile(vipImage.c_str());
		}else
		{
			vipLevel->setVisible(false);
		}
	}
}