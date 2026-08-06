@echo off
cd /d "%~dp0"
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\release\New-DarkEdenRelease.ps1" %*
if errorlevel 1 pause
