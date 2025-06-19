# 🚀 如何运行传感器数据采集系统

## 📋 运行前检查清单

### ✅ 必需软件
- [ ] **IAR EMBEDDED WORKBENCH** 5.3或更高版本已安装
- [ ] **MySQL数据库** 5.7或更高版本已安装并运行
- [ ] **Git** 已安装（用于克隆项目）
- [ ] **串口调试工具** (如PuTTY, 串口助手等)

### ✅ 必需硬件
- [ ] **STM32F103CB开发板** 或兼容型号
- [ ] **调试器** (J-Link, ST-Link等)
- [ ] **USB数据线** 连接开发板
- [ ] **传感器** (可选，用于实际测试)

## 🔧 第一步：环境设置

### 1.1 克隆项目
```bash
git clone https://github.com/xinzaiwuguisu/project-test.git
cd project-test
```

### 1.2 运行自动设置脚本
```bash
# Windows用户
scripts\setup_project.bat

# Linux/Mac用户
chmod +x scripts/setup_project.sh
scripts/setup_project.sh
```

### 1.3 配置MySQL数据库
```sql
-- 1. 启动MySQL服务
-- Windows: 服务管理器中启动MySQL
-- Linux: sudo systemctl start mysql
-- Mac: brew services start mysql

-- 2. 登录MySQL
mysql -u root -p

-- 3. 创建数据库和用户
CREATE DATABASE sensor_data;
CREATE USER 'sensor_user'@'localhost' IDENTIFIED BY 'sensor_pass';
GRANT ALL PRIVILEGES ON sensor_data.* TO 'sensor_user'@'localhost';
FLUSH PRIVILEGES;
USE sensor_data;

-- 4. 创建数据表
SOURCE database/create_tables.sql;

-- 5. 验证表创建
SHOW TABLES;
```

## 🏗️ 第二步：在IAR中打开和编译项目

### 2.1 启动IAR EMBEDDED WORKBENCH
1. 双击桌面上的IAR图标，或从开始菜单启动
2. 等待IAR完全加载

### 2.2 打开项目工作空间
1. 选择菜单 **File** → **Open** → **Workspace**
2. 浏览到项目目录
3. 选择 `project/sensor_system_unified.eww` 文件
4. 点击 **Open**

### 2.3 检查项目配置
1. 在项目树中确认所有文件都正确显示（没有红色文件）
2. 右键项目名称，选择 **Options**
3. 检查以下关键设置：

**General Options:**
- Target: STM32F103CB
- Output directories: Debug\Exe

**C/C++ Compiler:**
- Preprocessor → Defined symbols: 应包含 `DEBUG`, `STM32F103xB`
- Preprocessor → Include directories: 应包含 `$PROJ_DIR$\..\include`

### 2.4 编译项目
1. 选择 **Debug** 配置（在工具栏下拉菜单中）
2. 按 **F7** 键或选择 **Project** → **Rebuild All**
3. 查看编译输出窗口，确保没有错误
4. 成功编译后，应该在 `project/Debug/Exe/` 目录下看到 `sensor_system.out` 文件

## 🔌 第三步：硬件连接

### 3.1 连接STM32开发板
```
开发板连接：
┌─────────────────┐    USB线    ┌─────────────┐
│   STM32F103CB   │ ←────────→ │   电脑USB   │
│     开发板      │             │    接口     │
└─────────────────┘             └─────────────┘
```

### 3.2 连接调试器（如果需要）
```
调试器连接：
┌─────────────────┐   SWD/JTAG   ┌─────────────┐
│   STM32F103CB   │ ←─────────→ │   J-Link/   │
│     开发板      │              │   ST-Link   │
└─────────────────┘              └─────────────┘
```

### 3.3 连接传感器（可选）
```
传感器连接示例：
温湿度传感器:
- VCC → 3.3V
- GND → GND  
- DATA → PA1 (UART RX)

中断传感器:
- VCC → 3.3V
- GND → GND
- SIGNAL → PA2 (GPIO)
```

## 🚀 第四步：下载和运行程序

### 4.1 在IAR中下载程序
1. 确保开发板已连接并被识别
2. 在IAR中按 **Ctrl+D** 开始调试会话
3. IAR会自动下载程序到开发板
4. 下载完成后，程序会自动停在main函数入口

### 4.2 运行程序
1. 按 **F5** 键或点击 **Go** 按钮开始运行
2. 程序开始执行，LED指示灯应该开始闪烁（如果有）

## 📡 第五步：测试系统功能

