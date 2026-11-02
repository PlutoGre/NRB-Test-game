#include "libPP.h"
#include "libPPObj.h"
#include <string>
#include "libOS.h"
#import <PPAppPlatformKit/PPAppPlatformKit.h>

libPPObj* s_libPPOjb;


void libPP::init(bool isPrivateLogin )
{

    /**
     *必须写在程序window初始化之后。详情请commad + 鼠标左键 点击查看接口注释
     *初始化应用的AppId和AppKey。从开发者中心游戏列表获取（https://pay.25pp.com）
     *设置是否打印日志在控制台
     *设置充值页面初始化金额
     *是否需要客户端补发订单（详情请查阅接口注释）
     *用户注销后是否自动push出登陆界面
     *是否开放充值页面【操作在按钮被弹窗】
     *若关闭充值响应的提示语
     *初始化SDK界面代码
     */
    [[PPAppPlatformKit sharedInstance] setAppId:1021 AppKey:@"9e583cec111587f81a8fbc65107a2daf"];
    [[PPAppPlatformKit sharedInstance] setIsNSlogData:YES];
    [[PPAppPlatformKit sharedInstance] setRechargeAmount:@"10"];
    [[PPAppPlatformKit sharedInstance] setIsGetBillNo:NO];
    //[[PPAppPlatformKit sharedInstance] setIsLogOutPushLoginView:YES];
    [[PPAppPlatformKit sharedInstance] setIsOpenRecharge:YES];
    //[[PPAppPlatformKit sharedInstance] setCloseRechargeAlertMessage:@"关闭充值提示语"];
    [PPUIKit sharedInstance];


    s_libPPOjb = [libPPObj new];
    [s_libPPOjb registerNotification];

    [s_libPPOjb SNSInitResult:0];
}

void libPP::updateApp()
{
    libOS::getInstance()->openURL("http://1251001040.imgcache.qzoneapp.com/1251001040/download/pp/index_ios.html");
    //[[NdComPlatform defaultPlatform] NdAppVersionUpdate:0 delegate:s_libPPOjb];
    //[s_libPPOjb updateApp];
}
void libPP::final()
{
    //[s_libPPOjb unregisterNotification];
}
bool libPP::getLogined()
{
    return mEnableLogin;//[[NdComPlatform defaultPlatform] isLogined];
}
void libPP::login()
{
   [[PPLoginView sharedInstance] showLoginViewByRight];
}
void libPP::logout()
{
	//[[NdComPlatform defaultPlatform] NdLogout:0];
}
void libPP::switchUsers()
{
    [[PPCenterView sharedInstance] showCenterViewByRight];
}

void libPP::buyGoods(BUYINFO& info)
{
    if(info.cooOrderSerial=="")
    {
        info.cooOrderSerial = libOS::getInstance()->generateSerial();
    }
    //设置订单号
    NSString *billNO = [[NSString alloc] initWithUTF8String:info.cooOrderSerial.c_str()];
    NSString *pname = [[NSString alloc] initWithUTF8String:info.productName.c_str()];
    NSString *priStr = [NSString stringWithFormat:@"%.2lf",info.productPrice ];
    NSString *_price = [[NSString alloc] initWithFormat:@"%@",priStr];
    NSString *uin = [[NSString alloc] initWithUTF8String:loginUin().c_str()];
    int zoneID = 0;
    sscanf(info.description.c_str(),"%d",&zoneID);
    mBuyInfo = info;
    //调出充值并且兑换接口
    [[PPWebView sharedInstance] rechargeAndExchangeWebShow:billNO BillNoTitle:pname PayMoney:_price
                                                    RoleId:uin ZoneId:zoneID];

}
const std::string& libPP::loginUin()
{
    int userID = [[PPAppPlatformKit sharedInstance] currentUserId];

    NSString* retNS = [[NSString alloc] initWithFormat:@"%d",userID];
    static std::string retStr;
    if(retNS) retStr = [retNS UTF8String];
    retStr = "PPUSR_"+retStr;
    return retStr;
}
const std::string& libPP::sessionID()
{
    
    int sessionID = [[PPAppPlatformKit sharedInstance] currentSessionId];
    
    NSString* retNS = [[NSString alloc] initWithFormat:@"%d",sessionID];
    static std::string retStr;
    if(retNS) retStr = (const char*)[retNS UTF8String];
    return retStr;
}
const std::string& libPP::nickName()
{
    NSString* retNS = [[PPAppPlatformKit sharedInstance] currentUserName];
    static std::string retStr;
    if(retNS) retStr = [retNS UTF8String];
    return retStr;
}

void libPP::openBBS()
{
	 //[[NdComPlatform defaultPlatform] NdEnterAppBBS:0];
}


void libPP::userFeedBack()
{
    //[[NdComPlatform defaultPlatform] NdUserFeedBack];
}
void libPP::gamePause()
{
    //[[NdComPlatform defaultPlatform] NdPause];
}

const std::string libPP::getClientChannel()
{
    return "PP";
}
const unsigned int libPP::getPlatformId()
{
    return 0u;
}

std::string libPP::getPlatformMoneyName()
{
    return "PP币";
}