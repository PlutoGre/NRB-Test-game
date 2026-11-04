#include "stdafx.h"
#include "ExpansionSystemsPage.h"
#include "Disciple.h"
#include "Equip.h"
#include "Skill.h"
#include <algorithm>
#include <ctime>
USING_NS_CC;
USING_NS_CC_EXT;
namespace {
const char* modes[]={"transport","equipment","skills","crew","training","cabin"};
const char* titles[]={"运宝","装备养成","奥义扩展","船员养成","潜能训练","船舱"};
const char* SkinRoot="ported/growth/v1/nrb_growth_";
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
Json::Value uiAction(const char* action,int id=0){Json::Value out;out["action"]=action;out["id"]=id;return out;}
std::string number(int value){return StringConverter::toString(value);}
void textAt(CCNode* parent,const std::string& value,float x,float y,float width,float font=12,ccColor3B color=ccc3(80,42,15)){
    CCLabelTTF* text=caption(value,font,width);text->setColor(color);text->setAnchorPoint(ccp(0,1));text->setPosition(ccp(x,y));parent->addChild(text,5);
}
CCSprite* picture(CCNode* parent,const std::string& path,float x,float y,float w,float h,bool contain=false){
    CCFileUtils* files=CCFileUtils::sharedFileUtils();
    if(path.empty()||!files->isFileExist(files->fullPathForFilename(path.c_str())))return NULL;
    CCSprite* sprite=CCSprite::create(path.c_str());if(!sprite)return NULL;
    CCSize size=sprite->getContentSize();if(size.width<=0||size.height<=0)return NULL;
    if(contain)sprite->setScale(std::min(w/size.width,h/size.height));else{sprite->setScaleX(w/size.width);sprite->setScaleY(h/size.height);}
    sprite->setPosition(ccp(x,y));parent->addChild(sprite);return sprite;
}
void art(CCNode* parent,const char* name,float x,float y,float w,float h,bool contain=false){picture(parent,std::string(SkinRoot)+name+".png",x,y,w,h,contain);}
void panel(CCNode* parent,float x,float y,float w,float h){
    // 使用参考版纸张边框，保留角饰；尺寸变化只拉伸中部。
    CCLayerColor* fill=CCLayerColor::create(ccc4(239,207,146,250),w-4,h-4);fill->setPosition(ccp(x+2,y+2));parent->addChild(fill);
    CCScale9Sprite* paper=CCScale9Sprite::create((std::string(SkinRoot)+"u_SuperEvolutionBG02.png").c_str());
    if(paper){CCSize size=paper->getContentSize();paper->setCapInsets(CCRectMake(25,25,size.width-50,size.height-50));paper->setContentSize(CCSizeMake(w*2,h*2));paper->setScale(0.5f);paper->setPosition(ccp(x+w/2,y+h/2));parent->addChild(paper);}
}
std::string subjectImage(const Json::Value& subject,bool full){
    int id=subject["templateId"].asInt();std::string kind=subject["kind"].asString();
    if(id<=0)return "";
    if(kind=="equip"&&EquipTableManager::Get()->getEquipItemByID(id)){Equip value(id,true);return full?value.bodyPic():value.iconPic();}
    if(kind=="skill"&&SkillTableManager::Get()->getSkillItemByID(id)){Skill value(id,true);return full?value.bodyPic():value.iconPic();}
    if(kind=="card"&&DiscipleTableManager::Get()->getDiscipleItemByID(id)){Disciple value(id,true);return full?value.bodyPic():value.iconPic();}
    return "";
}
void portrait(CCNode* parent,const Json::Value& subject,float x,float y,float w,float h,bool full=false){
    art(parent,"u_icobg01",x,y,w,h);
    picture(parent,subjectImage(subject,full),x,y,w-8,h-8,true);
}
std::string operationName(const std::string& action){
    const char* keys[]={"temper","promote","decompose","synthesize","evolve","super","rebirth","replace","roll","accept","discard","refresh","start","claim","finish","unlock","upgrade","remove","assign","hull","ship"};
    const char* names[]={"淬炼","装备进阶","分解装备","合成装备","进化","超进化","培养重生","替换奥义","开始训练","保留结果","放弃结果","刷新品质","开始运宝","领取奖励","立即完成","解锁舱位","升级舱位","移出船员","安置船员","强化船体","切换船体"};
    for(int i=0;i<21;i++)if(action==keys[i])return names[i];return "确认操作";
}
bool onTab(const std::string& mode,int tab,const std::string& action){
    if(mode=="equipment"){const char* keys[]={"temper","promote","decompose","synthesize"};return action==keys[tab%4];}
    if(mode=="skills")return action==(tab==0?"evolve":"replace");
    if(mode=="crew"){const char* keys[]={"evolve","super","rebirth"};return action==keys[tab%3];}
    return action!="select"&&action!="page"&&action!="choice";
}
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
ExpansionSystemsPage::ExpansionSystemsPage(unsigned int id,int mode):AdventureBase(id),mMode(modes[mode]),mSelected(false),mBusy(false),mRegistered(false),mWait(0),mPoll(0),mTarget(0),mChoice(0),mOffset(0),mTab(0),mResetScroll(true),mScroll(NULL){
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
void ExpansionSystemsPage::button(CCNode* parent,const std::string& title,const Json::Value& action,float x,float y,float width,float height,bool active){
    bool local=action["action"].asString().find("ui_")==0;
    bool enabled=!mBusy&&active&&action.get("enabled",true).asBool()&&(local||mPending.isNull()||action["action"].asString()=="reload");
    CCNode* normal=CCNode::create();normal->setContentSize(CCSizeMake(width,height));
    const char* name=enabled?"u_buttenB01":"u_buttenB03";
    art(normal,name,width/2,height/2,width,height);
    CCLabelTTF* label=CCLabelTTF::create(title.c_str(),"Arial",12,CCSizeMake(width-14,0),kCCTextAlignmentCenter);label->setColor(ccc3(255,250,228));
    label->setPosition(ccp(width/2,height/2));label->setScale(std::min(1.0f,(height-8)/std::max(1.0f,label->getContentSize().height)));normal->addChild(label,2);
    CCMenuItemSprite* item=CCMenuItemSprite::create(normal,NULL,this,menu_selector(ExpansionSystemsPage::click));
    item->setContentSize(CCSizeMake(width,height));item->setTag(1000+mActions.size());mActions.append(action);
    item->setPosition(ccp(x,y));item->setEnabled(enabled);
    ExpansionMenu* menu=ExpansionMenu::create();menu->setPosition(CCPointZero);menu->addChild(item);parent->addChild(menu,10);
}

void ExpansionSystemsPage::refreshContent(){
    if(!mContentContainer)return;
    float distance=0;
    if(mScroll&&!mResetScroll)distance=mScroll->getContentSize().height-mScroll->getViewSize().height+mScroll->getContentOffset().y;
    mScroll=NULL;mContentContainer->removeAllChildrenWithCleanup(true);mActions=Json::Value(Json::arrayValue);
    CCSize original=mContentContainer->getContentSize();const CCRect clip=visibleArea(mContentContainer);
    CCPoint origin=mContentContainer->convertToWorldSpace(CCPointZero),one=mContentContainer->convertToWorldSpace(ccp(1,1));
    float sx=std::max(0.001f,fabsf(one.x-origin.x)),sy=std::max(0.001f,fabsf(one.y-origin.y));
    float width=std::min(original.width,clip.size.width/sx),unit=width/320.0f,height=clip.size.height/sy/unit;
    CCNode* canvas=CCNode::create();canvas->setContentSize(CCSizeMake(320,height));canvas->setScale(unit);
    canvas->setPosition(ccp((original.width-width)/2,(clip.origin.y-origin.y)/sy));mContentContainer->addChild(canvas);
    art(canvas,"u_backgroundB",160,height/2,320,height);
    std::string title=(!mState.isNull()&&mState.isObject())?mState["title"].asString():titles[std::find(modes,modes+6,mMode)-modes];
    textAt(canvas,title,12,height-7,180,18,ccc3(255,225,156));
    if(!mOverlay.empty()||!mConfirm.isNull()){renderOverlay(canvas,height);mResetScroll=true;return;}
    textAt(canvas,(!mState.isNull()&&mState.isObject())?"钻石 "+number(mState["goldcoins"].asInt())+"   贝里 "+mState["silvercoins"].asString():"正在读取角色存档…",12,height-31,230,10,ccc3(255,239,201));
    button(canvas,"说明",uiAction("ui_help"),283,height-20,56,26);
    button(canvas,mPending.isNull()?"刷新":"核对操作",uiAction("reload"),244,height-51,68,24);
    button(canvas,"传书",uiAction("mail"),295,height-51,42,24);
    const char* equipTabs[]={"淬炼","进阶","分解","合成"};const char* skillTabs[]={"奥义进化","天生替换"};const char* crewTabs[]={"船员进化","超进化","培养重生"};
    int count=mMode=="equipment"?4:mMode=="skills"?2:mMode=="crew"?3:0;
    if(count){
        float width=304.0f/count;
        for(int i=0;i<count;i++){
            const char* label=mMode=="equipment"?equipTabs[i]:mMode=="skills"?skillTabs[i]:crewTabs[i];
            button(canvas,std::string(i==mTab?"◆ ":"")+label,uiAction("ui_tab",i),8+width*(i+0.5f),height-78,width-3,26);
        }
    }else textAt(canvas,mMode=="transport"?"宝船品质 · 离线运输":mMode=="training"?"选择船员 · 训练 · 保留结果":"选择舱位 · 安置后备船员",12,height-68,298,12,ccc3(255,225,156));
    float viewHeight=std::max(60.0f,height-99),bodyHeight=std::max(viewHeight,mMode=="cabin"?530.0f:mMode=="transport"?425.0f:450.0f);
    mScroll=ExpansionScroll::create(CCSizeMake(320,viewHeight));mScroll->setDirection(kCCScrollViewDirectionVertical);mScroll->setPosition(ccp(0,3));
    CCNode* body=CCNode::create();body->setContentSize(CCSizeMake(320,bodyHeight));mScroll->setContainer(body);renderBody(body,bodyHeight);
    mScroll->setContentSize(body->getContentSize());
    float offset=viewHeight-body->getContentSize().height+std::max(0.0f,distance);mScroll->setContentOffset(ccp(0,std::min(0.0f,offset)));canvas->addChild(mScroll);
    mResetScroll=false;
}

void ExpansionSystemsPage::renderBody(CCNode* parent,float height){
    const Json::Value& view=mState["view"];const Json::Value& model=view["presentation"];const Json::Value& actions=view["actions"];
    const Json::Value& selected=model["selected"];const Json::Value& metrics=model["metrics"];
    float y=height;
    if(!mMessage.empty()){
        float messageHeight=caption(mMessage,11,284)->getContentSize().height+18;
        panel(parent,8,y-messageHeight,304,messageHeight);textAt(parent,mMessage,18,y-7,284,11,ccc3(145,45,20));y-=messageHeight+7;
    }
    if(model.isNull()||!model.isObject()){
        panel(parent,8,y-130,304,125);
        textAt(parent,mBusy?"正在读取服务器数据…":"请先更新并重启服务端，再进入此页。",22,y-18,276,14);
        textAt(parent,"界面加载完成前不会执行任何养成操作。",22,y-70,276,11);return;
    }
    if(mMode=="transport"){
        art(parent,"u_yunbaobg1",160,y-82,304,162);
        const char* chests[]={"u_yunbao_xiang4","u_yunbao_xiang3","u_yunbao_xiang2"};
        int quality=std::max(1,std::min(3,model["quality"].asInt()));art(parent,chests[quality-1],160,y-68,100,95,true);
        textAt(parent,"宝船已选定 · "+std::string(quality==3?"珍稀":quality==2?"精良":"普通"),67,y-130,200,14,ccc3(255,239,175));
        y-=173;
    }else if(mMode=="cabin"){
        panel(parent,8,y-174,304,170);textAt(parent,"船舱总览",21,y-13,280,16);
        const Json::Value& seats=model["seats"];
        for(unsigned int i=0;i<seats.size();i++){
            float x=47+(i%4)*75,cy=y-64-(i/4)*72;bool unlocked=seats[i]["unlocked"].asBool();int slot=seats[i]["slot"].asInt();
            portrait(parent,seats[i]["occupant"],x,cy,45,43);
            button(parent,std::string(slot==model["slot"].asInt()?"◆ ":"")+number(slot)+"号"+(unlocked?"舱":"锁"),uiAction("choice",slot),x,cy-31,69,23);
        }
        y-=181;
    }else if(mMode=="equipment"){
        art(parent,"u_EquipQuenchBG",160,y-75,304,148);
        if(mTab==3){
            textAt(parent,"装备工坊 · 选择合成配方",22,y-18,278,17,ccc3(255,226,156));
            textAt(parent,"分解闲置装备获得工坊碎片。\n合成后的成品通过传书领取。",22,y-64,278,12,ccc3(255,239,205));
        }else{
            portrait(parent,selected,67,y-61,78,80);
            textAt(parent,selected.get("name","请选择装备").asString(),120,y-19,180,15,ccc3(255,226,156));
            textAt(parent,mTab==2?"仅闲置装备可以分解":"原强化和穿戴关系保留",120,y-50,180,11,ccc3(255,239,205));
            button(parent,"选择装备",uiAction("ui_picker"),205,y-105,162,30);
        }
        y-=155;
    }else if(mMode=="skills"){
        art(parent,"u_SkillEvolutionBG01",160,y-77,304,150);art(parent,"u_SkillEvolutionPic",160,y-79,155,145,true);
        portrait(parent,selected,65,y-71,66,78);
        portrait(parent,mTab==0?selected:Json::Value(),255,y-71,66,78);
        textAt(parent,mTab==0?"进化预览":"在下方选择新奥义",204,y-119,107,10,ccc3(255,236,175));
        textAt(parent,selected.get("name","请选择奥义").asString(),14,y-119,120,11,ccc3(255,236,175));y-=155;
        button(parent,"更换奥义",uiAction("ui_picker"),160,y-16,123,29);y-=39;
    }else{
        if(mMode=="training")art(parent,"u_MemTrainingbg01",160,y-65,304,128);
        panel(parent,8,y-131,304,128);
        portrait(parent,selected,67,y-55,85,88,mMode=="crew");
        textAt(parent,selected.get("name","尚无可选对象").asString(),120,y-14,179,15);
        textAt(parent,(!selected.isNull()&&selected.isObject())?"Lv."+number(selected["level"].asInt()):"请先获得船员或装备",122,y-41,175,11);
        button(parent,mMode=="equipment"?"选择装备":"选择船员",uiAction("ui_picker"),205,y-93,162,30);
        y-=139;
    }
    if(mMode=="cabin"){
        int slot=model["slot"].asInt();const Json::Value& seat=model["seats"][std::max(0,slot-1)];const Json::Value& occupant=seat["occupant"];
        panel(parent,8,y-91,304,86);portrait(parent,occupant,46,y-42,50,51);
        textAt(parent,number(slot)+"号舱 · "+(seat["unlocked"].asBool()?"等级 "+number(seat["level"].asInt()):"未解锁"),83,y-12,220,13);
        textAt(parent,occupant.get("name","空舱").asString(),83,y-34,220,11);
        button(parent,"选择后备船员",uiAction("ui_picker"),210,y-65,157,25);y-=96;
        textAt(parent,"待安置："+selected.get("name","暂无后备船员").asString(),15,y-2,290,11);y-=25;
    }
    float metricsHeight=metrics.size()*21+16;
    if(metrics.size())panel(parent,8,y-metricsHeight,304,metricsHeight-3);
    for(unsigned int i=0;i<metrics.size();i++){
        textAt(parent,metrics[i]["label"].asString(),19,y-10-i*21,136,11);
        textAt(parent,metrics[i]["value"].asString(),157,y-10-i*21,143,11,ccc3(134,61,14));
    }
    y-=metricsHeight+7;
    if(mMode=="training"){
        textAt(parent,"训练后可先查看结果，再选择保留或放弃。",15,y,290,11,ccc3(255,238,190));y-=26;
    }
    bool any=false;
    for(unsigned int i=0;i<actions.size();i++){
        const Json::Value& a=actions[i];std::string action=a["action"].asString();if(!onTab(mMode,mTab,action))continue;
        any=true;
        if(!a["subject"].isNull()&&a["subject"].isObject()){
            panel(parent,8,y-64,304,60);portrait(parent,a["subject"],43,y-32,42,42);
            textAt(parent,a["subject"]["name"].asString(),75,y-12,224,12);y-=68;
        }
        const Json::Value& preview=model["previews"][action];
        if(preview.size()){
            float ph=preview.size()*23+28;panel(parent,8,y-ph,304,ph-3);textAt(parent,"成长预览",20,y-10,280,12);
            for(unsigned int j=0;j<preview.size();j++)textAt(parent,preview[j]["label"].asString()+"  "+preview[j]["value"].asString(),20,y-32-j*23,280,11);
            y-=ph+6;
        }
        std::string detail=a["label"].asString();size_t begin=detail.find("（");if(begin!=std::string::npos)detail=detail.substr(begin);
        CCLabelTTF* cost=caption(detail,11,286);cost->setColor(ccc3(255,232,170));cost->setAnchorPoint(ccp(0,1));cost->setPosition(ccp(17,y-3));parent->addChild(cost,5);y-=cost->getContentSize().height+11;
        button(parent,operationName(action),a,160,y-17,196,35);y-=47;
    }
    if(!any){panel(parent,8,y-62,304,58);textAt(parent,"当前没有可执行的操作。请更换对象，或先满足说明中的条件。",20,y-12,280,12);y-=68;}
    // 每一页内容高度按实际文本计算，长说明、更多配方和窄屏均可滚动到底。
    float extra=std::max(0.0f,12-y);
    if(extra>0){CCArray* children=parent->getChildren();CCObject* object=NULL;CCARRAY_FOREACH(children,object){CCNode* child=static_cast<CCNode*>(object);child->setPositionY(child->getPositionY()+extra);}parent->setContentSize(CCSizeMake(320,height+extra));}
}

void ExpansionSystemsPage::renderOverlay(CCNode* parent,float height){
    const bool confirm=!mConfirm.isNull(),picker=mOverlay=="picker",help=mOverlay=="help";
    std::string title=confirm?"确认"+operationName(mConfirm["action"].asString()):picker?"选择对象":help?"玩法说明":"操作结果";
    textAt(parent,title,16,height-36,290,15,ccc3(255,233,169));
    float viewHeight=std::max(40.0f,height-100);ExpansionScroll* scroll=ExpansionScroll::create(CCSizeMake(320,viewHeight));scroll->setPosition(ccp(0,50));scroll->setDirection(kCCScrollViewDirectionVertical);
    CCNode* content=CCNode::create();float total=picker?320:help?400:230;content->setContentSize(CCSizeMake(320,total));scroll->setContainer(content);
    panel(content,8,0,304,total);float y=total-14;
    if(picker){
        bool found=false;const Json::Value& actions=mState["view"]["actions"];
        for(unsigned int i=0;i<actions.size();i++){
            const Json::Value& a=actions[i];std::string action=a["action"].asString();
            if(action=="select"){
                found=true;portrait(content,a["subject"],45,y-28,45,46);std::string name=a["subject"].get("name",a["label"].asString()).asString();
                textAt(content,name,78,y-8,140,13);textAt(content,"Lv."+number(a["subject"]["level"].asInt())+"  #"+number(a["id"].asInt()),78,y-30,140,10);
                button(content,"选择",a,264,y-28,66,31);y-=65;
            }else if(action=="page"){button(content,a["id"].asInt()<mOffset?"上一页":"下一页",a,a["id"].asInt()<mOffset?84:236,24,128,28);}
        }
        if(!found)textAt(content,"没有可选择的对象。船舱仅显示未上阵的后备船员。",21,y-15,278,13);
    }else{
        std::string message;
        if(confirm){message=mConfirm["label"].asString()+"？\n\n";if(mConfirm["action"].asString()=="decompose")message+="装备将被删除，此操作不可撤销。\n\n";message+="以服务器校验结果为准，请勿重复提交。";}
        else if(help){for(unsigned int i=0;i<mState["view"]["lines"].size();i++)message+=mState["view"]["lines"][i].asString()+"\n\n";message+="奖励发至传书的操作，完成后请进入传书领取。";}
        else message=mMessage.empty()?"操作已完成，请查看最新属性。":mMessage;
        CCLabelTTF* label=caption(message,13,274);float required=label->getContentSize().height+30;
        if(required>total){content->removeAllChildrenWithCleanup(true);total=required;content->setContentSize(CCSizeMake(320,total));panel(content,8,0,304,total);}
        label->setColor(ccc3(80,42,15));label->setAnchorPoint(ccp(0,1));label->setPosition(ccp(23,total-15));content->addChild(label,5);
    }
    scroll->setContentSize(content->getContentSize());scroll->setContentOffset(ccp(0,viewHeight-total));parent->addChild(scroll);
    if(confirm){button(parent,"取消",uiAction("cancel"),85,26,126,32);button(parent,"确认操作",uiAction("confirm"),235,26,126,32);}
    else {button(parent,"返回",uiAction("ui_close"),85,26,126,32);button(parent,mPending.isNull()?"前往传书":"核对操作",uiAction(mPending.isNull()?"mail":"reload"),235,26,126,32);}
}
std::string ExpansionSystemsPage::nonce(){static unsigned int sequence=0;char value[33];sprintf(value,"%08x%08x%08x%08x",(unsigned int)time(NULL),(unsigned int)clock(),(unsigned int)rand(),++sequence);return value;}
void ExpansionSystemsPage::send(const Json::Value& request){if(!mSelected||mOwner!=GamePrecedure::Get()->getUin())return;std::string data=Json::FastWriter().write(request);mBusy=true;mWait=0;refreshContent();PacketManager::Get()->sendPakcet(48003,&data[0],data.size(),false);}
void ExpansionSystemsPage::info(){if(mBusy)return;Json::Value request;request["mode"]=mMode;request["action"]="info";request["id"]=mTarget;request["choice"]=mChoice;request["offset"]=mOffset;mInfo=nonce();request["requestId"]=mInfo;send(request);}
void ExpansionSystemsPage::savePending(){CCUserDefault::sharedUserDefault()->setStringForKey(mKey.c_str(),mPending.isNull()?"":Json::FastWriter().write(mPending));CCUserDefault::sharedUserDefault()->flush();}
void ExpansionSystemsPage::click(CCObject* sender){
    if(mBusy)return;
    if(sender){
        int index=static_cast<CCNode*>(sender)->getTag()-1000;if(index<0||index>=(int)mActions.size())return;
        // 旧触摸分发器还在反向遍历本页节点；这里只排队，下一帧再切换界面或提交。
        if(mState["_queuedUiAction"].isNull())mState["_queuedUiAction"]=mActions[index];return;
    }
    if(mState["_queuedUiAction"].isNull())return;
    Json::Value item=mState["_queuedUiAction"];mState.removeMember("_queuedUiAction");std::string action=item["action"].asString();
    if(action=="ui_tab"){mTab=item["id"].asInt();mMessage.clear();mResetScroll=true;refreshContent();}
    else if(action=="ui_picker"){mOverlay="picker";mResetScroll=true;refreshContent();}
    else if(action=="ui_help"){mOverlay="help";refreshContent();}
    else if(action=="ui_close"){mOverlay.clear();mConfirm=Json::Value();mResetScroll=true;refreshContent();}
    else if(action=="reload"){mConfirm=Json::Value();mOverlay.clear();if(mPending.isNull())info();else send(mPending);}
    else if(action=="mail"){MsgMainFrameChangePage message;message.pageName="SysMsgPage";MessageManager::Get()->sendMessage(&message);}
    else if(action=="select"){mTarget=item["id"].asInt();mOverlay.clear();mMessage.clear();mResetScroll=true;info();}
    else if(action=="choice"){mChoice=item["id"].asInt();mResetScroll=true;info();}
    else if(action=="page"){mOffset=item["id"].asInt();info();}
    else if(action=="cancel"){mConfirm=Json::Value();mResetScroll=true;refreshContent();}
    else if(action=="confirm"){
        mPending=mConfirm;mPending["mode"]=mMode;mPending["requestId"]=nonce();mPending["revision"]=mState["revision"];mPending["configRevision"]=mState["configRevision"];
        mConfirm=Json::Value();mOverlay.clear();savePending();send(mPending);
    }else{mConfirm=item;mOverlay.clear();refreshContent();}
}
void ExpansionSystemsPage::onReceivePacket(int opcode,const std::string& value){
    if(opcode!=48004||!mSelected||mOwner!=GamePrecedure::Get()->getUin())return;Json::Value reply;Json::Reader reader;
    if(!reader.parse(value,reply,false)||reply["mode"].asString()!=mMode)return;std::string id=reply["requestId"].asString();bool pending=!mPending.isNull()&&id==mPending["requestId"].asString();
    if(!pending&&id!=mInfo)return;mBusy=false;mWait=0;
    if(!reply.get("ok",false).asBool()){
        mMessage=reply["message"].asString();if(pending&&!reply.get("uncertain",false).asBool()){mPending=Json::Value();savePending();mOverlay="result";}refreshContent();return;
    }
    if(reply["owner"].asString()!=mOwner){failure();return;}
    mState=reply;
    if(!reply["view"]["presentation"]["selected"].isNull()&&reply["view"]["presentation"]["selected"].isObject())mTarget=reply["view"]["presentation"]["selected"]["id"].asInt();
    if(mMode=="cabin"){
        mChoice=reply["view"]["presentation"].get("slot",1).asInt();
        // 首页只缓存外观选择；船体权益及成长仍以服务端数据为准。
        int ship=reply["view"]["presentation"].get("ship",1).asInt();
        std::string key="HomeShip_"+StringConverter::toString(GamePrecedure::Get()->getServerID())+"_"+mOwner;
        CCUserDefault* defaults=CCUserDefault::sharedUserDefault();
        if(ship>=1&&ship<=2&&defaults->getIntegerForKey(key.c_str(),1)!=ship){defaults->setIntegerForKey(key.c_str(),ship);defaults->flush();}
    }
    if(pending){mMessage=reply["result"]["message"].asString();mPending=Json::Value();mInfo.clear();mOffset=0;savePending();mOverlay="result";}
    ServerDateManager::Get()->getUserBasicInfo().goldcoins=reply["goldcoins"].asUInt();refreshContent();
    if(!mPending.isNull())send(mPending);
}
void ExpansionSystemsPage::failure(){if(!mSelected)return;mBusy=false;mWait=0;mMessage="连接中断或超时，请点击刷新核对原操作";refreshContent();}
void ExpansionSystemsPage::loadData(){if(!mRegistered){PacketManager::Get()->regisiterPacketHandler(48004,this);mRegistered=true;}mSelected=true;mBusy=false;if(mPending.isNull())info();else send(mPending);}
void ExpansionSystemsPage::updatePage(bool selected,float dt){if(!selected){exitPage();return;}mSelected=true;if(!mState.isNull()&&mState.isObject()&&!mState["_queuedUiAction"].isNull()){bool leaving=mState["_queuedUiAction"]["action"].asString()=="mail";click(NULL);if(leaving)return;}if(!mSelected)return;if(mBusy){mWait+=dt;if(mWait>20)failure();}else if(mMode=="transport"){mPoll+=dt;if(mPoll>=5&&mConfirm.isNull()&&mPending.isNull()&&mOverlay.empty()){mPoll=0;info();}}}
void ExpansionSystemsPage::exitPage(){mSelected=false;mBusy=false;mConfirm=Json::Value();mState.removeMember("_queuedUiAction");mOverlay.clear();if(mRegistered){PacketManager::Get()->removePacketHandler(48004,this);mRegistered=false;}}
