
echo -----copy src-----

cp -uf ..\..\src\com\youai\DeviceUtil.java .\src\com\youai\DeviceUtil.java
cp -uf ..\..\src\com\youai\IGameActivity.java .\src\com\youai\IGameActivity.java
cp -uf ..\..\src\com\youai\IGameActivityState.java .\src\com\youai\IGameActivityState.java
cp -uf ..\..\src\com\youai\IPlatformLoginAndPay.java .\src\com\youai\IPlatformLoginAndPay.java
cp -uf ..\..\src\com\youai\IStateManager.java .\src\com\youai\IStateManager.java
cp -uf ..\..\src\com\youai\NetworkUtil.java .\src\com\youai\NetworkUtil.java
cp -uf ..\..\src\com\youai\PlatformAndGameInfo.java .\src\com\youai\PlatformAndGameInfo.java
cp -uf ..\..\src\com\youai\StorageUtil.java .\src\com\youai\StorageUtil.java

cp -uf ..\..\src\com\youai\dreamonepiece\DownloadApk.java .\src\com\youai\dreamonepiece\DownloadApk.java
cp -uf ..\..\src\com\youai\dreamonepiece\DownloadManagerPro.java .\src\com\youai\dreamonepiece\DownloadManagerPro.java
cp -uf ..\..\src\com\youai\dreamonepiece\GameActivity.java .\src\com\youai\dreamonepiece\GameActivity.java
cp -uf ..\..\src\com\youai\dreamonepiece\GameAnnounceDialog.java .\src\com\youai\dreamonepiece\GameAnnounceDialog.java
cp -uf ..\..\src\com\youai\dreamonepiece\GameAppState.java .\src\com\youai\dreamonepiece\GameAppState.java
cp -uf ..\..\src\com\youai\dreamonepiece\GameConfig.java .\src\com\youai\dreamonepiece\GameConfig.java
cp -uf ..\..\src\com\youai\dreamonepiece\GameContextState.java .\src\com\youai\dreamonepiece\GameContextState.java
cp -uf ..\..\src\com\youai\dreamonepiece\GameInterface.java .\src\com\youai\dreamonepiece\GameInterface.java
cp -uf ..\..\src\com\youai\dreamonepiece\GameLogoState.java .\src\com\youai\dreamonepiece\GameLogoState.java
cp -uf ..\..\src\com\youai\dreamonepiece\GameStateManager.java .\src\com\youai\dreamonepiece\GameStateManager.java
cp -uf ..\..\src\com\youai\dreamonepiece\GameUpdateState.java .\src\com\youai\dreamonepiece\GameUpdateState.java
cp -uf ..\..\src\com\youai\dreamonepiece\NotificationService.java .\src\com\youai\dreamonepiece\NotificationService.java
cp -uf ..\..\src\com\youai\dreamonepiece\PlatformSDKState.java .\src\com\youai\dreamonepiece\PlatformSDKState.javaref
cp -uf ..\..\src\com\youai\dreamonepiece\YouaiUpdateState.java .\src\com\youai\dreamonepiece\YouaiUpdateState.java

cp -uf ..\..\src\com\youai\dreamonepiece\platform\qihoo360\Constants.java .\src\com\youai\dreamonepiece\platform\qihoo360\Constants.java
cp -uf ..\..\src\com\youai\dreamonepiece\platform\qihoo360\Qihoo360Activity.java .\src\com\youai\dreamonepiece\platform\qihoo360\Qihoo360Activity.java
cp -uf ..\..\src\com\youai\dreamonepiece\platform\qihoo360\Platform360LoginAndPay.java .\src\com\youai\dreamonepiece\platform\qihoo360\Platform360LoginAndPay.java
cp -uf ..\..\src\com\youai\dreamonepiece\platform\qihoo360\readme.txt .\src\com\youai\dreamonepiece\platform\qihoo360\readme.txt

