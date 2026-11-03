#import <Foundation/Foundation.h>
#import <Security/Security.h>
#import <UIKit/UIKit.h>
#include "LocalPlatform.h"
#include "libOS.h"
#include "SeverConsts.h"
#include "ConnectionSettings.h"

static UIViewController *AccountPresenter() {
    UIViewController *controller = UIApplication.sharedApplication.keyWindow.rootViewController;
    while (controller.presentedViewController) controller = controller.presentedViewController;
    return controller;
}

// Only an opaque login token is cached. Passwords never enter UserDefaults,
// Keychain, game saves or diagnostic logs.
static NSMutableDictionary *AccountKey(NSString *base) {
    return [NSMutableDictionary dictionaryWithDictionary:@{(id)kSecClass:(id)kSecClassGenericPassword,
        (id)kSecAttrService:@"com.nrb.game.account.v1", (id)kSecAttrAccount:base}];
}
static NSDictionary *SavedAccount(NSString *base) {
    NSMutableDictionary *query = AccountKey(base);
    query[(id)kSecReturnData] = @YES; query[(id)kSecMatchLimit] = (id)kSecMatchLimitOne;
    CFTypeRef found = NULL;
    if (SecItemCopyMatching((CFDictionaryRef)query, &found) != errSecSuccess) return nil;
    NSDictionary *result = [NSJSONSerialization JSONObjectWithData:(NSData *)found options:0 error:nil];
    [result retain]; CFRelease(found);
    if (![result isKindOfClass:NSDictionary.class]) { [result release]; return nil; }
    return [result autorelease];
}
static void SaveAccount(NSString *base, NSDictionary *value) {
    NSMutableDictionary *query = AccountKey(base);
    SecItemDelete((CFDictionaryRef)query);
    if (!value) return;
    query[(id)kSecValueData] = [NSJSONSerialization dataWithJSONObject:
        @{ @"username":value[@"username"], @"token":value[@"token"] } options:0 error:nil];
    query[(id)kSecAttrAccessible] = (id)kSecAttrAccessibleAfterFirstUnlockThisDeviceOnly;
    if (SecItemAdd((CFDictionaryRef)query, NULL) != errSecSuccess)
        NSLog(@"[IOSAccount] SaveAccount: token cache unavailable; next launch needs credentials");
}

@interface OPAccountLogin : NSObject <NSURLSessionTaskDelegate> {
    LocalPlatform *_owner;
    NSString *_base;
    NSString *_lastName;
    NSURLSession *_http;
    BOOL _busy;
    BOOL _showing;
    NSUInteger _generation;
}
+ (instancetype)shared;
- (void)start:(LocalPlatform *)owner switching:(BOOL)switching;
- (void)forget;
@end

