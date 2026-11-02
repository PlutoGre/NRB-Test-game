
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

cp -uf ..\..\src\com\youai\dreamonepiece\platform\punchbox\ChuKongActivity.java .\src\com\youai\dreamonepiece\platform\punchbox\ChuKongActivity.java

cp -uf ..\..\src\com\youai\dreamonepiece\platform\punchbox\SdkHttpListener.java .\src\com\youai\dreamonepiece\platform\punchbox\SdkHttpListener.java 
cp -uf ..\..\src\com\youai\dreamonepiece\platform\punchbox\SdkHttpTask.java .\src\com\youai\dreamonepiece\platform\punchbox\SdkHttpTask.java 
cp -uf ..\..\src\com\youai\dreamonepiece\platform\punchbox\TokenInfoListener.java .\src\com\youai\dreamonepiece\platform\punchbox\TokenInfoListener.java 
cp -uf ..\..\src\com\youai\dreamonepiece\platform\punchbox\TokenInfoTask.java .\src\com\youai\dreamonepiece\platform\punchbox\TokenInfoTask.java 

cp -uf ..\..\src\com\youai\dreamonepiece\platform\punchbox\PlatformChuKongLoginAndPay.java .\src\com\youai\dreamonepiece\platform\punchbox\PlatformChuKongLoginAndPay.java

for /r ./src %%i in (*.java) do (
	sed -i "s/com.youai.dreamonepiece.platform.*.R;/com.youai.dreamonepiece.platform.punchbox.R;/g" %%i
)
sed -i "s/import com.youai.dreamonepiece.platform.*;/import com.youai.dreamonepiece.platform.punchbox.ChuKongActivity;/g;s/,.*.Activity.class);$/,ChuKongActivity.class);/g" .\src\com\youai\dreamonepiece\NotificationService.java
echo -----copy res-----

cp -ri ../../res_chukong/* .\res\

echo -----copy libs-----

cp -uf ..\..\libs\android-parcel.jar .\libs\android-parcel.jar
cp -uf ..\..\libs\android-support-v4.jar .\libs\android-support-v4.jar
cp -uf ..\..\libs\annotations.jar .\libs\annotations.jar

echo -----copy so-----

cp -uf ..\..\libs\armeabi\libDreamOnePiece.so .\libs\armeabi\libDreamOnePiece.so
cp -uf ..\..\libs\armeabi-v7a\libDreamOnePiece.so .\libs\armeabi-v7a\libDreamOnePiece.so
cp -uf ..\..\..\chukonglib\libjoygamesdk.so .\libs\armeabi\libjoygamesdk.so
cp -uf ..\..\..\chukonglib\libjoygamesdk.so .\libs\armeabi-v7a\libjoygamesdk.so
cp -uf ..\..\..\chukonglib\liblthj_unipaybusiness20130328.so .\libs\armeabi\liblthj_unipaybusiness20130328.so
cp -uf ..\..\..\chukonglib\liblthj_unipaybusiness20130328.so .\libs\armeabi-v7a\liblthj_unipaybusiness20130328.so

echo -----copy assets-----

set /p str=<..\assetsDir.txt
cp -urf %str% .\assets\
cp -rf ../../../chukongRes/* .\assets\
