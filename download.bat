@echo off
setlocal enabledelayedexpansion

echo ============================================
echo  qBittorrent - Download Build Artifact
echo ============================================
echo.

gh --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] GitHub CLI not found. Run upload_and_build.bat first.
    pause & exit /b 1
)

:: Show recent runs
echo Checking latest build status...
echo.
gh run list --workflow=build-release.yml --limit=5
echo.

:: Get latest completed run ID
for /f "tokens=*" %%i in ('gh run list --workflow=build-release.yml --status=completed --limit=1 --json databaseId --jq ".[0].databaseId" 2^>nul') do (
    set RUN_ID=%%i
)

if "!RUN_ID!"=="" (
    echo [INFO] No completed build found yet.
    echo  Wait for the build to finish, then run this script again.
    echo.
    echo  Watch live progress:
    echo    gh run watch
    pause & exit /b 1
)

echo Latest completed run ID: !RUN_ID!
echo.

:: Download
if not exist "output" mkdir output

echo Downloading...
gh run download !RUN_ID! --name qBittorrent-5.1.4-custom-windows-x64 --dir output\qBittorrent

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Download failed. Check if build succeeded:
    echo   gh run view !RUN_ID!
    pause & exit /b 1
)

echo.
echo ============================================
echo  Download complete!
echo  Location: %cd%\output\qBittorrent\
echo ============================================
echo.
explorer output\qBittorrent
pause
