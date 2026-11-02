#import <AVFoundation/AVFoundation.h>
#include "SimpleAudioEngine.h"
#include "CCFileUtils.h"
#include <map>
#include <set>

// Preserve the game's audio API using AVAudioPlayer instead of retired AudioSession C APIs.
static AVAudioPlayer *music = nil;
static std::string musicPath;
static float musicVolume = 1, effectVolume = 1;
static unsigned int nextEffect = 0;
static std::map<unsigned int, AVAudioPlayer *> effects;
static std::set<unsigned int> pausedEffects;
static NSMutableDictionary *effectCache = nil;
static CocosDenshion::SimpleAudioEngine *engine = 0;

@interface OPAudioEvents : NSObject <AVAudioPlayerDelegate>
@property(nonatomic, assign) BOOL resumeMusic;
@property(nonatomic, retain) NSMutableArray *interruptedEffects;
- (void)interrupted:(NSNotification *)notification;
@end
static OPAudioEvents *events = nil;
@implementation OPAudioEvents
- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)success {
    for (std::map<unsigned int, AVAudioPlayer *>::iterator it = effects.begin(); it != effects.end(); ++it) {
        if (it->second == player) {
            pausedEffects.erase(it->first);
            effects.erase(it);
            [player autorelease];
            break;
        }
    }
}
- (void)interrupted:(NSNotification *)notification {
    if ([notification.userInfo[AVAudioSessionInterruptionTypeKey] unsignedIntegerValue] == AVAudioSessionInterruptionTypeBegan) {
        self.resumeMusic = music.isPlaying;
        [music pause];
        self.interruptedEffects = [NSMutableArray array];
        for (std::map<unsigned int, AVAudioPlayer *>::iterator it = effects.begin(); it != effects.end(); ++it)
            if (it->second.isPlaying) { [self.interruptedEffects addObject:@(it->first)]; [it->second pause]; }
    } else {
        BOOL resume = [notification.userInfo[AVAudioSessionInterruptionOptionKey] unsignedIntegerValue]
            & AVAudioSessionInterruptionOptionShouldResume;
        if (resume) {
            [AVAudioSession.sharedInstance setActive:YES error:nil];
            if (self.resumeMusic) [music play];
            for (NSNumber *key in self.interruptedEffects) {
                std::map<unsigned int, AVAudioPlayer *>::iterator it = effects.find(key.unsignedIntValue);
                if (it != effects.end() && !pausedEffects.count(it->first)) [it->second play];
            }
        }
        self.interruptedEffects = nil;
    }
}
- (void)dealloc { [_interruptedEffects release]; [super dealloc]; }
@end

static NSString *AudioPath(const char *path) {
    if (!path || !*path) return nil;
    std::string full = cocos2d::CCFileUtils::sharedFileUtils()->fullPathForFilename(path);
    return @(full.c_str());
}
static AVAudioPlayer *CreatePlayer(NSString *path) {
    if (!path) return nil;
    NSError *error = nil;
    NSData *cached = effectCache[path];
    AVAudioPlayer *player = cached ? [[AVAudioPlayer alloc] initWithData:cached error:&error] :
        [[AVAudioPlayer alloc] initWithContentsOfURL:[NSURL fileURLWithPath:path] error:&error];
    if (!player) NSLog(@"[IOSLocal] Audio could not be loaded: %@", error.localizedDescription);
    [player prepareToPlay];
    return player;
}

