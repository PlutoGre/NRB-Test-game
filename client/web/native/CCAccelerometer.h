#pragma once
#include "platform/CCAccelerometerDelegate.h"
NS_CC_BEGIN
// Browser motion sensors require a separate permission gesture. The card game
// uses pointer input; no synthetic acceleration is sent to gameplay.
class CCAccelerometer {
public:
    static CCAccelerometer* sharedAccelerometer() { static CCAccelerometer value; return &value; }
    void removeDelegate(CCAccelerometerDelegate*) {}
    void addDelegate(CCAccelerometerDelegate*) {}
    void setDelegate(CCAccelerometerDelegate*) {}
    void setAccelerometerInterval(float) {}
};
NS_CC_END
