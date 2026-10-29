#import "ConnectionSettings.h"

static bool remoteEnabled = false;
static std::string remoteBase, remoteInvite;
static NSString *const resourcePrefix = @"/gameserver/dzm/1.8/";

bool OPRemoteEnabled() { return remoteEnabled; }
std::string OPRemoteBaseURL() { return remoteBase; }
std::string OPRemoteInvite() { return remoteInvite; }

std::string OPResourceURL(const std::string& original) {
    if (!remoteEnabled) return original;
    NSURLComponents *parts = [NSURLComponents componentsWithString:@(original.c_str())];
    // Cached configuration may still contain a previous LAN address or tunnel.
    // Only the original resource subtree may move to the selected origin.
    if (![parts.percentEncodedPath hasPrefix:resourcePrefix]) return "invalid:";
    NSString *result = [@(remoteBase.c_str()) stringByAppendingString:parts.percentEncodedPath];
    if (parts.percentEncodedQuery.length)
        result = [result stringByAppendingFormat:@"?%@", parts.percentEncodedQuery];
    return result.UTF8String;
}

void OPAuthorizeResourceRequest(NSMutableURLRequest *request) {
    if (remoteEnabled && [request.URL.absoluteString hasPrefix:
            [@(remoteBase.c_str()) stringByAppendingString:resourcePrefix]])
        [request setValue:[@"Bearer " stringByAppendingString:@(remoteInvite.c_str())]
            forHTTPHeaderField:@"Authorization"];
}

// This page runs before Cocos starts, so changing the origin cannot mix sessions.
@interface OPConnectionPage : UIViewController <NSURLSessionTaskDelegate> {
    UITextField *_origin;
    UITextField *_invite;
    UILabel *_message;
    UIButton *_connect;
    UIButton *_lan;
    NSURLSession *_session;
    NSURLSessionDataTask *_check;
    NSUInteger _generation;
    void (^_completion)(void);
}
- (id)initWithCompletion:(void (^)(void))completion;
@end

