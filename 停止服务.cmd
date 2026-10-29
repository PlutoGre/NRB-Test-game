@echo off
setlocal
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0deployment\Deploy-Server.ps1" -Action stop %*
set "server_result=%errorlevel%"
if not "%server_result%"=="0" echo Server shutdown failed. Read the message above and runtime logs.
pause
exit /b %server_result%
