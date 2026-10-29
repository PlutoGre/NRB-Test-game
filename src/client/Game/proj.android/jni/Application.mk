APP_STL := gnustl_static
APP_CPPFLAGS := -frtti -DCOCOS2D_DEBUG=0 -fexceptions -O3 -DANDROID
#APP_CPPFLAGS := -frtti -DCOCOS2D_DEBUG=1 -fexceptions -DANDROID -D_DEBUG
#APP_ABI := armeabi armeabi-v7a x86
#build发行版时考虑apk包含这3个ABI的binary，因为有些android设备用的intel atom芯片，另外早期arm芯片不支持v7a
APP_ABI := armeabi armeabi-v7a
APP_PLATFORM := android-9
APP_OPTIM := release
#APP_OPTIM := debug
