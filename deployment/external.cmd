@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Start-External.ps1" %*
set "external_result=%errorlevel%"
if not "%external_result%"=="0" echo External access failed. Read the message above and runtime logs.
pause
exit /b %external_result%
