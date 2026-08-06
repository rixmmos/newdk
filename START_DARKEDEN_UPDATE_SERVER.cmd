@echo off
cd /d "%~dp0"
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\release\Start-DarkEdenUpdateServer.ps1" %*
pause
