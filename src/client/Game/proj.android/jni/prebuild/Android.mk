# The unused legacy UDID binary is ARM-only; never package it for x86.
ifneq ($(filter armeabi armeabi-v7a,$(TARGET_ARCH_ABI)),)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := udid

LOCAL_SRC_FILES := libudid.so
include $(PREBUILT_SHARED_LIBRARY)
endif
