LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := lp91_static

LOCAL_MODULE_FILENAME := liblp91

LOCAL_SRC_FILES := lib91.cpp \
../include/libPlatform.cpp

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../cocos2dx/platform/android
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../libOS/include


include $(BUILD_STATIC_LIBRARY)
