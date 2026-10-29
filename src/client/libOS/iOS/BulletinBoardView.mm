
#import <string>
#import "BulletinBoardView.h"
#import "BulletinBoardPage.h"

#define kUserLoginInfoFileName @"userLoginInfoFile"
#define kLoginStatusSucceed YES
#define kLoginStatusFailed  NO

@implementation BulletinBoardView

@synthesize m_webView;
@synthesize navBar;
@synthesize canselItem;
@synthesize canselBtn;
@synthesize imageView;
@synthesize button;
//@synthesize targetURL;
//@synthesize preChangeURL;
@synthesize webKitDelegate;
@synthesize shouldLoadRequest;
@synthesize webTile;
@synthesize HUD;
@synthesize timer;
//@synthesize mOrientation;
@synthesize loadingTimeOut;
/*
- (void)addObservers {
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(deviceOrientationDidChange:)
												 name:@"UIDeviceOrientationDidChangeNotification" object:nil];
}

- (void)removeObservers {
	[[NSNotificationCenter defaultCenter] removeObserver:self
													name:@"UIDeviceOrientationDidChangeNotification" object:nil];
}
*/
- (id)initWithFrame:(CGRect)frame isNavBarHiden:(BOOL)isNavBarHiden
{
    CGRect screen = [UIApplication sharedApplication].keyWindow.rootViewController.view.frame;
    //self = [super initWithFrame:CGRectMake(0, 0, screen.size.height, screen.size.width)];
    self = [super initWithFrame:CGRectMake(0, 0, 640, 960)];
    self.transform = CGAffineTransformMakeScale(screen.size.width / 640, screen.size.height / 960);
    self.center = CGPointMake(screen.size.width/2,screen.size.height/2);
    
    if (self) {
        self.backgroundColor = [UIColor clearColor];
        NSLog(@"Creat a webKit!");
        shouldLoadRequest = YES;
        webKitDelegate = NULL;
        timer = nil;
        //NSString *path = [[NSBundle mainBundle] pathForResource:@"test" ofType:@"png"];
        //UIImage *image = [UIImage imageWithContentsOfFile:path];
        UIImage *image = [UIImage imageNamed:@"loadingScene/u_announcement.png"];
        imageView = [[UIImageView alloc] initWithImage:image];
        //[imageView setFrame:CGRectMake(0, 0, 480, 720)];
        [imageView setFrame:CGRectMake(0, 0, 609, 695)];
        //imageView.center = CGPointMake(self.frame.size.width/2,self.frame.size.height/2);
        imageView.center = CGPointMake(320,460);
        
        [self addSubview:imageView];

        CGRect webviewFrame = CGRectMake(0, 0, 500, 450);
        m_webView = [[UIWebView alloc] initWithFrame:webviewFrame];
        m_webView.center = CGPointMake(320,410);
        m_webView.opaque = YES;
        m_webView.backgroundColor = [UIColor whiteColor];
        m_webView.delegate = self;
        m_webView.scalesPageToFit = YES;
        [m_webView setAutoresizingMask:UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight];
        //[m_webView setAutoresizingMask:UIViewAutoresizingFlexibleHeight];
        [m_webView setAllowsInlineMediaPlayback:YES];
        [self addSubview:m_webView];
        
        button = [UIButton buttonWithType:UIButtonTypeCustom];
        button.frame = CGRectMake(0, 0, 205, 103);
        button.center = CGPointMake(320,712);
        //[button setTitle:@"login" forState:UIControlStateNormal];
        UIImage *image1 = [UIImage imageNamed:@"loadingScene/u_announcementB.png"];
        [button setBackgroundImage:image1 forState:UIControlStateNormal];
        UIImage *image2 = [UIImage imageNamed:@"loadingScene/u_announcementC.png"];
        [button setBackgroundImage:image2 forState:UIControlStateSelected];
        [button addTarget:self action:@selector(canselBtnAction:) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:button];
        
        HUD = [[MBProgressHUD alloc] initWithView:m_webView];
        HUD.labelText = @"Loading...";
        [m_webView addSubview:HUD];
        [HUD hide:NO];

        loadingTimeOut = 50;

        NSString *bundlePath = [[NSBundle mainBundle] pathForResource:@"ALSystemConfig" ofType:@"plist"];
        NSDictionary* dict = [NSDictionary dictionaryWithContentsOfFile:bundlePath];
        if(dict) {
            NSString* value = [dict valueForKey:@"loadingTimeOut"];
            if (value) {
                loadingTimeOut = [value integerValue];
            }
            else {
                NSLog(@"Cannot find value for loadingTimeOut, loadingTimeOut will be set to 50");
            }
        }
        else {
            NSLog(@"Cannot Find  ALSystemConfig.plist, loadingTimeOut will be set to 50");
        }
        NSLog(@"loadingTimeOut = %d",loadingTimeOut);
        
        //[self addObservers]; 

    }
    return self;
}

