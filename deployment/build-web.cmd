@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Build-Web.ps1" %*
set "web_result=%errorlevel%"
if not "%web_result%"=="0" echo Web packaging failed. Read the error above.
pause
exit /b %web_result%
