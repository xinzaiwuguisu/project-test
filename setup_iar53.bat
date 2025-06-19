@echo off
echo Setting up IAR 5.3 project directories...

REM Create Debug directories
if not exist "project\Debug" mkdir "project\Debug"
if not exist "project\Debug\Exe" mkdir "project\Debug\Exe"
if not exist "project\Debug\Obj" mkdir "project\Debug\Obj"
if not exist "project\Debug\List" mkdir "project\Debug\List"

REM Create Release directories
if not exist "project\Release" mkdir "project\Release"
if not exist "project\Release\Exe" mkdir "project\Release\Exe"
if not exist "project\Release\Obj" mkdir "project\Release\Obj"
if not exist "project\Release\List" mkdir "project\Release\List"

echo Directory structure created successfully!
echo.
echo Available IAR 5.3 project files:
echo 1. minimal_iar53.eww          - Minimal configuration (recommended for first try)
echo 2. sensor_system_iar53_fixed.eww - Full featured project
echo 3. sensor_system_v53.eww      - Original project file
echo.
echo To start:
echo 1. Open IAR EMBEDDED WORKBENCH 5.3
echo 2. Open one of the .eww files above
echo 3. Try to compile (F7)
echo.
echo If you encounter issues, check docs\IAR53_TROUBLESHOOTING.md
pause