@implementation OPConnectionPage
- (id)initWithCompletion:(void (^)(void))completion {
    if ((self = [super init])) _completion = [completion copy];
    return self;
}
- (UIInterfaceOrientationMask)supportedInterfaceOrientations { return UIInterfaceOrientationMaskPortrait; }
- (BOOL)shouldAutorotate { return NO; }
- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = UIColor.systemBackgroundColor;
    UIScrollView *scroll = [[[UIScrollView alloc] init] autorelease];
    scroll.translatesAutoresizingMaskIntoConstraints = NO;
    scroll.keyboardDismissMode = UIScrollViewKeyboardDismissModeOnDrag;
    [self.view addSubview:scroll];
    UIStackView *stack = [[[UIStackView alloc] init] autorelease];
    stack.axis = UILayoutConstraintAxisVertical;
    stack.spacing = 16;
    stack.translatesAutoresizingMaskIntoConstraints = NO;
    [scroll addSubview:stack];
    UILayoutGuide *safe = self.view.safeAreaLayoutGuide;
    [NSLayoutConstraint activateConstraints:@[
        [scroll.leadingAnchor constraintEqualToAnchor:safe.leadingAnchor],
        [scroll.trailingAnchor constraintEqualToAnchor:safe.trailingAnchor],
        [scroll.topAnchor constraintEqualToAnchor:safe.topAnchor],
        [scroll.bottomAnchor constraintEqualToAnchor:self.view.keyboardLayoutGuide.topAnchor],
        [stack.leadingAnchor constraintEqualToAnchor:scroll.contentLayoutGuide.leadingAnchor constant:24],
        [stack.trailingAnchor constraintEqualToAnchor:scroll.contentLayoutGuide.trailingAnchor constant:-24],
        [stack.topAnchor constraintEqualToAnchor:scroll.contentLayoutGuide.topAnchor constant:24],
        [stack.bottomAnchor constraintEqualToAnchor:scroll.contentLayoutGuide.bottomAnchor constant:-24],
        [stack.widthAnchor constraintEqualToAnchor:scroll.frameLayoutGuide.widthAnchor constant:-48]
    ]];
    UILabel *title = [[[UILabel alloc] init] autorelease];
    title.text = @"选择游戏服务器";
    title.font = [UIFont preferredFontForTextStyle:UIFontTextStyleTitle2];
    [stack addArrangedSubview:title];
    UILabel *help = [[[UILabel alloc] init] autorelease];
    help.numberOfLines = 0;
    help.text = @"外网测试：填写电脑启动隧道后显示的 HTTPS 地址和邀请码。地址变化后，关闭游戏并重新填写。";
    [stack addArrangedSubview:help];
    _origin = [[UITextField alloc] init];
    _origin.borderStyle = UITextBorderStyleRoundedRect;
    _origin.placeholder = @"https://xxx.trycloudflare.com";
    _origin.keyboardType = UIKeyboardTypeURL;
    _origin.autocapitalizationType = UITextAutocapitalizationTypeNone;
    _origin.autocorrectionType = UITextAutocorrectionTypeNo;
    _origin.clearButtonMode = UITextFieldViewModeWhileEditing;
    _origin.text = [NSUserDefaults.standardUserDefaults stringForKey:@"nrb.remote.origin"];
    [_origin addTarget:self action:@selector(originEdited) forControlEvents:UIControlEventEditingChanged];
    [stack addArrangedSubview:_origin];
    _invite = [[UITextField alloc] init];
    _invite.borderStyle = UITextBorderStyleRoundedRect;
    _invite.placeholder = @"本次服务器的邀请码";
    _invite.secureTextEntry = YES;
    _invite.autocapitalizationType = UITextAutocapitalizationTypeNone;
    _invite.autocorrectionType = UITextAutocorrectionTypeNo;
    _invite.text = [NSUserDefaults.standardUserDefaults stringForKey:@"nrb.remote.invite"];
    [_invite addTarget:self action:@selector(cancelCheck) forControlEvents:UIControlEventEditingChanged];
    [stack addArrangedSubview:_invite];
    _connect = [[UIButton buttonWithType:UIButtonTypeSystem] retain];
    [_connect setTitle:@"检测并进入外网游戏" forState:UIControlStateNormal];
    [_connect addTarget:self action:@selector(connectRemote) forControlEvents:UIControlEventTouchUpInside];
    [stack addArrangedSubview:_connect];
    _lan = [[UIButton buttonWithType:UIButtonTypeSystem] retain];
    [_lan setTitle:@"使用打包时的局域网配置" forState:UIControlStateNormal];
    [_lan addTarget:self action:@selector(connectLAN) forControlEvents:UIControlEventTouchUpInside];
    [stack addArrangedSubview:_lan];
    _message = [[UILabel alloc] init];
    _message.numberOfLines = 0;
    _message.text = @"外网模式使用加密连接；局域网模式需要手机能访问打包时配置的电脑。";
    [stack addArrangedSubview:_message];
}
- (void)cancelCheck {
    ++_generation;
    [_check cancel]; [_check release]; _check = nil;
    [_session invalidateAndCancel]; [_session release]; _session = nil;
    _connect.enabled = YES;
    _lan.enabled = YES;
}
- (void)originEdited {
    [self cancelCheck];
    // Never send a remembered invitation to a newly entered host by accident.
    _invite.text = @"";
    [NSUserDefaults.standardUserDefaults removeObjectForKey:@"nrb.remote.invite"];
    _message.text = @"地址已修改，请重新输入对应服务器的邀请码。";
}
- (void)finishSelection {
    [self retain]; // Replacing the window root may release this page immediately.
    [self cancelCheck];
    void (^completion)(void) = [_completion copy];
    [_completion release]; _completion = nil;
    if (completion) completion();
    [completion release];
    [self release];
}
- (void)connectLAN {
    remoteEnabled = false;
    remoteBase.clear(); remoteInvite.clear();
    [self finishSelection];
}
- (void)connectRemote {
    [self cancelCheck];
    NSCharacterSet *whitespace = NSCharacterSet.whitespaceAndNewlineCharacterSet;
    NSString *text = [_origin.text stringByTrimmingCharactersInSet:whitespace];
    NSURLComponents *parts = [NSURLComponents componentsWithString:text];
    if (![parts.scheme.lowercaseString isEqualToString:@"https"] || !parts.host.length
            || parts.user != nil || parts.password != nil || parts.query != nil || parts.fragment != nil
            || (parts.path.length && ![parts.path isEqualToString:@"/"]) || !parts.URL) {
        _message.text = @"请输入完整的 HTTPS 域名，不要附带路径、查询参数或账号密码。";
        return;
    }
    parts.scheme = @"https";
    parts.host = parts.host.lowercaseString;
    parts.path = @"";
    NSString *base = parts.URL.absoluteString;
    NSString *invite = [_invite.text stringByTrimmingCharactersInSet:whitespace];
    NSRegularExpression *validInvite = [NSRegularExpression regularExpressionWithPattern:@"^[A-Za-z0-9_-]{32,128}$"
        options:0 error:nil];
    if (![validInvite numberOfMatchesInString:invite options:0 range:NSMakeRange(0, invite.length)]) {
        _message.text = @"请输入电脑显示的完整邀请码。";
        return;
    }
    [self.view endEditing:YES];
    _connect.enabled = NO; _lan.enabled = NO;
    _message.text = @"正在检测隧道、邀请码和游戏服务端……";
    NSUInteger generation = _generation;
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:
        [NSURL URLWithString:[base stringByAppendingString:@"/check"]]
        cachePolicy:NSURLRequestReloadIgnoringLocalCacheData timeoutInterval:15];
    [request setValue:[@"Bearer " stringByAppendingString:invite] forHTTPHeaderField:@"Authorization"];
    _session = [[NSURLSession sessionWithConfiguration:NSURLSessionConfiguration.ephemeralSessionConfiguration
        delegate:self delegateQueue:nil] retain];
    _check = [[_session dataTaskWithRequest:request completionHandler:
        ^(NSData *data, NSURLResponse *response, NSError *error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            if (generation != _generation) return;
            NSInteger status = [response isKindOfClass:NSHTTPURLResponse.class]
                ? [(NSHTTPURLResponse *)response statusCode] : 0;
            id value = data.length && data.length < 65536
                ? [NSJSONSerialization JSONObjectWithData:data options:0 error:nil] : nil;
            bool ok = !error && status == 200 && [value isKindOfClass:NSDictionary.class]
                && [value[@"gateway"] isEqual:@"nrb"] && [value[@"protocol"] isEqual:@"nrb-tcp-v1"]
                && [value[@"backend"] isEqual:@YES];
            [self cancelCheck];
            if (!ok) {
                _message.text = status == 401 ? @"邀请码不正确，请向开服电脑确认。"
                    : status == 503 ? @"隧道或游戏服务端尚未就绪，请检查电脑上的服务状态。"
                    : @"连接检测失败，请检查域名、手机网络以及电脑上的隧道。";
                return;
            }
            if (UIApplication.sharedApplication.applicationState != UIApplicationStateActive) {
                _message.text = @"请回到前台后再次点击连接。";
                return;
            }
            remoteBase = base.UTF8String; remoteInvite = invite.UTF8String; remoteEnabled = true;
            [NSUserDefaults.standardUserDefaults setObject:base forKey:@"nrb.remote.origin"];
            [NSUserDefaults.standardUserDefaults setObject:invite forKey:@"nrb.remote.invite"];
            NSLog(@"[IOSRemote] Gateway check succeeded; starting game");
            [self finishSelection];
        });
    }] retain];
    [_check resume];
}
- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task
    willPerformHTTPRedirection:(NSHTTPURLResponse *)response newRequest:(NSURLRequest *)request
    completionHandler:(void (^)(NSURLRequest *))completionHandler {
    completionHandler(nil);
}
- (void)dealloc {
    [self cancelCheck];
    [_origin release]; [_invite release]; [_message release]; [_connect release]; [_lan release];
    [_completion release]; [super dealloc];
}
@end

UIViewController *OPConnectionController(void (^completion)(void)) {
    return [[[OPConnectionPage alloc] initWithCompletion:completion] autorelease];
}
