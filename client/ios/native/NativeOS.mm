#import <UIKit/UIKit.h>
#import <UserNotifications/UserNotifications.h>
#import <SystemConfiguration/SystemConfiguration.h>
#include <netinet/in.h>
#include <os/proc.h>
#include "libOS.h"

libOS *libOS::m_sInstance = 0;

static UIViewController *Presenter() {
    UIViewController *controller = UIApplication.sharedApplication.keyWindow.rootViewController;
    while (controller.presentedViewController) controller = controller.presentedViewController;
    return controller;
}

long libOS::avalibleMemory() {
    return (long)(os_proc_available_memory() / (1024 * 1024));
}
void libOS::rmdir(const char *path) {
    NSError *error = nil;
    if (![NSFileManager.defaultManager removeItemAtPath:@(path) error:&error])
        NSLog(@"[IOSLocal] Removing cached directory failed: %@", error.localizedDescription);
}
const std::string& libOS::generateSerial() {
    static std::string serial;
    serial = [[NSUUID.UUID.UUIDString stringByReplacingOccurrencesOfString:@"-" withString:@""] UTF8String];
    return serial;
}
void libOS::showInputbox(bool multiline) {
    dispatch_async(dispatch_get_main_queue(), ^{
        UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"请输入内容" message:nil
            preferredStyle:UIAlertControllerStyleAlert];
        [alert addTextFieldWithConfigurationHandler:^(UITextField *field) {
            field.autocorrectionType = UITextAutocorrectionTypeNo;
        }];
        [alert addAction:[UIAlertAction actionWithTitle:@"取消" style:UIAlertActionStyleCancel handler:nil]];
        [alert addAction:[UIAlertAction actionWithTitle:@"确定" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
            _boardcastInputBoxOK([(alert.textFields.firstObject.text ?: @"") UTF8String]);
        }]];
        [Presenter() presentViewController:alert animated:YES completion:nil];
    });
}
void libOS::showMessagebox(const std::string& message, int tag) {
    NSString *text = [NSString stringWithUTF8String:message.c_str()] ?: @"";
    dispatch_async(dispatch_get_main_queue(), ^{
        UIAlertController *alert = [UIAlertController alertControllerWithTitle:nil message:text
            preferredStyle:UIAlertControllerStyleAlert];
        [alert addAction:[UIAlertAction actionWithTitle:@"确定" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
            _boardcastMessageboxOK(tag);
        }]];
        [Presenter() presentViewController:alert animated:YES completion:nil];
    });
}
void libOS::openURL(const std::string& value) {
    NSString *text = [NSString stringWithUTF8String:value.c_str()];
    if (!text.length) return;
    if (![text containsString:@":"]) text = [@"https://" stringByAppendingString:text];
    NSURL *url = [NSURL URLWithString:text];
    if (url) dispatch_async(dispatch_get_main_queue(), ^{
        [UIApplication.sharedApplication openURL:url options:@{} completionHandler:^(BOOL success) {
            if (!success) NSLog(@"[IOSLocal] Could not open external URL");
        }];
    });
}
void libOS::emailTo(const std::string& mailto, const std::string& cc,
                  const std::string& title, const std::string& body) {
    NSURLComponents *url = [NSURLComponents componentsWithString:
        [@"mailto:" stringByAppendingString:@(mailto.c_str())]];
    url.queryItems = @[[NSURLQueryItem queryItemWithName:@"cc" value:@(cc.c_str())],
        [NSURLQueryItem queryItemWithName:@"subject" value:@(title.c_str())],
        [NSURLQueryItem queryItemWithName:@"body" value:@(body.c_str())]];
    if (url.URL) openURL([url.URL.absoluteString UTF8String]);
}
void libOS::setWaiting(bool show) {
    dispatch_async(dispatch_get_main_queue(), ^{
        static UIActivityIndicatorView *indicator = nil;
        if (!indicator) {
            indicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleLarge];
            indicator.userInteractionEnabled = NO;
            indicator.hidesWhenStopped = YES;
        }
        if (show) {
            UIView *view = UIApplication.sharedApplication.keyWindow.rootViewController.view;
            indicator.center = CGPointMake(CGRectGetMidX(view.bounds), CGRectGetMidY(view.bounds));
            [view addSubview:indicator];
            [indicator startAnimating];
        } else { [indicator stopAnimating]; [indicator removeFromSuperview]; }
    });
}
long long libOS::getFreeSpace() {
    NSDictionary *attributes = [NSFileManager.defaultManager attributesOfFileSystemForPath:NSHomeDirectory() error:nil];
    return [attributes[NSFileSystemFreeSize] longLongValue];
}
NetworkStatus libOS::getNetWork() {
    sockaddr_in address = {};
    address.sin_len = sizeof(address); address.sin_family = AF_INET;
    SCNetworkReachabilityRef reachability = SCNetworkReachabilityCreateWithAddress(NULL, (sockaddr *)&address);
    if (!reachability) return NotReachable;
    SCNetworkReachabilityFlags flags = 0;
    BOOL ok = SCNetworkReachabilityGetFlags(reachability, &flags);
    CFRelease(reachability);
    if (!ok || !(flags & kSCNetworkReachabilityFlagsReachable) || (flags & kSCNetworkReachabilityFlagsConnectionRequired))
        return NotReachable;
    return (flags & kSCNetworkReachabilityFlagsIsWWAN) ? ReachableViaWWAN : ReachableViaWiFi;
}
void libOS::clearNotification() {
    [UNUserNotificationCenter.currentNotificationCenter removeAllPendingNotificationRequests];
}
void libOS::addNotification(const std::string& message, int secondsdelay, bool daily) {
    NSString *text = @(message.c_str());
    [UNUserNotificationCenter.currentNotificationCenter requestAuthorizationWithOptions:
        UNAuthorizationOptionAlert | UNAuthorizationOptionSound completionHandler:^(BOOL granted, NSError *error) {
        if (!granted) return;
        UNMutableNotificationContent *content = [[[UNMutableNotificationContent alloc] init] autorelease];
        content.body = text;
        content.sound = UNNotificationSound.defaultSound;
        NSTimeInterval interval = daily ? MAX(60, secondsdelay) : MAX(1, secondsdelay);
        UNTimeIntervalNotificationTrigger *trigger = [UNTimeIntervalNotificationTrigger triggerWithTimeInterval:interval repeats:daily];
        UNNotificationRequest *request = [UNNotificationRequest requestWithIdentifier:NSUUID.UUID.UUIDString content:content trigger:trigger];
        [UNUserNotificationCenter.currentNotificationCenter addNotificationRequest:request withCompletionHandler:^(NSError *error) {
            if (error) NSLog(@"[IOSLocal] Scheduling notification failed: %@", error.localizedDescription);
        }];
    }];
}
const std::string libOS::getDeviceID() {
    NSUserDefaults *defaults = NSUserDefaults.standardUserDefaults;
    NSString *identity = [defaults stringForKey:@"OnePieceDeviceIdentity"];
    if (!identity.length) { identity = NSUUID.UUID.UUIDString; [defaults setObject:identity forKey:@"OnePieceDeviceIdentity"]; }
    return [identity UTF8String];
}
const std::string libOS::getPlatformInfo() { return "ios_arm64"; }