- (void)drawRect:(CGRect)rect
{
//    if (mOrientation == ALSystem::WK_OrientationDefualt) {
//        return;
//    }

    //[self rotateViewToOrientation:mOrientation];
}

- (void) canselBtnAction:(id)sender
{
    if (timer) {
        if ([timer isValid] == TRUE) {
            [timer invalidate];
        }
        timer = nil;
    }    
    if (webKitDelegate != NULL) {
        webKitDelegate->onBtnAction();
    }
}

- (void)webViewOpenURL:(NSString *)URL
{
    //[targetURL setString:URL];
    //if (webKitDelegate != NULL) {
    [m_webView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:URL]]];
    //}
}
- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType
{      
    NSString *requestString = [[request URL] absoluteString];
    NSLog(@"WebKit will open URL: %@",requestString);    
    //[preChangeURL setString:targetURL];
    //[targetURL setString:requestString];
    //std::string stdPreChangeURL([preChangeURL UTF8String]);
    //std::string stdTargetURL([targetURL UTF8String]);
    //if (webKitDelegate != NULL) {
    //    webKitDelegate->onWebKitWillChangeURL(stdPreChangeURL, stdTargetURL);
    //}
    return shouldLoadRequest;
}
- (void)timeOut
{
    [m_webView stopLoading];

    [HUD hide:NO];
    [self closeTimer];
    
    const std::string stdErrorStr("WebKit loading time out!");
    NSLog(@"WebKit loading time out! %d seconds", loadingTimeOut);
    if (webKitDelegate != NULL) {
        webKitDelegate->onLoadingTimeOut();
    }
}

- (void)webViewDidStartLoad:(UIWebView *)webView
{
    [HUD show:NO];
    [self closeTimer];
    timer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)loadingTimeOut target:self selector:@selector(timeOut) userInfo:nil repeats:NO];
    //if (m_bIsNavBarNeedHide) {
    //    if (m_bIsNavBarHidden) {
    //        [self webKitShowNavigationBar];
    //    }
    //}
    if (webKitDelegate != NULL) {
        webKitDelegate->onStartLoad();
    }
}

- (void)webViewDidFinishLoad:(UIWebView *)webView
{    
    [HUD hide:NO];
    [self closeTimer];
    //if (m_bIsNavBarNeedHide) {
    //    if (!m_bIsNavBarHidden) {
    //        [self webKitHideNavigationBar];
    //    }
    //}

    if (webKitDelegate != NULL) {
        webKitDelegate->onFinishLoad();
    }
}

- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
    [HUD hide:NO];
    [self closeTimer];

    if (webKitDelegate != NULL) {        
        NSString* errorStr = [NSString stringWithString:[error description]];
        const std::string stdErrorStr([errorStr UTF8String]);
        webKitDelegate->onFailLoadWithError(stdErrorStr);
    }
}

- (void)webViewEvaluatingJavaScriptFromString:(NSString *)jsString
{
    if (m_webView != NULL) {
        [m_webView stringByEvaluatingJavaScriptFromString:jsString];  
    }
}

- (void)closeTimer
{
    if (timer) {
        if ([timer isValid] == TRUE) {
            [timer invalidate];
        }
        timer = nil;
    }
}

-(void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	UITouch* touch = [touches anyObject];
	if([touch tapCount]>0)
	{
	}
}
-(void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	UITouch* touch = [touches anyObject];
	CGPoint  lastpoint = [touch locationInView:self];
}


- (void) dealloc
{
    NSLog(@"Close a webKit!");
    webKitDelegate = NULL;
    [HUD release]; HUD = nil;
    //[targetURL release]; targetURL = nil;
    //[preChangeURL release]; preChangeURL = nil;
    [navBar release]; navBar = nil;
    [canselItem release]; canselItem = nil;
    [canselBtn release]; canselBtn = nil;
    [m_webView release]; m_webView = nil;
    [imageView release]; imageView = nil;
    [super dealloc];
}

@end