@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Build-Android.ps1" %*
set "android_result=%errorlevel%"
if not "%android_result%"=="0" echo Android packaging failed. Read the error above.
pause
exit /b %android_result%
