LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := cocos_lua_static

LOCAL_MODULE_FILENAME := liblua

LOCAL_SRC_FILES := ../cocos2dx_support/CCLuaBridge.cpp \
          ../cocos2dx_support/CCLuaEngine.cpp \
          ../cocos2dx_support/CCLuaStack.cpp \
          ../cocos2dx_support/CCLuaValue.cpp \
          ../cocos2dx_support/Cocos2dxLuaLoader.cpp \
          ../cocos2dx_support/LuaCocos2d.cpp \
          ../tolua/tolua_event.c \
          ../tolua/tolua_is.c \
          ../tolua/tolua_map.c \
          ../tolua/tolua_push.c \
          ../tolua/tolua_to.c \
          ../cocos2dx_support/tolua_fix.c \
          ../lua/loadlib.c \
          ../lua/lmem.c \
          ../lua/lmathlib.c \
          ../lua/llex.c \
          ../lua/liolib.c \
          ../lua/linit.c \
          ../lua/lgc.c \
          ../lua/lfunc.c \
          ../lua/ldump.c \
          ../lua/ldo.c \
          ../lua/ldebug.c \
          ../lua/ldblib.c \
          ../lua/lcode.c \
          ../lua/lbaselib.c \
          ../lua/lauxlib.c \
          ../lua/lapi.c \
          ../lua/print.c \
          ../lua/lzio.c \
          ../lua/lvm.c \
          ../lua/lundump.c \
          ../lua/lua.c \
          ../lua/ltm.c \
          ../lua/ltablib.c \
          ../lua/ltable.c \
          ../lua/lstrlib.c \
          ../lua/lstring.c \
          ../lua/lstate.c \
          ../lua/lparser.c \
          ../lua/loslib.c \
          ../lua/lopcodes.c \
          ../lua/lobject.c
          
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../luajit/include \
                           $(LOCAL_PATH)/../tolua \
                           $(LOCAL_PATH)/../cocos2dx_support 
          
          
#$(LOCAL_PATH)/../luajit/include \
		  
LOCAL_C_INCLUDES := $(LOCAL_PATH)/ \
                    $(LOCAL_PATH)/../lua \
                    $(LOCAL_PATH)/../tolua \
                    $(LOCAL_PATH)/../../../cocos2dx \
                    $(LOCAL_PATH)/../../../cocos2dx/include \
                    $(LOCAL_PATH)/../../../cocos2dx/platform \
                    $(LOCAL_PATH)/../../../cocos2dx/platform/android \
                    $(LOCAL_PATH)/../../../cocos2dx/kazmath/include \
                    $(LOCAL_PATH)/../../../CocosDenshion/include

#LOCAL_WHOLE_STATIC_LIBRARIES := luajit_static

LOCAL_CFLAGS += -Wno-psabi
LOCAL_EXPORT_CFLAGS += -Wno-psabi

include $(BUILD_STATIC_LIBRARY)

#$(call import-module,scripting/lua/luajit)
