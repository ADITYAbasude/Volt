@echo off
echo Installing Volt Editor Context Menu...

REM Get the directory where this batch file is located
set VOLT_PATH=%~dp0Volt.exe

REM Check if Volt.exe exists in the current directory
if not exist "%VOLT_PATH%" (
    echo Error: Volt.exe not found in current directory!
    echo Please place this install.bat file in the same folder as Volt.exe
    pause
    exit /b 1
)

echo Found Volt.exe at: %VOLT_PATH%
echo Adding registry entries...

REM Add context menu for files
reg add "HKEY_CLASSES_ROOT\*\shell\OpenWithVolt" /ve /d "Open with Volt Editor" /f >nul 2>&1
reg add "HKEY_CLASSES_ROOT\*\shell\OpenWithVolt" /v "Icon" /d "\"%VOLT_PATH%\",0" /f >nul 2>&1
reg add "HKEY_CLASSES_ROOT\*\shell\OpenWithVolt\command" /ve /d "\"\"%VOLT_PATH%\"\" \"\"%%1\"\"" /f >nul 2>&1

REM Add context menu for folders
reg add "HKEY_CLASSES_ROOT\Directory\shell\OpenWithVolt" /ve /d "Open folder with Volt Editor" /f >nul 2>&1
reg add "HKEY_CLASSES_ROOT\Directory\shell\OpenWithVolt" /v "Icon" /d "\"%VOLT_PATH%\",0" /f >nul 2>&1
reg add "HKEY_CLASSES_ROOT\Directory\shell\OpenWithVolt\command" /ve /d "\"\"%VOLT_PATH%\"\" \"\"%%1\"\"" /f >nul 2>&1

REM Add context menu for folder background
reg add "HKEY_CLASSES_ROOT\Directory\Background\shell\OpenWithVolt" /ve /d "Open with Volt Editor" /f >nul 2>&1
reg add "HKEY_CLASSES_ROOT\Directory\Background\shell\OpenWithVolt" /v "Icon" /d "\"%VOLT_PATH%\",0" /f >nul 2>&1
reg add "HKEY_CLASSES_ROOT\Directory\Background\shell\OpenWithVolt\command" /ve /d "\"\"%VOLT_PATH%\"\" \"\"%%V\"\"" /f >nul 2>&1

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ✅ Context menu installed successfully!
    echo.
    echo You can now:
    echo • Right-click on any file → "Open with Volt Editor"
    echo • Right-click on any folder → "Open folder with Volt Editor" 
    echo • Right-click in empty space within a folder → "Open with Volt Editor"
    echo.
) else (
    echo.
    echo ❌ Installation failed! Please run this script as Administrator.
    echo.
    echo Right-click on install.bat and select "Run as administrator"
    echo.
)

pause
