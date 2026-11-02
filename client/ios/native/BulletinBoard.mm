#import <UIKit/UIKit.h>
#import <WebKit/WebKit.h>
#include <string>
#include "BulletinBoardPage.h"

// The close button stays visible even if the announcement page never loads.
@interface OPBulletin : UIView <WKNavigationDelegate>
@property(nonatomic, retain) WKWebView *browser;
@property(nonatomic, retain) NSTimer *timer;
@property(nonatomic, assign) BulletinBoardPageListener *listener;
- (void)closeTapped;
- (void)timedOut;
@end
static OPBulletin *bulletin = nil;
static int timeoutSeconds = 15;
BulletinBoardPage *BulletinBoardPage::mInstance = 0;

@implementation OPBulletin
- (void)layoutSubviews {
    [super layoutSubviews];
    UIEdgeInsets insets = self.safeAreaInsets;
    self.browser.frame = CGRectMake(insets.left, insets.top + 48,
        self.bounds.size.width - insets.left - insets.right,
        MAX(0, self.bounds.size.height - insets.top - insets.bottom - 48));
    [self viewWithTag:100].frame = CGRectMake(self.bounds.size.width - insets.right - 88, insets.top, 80, 44);
}
- (void)closeTapped {
    BulletinBoardPageListener *callback = self.listener;
    BulletinBoardPage::getInstance()->close();
    if (callback) callback->onBtnAction();
}
- (void)timedOut {
    BulletinBoardPageListener *callback = self.listener;
    NSLog(@"[IOSLocal] Announcement timed out; closing overlay");
    BulletinBoardPage::getInstance()->close();
    if (callback) callback->onLoadingTimeOut();
}
- (void)webView:(WKWebView *)webView didFinishNavigation:(WKNavigation *)navigation {
    [self.timer invalidate]; self.timer = nil;
    if (self.listener) self.listener->onFinishLoad();
}
- (void)webView:(WKWebView *)webView didFailProvisionalNavigation:(WKNavigation *)navigation withError:(NSError *)error {
    BulletinBoardPageListener *callback = self.listener;
    std::string message = [error.localizedDescription UTF8String];
    NSLog(@"[IOSLocal] Announcement failed: %@", error.localizedDescription);
    BulletinBoardPage::getInstance()->close();
    if (callback) callback->onFailLoadWithError(message);
}
- (void)webView:(WKWebView *)webView didFailNavigation:(WKNavigation *)navigation withError:(NSError *)error {
    [self webView:webView didFailProvisionalNavigation:navigation withError:error];
}
- (void)webViewWebContentProcessDidTerminate:(WKWebView *)webView { [self timedOut]; }
- (void)dealloc {
    [_timer invalidate]; [_timer release];
    _browser.navigationDelegate = nil; [_browser stopLoading]; [_browser release];
    [super dealloc];
}
@end

void BulletinBoardPage::init(float, float, float, float, BulletinBoardPageListener *listener) {
    close();
    UIView *parent = UIApplication.sharedApplication.keyWindow.rootViewController.view;
    bulletin = [[OPBulletin alloc] initWithFrame:parent.bounds];
    bulletin.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    bulletin.backgroundColor = UIColor.whiteColor;
    bulletin.listener = listener;
    bulletin.browser = [[[WKWebView alloc] initWithFrame:CGRectZero] autorelease];
    bulletin.browser.navigationDelegate = bulletin;
    [bulletin addSubview:bulletin.browser];
    UIButton *button = [UIButton buttonWithType:UIButtonTypeSystem];
    button.tag = 100;
    [button setTitle:@"关闭" forState:UIControlStateNormal];
    [button addTarget:bulletin action:@selector(closeTapped) forControlEvents:UIControlEventTouchUpInside];
    [bulletin addSubview:button];
}
void BulletinBoardPage::show(const std::string& value) {
    if (!bulletin) return;
    NSURL *url = [NSURL URLWithString:@(value.c_str())];
    if (!url || !([url.scheme isEqualToString:@"http"] || [url.scheme isEqualToString:@"https"])) { close(); return; }
    UIView *parent = UIApplication.sharedApplication.keyWindow.rootViewController.view;
    [parent addSubview:bulletin];
    [bulletin setNeedsLayout];
    [bulletin.timer invalidate];
    bulletin.timer = [NSTimer scheduledTimerWithTimeInterval:timeoutSeconds target:bulletin selector:@selector(timedOut)
        userInfo:nil repeats:NO];
    [bulletin.browser loadRequest:[NSURLRequest requestWithURL:url cachePolicy:NSURLRequestReloadIgnoringLocalCacheData
        timeoutInterval:timeoutSeconds]];
    if (bulletin.listener) bulletin.listener->onStartLoad();
}
void BulletinBoardPage::close() {
    if (!bulletin) return;
    OPBulletin *old = bulletin;
    bulletin = nil;
    old.listener = NULL;
    [old.timer invalidate]; old.timer = nil;
    old.browser.navigationDelegate = nil;
    [old.browser stopLoading]; [old removeFromSuperview];
    // Delegate callbacks may still be unwinding on this run loop.
    [old autorelease];
}
void BulletinBoardPage::webKitSetLoadingTimeOut(int seconds) { timeoutSeconds = MAX(1, seconds); }
