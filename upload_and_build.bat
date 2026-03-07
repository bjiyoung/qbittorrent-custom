@echo off
setlocal enabledelayedexpansion

echo ============================================
echo  qBittorrent Custom Build - Upload + Build
echo ============================================
echo.

cd /d "%~dp0"
echo Working directory: %cd%
echo.

:: Check Git
git --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] Git not found. Download: https://git-scm.com/download/win
    pause & exit /b 1
)

:: Check GitHub CLI
gh --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [INFO] Installing GitHub CLI...
    winget install GitHub.cli
    set "PATH=%PATH%;C:\Program Files\GitHub CLI"
    gh --version >nul 2>&1
    if %errorlevel% neq 0 (
        echo [ERROR] Install failed. Download manually: https://cli.github.com
        pause & exit /b 1
    )
)

:: Check GitHub login
gh auth status >nul 2>&1
if %errorlevel% neq 0 (
    echo [INFO] GitHub login required.
    gh auth login
    gh auth status >nul 2>&1
    if %errorlevel% neq 0 (
        echo [ERROR] Login failed.
        pause & exit /b 1
    )
)
echo [OK] GitHub login confirmed.
echo.

:: Check source root
if not exist "CMakeLists.txt" (
    echo [ERROR] CMakeLists.txt not found in: %cd%
    echo  This bat file must be inside the qbittorrent source folder.
    pause & exit /b 1
)

:: Fix line ending issues
git config core.autocrlf false
git config core.eol lf

:: -- Step 1: Commit changes ------------------
echo [1/4] Checking for changes...
echo.

if not exist ".git" (
    echo  -> Initializing new repository
    git init
    git add -A
    git commit -m "qBittorrent 5.1.4 with custom patches"
) else (
    git add -A

    set "CHANGED="
    for /f %%i in ('git status --porcelain') do set CHANGED=1

    if defined CHANGED (
        echo  -> Changed files:
        git status --short
        echo.

        set "BUILD=0"
        for /f "tokens=3" %%a in ('findstr /r "^#define QBT_VERSION_BUILD" src\base\version.h.in') do set "BUILD=%%a"
        set "MSG=Update custom patches v5.1.4.!BUILD!"
        echo  -> Committing: !MSG!
        git commit -m "!MSG!"
        echo [OK] Committed.
    ) else (
        echo  -> No changes detected. Pushing current commit as-is.
        echo.
        git log --oneline -3
    )
)
echo.

:: -- Step 2: Push to GitHub ------------------
echo [2/4] Pushing to GitHub...
git remote get-url origin >nul 2>&1
if %errorlevel% neq 0 (
    echo  -> Creating new repository...
    gh repo create qbittorrent-custom --public --source=. --push
    if %errorlevel% neq 0 (
        echo [ERROR] Failed to create repository.
        echo  Try deleting existing repo: gh repo delete qbittorrent-custom --yes
        pause & exit /b 1
    )
) else (
    git push origin master
    if %errorlevel% neq 0 (
        echo  -> Retrying with force push...
        git push --force origin master
        if %errorlevel% neq 0 (
            echo [ERROR] Push failed.
            pause & exit /b 1
        )
    )
)
echo [OK] Push complete.
echo.

:: -- Step 3: Done ----------------------------
echo [3/4] Done! Recent commits:
git log --oneline -3
echo.
echo ============================================
echo  Build started! (approx 30-50 minutes)
echo ============================================
echo.
echo  Check build status:
echo    gh run list --workflow=build-release.yml
echo.
echo  After build completes, run:
echo    download.bat
echo.
gh repo view --web
echo.
pause
