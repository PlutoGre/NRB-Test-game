

APPNAME="DreamOnePiece"
APP_ANDROID_NAME="com.youai.dreamonepiece"

echo "«Î»∑»œndkgdb.sh¿ÔSDKROOT&NDK_ROOT&COCOS2DX_ROOT"

export SDK_ROOT=/cygdrive/D/adt-bundle-windows-x86-20130219/sdk
export NDK_ROOT=/cygdrive/D/adt-bundle-windows-x86-20130219/android-ndk-r8e
export COCOS2DX_ROOT=/cygdrive/D/WorkSVN/CardGame_1/Branches/forJavascriptDev/client

if [ -z "${SDK_ROOT+aaa}" ]; then
# ... if SDK_ROOT is not set, use "$HOME/bin/android-sdk"
    SDK_ROOT="$HOME/bin/android-sdk"
fi

if [ -z "${NDK_ROOT+aaa}" ]; then
# ... if NDK_ROOT is not set, use "$HOME/bin/android-ndk"
    NDK_ROOT="$HOME/bin/android-ndk"
fi

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# ... use paths relative to current directory


APP_ROOT="$DIR"/..
APP_ANDROID_ROOT="$DIR"

echo "NDK_ROOT = $NDK_ROOT"
echo "SDK_ROOT = $SDK_ROOT"
echo "COCOS2DX_ROOT = $COCOS2DX_ROOT"
echo "APP_ROOT = $APP_ROOT"
echo "APP_ANDROID_ROOT = $APP_ANDROID_ROOT"
echo "APP_ANDROID_NAME = $APP_ANDROID_NAME"

echo
echo "Killing and restarting ${APP_ANDROID_NAME}"
echo

set -x

"${SDK_ROOT}"/platform-tools/adb shell am force-stop "${APP_ANDROID_NAME}"

NDK_MODULE_PATH="${APP_ROOT}"/..:"${COCOS2DX_ROOT}":"${COCOS2DX_ROOT}"/cocos2dx/platform/third_party/android/prebuilt \
    "${NDK_ROOT}"/ndk-gdb \
    --adb="${SDK_ROOT}"/platform-tools/adb \
    --verbose \
    --start \
    --force
