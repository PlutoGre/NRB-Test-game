#include "cocos2d.h"
#include "AppDelegate.h"
#include "Concurrency.h"
#include "CCDevice.h"
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <stdarg.h>
#include <stdio.h>

using namespace cocos2d;
static SDL_Window* window = 0;
static SDL_GLContext context = 0;
static CCEGLView* sharedView = 0;
static double intervalMs = 1000.0 / 30, lastFrame = 0;
static bool mouseDown = false;
static std::map<SDL_FingerID, int> fingers;
static int fingerSequence = 1;

EM_JS(void, browserIME, (int open, const char* content), {
    if (open) window.NRB.ime(Module, UTF8ToString(content)); else window.NRB.closeInput();
});
extern "C" EMSCRIPTEN_KEEPALIVE void nrb_ime(const char* text) {
    CCIMEDispatcher* dispatcher = CCIMEDispatcher::sharedDispatcher();
    const char* old = dispatcher->getContentText();
    long characters = old ? cc_utf8_strlen(old, -1) : 0;
    for (long i = 0; i < characters; ++i) dispatcher->dispatchDeleteBackward();
    dispatcher->dispatchInsertText(text, strlen(text));
}
extern "C" EMSCRIPTEN_KEEPALIVE void nrb_backspace() {
    CCIMEDispatcher::sharedDispatcher()->dispatchDeleteBackward();
}
extern "C" EMSCRIPTEN_KEEPALIVE void nrb_visibility(int hidden) {
    CCDirector* director = CCDirector::sharedDirector();
    if (hidden) {
        CCEGLView* view = CCEGLView::sharedOpenGLView();
        int id = 0; float x = 0, y = 0;
        if (mouseDown) view->handleTouchesCancel(1, &id, &x, &y);
        for (std::map<SDL_FingerID, int>::iterator it = fingers.begin(); it != fingers.end(); ++it) {
            id = it->second; view->handleTouchesCancel(1, &id, &x, &y);
        }
        mouseDown = false; fingers.clear(); director->pause();
    } else {
        director->resume(); director->setNextDeltaTimeZero(true); lastFrame = 0;
    }
}

static void frame() {
    CCEGLView* view = CCEGLView::sharedOpenGLView();
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        int id = 0;
        float x = 0, y = 0;
        if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEMOTION) {
            if (event.type == SDL_MOUSEMOTION) {
                if (event.motion.which == SDL_TOUCH_MOUSEID || !mouseDown) continue;
                x = event.motion.x; y = event.motion.y;
            } else {
                if (event.button.which == SDL_TOUCH_MOUSEID || event.button.button != SDL_BUTTON_LEFT) continue;
                x = event.button.x; y = event.button.y;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) { mouseDown = true; view->handleTouchesBegin(1, &id, &x, &y); }
            if (event.type == SDL_MOUSEBUTTONUP) { mouseDown = false; view->handleTouchesEnd(1, &id, &x, &y); }
            if (event.type == SDL_MOUSEMOTION) view->handleTouchesMove(1, &id, &x, &y);
        } else if (event.type == SDL_FINGERDOWN || event.type == SDL_FINGERUP || event.type == SDL_FINGERMOTION) {
            if (event.type == SDL_FINGERDOWN) fingers[event.tfinger.fingerId] = fingerSequence++;
            if (!fingers.count(event.tfinger.fingerId)) continue;
            id = fingers[event.tfinger.fingerId];
            x = event.tfinger.x * view->getFrameSize().width;
            y = event.tfinger.y * view->getFrameSize().height;
            if (event.type == SDL_FINGERDOWN) view->handleTouchesBegin(1, &id, &x, &y);
            if (event.type == SDL_FINGERMOTION) view->handleTouchesMove(1, &id, &x, &y);
            if (event.type == SDL_FINGERUP) { view->handleTouchesEnd(1, &id, &x, &y); fingers.erase(event.tfinger.fingerId); }
        } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            CCDirector::sharedDirector()->getKeypadDispatcher()->dispatchKeypadMSG(kTypeBackClicked);
        }
    }
    double now = emscripten_get_now();
    if (now - lastFrame + 0.5 < intervalMs) return;
    lastFrame = now;
    CCDirector::sharedDirector()->mainLoop();
}

NS_CC_BEGIN
CCApplication* CCApplication::instance = 0;
CCApplication::CCApplication() { instance = this; }
CCApplication::~CCApplication() { instance = 0; }
CCApplication* CCApplication::sharedApplication() { return instance; }
void CCApplication::setAnimationInterval(double value) { intervalMs = value * 1000; }
ccLanguageType CCApplication::getCurrentLanguage() { return kLanguageChinese; }
TargetPlatform CCApplication::getTargetPlatform() { return kTargetWeb; }
int CCApplication::run() {
    if (!applicationDidFinishLaunching()) return 1;
    // Browser keeps ownership of the UI thread. Never run the native while loop.
    emscripten_set_main_loop(frame, 0, true);
    return 0;
}
CCEGLView::CCEGLView() {
    SDL_SetMainReady();
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
    if (SDL_Init(SDL_INIT_VIDEO) != 0) throw std::runtime_error(SDL_GetError());
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    window = SDL_CreateWindow("男人帮", 0, 0, 640, 960, SDL_WINDOW_OPENGL);
    if (window) context = SDL_GL_CreateContext(window);
    if (!context) throw std::runtime_error(SDL_GetError());
    CCEGLViewProtocol::setFrameSize(640, 960);
}
CCEGLView::~CCEGLView() {
    sharedView = 0;
    if (context) SDL_GL_DeleteContext(context);
    if (window) SDL_DestroyWindow(window);
    context = 0; window = 0;
}
CCEGLView* CCEGLView::sharedOpenGLView() {
    // CCDirector owns and deletes the view when replacing it.
    if (!sharedView) sharedView = new CCEGLView;
    return sharedView;
}
bool CCEGLView::isOpenGLReady() { return context != 0; }
void CCEGLView::setFrameSize(float width, float height) {
    SDL_SetWindowSize(window, width, height);
    CCEGLViewProtocol::setFrameSize(width, height);
}
void CCEGLView::swapBuffers() { SDL_GL_SwapWindow(window); }
void CCEGLView::end() {
    emscripten_cancel_main_loop();
    EM_ASM({ window.NRB.message('游戏已退出，点击确定重新进入。', () => location.reload()); });
}
void CCEGLView::setIMEKeyboardState(bool open) {
    const char* text = CCIMEDispatcher::sharedDispatcher()->getContentText();
    browserIME(open, text ? text : "");
}
void CCLog(const char* format, ...) {
    va_list args; va_start(args, format); vprintf(format, args); va_end(args); putchar('\n');
}
void CCLuaLog(const char* text) { CCLog("%s", text); }
void CCMessageBox(const char* text, const char*) { EM_ASM({ window.NRB.message(UTF8ToString($0)); }, text); }
int CCDevice::getDPI() { return 160; }
NS_CC_END

// Only Mutex survives in Web gameplay; no worker threads or blocking sleeps.
Mutex::Mutex() { pthread_mutex_init(&_mutex, 0); }
Mutex::~Mutex() { pthread_mutex_destroy(&_mutex); }
void Mutex::lock() { pthread_mutex_lock(&_mutex); }
void Mutex::unlock() { pthread_mutex_unlock(&_mutex); }

int main() {
    extern void initializeWebPlatform();
    initializeWebPlatform();
    // AppDelegate inherits CCApplication privately and must outlive main().
    static AppDelegate delegate;
    return CCApplication::sharedApplication()->run();
}
