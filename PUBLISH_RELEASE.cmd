@echo off
cd /d "%~dp0"
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\release\Publish-DarkEdenRelease.ps1" %*
if errorlevel 1 pause
