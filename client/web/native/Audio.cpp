#include "SimpleAudioEngine.h"
#include "cocos2d.h"
#include <emscripten.h>

namespace CocosDenshion {
static SimpleAudioEngine* engine = 0;
static float musicVolume = 1, effectsVolume = 1;
static unsigned int sequence = 1;
static std::string resolved(const char* path) { return cocos2d::CCFileUtils::sharedFileUtils()->fullPathForFilename(path); }
SimpleAudioEngine::SimpleAudioEngine() {}
SimpleAudioEngine::~SimpleAudioEngine() {}
SimpleAudioEngine* SimpleAudioEngine::sharedEngine() { if (!engine) engine = new SimpleAudioEngine; return engine; }
void SimpleAudioEngine::end() { if (engine) { engine->stopAllEffects(); engine->stopBackgroundMusic(true); delete engine; engine = 0; } }
void SimpleAudioEngine::preloadBackgroundMusic(const char* path) { preloadEffect(path); }
void SimpleAudioEngine::playBackgroundMusic(const char* path, bool loop) {
    EM_ASM({ window.NRB.audio.play(Module, UTF8ToString($0), 0, !!$1, $2); }, resolved(path).c_str(), loop, musicVolume);
}
void SimpleAudioEngine::stopBackgroundMusic(bool release) { EM_ASM({ window.NRB.audio.stop(0, !!$0); }, release); }
void SimpleAudioEngine::pauseBackgroundMusic() { pauseEffect(0); }
void SimpleAudioEngine::resumeBackgroundMusic() { resumeEffect(0); }
void SimpleAudioEngine::rewindBackgroundMusic() { EM_ASM({ window.NRB.audio.rewind(0); }); }
bool SimpleAudioEngine::willPlayBackgroundMusic() { return true; }
bool SimpleAudioEngine::isBackgroundMusicPlaying() { return EM_ASM_INT({ return window.NRB.audio.playing(0); }); }
float SimpleAudioEngine::getBackgroundMusicVolume() { return musicVolume; }
float SimpleAudioEngine::getEffectsVolume() { return effectsVolume; }
void SimpleAudioEngine::setBackgroundMusicVolume(float volume) { musicVolume = std::max(0.f, std::min(1.f, volume)); EM_ASM({ window.NRB.audio.volume(true, $0); }, musicVolume); }
void SimpleAudioEngine::setEffectsVolume(float volume) { effectsVolume = std::max(0.f, std::min(1.f, volume)); EM_ASM({ window.NRB.audio.volume(false, $0); }, effectsVolume); }
unsigned int SimpleAudioEngine::playEffect(const char* path, bool loop) {
    unsigned int id = sequence++;
    EM_ASM({ window.NRB.audio.play(Module, UTF8ToString($0), $1, !!$2, $3); }, resolved(path).c_str(), id, loop, effectsVolume);
    return id;
}
void SimpleAudioEngine::pauseEffect(unsigned int id) { EM_ASM({ window.NRB.audio.pause($0); }, id); }
void SimpleAudioEngine::resumeEffect(unsigned int id) { EM_ASM({ window.NRB.audio.resume($0); }, id); }
void SimpleAudioEngine::stopEffect(unsigned int id) { EM_ASM({ window.NRB.audio.stop($0); }, id); }
void SimpleAudioEngine::pauseAllEffects() { EM_ASM({ window.NRB.audio.each('pause'); }); }
void SimpleAudioEngine::resumeAllEffects() { EM_ASM({ window.NRB.audio.each('resume'); }); }
void SimpleAudioEngine::stopAllEffects() { EM_ASM({ window.NRB.audio.each('stop'); }); }
void SimpleAudioEngine::preloadEffect(const char* path) { EM_ASM({ window.NRB.audio.load(Module, UTF8ToString($0)); }, resolved(path).c_str()); }
void SimpleAudioEngine::unloadEffect(const char* path) { EM_ASM({ window.NRB.audio.cache.delete(UTF8ToString($0)); }, resolved(path).c_str()); }
}
