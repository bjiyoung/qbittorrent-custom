@echo off
setlocal enabledelayedexpansion

echo ============================================
echo  qBittorrent Custom Build - GitHub Upload
echo ============================================
echo.

:: -- Move to the folder where this .bat file lives --
cd /d "%~dp0"
echo Working directory: %cd%
echo.

:: -- Check Git -------------------------------
git --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] Git is not installed.
    echo  Download: https://git-scm.com/download/win
    pause & exit /b 1
)

:: -- Check GitHub CLI ------------------------
gh --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [INFO] GitHub CLI not found. Installing...
    winget install GitHub.cli
    :: Refresh PATH without restarting
    set "PATH=%PATH%;C:\Program Files\GitHub CLI"
    gh --version >nul 2>&1
    if %errorlevel% neq 0 (
        echo [ERROR] Install failed. Download manually: https://cli.github.com
        pause & exit /b 1
    )
)

:: -- Check GitHub login ----------------------
gh auth status >nul 2>&1
if %errorlevel% neq 0 (
    echo [INFO] GitHub login required.
    gh auth login
    :: gh auth login may return non-zero even on success, so re-check
    gh auth status >nul 2>&1
    if %errorlevel% neq 0 (
        echo [ERROR] Login failed. Please try again.
        pause & exit /b 1
    )
)
echo [OK] GitHub login confirmed.
echo.

:: -- Check we are in source root -------------
if not exist "CMakeLists.txt" (
    echo [ERROR] CMakeLists.txt not found in: %cd%
    echo  Make sure this .bat file is inside the qbittorrent-5.1.4 folder.
    pause & exit /b 1
)

:: -- Git init --------------------------------
echo [1/4] Setting up Git repository...
if not exist ".git" (
    git init
    git add .
    git commit -m "qBittorrent 5.1.4 with custom patches"
) else (
    git add .
    git diff --cached --quiet
    if %errorlevel% neq 0 (
        git commit -m "Update custom patches"
    ) else (
        echo      Nothing to commit.
    )
)
echo.

:: -- Create GitHub repo and push -------------
echo [2/4] Pushing to GitHub...
git remote get-url origin >nul 2>&1
if %errorlevel% neq 0 (
    gh repo create qbittorrent-custom --public --source=. --push
    if %errorlevel% neq 0 (
        echo [ERROR] Failed to create repository.
        echo  A repo named 'qbittorrent-custom' may already exist on your account.
        echo  Delete it first:  gh repo delete qbittorrent-custom --yes
        pause & exit /b 1
    )
) else (
    git push
)
echo.

:: -- Trigger workflow ------------------------
echo [3/4] Starting build workflow...
gh workflow run build-release.yml
if %errorlevel% neq 0 (
    echo [WARNING] Could not trigger workflow automatically.
    echo  The workflow will start automatically on the next push.
)
echo.

:: -- Done ------------------------------------
echo [4/4] Done!
echo.
echo ============================================
echo  Build started! (takes about 30-50 minutes)
echo ============================================
echo.
echo  Check status:
echo    gh run list --workflow=build-release.yml
echo.
echo  After build completes, run:
echo    download.bat
echo.
echo  Open in browser:
gh repo view --web
echo.
pause
