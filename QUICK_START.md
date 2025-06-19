# 🚀 快速开始指南

## ⚡ 30秒快速启动

```bash
# 1. 克隆项目
git clone https://github.com/xinzaiwuguisu/project-test.git
cd project-test

# 2. 运行设置脚本
scripts/setup_project.bat  # Windows
# 或
scripts/setup_project.sh   # Linux/Mac

# 3. 打开IAR项目
# 启动 IAR EMBEDDED WORKBENCH
# 打开 project/sensor_system_unified.eww

# 4. 编译运行
# 按 F7 编译项目
```

## 📋 系统要求

### 必需软件
- ✅ **IAR EMBEDDED WORKBENCH** 5.3或更高版本
- ✅ **MySQL** 5.7或更高版本
- ✅ **Git** 版本控制工具

### 必需硬件
- ✅ **STM32F103CB** 微控制器开发板
- ✅ **调试器** (J-Link, ST-Link等)
- ✅ **串口连接** (USB转串口或板载串口)

## 🔧 详细设置步骤

### 第一步：环境准备

#### 1.1 安装IAR EMBEDDED WORKBENCH
- 下载并安装IAR EMBEDDED WORKBENCH for ARM
- 确保版本为5.3或更高
- 激活许可证

#### 1.2 安装MySQL数据库
```bash
# Windows (使用MySQL Installer)
# 下载：https://dev.mysql.com/downloads/installer/

# Linux (Ubuntu/Debian)
sudo apt update
sudo apt install mysql-server

# macOS (使用Homebrew)
brew install mysql
```

#### 1.3 配置MySQL
```sql
-- 创建数据库
CREATE DATABASE sensor_data;

-- 创建用户
CREATE USER 'sensor_user'@'localhost' IDENTIFIED BY 'sensor_pass';
GRANT ALL PRIVILEGES ON sensor_data.* TO 'sensor_user'@'localhost';
FLUSH PRIVILEGES;
```

### 第二步：项目设置

#### 2.1 克隆项目
```bash
git clone https://github.com/xinzaiwuguisu/project-test.git
cd project-test
```

#### 2.2 运行自动设置
```bash
# Windows
scripts\setup_project.bat

# Linux/Mac
chmod +x scripts/setup_project.sh
scripts/setup_project.sh
```

#### 2.3 创建数据库表
```bash
mysql -u sensor_user -p sensor_data < database/create_tables.sql
```

### 第三步：IAR项目配置

#### 3.1 打开项目
1. 启动 **IAR EMBEDDED WORKBENCH**
2. 选择 **File** → **Open** → **Workspace**
3. 浏览到项目目录，选择 `project/sensor_system_unified.eww`
4. 点击 **Open**

#### 3.2 验证项目配置
1. 检查项目树中所有文件是否正确显示
2. 右键项目名称，选择 **Options**
3. 验证以下设置：

**General Options:**
- Target: STM32F103CB
- Output directories: Debug\Exe, Debug\Obj, Debug\List

**C/C++ Compiler:**
- Preprocessor → Defined symbols: DEBUG, STM32F103xB
- Preprocessor → Include directories: $PROJ_DIR$\..\include

#### 3.3 编译项目
1. 选择 **Debug** 配置
2. 按 **F7** 或选择 **Project** → **Rebuild All**
3. 检查编译输出，确保无错误

### 第四步：硬件连接

#### 4.1 连接开发板
1. 使用USB线连接STM32开发板到电脑
2. 连接调试器（如果使用外部调试器）
3. 确保驱动程序已正确安装

#### 4.2 连接传感器
```
传感器1 (温湿度):
- VCC → 3.3V
- GND → GND
- DATA → PA1 (UART RX)

传感器2 (中断):
- VCC → 3.3V
- GND → GND
- SIGNAL → PA2 (GPIO)
```

### 第五步：下载和测试

#### 5.1 下载程序
1. 在IAR中按 **Ctrl+D** 开始调试会话
2. 程序将自动下载到开发板
3. 按 **F5** 开始运行

#### 5.2 测试功能
1. 打开串口调试工具（波特率115200）
2. 发送测试数据：
   ```
   2021001ZS,25.6,60.2
   2021001ZS,DOOR_SENSOR,1
   ```
3. 检查数据库中是否有新记录

## 🔍 故障排除

### 常见问题

#### ❌ 编译错误
**问题**: 找不到头文件或编译失败
**解决方案**:
1. 检查包含路径设置
2. 确认IAR版本兼容性
3. 查看 [详细故障排除指南](docs/IAR53_TROUBLESHOOTING.md)

#### ❌ 数据库连接失败
**问题**: 无法连接到MySQL数据库
**解决方案**:
1. 检查MySQL服务是否运行
2. 验证用户名和密码
3. 确认数据库和表已创建

#### ❌ 串口通信问题
**问题**: 无法接收传感器数据
**解决方案**:
1. 检查串口连接和波特率
2. 验证硬件连接
3. 使用串口调试工具测试

#### ❌ 下载失败
**问题**: 无法下载程序到开发板
**解决方案**:
1. 检查调试器连接
2. 确认目标芯片型号
3. 重新安装调试器驱动

## 📚 进阶使用

### 自定义配置
编辑 `include/config.h` 文件修改系统参数：
```c
#define UART_BAUD_RATE          115200    // 串口波特率
#define RX_BUFFER_SIZE          256       // 接收缓冲区大小
#define DB_HOST                 "localhost" // 数据库主机
```

### 添加新传感器
1. 在 `sensor_type_t` 枚举中添加新类型
2. 实现对应的解析函数
3. 更新数据库表结构

### 性能优化
1. 调整缓冲区大小
2. 优化数据库查询
3. 使用中断驱动的通信

## 📞 获取帮助

### 文档资源
- 📖 [项目总览](PROJECT_OVERVIEW.md) - 完整的项目架构说明
- 🔧 [故障排除指南](docs/IAR53_TROUBLESHOOTING.md) - 详细的问题解决方案
- 💡 [示例代码](examples/sensor_data_examples.c) - 完整的使用示例
- 📋 [文件清单](MANIFEST.md) - 所有文件的详细说明

### 技术支持
- 🐛 **问题反馈**: [GitHub Issues](https://github.com/xinzaiwuguisu/project-test/issues)
- 💬 **讨论交流**: [GitHub Discussions](https://github.com/xinzaiwuguisu/project-test/discussions)
- 📧 **直接联系**: 通过GitHub私信

### 社区资源
- 🌟 **项目主页**: https://github.com/xinzaiwuguisu/project-test
- 📦 **版本发布**: https://github.com/xinzaiwuguisu/project-test/releases
- 🔄 **更新日志**: 查看 [VERSION](VERSION) 文件

---

**提示**: 如果这是您第一次使用，建议先阅读 [项目总览](PROJECT_OVERVIEW.md) 了解整体架构，然后按照本指南逐步操作。

**最后更新**: 2025-06-19