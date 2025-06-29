@echo off
echo Removing Volt Editor Context Menu...

REM Remove context menu entries
reg delete "HKEY_CLASSES_ROOT\*\shell\OpenWithVolt" /f >nul 2>&1
reg delete "HKEY_CLASSES_ROOT\Directory\shell\OpenWithVolt" /f >nul 2>&1
reg delete "HKEY_CLASSES_ROOT\Directory\Background\shell\OpenWithVolt" /f >nul 2>&1

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ✅ Context menu removed successfully!
    echo.
    echo "Open with Volt Editor" has been removed from all context menus.
    echo.
) else (
    echo.
    echo ❌ Removal failed! Please run this script as Administrator.
    echo.
    echo Right-click on uninstall.bat and select "Run as administrator"
    echo.
)

pause
