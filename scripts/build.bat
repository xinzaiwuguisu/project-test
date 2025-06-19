@echo off
setlocal

echo ========================================
echo   传感器数据采集系统 - 自动构建
echo ========================================
echo.

REM 设置IAR路径（请根据实际安装路径修改）
set IAR_PATH="C:\Program Files (x86)\IAR Systems\Embedded Workbench 5.3\arm\bin"
set PROJECT_PATH=%~dp0..\project\sensor_system_unified.ewp

REM 检查IAR是否存在
if not exist %IAR_PATH%\iarbuild.exe (
    echo 错误：找不到IAR构建工具
    echo 请检查IAR安装路径：%IAR_PATH%
    echo 或手动修改此脚本中的IAR_PATH变量
    pause
    exit /b 1
)

echo 找到IAR构建工具：%IAR_PATH%\iarbuild.exe
echo 项目文件：%PROJECT_PATH%
echo.

REM 构建Debug版本
echo 正在构建Debug版本...
%IAR_PATH%\iarbuild.exe %PROJECT_PATH% -build Debug
if %ERRORLEVEL% neq 0 (
    echo Debug构建失败！
    pause
    exit /b 1
)
echo Debug构建成功！
echo.

REM 构建Release版本
echo 正在构建Release版本...
%IAR_PATH%\iarbuild.exe %PROJECT_PATH% -build Release
if %ERRORLEVEL% neq 0 (
    echo Release构建失败！
    pause
    exit /b 1
)
echo Release构建成功！
echo.

echo ========================================
echo   构建完成！
echo ========================================
echo.
echo 输出文件位置：
echo - Debug版本：  project\Debug\Exe\sensor_system.out
echo - Release版本：project\Release\Exe\sensor_system.out
echo.

pause