#!/bin/bash

# 编译测试程序脚本
# 用于在Linux环境下编译和测试代码基本功能

echo "=== 编译传感器数据采集系统测试程序 ==="

# 创建构建目录
mkdir -p build

# 编译参数
CC=gcc
CFLAGS="-Wall -Wextra -std=c99 -g -DTEST_MODE=1"
INCLUDES="-I../src"
SOURCES="../src/sensor_data.c ../src/database.c ../src/communication.c test_main.c"
OUTPUT="build/test_sensor_system"

# 添加模拟的嵌入式函数定义
cat > build/embedded_stubs.c << 'EOF'
#include <stdint.h>

/* 模拟嵌入式系统函数 */
void __disable_irq(void) {
    /* 模拟禁用中断 */
}

void __enable_irq(void) {
    /* 模拟启用中断 */
}
EOF

# 编译程序
echo "正在编译..."
$CC $CFLAGS $INCLUDES $SOURCES build/embedded_stubs.c -o $OUTPUT

if [ $? -eq 0 ]; then
    echo "编译成功！"
    echo "可执行文件: $OUTPUT"
    
    # 运行测试
    echo ""
    echo "=== 运行测试程序 ==="
    ./$OUTPUT
    
    echo ""
    echo "=== 测试完成 ==="
else
    echo "编译失败！"
    exit 1
fi