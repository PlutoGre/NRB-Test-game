@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Deploy-Server.ps1" %*
set "deploy_result=%errorlevel%"
if not "%deploy_result%"=="0" echo Deployment failed. Read the message above and deployment logs.
pause
exit /b %deploy_result%
