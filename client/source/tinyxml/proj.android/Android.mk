LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := tinyxml_static

LOCAL_MODULE_FILENAME := libtinyxml

LOCAL_SRC_FILES := 	../tinystr.cpp \
										../tinyxml.cpp \
										../tinyxmlerror.cpp \
										../tinyxmlparser.cpp

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../

include $(BUILD_STATIC_LIBRARY)
