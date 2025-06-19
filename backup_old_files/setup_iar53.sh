#!/bin/bash

echo "Setting up IAR 5.3 project directories..."

# Create Debug directories
mkdir -p "project/Debug/Exe"
mkdir -p "project/Debug/Obj"
mkdir -p "project/Debug/List"

# Create Release directories
mkdir -p "project/Release/Exe"
mkdir -p "project/Release/Obj"
mkdir -p "project/Release/List"

echo "Directory structure created successfully!"
echo ""
echo "Available IAR 5.3 project files:"
echo "1. minimal_iar53.eww          - Minimal configuration (recommended for first try)"
echo "2. sensor_system_iar53_fixed.eww - Full featured project"
echo "3. sensor_system_v53.eww      - Original project file"
echo ""
echo "To start:"
echo "1. Open IAR EMBEDDED WORKBENCH 5.3"
echo "2. Open one of the .eww files above"
echo "3. Try to compile (F7)"
echo ""
echo "If you encounter issues, check docs/IAR53_TROUBLESHOOTING.md"