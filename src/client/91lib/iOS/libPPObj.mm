//
//  libPPObj.m
//  libPP
//
//  Created by wzy on 13-3-5.
//  Copyright (c) 2013年 youai. All rights reserved.
//
#include "libPP.h"
#import "libPPObj.h"



@implementation libPPObj
-(void) SNSInitResult:(NSNotification *)notify
{
    [self registerNotification];
    std::string outstr = "";
    libPlatformManager::getPlatform()->_boardcastUpdateCheckDone(true,outstr);
}

-(void) registerNotification
{
    
    //添加监听请求登陆【只成功有效】
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(SNSLoginResult:)
                                                 name:PP_CLIENT_LOGIN_NOTIFICATION object:nil];
    //添加监听关闭客户端页面信息
    //[[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(closePageViewCallBack:) name:PP_CLIENT_CLOSEPAGEVIEW_NOTIFICATION object:nil];
    //添加监听兑换结果返回信息
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(NdUniPayAysnResult:)
                                                 name:PP_CLIENT_EXCHANGE_NOTIFICATION object:nil];
    
    //添加监听注销
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(PlatformLogout:)
                                                 name:PP_CLIENT_LOGOFF_NOTIFICATION object:nil];
    
    //添加监听关闭Web页面
    //[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(closeWebViewCallBack:) name:PP_CLIENT_CLOSEWEB_NOTIFICATION object:nil];
    
    //添加监听补发订单
    //[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(postBillNoCallBack:) name:PP_CLIENT_POSTBILLNOLIST_NOTIFICATION object:nil];

    
    CGRect rect = [UIScreen mainScreen].applicationFrame; //获取屏幕大小
    waitView = [[UIActivityIndicatorView alloc] initWithFrame:rect];//定义图标大小，此处为32x32
    [waitView setCenter:CGPointMake(rect.size.width/2,rect.size.height/2)];//根据屏幕大小获取中心点
    [waitView setActivityIndicatorViewStyle:UIActivityIndicatorViewStyleWhiteLarge];//设置图标风格，采用灰色的，其他还有白色设置
    [waitView setBackgroundColor:[ UIColor colorWithWhite: 0.0 alpha: 0.5 ]];
    //[self.view addSubview:waitView];//添加该waitView
    [[[UIApplication sharedApplication] keyWindow] addSubview:waitView];
}

-(void) unregisterNotification
{
  //  [[NSNotificationCenter defaultCenter] removeObserver:self name:kNdCPBuyResultNotification object:nil];
}

- (void)PlatformLogout:(NSNotification *)notify
{
	libPP* plat = dynamic_cast<libPP*>(libPlatformManager::getPlatform());
	if(plat)
	{
		plat->_disableLogin();
	}
    libPlatformManager::getPlatform()->_boardcastPlatformLogout();
}
//登录
- (void)SNSLoginResult:(NSNotification *)notify
{
    NSString* strTip = [NSString stringWithFormat:@"游客账号登录成功"];
    std::string out = [strTip UTF8String];
    libPP* plat = dynamic_cast<libPP*>(libPlatformManager::getPlatform());
	if(plat)
	{
		plat->_enableLogin();
	}
    libPlatformManager::getPlatform()->_boardcastLoginResult(true, out);
	
}

-(void)NdUniPayAysnResult:(NSNotification*)notify
{
    if (PP_ISNSLOG) {
        NSLog(@"兑换的回调%@",notify.object);
    }
    //回调购买成功。其余都是失败
    if ([[notify object] isEqualToString:@"购买成功"])
    {
		libPP* plat = dynamic_cast<libPP*>(libPlatformManager::getPlatform());
		if(plat)
		{
			BUYINFO info = plat->getBuyInfo();
			std::string log("购买成功");
			libPlatformManager::getPlatform()->_boardcastBuyinfoSent(true, info, log);
		}
        
    }
}

-(void) uploadChannelIdDidFinish:(int)error
{
}

-(void) updateApp
{
 
}

@end
