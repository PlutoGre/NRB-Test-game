LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := OS_static

LOCAL_MODULE_FILENAME := libOS

LOCAL_SRC_FILES := libOS.cpp

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../cocos2dx/platform/android


include $(BUILD_STATIC_LIBRARY)
