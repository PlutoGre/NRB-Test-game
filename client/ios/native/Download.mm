#import <Foundation/Foundation.h>
#include "CurlDownload.h"
#include "GameMaths.h"
#include "ConnectionSettings.h"

// Resource redirects must not carry the invitation to another host or scheme.
@interface OPResourceSessionDelegate : NSObject <NSURLSessionTaskDelegate>
@end
@implementation OPResourceSessionDelegate
- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task
    willPerformHTTPRedirection:(NSHTTPURLResponse *)response newRequest:(NSURLRequest *)request
    completionHandler:(void (^)(NSURLRequest *))completionHandler {
    completionHandler(nil);
}
@end

static NSURLSession *resourceSession() {
    static NSURLSession *session = nil;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        OPResourceSessionDelegate *delegate = [[OPResourceSessionDelegate alloc] init];
        session = [[NSURLSession sessionWithConfiguration:NSURLSessionConfiguration.ephemeralSessionConfiguration
            delegate:delegate delegateQueue:nil] retain];
        [delegate release];
    });
    return session;
}

// NSURLSession provides iOS TLS, timeouts and cancellation without a 32-bit curl binary.
static NSURLSessionDataTask *activeDownload = nil;
static unsigned long downloadGeneration = 0;

CurlDownload::~CurlDownload() {
    ++downloadGeneration;
    [activeDownload cancel]; [activeDownload release]; activeDownload = nil;
    removeAllListener();
}
void CurlDownload::downloadFile(const std::string& url, const std::string& filename) {
    mDownloadQueue.push_back(DownloadFile(url, filename));
}
void CurlDownload::downloadFile(const std::string& url, const std::string& filename, unsigned short crc) {
    mDownloadQueue.push_back(DownloadFile(url, filename, crc));
}
void CurlDownload::update(float) {
    if (activeDownload || mDownloadQueue.empty()) return;
    DownloadFile file = mDownloadQueue.front();
    mDownloadQueue.pop_front();
    const unsigned long generation = downloadGeneration;
    std::string actualURL = OPResourceURL(file._url);
    NSURL *url = [NSURL URLWithString:@(actualURL.c_str())];
    // An invalid URL still completes through the regular failure callback.
    if (!url) url = [NSURL URLWithString:@"invalid:"];
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:url cachePolicy:NSURLRequestReloadIgnoringLocalCacheData
        timeoutInterval:60];
    OPAuthorizeResourceRequest(request);
    activeDownload = [[resourceSession() dataTaskWithRequest:request completionHandler:
        ^(NSData *data, NSURLResponse *response, NSError *error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            if (generation != downloadGeneration) return;
            NSInteger status = [response isKindOfClass:NSHTTPURLResponse.class] ? [(NSHTTPURLResponse *)response statusCode] : 0;
            bool ok = !error && status >= 200 && status < 300 && data != nil;
            if (ok && file._checkCRC)
                ok = GameMaths::GetCRC16((unsigned char *)data.bytes, data.length) == file._crc;
            NSError *writeError = nil;
            if (ok) {
                NSString *path = @(file._filename.c_str());
                ok = [NSFileManager.defaultManager createDirectoryAtPath:path.stringByDeletingLastPathComponent
                    withIntermediateDirectories:YES attributes:nil error:&writeError];
                if (ok) ok = [data writeToFile:path options:NSDataWritingAtomic error:&writeError];
            }
            if (!ok) NSLog(@"[IOSLocal] Resource download failed (HTTP %ld): %@", (long)status,
                error.localizedDescription ?: writeError.localizedDescription ?: @"Invalid response or CRC mismatch");
            [activeDownload release]; activeDownload = nil;
            _mutex.lock();
            std::set<DownloadListener *> listeners = mListeners;
            _mutex.unlock();
            for (std::set<DownloadListener *>::iterator it = listeners.begin(); it != listeners.end(); ++it) {
                // A previous callback may have removed another listener.
                _mutex.lock(); bool present = mListeners.count(*it) != 0; _mutex.unlock();
                if (!present) continue;
                if (ok) (*it)->downloaded(file._url, file._filename);
                else (*it)->downloadFailed(file._url, file._filename);
            }
        });
    }] retain];
    [activeDownload resume];
}
