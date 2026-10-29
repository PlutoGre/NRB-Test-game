
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

cp -uf ..\..\src\com\youai\dreamonepiece\platform\feiliu\FeiLiuActivity.java .\src\com\youai\dreamonepiece\platform\feiliu\FeiLiuActivity.java
cp -uf ..\..\src\com\youai\dreamonepiece\platform\feiliu\PlatformFeiLiuLoginAndPay.java .\src\com\youai\dreamonepiece\platform\feiliu\PlatformFeiLiuLoginAndPay.java

for /r ./src %%i in (*.java) do (
	sed -i "s/com.youai.dreamonepiece.platform.*.R;/com.youai.dreamonepiece.platform.feiliu.R;/g" %%i
)

sed -i "s/import com.youai.dreamonepiece.platform.*;/import com.youai.dreamonepiece.platform.feiliu.FeiLiuActivity;/g;s/,.*.Activity.class);$/,FeiLiuActivity.class);/g" .\src\com\youai\dreamonepiece\NotificationService.java

echo -----copy res-----

cp -ri ../../res_feiliu/* .\res\

echo -----copy libs-----

cp -uf ..\..\libs\feiliu\alipay_msp.jar .\libs\alipay_msp.jar
cp -uf ..\..\libs\android-parcel.jar .\libs\android-parcel.jar
cp -uf ..\..\libs\android-support-v4.jar .\libs\android-support-v4.jar
cp -uf ..\..\libs\feiliu\androidsdklibrary.jar .\libs\androidsdklibrary.jar
cp -uf ..\..\libs\feiliu\gamehelper.jar .\libs\gamehelper.jar
cp -uf ..\..\libs\feiliu\oauth20.jar .\libs\oauth20.jar
cp -uf ..\..\libs\feiliu\oauth20-api.jar .\libs\oauth20-api.jar
cp -uf ..\..\libs\feiliu\sinaweibosso.jar .\libs\sinaweibosso.jar
cp -uf ..\..\libs\feiliu\tencent_openapi.jar .\libs\tencent_openapi.jar
cp -uf ..\..\libs\feiliu\mokredit.jar .\libs\mokredit.jar
cp -uf ..\..\libs\feiliu\UPPayAssistEx.jar .\libs\UPPayAssistEx.jar
echo -----copy so-----

cp -uf ..\..\libs\armeabi\libDreamOnePiece.so .\libs\armeabi\libDreamOnePiece.so
cp -uf ..\..\libs\armeabi-v7a\libDreamOnePiece.so .\libs\armeabi-v7a\libDreamOnePiece.so

echo -----copy assets-----

set /p str=<..\assetsDir.txt
cp -urf %str% .\assets\
cp -rf ../../../feiliuRes/* .\assets\

