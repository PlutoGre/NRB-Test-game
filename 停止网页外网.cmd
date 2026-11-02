@echo off
chcp 65001 >nul
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0deployment\Start-Web.ps1" -External -Action stop %*
set "web_result=%errorlevel%"
if not "%web_result%"=="0" echo External Web stop failed. Read the message above.
pause
exit /b %web_result%
