#pragma once
#include "platform/CCEGLViewProtocol.h"
NS_CC_BEGIN
class CCEGLView : public CCEGLViewProtocol {
public:
    CCEGLView();
    virtual ~CCEGLView();
    static CCEGLView* sharedOpenGLView();
    bool isOpenGLReady();
    void end();
    void swapBuffers();
    void setIMEKeyboardState(bool open);
    void setFrameSize(float width, float height);
    void setFrameZoomFactor(float) {}
    float getFrameZoomFactor() { return 1; }
};
NS_CC_END
