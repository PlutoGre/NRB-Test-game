#import <Foundation/Foundation.h>
#include "ConnectionSettings.h"
#include "RemoteSocket.h"
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>
#include <map>
#include <mutex>

@interface OPStreamBridge : NSObject <NSURLSessionWebSocketDelegate> {
    int _peer;
    NSCondition *_condition;
    NSURLSession *_session;
    NSURLSessionWebSocketTask *_task;
    BOOL _opened, _closed, _started;
}
- (id)initWithPeer:(int)peer;
- (BOOL)connectWithTimeout:(unsigned long)milliseconds;
- (void)stop;
- (void)receiveNext;
- (void)sendLoop;
@end

@implementation OPStreamBridge
- (id)initWithPeer:(int)peer {
    if ((self = [super init])) {
        _peer = peer;
        _condition = [[NSCondition alloc] init];
        struct timeval timeout = {15, 0};
        setsockopt(_peer, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    }
    return self;
}
- (BOOL)connectWithTimeout:(unsigned long)milliseconds {
    [_condition lock];
    if (_started || _closed) { [_condition unlock]; return NO; }
    _started = YES;
    NSString *base = @(OPRemoteBaseURL().c_str());
    NSString *address = [[@"wss://" stringByAppendingString:[base substringFromIndex:8]] stringByAppendingString:@"/game"];
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:address]];
    [request setValue:[@"Bearer " stringByAppendingString:@(OPRemoteInvite().c_str())]
        forHTTPHeaderField:@"Authorization"];
    [request setValue:@"nrb-tcp-v1" forHTTPHeaderField:@"Sec-WebSocket-Protocol"];
    _session = [[NSURLSession sessionWithConfiguration:NSURLSessionConfiguration.ephemeralSessionConfiguration
        delegate:self delegateQueue:nil] retain];
    _task = [[_session webSocketTaskWithRequest:request] retain];
    _task.maximumMessageSize = 1024 * 1024;
    [_task resume];
    NSDate *deadline = [NSDate dateWithTimeIntervalSinceNow:milliseconds / 1000.0];
    while (!_opened && !_closed && [_condition waitUntilDate:deadline]) {}
    BOOL ok = _opened && !_closed;
    [_condition unlock];
    if (ok) [NSThread detachNewThreadSelector:@selector(sendLoop) toTarget:self withObject:nil];
    else [self stop];
    return ok;
}
- (void)stop {
    [_condition lock];
    if (_closed) { [_condition unlock]; return; }
    _closed = YES;
    [_condition broadcast];
    // Shutdown wakes both blocked directions. Close only at dealloc so a delayed
    // callback can never write to an unrelated descriptor that reused this number.
    shutdown(_peer, SHUT_RDWR);
    NSURLSessionWebSocketTask *task = _task; _task = nil;
    NSURLSession *session = _session; _session = nil;
    [_condition unlock];
    [task cancelWithCloseCode:NSURLSessionWebSocketCloseCodeNormalClosure reason:nil];
    [session invalidateAndCancel];
    [task release]; [session release];
    NSLog(@"[IOSRemote] Game connection closed");
}
- (void)URLSession:(NSURLSession *)session webSocketTask:(NSURLSessionWebSocketTask *)task
    didOpenWithProtocol:(NSString *)protocol {
    if (![protocol isEqualToString:@"nrb-tcp-v1"]) { [self stop]; return; }
    [_condition lock];
    BOOL active = !_closed;
    if (active) { _opened = YES; [_condition broadcast]; }
    [_condition unlock];
    if (active) {
        NSLog(@"[IOSRemote] Binary WebSocket connected");
        [self receiveNext];
    }
}
- (void)URLSession:(NSURLSession *)session webSocketTask:(NSURLSessionWebSocketTask *)task
    didCloseWithCode:(NSURLSessionWebSocketCloseCode)closeCode reason:(NSData *)reason {
    [self stop];
}
- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task didCompleteWithError:(NSError *)error {
    [self stop];
}
- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task
    willPerformHTTPRedirection:(NSHTTPURLResponse *)response newRequest:(NSURLRequest *)request
    completionHandler:(void (^)(NSURLRequest *))completionHandler {
    completionHandler(nil);
}
- (void)receiveNext {
    [_condition lock];
    NSURLSessionWebSocketTask *task = _closed ? nil : [_task retain];
    [_condition unlock];
    [task receiveMessageWithCompletionHandler:^(NSURLSessionWebSocketMessage *message, NSError *error) {
        if (error || !message || message.type != NSURLSessionWebSocketMessageTypeData) { [self stop]; return; }
        NSData *data = message.data;
        // One outstanding receive/write bounds memory and preserves stream order.
        dispatch_async(dispatch_get_global_queue(QOS_CLASS_UTILITY, 0), ^{
            @autoreleasepool {
                const char *bytes = (const char *)data.bytes;
                NSUInteger remaining = data.length;
                while (remaining) {
                    ssize_t count = send(_peer, bytes, remaining, 0);
                    if (count < 0 && errno == EINTR) continue;
                    if (count <= 0) { [self stop]; return; }
                    bytes += count; remaining -= count;
                }
                [self receiveNext];
            }
        });
    }];
    [task release];
}
- (void)sendLoop {
    while (true) {
        @autoreleasepool {
            char bytes[16384];
            ssize_t count = recv(_peer, bytes, sizeof(bytes), 0);
            if (count < 0 && errno == EINTR) continue;
            if (count <= 0) { [self stop]; return; }
            [_condition lock];
            NSURLSessionWebSocketTask *task = _closed ? nil : [_task retain];
            [_condition unlock];
            if (!task) return;
            NSData *data = [NSData dataWithBytes:bytes length:count];
            NSURLSessionWebSocketMessage *message = [[[NSURLSessionWebSocketMessage alloc] initWithData:data] autorelease];
            dispatch_semaphore_t completed = dispatch_semaphore_create(0);
            dispatch_retain(completed); // The callback may arrive after the wait times out.
            __block BOOL succeeded = NO;
            [task sendMessage:message completionHandler:^(NSError *error) {
                succeeded = error == nil;
                dispatch_semaphore_signal(completed);
                dispatch_release(completed);
            }];
            [task release];
            long waited = dispatch_semaphore_wait(completed, dispatch_time(DISPATCH_TIME_NOW, 15 * NSEC_PER_SEC));
            dispatch_release(completed);
            if (waited || !succeeded) { [self stop]; return; }
        }
    }
}
- (void)dealloc {
    [self stop];
    close(_peer);
    [_condition release];
    [super dealloc];
}
@end

