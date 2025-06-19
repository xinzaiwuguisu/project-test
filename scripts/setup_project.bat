@echo off
echo ========================================
echo   传感器数据采集系统 - 项目设置
echo ========================================
echo.

echo 正在创建项目目录结构...

REM 创建Debug目录
if not exist "project\Debug" mkdir "project\Debug"
if not exist "project\Debug\Exe" mkdir "project\Debug\Exe"
if not exist "project\Debug\Obj" mkdir "project\Debug\Obj"
if not exist "project\Debug\List" mkdir "project\Debug\List"

REM 创建Release目录
if not exist "project\Release" mkdir "project\Release"
if not exist "project\Release\Exe" mkdir "project\Release\Exe"
if not exist "project\Release\Obj" mkdir "project\Release\Obj"
if not exist "project\Release\List" mkdir "project\Release\List"

REM 创建其他必要目录
if not exist "logs" mkdir "logs"
if not exist "backup" mkdir "backup"

echo 目录结构创建完成！
echo.

echo 项目信息：
echo - 项目名称：传感器数据采集系统
echo - IAR版本：5.3+ 兼容
echo - 目标硬件：STM32F103CB
echo - 编译配置：Debug / Release
echo.

echo 使用说明：
echo 1. 打开 IAR EMBEDDED WORKBENCH
echo 2. 打开工作空间文件：project\sensor_system_unified.eww
echo 3. 选择 Debug 或 Release 配置
echo 4. 按 F7 编译项目
echo.

echo 项目结构：
echo - src\          源代码文件
echo - include\      头文件
echo - project\      IAR项目文件
echo - database\     数据库脚本
echo - docs\         文档
echo - scripts\      构建脚本
echo.

echo 如需帮助，请查看 README.md 或 docs\ 目录下的文档
echo.

pause