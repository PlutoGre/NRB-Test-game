
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
mkdir .\src\com\youai\dreamonepiece\platform\qihoo360

echo -----mkdir src-----
mkdir .\src\com\youai\dreamonepiece\platform\qihoo360\appserver

echo -----mkdir src-----
mkdir .\src\com\youai\dreamonepiece\platform\qihoo360\common

echo -----mkdir src-----
mkdir .\src\com\youai\dreamonepiece\platform\qihoo360\utils

echo -----copy qihoo360PlatformSDKState.java-----
cp -uf .\qihoo360PlatformSDKState.java .\src\com\youai\dreamonepiece\PlatformSDKState.java

echo -----rm armeabi/v7a-----
rm -rf .\libs

echo -----mkdir armeabi/v7a-----
mkdir .\libs\armeabi
mkdir .\libs\armeabi-v7a