### 5.1 打开串口调试工具
```
串口设置：
- 波特率: 115200
- 数据位: 8
- 停止位: 1
- 校验位: 无
- 流控制: 无
```

### 5.2 发送测试数据
在串口工具中发送以下测试数据：

**传感器1测试（温湿度）：**
```
2021001ZS,25.6,60.2
2021002LM,23.5,55.8
2021003WH,26.8,65.3
```

**传感器2测试（中断）：**
```
2021001ZS,DOOR_SENSOR,1
2021002LM,MOTION_SENSOR,2
2021003WH,WINDOW_SENSOR,1
```

### 5.3 验证数据库存储
```sql
-- 检查传感器1数据表
SELECT * FROM sensor1_data ORDER BY timestamp DESC LIMIT 5;

-- 检查传感器2数据表  
SELECT * FROM sensor2_data ORDER BY timestamp DESC LIMIT 5;

-- 检查数据统计
SELECT COUNT(*) as total_records FROM sensor1_data;
SELECT COUNT(*) as total_records FROM sensor2_data;
```

## 🔍 第六步：监控和调试

### 6.1 查看系统输出
在串口调试工具中应该能看到：
```
[INFO] 系统初始化完成
[INFO] 数据库连接成功
[INFO] UART通信已启动
[DEBUG] 接收到数据: 2021001ZS,25.6,60.2
[INFO] 传感器1数据解析成功
[INFO] 数据已存储到数据库
```

### 6.2 使用IAR调试功能
1. **设置断点**: 在关键函数处设置断点
2. **变量监视**: 监视重要变量的值
3. **内存查看**: 查看缓冲区内容
4. **单步调试**: 逐行执行代码

### 6.3 性能监控
```c
// 在代码中添加性能监控
uint32_t start_time = get_timestamp();
// 执行数据处理
uint32_t end_time = get_timestamp();
printf("处理时间: %lu ms\n", end_time - start_time);
```

## ⚠️ 常见问题解决

### 问题1: 编译失败
**症状**: IAR显示编译错误
**解决方案**:
1. 检查IAR版本是否为5.3+
2. 确认所有头文件路径正确
3. 查看 [故障排除指南](docs/IAR53_TROUBLESHOOTING.md)

### 问题2: 无法下载程序
**症状**: IAR提示下载失败
**解决方案**:
1. 检查USB连接
2. 确认调试器驱动已安装
3. 重新选择目标设备

### 问题3: 串口无数据
**症状**: 串口调试工具收不到数据
**解决方案**:
1. 检查串口号和波特率
2. 确认USB转串口驱动已安装
3. 验证硬件连接

### 问题4: 数据库连接失败
**症状**: 程序提示数据库连接错误
**解决方案**:
1. 确认MySQL服务正在运行
2. 检查用户名密码是否正确
3. 验证数据库和表是否已创建

## 🎯 高级运行选项

### 选项1: 批量测试模式
```bash
# 运行自动化测试脚本
scripts/run_tests.bat  # Windows
scripts/run_tests.sh   # Linux/Mac
```

### 选项2: 性能测试模式
```c
// 在config.h中启用性能测试
#define ENABLE_PERFORMANCE_TEST 1
#define TEST_DATA_COUNT 1000
```

### 选项3: 调试模式
```c
// 在config.h中启用详细调试
#define DEBUG_LEVEL 3
#define ENABLE_VERBOSE_OUTPUT 1
```

## 📊 运行状态检查

### 系统正常运行的标志：
- ✅ LED指示灯正常闪烁
- ✅ 串口有调试输出
- ✅ 数据库中有新记录
- ✅ 无错误提示信息

### 性能指标：
- 📈 数据处理延迟 < 1ms
- 📈 内存使用 < 2KB
- 📈 CPU使用率 < 50%
- 📈 数据库写入成功率 > 99%

## 🔄 停止和重启

### 正常停止：
1. 在IAR中按 **Ctrl+Shift+D** 停止调试
2. 断开USB连接
3. 关闭串口调试工具

### 重启系统：
1. 重新连接硬件
2. 在IAR中重新开始调试会话
3. 按F5运行程序

## 📞 获取帮助

如果遇到问题：
1. 📖 查看 [快速开始指南](QUICK_START.md)
2. 🔧 参考 [故障排除文档](docs/IAR53_TROUBLESHOOTING.md)
3. 💡 查看 [示例代码](examples/sensor_data_examples.c)
4. 🐛 在GitHub提交Issue: https://github.com/xinzaiwuguisu/project-test/issues

---

**提示**: 首次运行建议按照本指南逐步操作，确保每一步都成功完成后再进行下一步。

**最后更新**: 2025-06-19