namespace CocosDenshion {
SimpleAudioEngine::SimpleAudioEngine() {
    effectCache = [[NSMutableDictionary alloc] init];
    events = [[OPAudioEvents alloc] init];
    NSError *error = nil;
    if (![AVAudioSession.sharedInstance setCategory:AVAudioSessionCategoryAmbient error:&error])
        NSLog(@"[IOSLocal] Audio session setup failed: %@", error.localizedDescription);
    [AVAudioSession.sharedInstance setActive:YES error:nil];
    [NSNotificationCenter.defaultCenter addObserver:events selector:@selector(interrupted:)
        name:AVAudioSessionInterruptionNotification object:nil];
}
SimpleAudioEngine::~SimpleAudioEngine() {
    [NSNotificationCenter.defaultCenter removeObserver:events];
    stopAllEffects(); stopBackgroundMusic(true);
    [effectCache release]; effectCache = nil;
    [events release]; events = nil;
}
SimpleAudioEngine *SimpleAudioEngine::sharedEngine() {
    if (!engine) engine = new SimpleAudioEngine;
    return engine;
}
void SimpleAudioEngine::end() { delete engine; engine = 0; }
void SimpleAudioEngine::preloadBackgroundMusic(const char *path) {
    NSString *full = AudioPath(path);
    if (!full || musicPath == [full UTF8String]) return;
    [music stop]; [music release];
    music = CreatePlayer(full); music.volume = musicVolume;
    musicPath = [full UTF8String];
}
void SimpleAudioEngine::playBackgroundMusic(const char *path, bool loop) {
    preloadBackgroundMusic(path); music.numberOfLoops = loop ? -1 : 0; [music play];
}
void SimpleAudioEngine::stopBackgroundMusic(bool release) {
    [music stop]; music.currentTime = 0;
    if (release) { [music release]; music = nil; musicPath.clear(); }
}
void SimpleAudioEngine::pauseBackgroundMusic() { [music pause]; }
void SimpleAudioEngine::resumeBackgroundMusic() { [music play]; }
void SimpleAudioEngine::rewindBackgroundMusic() { music.currentTime = 0; [music play]; }
bool SimpleAudioEngine::willPlayBackgroundMusic() { return !AVAudioSession.sharedInstance.otherAudioPlaying; }
bool SimpleAudioEngine::isBackgroundMusicPlaying() { return music.isPlaying; }
float SimpleAudioEngine::getBackgroundMusicVolume() { return musicVolume; }
void SimpleAudioEngine::setBackgroundMusicVolume(float value) { musicVolume = MAX(0, MIN(1, value)); music.volume = musicVolume; }
float SimpleAudioEngine::getEffectsVolume() { return effectVolume; }
void SimpleAudioEngine::setEffectsVolume(float value) {
    effectVolume = MAX(0, MIN(1, value));
    for (std::map<unsigned int, AVAudioPlayer *>::iterator it = effects.begin(); it != effects.end(); ++it)
        it->second.volume = effectVolume;
}
unsigned int SimpleAudioEngine::playEffect(const char *path, bool loop) {
    AVAudioPlayer *player = CreatePlayer(AudioPath(path));
    if (!player) return 0;
    // Bound retained players when many short effects finish between callbacks.
    if (effects.size() >= 64) { [player release]; return 0; }
    unsigned int identifier = ++nextEffect;
    if (!identifier) identifier = ++nextEffect;
    effects[identifier] = player;
    player.volume = effectVolume; player.numberOfLoops = loop ? -1 : 0; player.delegate = events;
    if (![player play]) { stopEffect(identifier); return 0; }
    return identifier;
}
void SimpleAudioEngine::pauseEffect(unsigned int id) {
    if (effects.count(id)) { [effects[id] pause]; pausedEffects.insert(id); }
}
void SimpleAudioEngine::pauseAllEffects() {
    for (std::map<unsigned int, AVAudioPlayer *>::iterator it = effects.begin(); it != effects.end(); ++it) pauseEffect(it->first);
}
void SimpleAudioEngine::resumeEffect(unsigned int id) {
    if (effects.count(id)) { [effects[id] play]; pausedEffects.erase(id); }
}
void SimpleAudioEngine::resumeAllEffects() {
    std::set<unsigned int> paused = pausedEffects;
    for (std::set<unsigned int>::iterator it = paused.begin(); it != paused.end(); ++it) resumeEffect(*it);
}
void SimpleAudioEngine::stopEffect(unsigned int id) {
    if (!effects.count(id)) return;
    AVAudioPlayer *player = effects[id]; player.delegate = nil; [player stop]; [player release];
    effects.erase(id); pausedEffects.erase(id);
}
void SimpleAudioEngine::stopAllEffects() { while (!effects.empty()) stopEffect(effects.begin()->first); }
void SimpleAudioEngine::preloadEffect(const char *path) {
    NSString *full = AudioPath(path);
    if (full && !effectCache[full]) {
        NSData *data = [NSData dataWithContentsOfFile:full];
        if (data) effectCache[full] = data;
    }
}
void SimpleAudioEngine::unloadEffect(const char *path) {
    NSString *full = AudioPath(path); if (full) [effectCache removeObjectForKey:full];
}
}
