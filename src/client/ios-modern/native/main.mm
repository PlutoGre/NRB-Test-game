#import <UIKit/UIKit.h>
#include "cocos2d.h"
#import "EAGLView.h"
#include "AppDelegate.h"
#include <signal.h>
#include "ConnectionSettings.h"

// Keep the legacy GL renderer inside the safe area on notched iPhones.
@interface OPGameController : UIViewController
@property(nonatomic, retain) EAGLView *gameView;
@end
@implementation OPGameController
- (BOOL)prefersStatusBarHidden { return YES; }
- (BOOL)prefersHomeIndicatorAutoHidden { return YES; }
- (BOOL)shouldAutorotate { return NO; }
- (UIInterfaceOrientationMask)supportedInterfaceOrientations { return UIInterfaceOrientationMaskPortrait; }
- (void)viewDidLayoutSubviews {
    [super viewDidLayoutSubviews];
    self.gameView.frame = UIEdgeInsetsInsetRect(self.view.bounds, self.view.safeAreaInsets);
}
- (void)dealloc { [_gameView release]; [super dealloc]; }
@end

@interface OPApplication : UIResponder <UIApplicationDelegate>
@property(nonatomic, retain) UIWindow *window;
@property(nonatomic, assign) BOOL gameStarted;
- (void)startGame;
@end
@implementation OPApplication
static AppDelegate gameApplication;
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)options {
    // A disconnected TCP peer must report an error instead of terminating iOS via SIGPIPE.
    signal(SIGPIPE, SIG_IGN);
    self.window = [[[UIWindow alloc] initWithFrame:UIScreen.mainScreen.bounds] autorelease];
    self.window.rootViewController = OPConnectionController(^{ [self startGame]; });
    [self.window makeKeyAndVisible];
    return YES;
}
- (void)startGame {
    if (self.gameStarted) return;
    OPGameController *controller = [[[OPGameController alloc] init] autorelease];
    controller.view.backgroundColor = UIColor.blackColor;
    self.window.rootViewController = controller;
    [self.window makeKeyAndVisible];
    [controller.view layoutIfNeeded];
    CGRect frame = UIEdgeInsetsInsetRect(controller.view.bounds, controller.view.safeAreaInsets);
    controller.gameView = [EAGLView viewWithFrame:frame pixelFormat:kEAGLColorFormatRGBA8
        depthFormat:GL_DEPTH_COMPONENT16 preserveBackbuffer:NO sharegroup:nil
        multiSampling:NO numberOfSamples:0];
    if (!controller.gameView) { NSLog(@"[IOSLocal] OpenGL ES view creation failed"); return; }
    [controller.view addSubview:controller.gameView];
    [controller.gameView layoutIfNeeded];
    UIApplication.sharedApplication.idleTimerDisabled = YES;
    self.gameStarted = YES;
    NSLog(@"[IOSLocal] Starting ARM64 development client");
    cocos2d::CCApplication::sharedApplication()->run();
}
- (void)applicationWillResignActive:(UIApplication *)application {
    if (!self.gameStarted) return;
    cocos2d::CCDirector::sharedDirector()->pause();
}
- (void)applicationDidBecomeActive:(UIApplication *)application {
    if (!self.gameStarted) return;
    cocos2d::CCDirector::sharedDirector()->resume();
}
- (void)applicationDidEnterBackground:(UIApplication *)application {
    if (!self.gameStarted) return;
    cocos2d::CCApplication::sharedApplication()->applicationDidEnterBackground();
}
- (void)applicationWillEnterForeground:(UIApplication *)application {
    if (!self.gameStarted) return;
    cocos2d::CCApplication::sharedApplication()->applicationWillEnterForeground();
}
- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    if (!self.gameStarted) return;
    cocos2d::CCApplication::sharedApplication()->purgeCachedData();
    cocos2d::CCDirector::sharedDirector()->purgeCachedData();
}
- (void)dealloc { [_window release]; [super dealloc]; }
@end

int main(int argc, char **argv) {
    @autoreleasepool { return UIApplicationMain(argc, argv, nil, NSStringFromClass(OPApplication.class)); }
}