cp -uf ..\..\src\com\youai\dreamonepiece\platform\qihoo360\common\QihooPayInfo.java .\src\com\youai\dreamonepiece\platform\qihoo360\common\QihooPayInfo.java
cp -uf ..\..\src\com\youai\dreamonepiece\platform\qihoo360\common\SdkAccountListener.java .\src\com\youai\dreamonepiece\platform\qihoo360\common\SdkAccountListener.java
cp -uf ..\..\src\com\youai\dreamonepiece\platform\qihoo360\common\SdkHttpListener.java .\src\com\youai\dreamonepiece\platform\qihoo360\common\SdkHttpListener.java
cp -uf ..\..\src\com\youai\dreamonepiece\platform\qihoo360\common\SdkHttpTask.java .\src\com\youai\dreamonepiece\platform\qihoo360\common\SdkHttpTask.java
cp -uf ..\..\src\com\youai\dreamonepiece\platform\qihoo360\common\SdkUserBaseActivity.java .\src\com\youai\dreamonepiece\platform\qihoo360\common\SdkUserBaseActivity.java
cp -uf ..\..\src\com\youai\dreamonepiece\platform\qihoo360\common\TryAccount.java .\src\com\youai\dreamonepiece\platform\qihoo360\common\TryAccount.java

cp -uf ..\..\src\com\youai\dreamonepiece\platform\qihoo360\appserver\QihooUserInfo.java .\src\com\youai\dreamonepiece\platform\qihoo360\appserver\QihooUserInfo.java
cp -uf ..\..\src\com\youai\dreamonepiece\platform\qihoo360\appserver\QihooUserInfoListener.java .\src\com\youai\dreamonepiece\platform\qihoo360\appserver\QihooUserInfoListener.java
cp -uf ..\..\src\com\youai\dreamonepiece\platform\qihoo360\appserver\QihooUserInfoTask.java .\src\com\youai\dreamonepiece\platform\qihoo360\appserver\QihooUserInfoTask.java
cp -uf ..\..\src\com\youai\dreamonepiece\platform\qihoo360\appserver\TokenInfo.java .\src\com\youai\dreamonepiece\platform\qihoo360\appserver\TokenInfo.java
cp -uf ..\..\src\com\youai\dreamonepiece\platform\qihoo360\appserver\TokenInfoListener.java .\src\com\youai\dreamonepiece\platform\qihoo360\appserver\TokenInfoListener.java
cp -uf ..\..\src\com\youai\dreamonepiece\platform\qihoo360\appserver\TokenInfoTask.java .\src\com\youai\dreamonepiece\platform\qihoo360\appserver\TokenInfoTask.java

cp -uf ..\..\src\com\youai\dreamonepiece\platform\qihoo360\utils\ProgressUtil.java .\src\com\youai\dreamonepiece\platform\qihoo360\utils\ProgressUtil.java

for /r ./src %%i in (*.java) do (
	sed -i "s/com.youai.dreamonepiece.platform.*.R;/com.youai.dreamonepiece.platform.qihoo360.R;/g" %%i
)
sed -i "s/import com.youai.dreamonepiece.platform.*;/import com.youai.dreamonepiece.platform.qihoo360.Qihoo360Activity;/g;s/,.*.Activity.class);$/,Qihoo360Activity.class);/g" .\src\com\youai\dreamonepiece\NotificationService.java

echo -----copy res-----

cp -ri ../../res_360/* .\res\

echo -----copy libs-----

cp -uf ..\..\libs\qihoo360\app_360game.jar .\libs\app_360game.jar
cp -uf ..\..\libs\android-parcel.jar .\libs\android-parcel.jar
cp -uf ..\..\libs\android-support-v4.jar .\libs\android-support-v4.jar
cp -uf ..\..\libs\annotations.jar .\libs\annotations.jar
cp -uf ..\..\libs\qihoo360\360SDK.jar .\libs\360SDK.jar
cp -uf ..\..\..\360Lib\upomp_bypay_1_1_lib.jar .\libs\upomp_bypay_1_1_lib.jar

echo -----copy so-----

cp -uf ..\..\libs\armeabi\libDreamOnePiece.so .\libs\armeabi\libDreamOnePiece.so
cp -uf ..\..\..\360Lib\libpaypalm_app_plugin_jar_360game.so .\libs\armeabi\libpaypalm_app_plugin_jar_360game.so
cp -uf ..\..\libs\armeabi-v7a\libDreamOnePiece.so .\libs\armeabi-v7a\libDreamOnePiece.so
cp -uf ..\..\..\360Lib\libpaypalm_app_plugin_jar_360game.so .\libs\armeabi-v7a\libpaypalm_app_plugin_jar_360game.so
cp -urf ..\..\..\360Lib\mips .\libs\

echo -----copy assets-----

set /p str=<..\assetsDir.txt
cp -urf %str% .\assets\
cp -rf ../../../360Res/* .\assets\
