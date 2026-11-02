@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Start-Web.ps1" %*
set "web_result=%errorlevel%"
if not "%web_result%"=="0" echo Web startup failed. Read the error above.
pause
exit /b %web_result%
