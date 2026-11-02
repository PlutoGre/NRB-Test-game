#import <Foundation/Foundation.h>
#include "LocalPlatform.h"
#include "libOS.h"

void LocalPlatform::init(bool) {
    // Defer callbacks until LoadingFrame finishes creating its scene.
    dispatch_async(dispatch_get_main_queue(), ^{
        _boardcastInitDone(true, "iOS local channel ready");
        updateApp();
    });
}

void LocalPlatform::updateApp() {
    _boardcastUpdateCheckDone(true, "Resource updates use the configured game server");
}

void LocalPlatform::login() {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSString *identity = [defaults stringForKey:@"OnePieceLocalAccount"];
    if (!identity.length) {
        identity = [@"ios_" stringByAppendingString:
            [[[NSUUID UUID] UUIDString] stringByReplacingOccurrencesOfString:@"-" withString:@""]];
        [defaults setObject:identity forKey:@"OnePieceLocalAccount"];
    }
    uid = [identity UTF8String];
    loggedIn = true;
    NSLog(@"[IOSLocal] Local account login completed");
    _boardcastLoginResult(true, "Local login completed");
}

void LocalPlatform::logout() {
    loggedIn = false;
    _boardcastPlatformLogout();
}

void LocalPlatform::switchUsers() {
    // Keep the existing character identity across reconnects and launches.
    login();
}

void LocalPlatform::buyGoods(BUYINFO& info) {
    NSLog(@"[IOSLocal] Purchase unavailable in the development channel");
    _boardcastBuyinfoSent(false, info, "Purchases are not configured");
    libOS::getInstance()->showMessagebox("当前为本地开发客户端，未接入充值服务。");
}
