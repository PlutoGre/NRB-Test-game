LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := udid

LOCAL_SRC_FILES := libudid.so
include $(PREBUILT_SHARED_LIBRARY)