@implementation OPAccountLogin
+ (instancetype)shared {
    static OPAccountLogin *value;
    if (!value) value = [[OPAccountLogin alloc] init];
    return value;
}
- (void)request:(NSString *)action body:(NSDictionary *)body completion:(void (^)(NSDictionary *, NSInteger, NSString *))completion {
    if (!_http) {
        NSURLSessionConfiguration *config = NSURLSessionConfiguration.ephemeralSessionConfiguration;
        config.timeoutIntervalForRequest = 20; config.timeoutIntervalForResource = 25;
        config.HTTPCookieStorage = nil; config.HTTPShouldSetCookies = NO;
        _http = [[NSURLSession sessionWithConfiguration:config delegate:self delegateQueue:NSOperationQueue.mainQueue] retain];
    }
    NSURL *url = [NSURL URLWithString:[_base stringByAppendingFormat:@"/account/v1/%@", action]];
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:url];
    request.HTTPMethod = @"POST"; request.HTTPBody = [NSJSONSerialization dataWithJSONObject:body options:0 error:nil];
    [request setValue:@"application/json; charset=utf-8" forHTTPHeaderField:@"Content-Type"];
    if (OPRemoteEnabled()) [request setValue:[@"Bearer " stringByAppendingString:@(OPRemoteInvite().c_str())]
        forHTTPHeaderField:@"Authorization"];
    [[_http dataTaskWithRequest:request completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
        NSInteger code = [response isKindOfClass:NSHTTPURLResponse.class] ? [(NSHTTPURLResponse *)response statusCode] : 0;
        NSDictionary *value = data && data.length <= 8192 ? [NSJSONSerialization JSONObjectWithData:data options:0 error:nil] : nil;
        if (![value isKindOfClass:NSDictionary.class]) value = nil;
        NSString *message = (code == 200 && [value[@"ok"] boolValue]) ? nil :
            ([value[@"message"] isKindOfClass:NSString.class] ? value[@"message"] : @"无法连接账号服务，请确认服务端已启动后重试");
        if (error) NSLog(@"[IOSAccount] request: connection failed, code=%ld", (long)error.code);
        completion(value, code, message);
    }] resume];
}
- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task
    willPerformHTTPRedirection:(NSHTTPURLResponse *)response newRequest:(NSURLRequest *)request
    completionHandler:(void (^)(NSURLRequest *))completionHandler {
    // Never replay credentials to a redirect destination.
    completionHandler(nil);
}
- (void)presentAccountAlert:(UIAlertController *)alert {
    UIViewController *presenter = AccountPresenter();
    // A fast LAN response may arrive before the previous account alert has
    // finished dismissing. Wait for UIKit instead of losing the retry dialog.
    if (!presenter.view.window || presenter.isBeingPresented || presenter.isBeingDismissed
            || presenter.transitionCoordinator || [presenter isKindOfClass:UIAlertController.class]) {
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 100 * NSEC_PER_MSEC), dispatch_get_main_queue(), ^{
            [self presentAccountAlert:alert];
        });
        return;
    }
    [presenter presentViewController:alert animated:YES completion:nil];
}
- (void)form:(BOOL)registering message:(NSString *)message {
    if (_showing || _busy) return;
    _showing = YES;
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:registering ? @"注册游戏账号" : @"登录游戏账号"
        message:message ?: @"账号为 3–32 位字母或数字，不区分大小写；密码为 6–64 个字符。" preferredStyle:UIAlertControllerStyleAlert];
    [alert addTextFieldWithConfigurationHandler:^(UITextField *field) {
        field.placeholder = @"账号"; field.text = _lastName;
        field.autocapitalizationType = UITextAutocapitalizationTypeNone;
        field.autocorrectionType = UITextAutocorrectionTypeNo; field.textContentType = UITextContentTypeUsername;
    }];
    [alert addTextFieldWithConfigurationHandler:^(UITextField *field) {
        field.placeholder = @"密码"; field.secureTextEntry = YES;
        field.textContentType = registering ? UITextContentTypeNewPassword : UITextContentTypePassword;
    }];
    if (registering) [alert addTextFieldWithConfigurationHandler:^(UITextField *field) {
        field.placeholder = @"确认密码"; field.secureTextEntry = YES; field.textContentType = UITextContentTypeNewPassword;
    }];
    NSArray<UITextField *> *fields = alert.textFields;
    [alert addAction:[UIAlertAction actionWithTitle:registering ? @"注册并登录" : @"登录" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
        _showing = NO;
        NSString *name = fields[0].text ?: @"", *password = fields[1].text ?: @"";
        [_lastName release]; _lastName = [name copy];
        if (registering && ![password isEqualToString:fields[2].text]) {
            dispatch_async(dispatch_get_main_queue(), ^{ [self form:YES message:@"两次输入的密码不一致"]; }); return;
        }
        NSMutableDictionary *body = [NSMutableDictionary dictionaryWithDictionary:@{@"username":name, @"password":password}];
        if (registering) body[@"confirmPassword"] = fields[2].text ?: @"";
        _busy = YES; libOS::getInstance()->setWaiting(true);
        NSUInteger expected = _generation;
        [self request:registering ? @"register" : @"login" body:body completion:^(NSDictionary *value, NSInteger code, NSString *failure) {
            if (expected != _generation) return;
            _busy = NO; libOS::getInstance()->setWaiting(false);
            if (failure) { [self form:registering message:failure]; return; }
            [self accept:value];
        }];
    }]];
    [alert addAction:[UIAlertAction actionWithTitle:registering ? @"已有账号" : @"注册账号" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
        _showing = NO;
        dispatch_async(dispatch_get_main_queue(), ^{ [self form:!registering message:nil]; });
    }]];
    [alert addAction:[UIAlertAction actionWithTitle:@"取消" style:UIAlertActionStyleCancel handler:^(UIAlertAction *action) {
        _showing = NO;
    }]];
    [self presentAccountAlert:alert];
}
- (void)accept:(NSDictionary *)value {
    NSString *uid = value[@"uid"], *token = value[@"token"], *name = value[@"username"];
    NSRegularExpression *uidPattern = [NSRegularExpression regularExpressionWithPattern:@"^acct_[a-f0-9]{32}$" options:0 error:nil];
    NSRegularExpression *tokenPattern = [NSRegularExpression regularExpressionWithPattern:@"^nrb1_[a-f0-9]{64}$" options:0 error:nil];
    if (![uid isKindOfClass:NSString.class] || ![token isKindOfClass:NSString.class] || ![name isKindOfClass:NSString.class]
        || ![uidPattern numberOfMatchesInString:uid options:0 range:NSMakeRange(0, uid.length)]
        || ![tokenPattern numberOfMatchesInString:token options:0 range:NSMakeRange(0, token.length)]) {
        [self form:NO message:@"账号接口版本不兼容，请更新服务端"]; return;
    }
    SaveAccount(_base, value);
    [_lastName release]; _lastName = [name copy];
    NSLog(@"[IOSAccount] accept: server account authenticated");
    _owner->acceptAccount(uid.UTF8String, name.UTF8String, token.UTF8String);
}
- (void)start:(LocalPlatform *)owner switching:(BOOL)switching {
    if (_showing || (_busy && !switching)) return;
    if (switching) { _busy = NO; libOS::getInstance()->setWaiting(false); }
    _owner = owner;
    if (switching) [self forget];
    NSString *base = @(OPRemoteEnabled() ? OPRemoteBaseURL().c_str() : SeverConsts::Get()->getAccountBaseURL().c_str());
    NSURL *url = [NSURL URLWithString:base];
    if (!url.host.length || ![@[@"http", @"https"] containsObject:url.scheme]) {
        libOS::getInstance()->showMessagebox("请等待服务器配置加载完成后重试"); return;
    }
    [_base release]; _base = [base copy];
    NSDictionary *value = SavedAccount(_base);
    if (value[@"username"]) { [_lastName release]; _lastName = [value[@"username"] copy]; }
    NSString *token = value[@"token"];
    if (switching || !token.length) { [self form:NO message:nil]; return; }
    _busy = YES; NSUInteger expected = _generation; libOS::getInstance()->setWaiting(true);
    [self request:@"resume" body:@{@"token":token} completion:^(NSDictionary *result, NSInteger code, NSString *failure) {
        if (expected != _generation) return;
        _busy = NO; libOS::getInstance()->setWaiting(false);
        if (failure) { if (code == 401) SaveAccount(_base, nil); [self form:NO message:failure]; }
        else [self accept:result];
    }];
}
- (void)forget {
    ++_generation;
    if (!_base) return;
    NSString *token = SavedAccount(_base)[@"token"];
    SaveAccount(_base, nil);
    if (token.length) [self request:@"logout" body:@{@"token":token} completion:^(NSDictionary *, NSInteger, NSString *) {}];
}
@end

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
    dispatch_async(dispatch_get_main_queue(), ^{ [[OPAccountLogin shared] start:this switching:NO]; });
}

void LocalPlatform::acceptAccount(const std::string& identity, const std::string& name, const std::string& session) {
    uid = identity; username = name; token = session; loggedIn = true;
    _boardcastLoginResult(true, "Server account authenticated");
}

void LocalPlatform::logout() {
    clearAccount();
    dispatch_async(dispatch_get_main_queue(), ^{ [[OPAccountLogin shared] forget]; });
    _boardcastPlatformLogout();
}

void LocalPlatform::switchUsers() {
    clearAccount();
    dispatch_async(dispatch_get_main_queue(), ^{ [[OPAccountLogin shared] start:this switching:YES]; });
}

void LocalPlatform::buyGoods(BUYINFO& info) {
    NSLog(@"[IOSLocal] Purchase unavailable in the development channel");
    _boardcastBuyinfoSent(false, info, "Purchases are not configured");
    libOS::getInstance()->showMessagebox("当前为本地开发客户端，未接入充值服务。");
}
