//
//  HXAppPlatformKitPro.h
//  HXAppPlatformKitPro
//
//  Created by teng looyao on 13-5-20.
//  Copyright (c) 2013年 teng looyao. All rights reserved.
//

#import <Foundation/Foundation.h>

//用户注册完成的通知
#define HX_NOTIFICATION_REGISTER @"HX_NOTIFICATION_REGISTER"
//用户登录的通知
#define HX_NOTIFICATION_LOGIN @"HX_NOTIFICATION_LOGIN"
//用户注销的通知
#define HX_NOTIFICATION_LOGOUT @"HX_NOTIFICATION_LOGOUT"
//SDK界面关闭的通知
#define HX_NOTIFICATION_CLOSEVIEW @"HX_NOTIFICATION_CLOSEVIEW"


@interface HXAppPlatformKitPro : NSObject

//设置支持的旋转方向
+ (void)setSupportOrientationPortrait:(BOOL)portrait portraitUpsideDown:(BOOL)portraitUpsideDown landscapeLeft:(BOOL)landscapeLeft landscapeRight:(BOOL)landscapeRight;

//显示登录界面
+ (void)showLoginView;

//显示平台界面
+ (void)showPlatformView;

//获取当前用户ID
+ (NSString *)getUserId;

//获取当前用户的SessionId
+ (NSString *)getSessionId;

//设置游戏的appId 和 appKey, 合作接入时分配, 可以找联系客服获取
+ (void)setAppId:(int)appId appKey:(NSString *)appKey;

//注销登录
+ (void)logout;

//充值
//@param amount: 充值金额 (元)
//@param orderIdCom: 厂商订单号
+ (void)setPayViewAmount:(float)amount orderIdCom:(NSString *)orderIdCom;


//充值(不使用充值平台用户系统的, 直接调用支付)
//@param account: 充值账号
//@param amount: 充值金额 (元)
//@param orderIdCom: 厂商订单号
+ (void)setPayViewAccount:(NSString *)account amount:(float)amount orderIdCom:(NSString *)orderIdCom;

@end