static std::mutex bridgesMutex;
static std::map<int, OPStreamBridge *> bridges;

int OPCreateRemoteSocket() {
    @autoreleasepool {
        int descriptors[2];
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, descriptors) != 0) return -1;
        if (descriptors[0] >= FD_SETSIZE) { close(descriptors[0]); close(descriptors[1]); return -1; }
        OPStreamBridge *bridge = [[OPStreamBridge alloc] initWithPeer:descriptors[1]];
        std::lock_guard<std::mutex> lock(bridgesMutex);
        bridges[descriptors[0]] = bridge;
        return descriptors[0];
    }
}

bool OPConnectRemoteSocket(int descriptor, unsigned long timeoutMilliseconds) {
    @autoreleasepool {
        OPStreamBridge *bridge = nil;
        {
            std::lock_guard<std::mutex> lock(bridgesMutex);
            auto entry = bridges.find(descriptor);
            if (entry != bridges.end()) bridge = [entry->second retain];
        }
        BOOL connected = [bridge connectWithTimeout:timeoutMilliseconds];
        [bridge release];
        return connected;
    }
}

void OPCloseRemoteSocket(int descriptor) {
    @autoreleasepool {
        OPStreamBridge *bridge = nil;
        {
            std::lock_guard<std::mutex> lock(bridgesMutex);
            auto entry = bridges.find(descriptor);
            if (entry != bridges.end()) { bridge = entry->second; bridges.erase(entry); }
        }
        [bridge stop];
        [bridge release];
    }
}
