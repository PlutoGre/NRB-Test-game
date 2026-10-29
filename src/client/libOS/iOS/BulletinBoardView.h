
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "MBProgressHUD.h"

class BulletinBoardPageListener;

@interface BulletinBoardView : UIView <UIWebViewDelegate,MBProgressHUDDelegate>
{
    UIWebView* m_webView;
    UINavigationBar* navBar;
    UINavigationItem* canselItem;
    UIBarButtonItem* canselBtn;
    UIImageView* imageView;
    UIButton* button;
    //NSMutableString *targetURL;
    //NSMutableString *preChangeURL;
    BulletinBoardPageListener *webKitDelegate;
    bool shouldLoadRequest;
    NSMutableString* webTile;
    MBProgressHUD* HUD;
    NSTimer* timer;
    NSInteger loadingTimeOut;
    //ALSystem::WK_Orientation mOrientation;
    //bool m_bIsNavBarNeedHide;
    //bool m_bIsNavBarHidden;
}
@property(nonatomic,retain) UIWebView* m_webView;
@property(nonatomic,retain) UINavigationBar* navBar;
@property(nonatomic,retain) UINavigationItem* canselItem;
@property(nonatomic,retain) UIBarButtonItem* canselBtn;
@property(nonatomic,retain) UIImageView* imageView;
@property(nonatomic,retain) UIButton* button;
@property(readwrite) BulletinBoardPageListener *webKitDelegate;
//@property(nonatomic,retain) NSMutableString *targetURL;
//@property(nonatomic,retain) NSMutableString *preChangeURL;
@property(readwrite) bool shouldLoadRequest;
@property(nonatomic,retain) NSMutableString* webTile;
@property(nonatomic,retain) MBProgressHUD* HUD;
@property(nonatomic, retain) NSTimer* timer;
//@property(readwrite) ALSystem::WK_Orientation mOrientation;
@property(nonatomic, assign) NSInteger loadingTimeOut;



- (id)initWithFrame:(CGRect)frame isNavBarHiden:(BOOL)isNavBarHiden;
- (void)webViewOpenURL:(NSString *)URL;
//- (void)webViewEvaluatingJavaScriptFromString:(NSString *)jsString;
- (void)closeTimer;
//- (void)webKitHideNavigationBar;
//- (void)webKitShowNavigationBar;
@end
