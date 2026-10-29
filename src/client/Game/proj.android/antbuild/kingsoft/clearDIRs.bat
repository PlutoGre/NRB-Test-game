
echo -----rm assets-----
rm -rf .\assets

echo -----mkdir assets-----
mkdir .\assets

echo -----rm res-----
rm -rf .\res

echo -----mkdir res-----
mkdir .\res

echo -----rm src-----
rm -rf .\src

echo -----mkdir src-----
mkdir .\src\com\youai\dreamonepiece\platform\kingsoft

echo -----copy jinshanPlatformSDKState.java-----
cp -uf .\jinshanPlatformSDKState.java .\src\com\youai\dreamonepiece\PlatformSDKState.java

echo -----rm armeabi/v7a-----
rm -rf .\libs\armeabi
rm -rf .\libs\armeabi-v7a

echo -----mkdir armeabi/v7a-----
mkdir .\libs\armeabi
mkdir .\libs\armeabi-v7a
