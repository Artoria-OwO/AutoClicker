@echo off
chcp 65001 >nul
setlocal

echo ===== Windows 连点工具 GUI 编译 =====
echo.

set "VSDIR=C:\Program Files\Microsoft Visual Studio\2022\Community"
set "VSDEVCMD=%VSDIR%\Common7\Tools\VsDevCmd.bat"

if not exist "%VSDEVCMD%" (
    echo 错误: 未找到 VsDevCmd.bat
    echo 路径: %VSDEVCMD%
    echo 请确认已安装 Visual Studio 2022 Community
    pause
    exit /b 1
)

echo 正在初始化 MSVC 环境...
call "%VSDEVCMD%" -arch=amd64
if %ERRORLEVEL% NEQ 0 (
    echo VsDevCmd 初始化失败
    pause
    exit /b 1
)

echo.
echo 正在编译 autoclicker_gui...
echo.

cl /nologo /EHsc /O2 /W3 /utf-8 /Fe:autoclicker_gui.exe autoclicker_gui.cpp /link /SUBSYSTEM:WINDOWS /MANIFEST:EMBED /MANIFESTINPUT:app.manifest /MANIFESTUAC:"level='requireAdministrator' uiAccess='false'" user32.lib gdi32.lib comctl32.lib dwmapi.lib shell32.lib advapi32.lib
if %ERRORLEVEL% NEQ 0 (
    echo 编译失败
    del resource.res 2>nul
    pause
    exit /b 1
)

del autoclicker_gui.obj 2>nul

echo.
echo ========================================
echo 编译成功!
echo 可执行文件: autoclicker_gui.exe
echo ========================================
echo.
pause
