#include "stdafx.h"
#include "ExpansionSystemsPage.h"
#include <algorithm>
#include <ctime>
USING_NS_CC;
USING_NS_CC_EXT;
namespace {
const char* modes[]={"transport","equipment","skills","crew","training","cabin"};
const char* titles[]={"运宝","装备养成","奥义扩展","船员养成","潜能训练","船舱"};
CCRect visibleArea(CCNode* node);
class ExpansionMenu : public CCMenu {
public:
    static ExpansionMenu* create(){ExpansionMenu* out=new ExpansionMenu();if(!out->init()){delete out;return NULL;}out->setScrollViewChild(true);out->autorelease();return out;}
    virtual void registerWithTouchDispatcher(){CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,kCCMenuHandlerPriority-3,false);}
    virtual bool ccTouchBegan(CCTouch* touch,CCEvent* event){
        // 嵌套旧滚动层会覆盖菜单的裁剪缓存；点击前按当前世界坐标重算。
        // 长列表滚到末尾后，传书和合成按钮仍使用实际可见区域。
        setTouchArea(visibleArea(this));return CCMenu::ccTouchBegan(touch,event);
    }
};
class ExpansionScroll : public CCScrollView {
public:
    static ExpansionScroll* create(CCSize size){ExpansionScroll* out=new ExpansionScroll();if(!out->initWithViewSize(size)){delete out;return NULL;}out->autorelease();return out;}
    virtual void registerWithTouchDispatcher(){CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,kCCMenuHandlerPriority-2,true);}
};
CCLabelTTF* caption(const std::string& text,float font,float width){return CCLabelTTF::create(text.c_str(),"Arial",font,CCSizeMake(width,0),kCCTextAlignmentLeft);}
CCRect visibleArea(CCNode* node){
    const CCSize window=CCDirector::sharedDirector()->getWinSize();float left=0,bottom=0,right=window.width,top=window.height;
    for(CCNode* parent=node->getParent();parent;parent=parent->getParent())if(CCScrollView* view=dynamic_cast<CCScrollView*>(parent)){
        const CCSize size=view->getViewSize();CCPoint a=view->convertToWorldSpace(CCPointZero),b=view->convertToWorldSpace(ccp(size.width,size.height));
        left=std::max(left,std::min(a.x,b.x));bottom=std::max(bottom,std::min(a.y,b.y));right=std::min(right,std::max(a.x,b.x));top=std::min(top,std::max(a.y,b.y));
    }
    CCBContainer* frame=CCBManager::Get()->getPage("MainFrame");
    CCScrollView* navigation=frame?dynamic_cast<CCScrollView*>(frame->getVariable("mMainbuttons")):NULL;
    if(navigation&&navigation->isVisible()){
        CCPoint a=navigation->convertToWorldSpace(CCPointZero),b=navigation->convertToWorldSpace(ccp(0,navigation->getViewSize().height));
        bottom=std::max(bottom,std::max(a.y,b.y)+3.0f);
    }
    return CCRectMake(left,bottom,std::max(1.0f,right-left),std::max(1.0f,top-bottom));
}
}
ExpansionSystemsPage::ExpansionSystemsPage(unsigned int id,int mode):AdventureBase(id),mMode(modes[mode]),mSelected(false),mBusy(false),mRegistered(false),mWait(0),mPoll(0),mTarget(0),mChoice(0),mOffset(0),mMenu(NULL){
    mIsInitData=true;mOwner=GamePrecedure::Get()->getUin();
    mKey="Expansion_"+StringConverter::toString(GamePrecedure::Get()->getServerID())+"_"+mOwner+"_"+mMode;
    std::string saved=CCUserDefault::sharedUserDefault()->getStringForKey(mKey.c_str(),"");Json::Reader reader;
    if(!saved.empty()&&!reader.parse(saved,mPending,false))mPending=Json::Value();
    mContentContainer=NULL;
}
ExpansionSystemsPage::~ExpansionSystemsPage(){exitPage();}
const std::string& ExpansionSystemsPage::getCcbiFileName() const {static const std::string unused="";return unused;}
void ExpansionSystemsPage::init(){
    // 直接使用冒险内容区尺寸，保留原顶部活动导航和底部主导航。
    mContentContainer=CCBContainer::create();mContentContainer->retain();
    BasePage* page=CCBManager::Get()->getPage("AdventurePage");
    CCScrollView* view=page?dynamic_cast<CCScrollView*>(page->getVariable("mContent")):NULL;
    mContentContainer->setContentSize(view?view->getViewSize():CCSizeMake(320,360));refreshContent();
}
void ExpansionSystemsPage::refreshContent(){
    if(!mContentContainer)return;mContentContainer->removeAllChildrenWithCleanup(true);mMenu=NULL;
    CCSize size=mContentContainer->getContentSize();
    mContentContainer->addChild(CCLayerColor::create(ccc4(20,30,48,255),size.width,size.height));
    // 旧冒险页采用设计坐标并保留底部导航空白；按祖先滚动层的实际裁剪范围排版。
    const CCRect clip=visibleArea(mContentContainer);
    CCPoint origin=mContentContainer->convertToWorldSpace(CCPointZero),one=mContentContainer->convertToWorldSpace(ccp(1,1));
    const float scaleX=std::max(0.001f,fabsf(one.x-origin.x)),scaleY=std::max(0.001f,fabsf(one.y-origin.y));
    const float visibleWidth=std::min(size.width,clip.size.width/scaleX);
    CCNode* canvas=CCNode::create();canvas->setPosition(ccp((size.width-visibleWidth)/2,(clip.origin.y-origin.y)/scaleY));
    size=CCSizeMake(visibleWidth,clip.size.height/scaleY);canvas->setContentSize(size);mContentContainer->addChild(canvas);
    float unit=size.width/320.0f;
    std::string title=mState.isObject()?mState["title"].asString():titles[std::find(modes,modes+6,mMode)-modes];
    CCLabelTTF* heading=caption(title,19*unit,size.width-24*unit);heading->setColor(ccc3(255,215,120));heading->setAnchorPoint(ccp(0,1));heading->setPosition(ccp(12*unit,size.height-9*unit));canvas->addChild(heading);
    std::string wallet=mState.isObject()?"钻石 "+StringConverter::toString(mState["goldcoins"].asInt())+"　贝里 "+mState["silvercoins"].asString():"正在读取服务器存档…";
    CCLabelTTF* money=caption(wallet,11*unit,size.width-24*unit);money->setAnchorPoint(ccp(0,1));money->setPosition(ccp(12*unit,size.height-35*unit));canvas->addChild(money);
    std::vector<std::string> lines;
    if(!mMessage.empty())lines.push_back(mMessage);
    if(!mConfirm.isNull())lines.push_back("确认操作："+mConfirm["label"].asString()+"？");
    else for(unsigned int i=0;i<mState["view"]["lines"].size();i++)lines.push_back(mState["view"]["lines"][i].asString());
    mActions=Json::Value(Json::arrayValue);
    if(!mConfirm.isNull()){
        Json::Value yes;yes["label"]="确认操作";yes["action"]="confirm";mActions.append(yes);
        Json::Value no;no["label"]="取消";no["action"]="cancel";mActions.append(no);
    }else if(mPending.isNull())mActions=mState["view"]["actions"];
    if(!mActions.isArray())mActions=Json::Value(Json::arrayValue);
    Json::Value reload;reload["label"]=mBusy?"正在连接…":"刷新 / 核对";reload["action"]="reload";mActions.append(reload);
    Json::Value mail;mail["label"]="前往传书";mail["action"]="mail";mActions.append(mail);
    float y=0;std::vector<CCLabelTTF*> labels;
    for(unsigned int i=0;i<lines.size();i++){CCLabelTTF* text=caption(lines[i],12*unit,size.width-32*unit);labels.push_back(text);y+=text->getContentSize().height+9*unit;}
    // 核对及传书固定在页头，避免长背包列表把常用导航推到末尾。
    ExpansionMenu* controls=ExpansionMenu::create();controls->setPosition(CCPointZero);canvas->addChild(controls,3);
    for(unsigned int i=0;i<mActions.size();i++){
        const std::string action=mActions[i]["action"].asString();if(action!="reload"&&action!="mail")continue;
        const float x=size.width*(action=="reload"?0.25f:0.75f),y=size.height-65*unit;
        CCLayerColor* bg=CCLayerColor::create(ccc4(40,65,87,255),size.width/2-14*unit,25*unit);bg->setPosition(ccp(x-bg->getContentSize().width/2,y-12.5f*unit));canvas->addChild(bg,2);
        CCMenuItemLabel* item=CCMenuItemLabel::create(caption(mActions[i]["label"].asString(),11*unit,size.width/2-25*unit),this,menu_selector(ExpansionSystemsPage::click));
        item->setTag(1000+i);item->setPosition(ccp(x,y));item->setEnabled(!mBusy);controls->addChild(item);
    }
    float buttonsHeight=(mActions.size()-2)*43*unit;CCSize viewSize=CCSizeMake(size.width-12*unit,std::max(40.0f*unit,size.height-84*unit));
    ExpansionScroll* scroll=ExpansionScroll::create(viewSize);scroll->setPosition(ccp(6*unit,3*unit));scroll->setDirection(kCCScrollViewDirectionVertical);
    CCNode* content=CCNode::create();float height=std::max(viewSize.height,y+buttonsHeight+10*unit);content->setContentSize(CCSizeMake(viewSize.width,height));scroll->setContainer(content);
    float top=height-5*unit;
    for(unsigned int i=0;i<labels.size();i++){labels[i]->setAnchorPoint(ccp(0,1));labels[i]->setPosition(ccp(10*unit,top));content->addChild(labels[i]);top-=labels[i]->getContentSize().height+9*unit;}
    mMenu=ExpansionMenu::create();mMenu->setPosition(CCPointZero);content->addChild(mMenu,2);
    for(unsigned int i=0;i<mActions.size();i++){
        const std::string action=mActions[i]["action"].asString();if(action=="reload"||action=="mail")continue;
        float rowHeight=39*unit;
        CCLayerColor* bg=CCLayerColor::create(ccc4(40,65,87,255),viewSize.width-16*unit,rowHeight);bg->setPosition(ccp(8*unit,top-rowHeight));content->addChild(bg);
        CCLabelTTF* text=caption(mActions[i]["label"].asString(),13*unit,viewSize.width-34*unit);text->setColor(ccc3(255,225,160));
        CCMenuItemLabel* item=CCMenuItemLabel::create(text,this,menu_selector(ExpansionSystemsPage::click));item->setTag(1000+i);item->setPosition(ccp(viewSize.width/2,top-rowHeight/2));
        item->setEnabled(!mBusy&&mActions[i].get("enabled",true).asBool());mMenu->addChild(item);top-=43*unit;
    }
    scroll->setContentOffset(ccp(0,viewSize.height-height));canvas->addChild(scroll);
}
std::string ExpansionSystemsPage::nonce(){static unsigned int sequence=0;char value[33];sprintf(value,"%08x%08x%08x%08x",(unsigned int)time(NULL),(unsigned int)clock(),(unsigned int)rand(),++sequence);return value;}
void ExpansionSystemsPage::send(const Json::Value& request){if(!mSelected||mOwner!=GamePrecedure::Get()->getUin())return;std::string data=Json::FastWriter().write(request);mBusy=true;mWait=0;refreshContent();PacketManager::Get()->sendPakcet(48003,&data[0],data.size(),false);}
void ExpansionSystemsPage::info(){if(mBusy)return;Json::Value request;request["mode"]=mMode;request["action"]="info";request["id"]=mTarget;request["choice"]=mChoice;request["offset"]=mOffset;mInfo=nonce();request["requestId"]=mInfo;send(request);}
void ExpansionSystemsPage::savePending(){CCUserDefault::sharedUserDefault()->setStringForKey(mKey.c_str(),mPending.isNull()?"":Json::FastWriter().write(mPending));CCUserDefault::sharedUserDefault()->flush();}
void ExpansionSystemsPage::click(CCObject* sender){
    if(mBusy)return;int index=static_cast<CCNode*>(sender)->getTag()-1000;if(index<0||index>=(int)mActions.size())return;
    Json::Value item=mActions[index];std::string action=item["action"].asString();
    if(action=="reload"){mConfirm=Json::Value();if(mPending.isNull())info();else send(mPending);}
    else if(action=="mail"){MsgMainFrameChangePage message;message.pageName="SysMsgPage";MessageManager::Get()->sendMessage(&message);}
    else if(action=="select"){mTarget=item["id"].asInt();mOffset=0;info();}
    else if(action=="choice"){mChoice=item["id"].asInt();info();}
    else if(action=="page"){mOffset=item["id"].asInt();info();}
    else if(action=="cancel"){mConfirm=Json::Value();refreshContent();}
    else if(action=="confirm"){
        mPending=mConfirm;mPending["mode"]=mMode;mPending["requestId"]=nonce();mPending["revision"]=mState["revision"];mPending["configRevision"]=mState["configRevision"];
        mConfirm=Json::Value();savePending();send(mPending);
    }else{mConfirm=item;refreshContent();}
}
void ExpansionSystemsPage::onReceivePacket(int opcode,const std::string& value){
    if(opcode!=48004||!mSelected||mOwner!=GamePrecedure::Get()->getUin())return;Json::Value reply;Json::Reader reader;
    if(!reader.parse(value,reply,false)||reply["mode"].asString()!=mMode)return;std::string id=reply["requestId"].asString();bool pending=!mPending.isNull()&&id==mPending["requestId"].asString();
    if(!pending&&id!=mInfo)return;mBusy=false;mWait=0;
    if(!reply.get("ok",false).asBool()){
        mMessage=reply["message"].asString();if(pending&&!reply.get("uncertain",false).asBool()){mPending=Json::Value();savePending();}refreshContent();return;
    }
    if(reply["owner"].asString()!=mOwner){failure();return;}
    mState=reply;if(pending){mMessage=reply["result"]["message"].asString();mPending=Json::Value();mInfo.clear();savePending();}
    ServerDateManager::Get()->getUserBasicInfo().goldcoins=reply["goldcoins"].asUInt();refreshContent();
    if(!mPending.isNull())send(mPending);
}
void ExpansionSystemsPage::failure(){if(!mSelected)return;mBusy=false;mWait=0;mMessage="连接中断或超时，请点击刷新核对原操作";refreshContent();}
void ExpansionSystemsPage::loadData(){if(!mRegistered){PacketManager::Get()->regisiterPacketHandler(48004,this);mRegistered=true;}mSelected=true;mBusy=false;if(mPending.isNull())info();else send(mPending);}
void ExpansionSystemsPage::updatePage(bool selected,float dt){if(!selected){exitPage();return;}mSelected=true;if(mBusy){mWait+=dt;if(mWait>20)failure();}else if(mMode=="transport"){mPoll+=dt;if(mPoll>=5&&mConfirm.isNull()&&mPending.isNull()){mPoll=0;info();}}}
void ExpansionSystemsPage::exitPage(){mSelected=false;mBusy=false;mConfirm=Json::Value();if(mRegistered){PacketManager::Get()->removePacketHandler(48004,this);mRegistered=false;}}
