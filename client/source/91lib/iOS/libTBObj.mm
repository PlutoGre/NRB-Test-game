//
//  libTBObj.m
//  libTB
//
//  Created by lvjc on 13-7-25.
//  Copyright (c) 2013年 youai. All rights reserved.
//
#include "libTB.h"
#import "libTBObj.h"
#import <TBPlatform/TBPlatform.h>


@implementation libTBObj
-(void) SNSInitResult:(NSNotification *)notify
{
    [self registerNotification];
    std::string outstr = "";
    libPlatformManager::getPlatform()->_boardcastUpdateCheckDone(true,outstr);
}

-(void) registerNotification
{
 
    /* 监听登录结果通知（新版接口统一成功和失败的通知）*/
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(SNSLoginResult:)
                                                 name:(NSString *)kTBLoginNotification
                                               object:nil];
    /* 监听离开平台通知*/

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(PlatformLogout:)
                                                 name:(NSString *)kTBLeavePlatformNotification
                                               object:nil];
    
    
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
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

/**
 *	@brief	平台关闭通知，监听离开平台通知以对游戏界面进行重新调整
 */	
- (void)PlatformLogout:(NSNotification *)notify
{
    static int leaveCount=1;
    if(leaveCount%2!=0)
    {
        libTB* plat = dynamic_cast<libTB*>(libPlatformManager::getPlatform());
        if(plat)
        {
            plat->_disableLogin();
        }
        libPlatformManager::getPlatform()->_boardcastPlatformLogout();
    }
    leaveCount++;
}

/**
 *	@brief	登录通知监听方法，登录成功、失败都在这个方法处理
 *
 *	@param 	notification 	通知的userInfo包含登录结果信息
 */
- (void)SNSLoginResult:(NSNotification *)notify
{
    NSDictionary *dict = [notify userInfo];
	BOOL success = [[dict objectForKey:@"result"] boolValue];
    
    
	/*登录成功后处理*/
	if([[TBPlatform defaultPlatform] isLogined] && success)
    {
		NSString* strTip = [NSString stringWithFormat:@"游客账号登录成功"];
        std::string out = [strTip UTF8String];
        libTB* plat = dynamic_cast<libTB*>(libPlatformManager::getPlatform());
        if(plat)
        {
            plat->_enableLogin();
        }
        libPlatformManager::getPlatform()->_boardcastLoginResult(true, out);
	}
	else
    {
        /*登录失败处理和相应提示*/
		int error = [[dict objectForKey:@"error"] intValue];
		NSString* strTip = [NSString stringWithFormat:@"登录失败, error=%d", error];
		switch (error) {
			case TB_PLATFORM_LOGIN_INCORRECT_ACCOUNT_OR_PASSWORD_ERROR:
				strTip = @"帐号或密码错误，请重试";
				break;
			case TB_PLATFORM_LOGIN_INVALID_ACCOUNT_ERROR:
				strTip = @"帐号被禁用，请与管理员联系";
				break;
			case TB_PLATFORM_LOGIN_SYNC_FAILED_ERROR:
				strTip = @"同步帐号失败，请重试";
				break;
			case TB_PLATFORM_LOGIN_REQUEST_FAILED_ERROR:
				strTip = @"登录请求失败，请检查网络";
				break;
			default:
				break;
		}
        std::string out = [strTip UTF8String];
        libPlatformManager::getPlatform()->_boardcastLoginResult(false, out);
        
	}
	
}

-(void)NdUniPayAysnResult:(NSNotification*)notify
{
    //回调购买成功。其余都是失败
    if ([[notify object] isEqualToString:@"购买成功"])
    {
		libTB* plat = dynamic_cast<libTB*>(libPlatformManager::getPlatform());
		if(plat)
		{
			BUYINFO info = plat->getBuyInfo();
			std::string log("购买成功");
			libPlatformManager::getPlatform()->_boardcastBuyinfoSent(true, info, log);
		}
        
    }
}

#pragma mark - BuyGoods Delegate
- (void)TBBuyGoodsDidSuccessWithOrder:(NSString*)order
{
    libTB* plat = dynamic_cast<libTB*>(libPlatformManager::getPlatform());
    if(plat)
    {
        BUYINFO info = plat->getBuyInfo();
        std::string log("购买成功");
        libPlatformManager::getPlatform()->_boardcastBuyinfoSent(true, info, log);
    }
}

- (void)TBBuyGoodsDidFailedWithOrder:(NSString *)order resultCode:(TB_BUYGOODS_ERROR)errorType;{
    switch (errorType) {
        case kBuyGoodsOrderEmpty:
            NSLog(@"订单号为空");
            break;
        case kBuyGoodsBalanceNotEnough:
            NSLog(@"推币余额不足");
            break;
        case kBuyGoodsServerError:
            NSLog(@"服务器错误");
            break;
        case kBuyGoodsOtherError:
            NSLog(@"其他错误");
            
            break;
        default:
            break;
    }
}

/**
 *	@brief	检查更新完成后回调
 *
 *	@param 	updateResult 	结果标识
 */
-(void) appVersionUpdateDidFinish:(TB_APP_UPDATE_RESULT)updateResult
{
    NSString *title = nil;
    NSLog(@"update result:%d", updateResult);
    switch (updateResult) {
        case TB_APP_UPDATE_NO_NEW_VERSION:
            title = @"无可用更新";//正常进入游戏
            break;
        case TB_APP_UPDATE_NEW_VERSION_DOWNLOAD_FAIL:
            title = @"下载新版本失败,\n请重新启动游戏！";//可以正常进入游戏，强制更新可由服务端限制登录
            break;
        case TB_APP_UPDATE_CHECK_NEW_VERSION_FAIL:
            title = @"检测新版本信息失败,\n请重新启动游戏！"; /*可能是网络问题，那么建议检查下网络，也可以直接进入游戏，
                                   这边的风险在于如果是客户端与服务器版本不兼容，容易引起客户端异常*/
            break;
        case TB_APP_UPDATE_UPDATE_CANCEL_BY_USER:
            title = @"用户取消更新,\n请重新启动游戏！"; //进入游戏，需要的话，可以提示进一步提示玩家更新的好处和目的
            break;
        default:
            break;
    }
    
    std::string outstr([title UTF8String]);
    
    if (TB_APP_UPDATE_NO_NEW_VERSION == updateResult)
    {
        [self registerNotification];
		libTB* platTB = dynamic_cast<libTB*>(libPlatformManager::getPlatform());
		if(platTB)
		{
			platTB->_enableLogin();
		}
        libPlatformManager::getPlatform()->_boardcastUpdateCheckDone(true,outstr);
    }
    else
    {
        libPlatformManager::getPlatform()->_boardcastUpdateCheckDone(false,outstr);
    }
    
}

-(void) uploadChannelIdDidFinish:(int)error
{
}

-(void) updateApp
{
 
}

@end
