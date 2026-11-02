LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := flash2cocos2dx_static

LOCAL_MODULE_FILENAME := libflash2cocos2dx

LOCAL_SRC_FILES := 	../CArmature.cpp \
										../CAnimation.cpp \
										../CArmatureAniData.cpp \
										../CBaseNode.cpp \
										../CBone.cpp \
										../CBoneAniData.cpp \
										../CConnectionData.cpp \
										../CEventFrame.cpp \
										../CFrameNode.cpp \
										../CFrameNodeList.cpp \
										../CMathHelper.cpp \
										../CProcessBase.cpp \
										../CTween.cpp \
										../CTweenNode.cpp \
										../CXLog.cpp

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../cocos2dx \
										$(LOCAL_PATH)/../../cocos2dx/include \
										$(LOCAL_PATH)/../../cocos2dx/kazmath/include \
										$(LOCAL_PATH)/../../tinyxml \
										$(LOCAL_PATH)/../../cocos2dx/platform/android


include $(BUILD_STATIC_LIBRARY)
