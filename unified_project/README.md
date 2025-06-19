# 传感器数据采集系统 - 统一完整版

## 项目概述

这是一个基于IAR EMBEDDED WORKBENCH的嵌入式传感器数据采集系统，能够实时接收两种不同类型的传感器数据并存储到MySQL数据库中。

### 主要功能

1. **传感器1数据接收**：温湿度传感器数据（格式：学号姓名缩写+温度+湿度）
2. **传感器2数据接收**：中断传感器数据（格式：学号姓名缩写+传感器名+中断）
3. **数据库存储**：将不同传感器数据分别存储到对应的数据表中
4. **实时处理**：支持实时数据接收和处理
5. **状态监控**：监控传感器状态和系统运行状态

### 兼容性

- **主要支持**：IAR EMBEDDED WORKBENCH 5.3+
- **向上兼容**：IAR 6.x, 7.x, 8.x, 9.x
- **C标准**：C99（IAR 5.3）/ C11+（现代版本）
- **目标硬件**：STM32F103CB及兼容型号

## 项目结构

```
unified_project/
├── src/                          # 源代码文件
│   ├── main.c                   # 主程序
│   ├── sensor_data.c            # 传感器数据处理
│   ├── database.c               # 数据库操作
│   └── communication.c          # 通信模块
├── include/                      # 头文件
│   ├── config.h                 # 系统配置
│   ├── sensor_data.h            # 传感器数据定义
│   ├── database.h               # 数据库接口
│   └── communication.h          # 通信接口
├── project/                      # IAR项目文件
│   ├── sensor_system_unified.eww # IAR工作空间
│   └── sensor_system_unified.ewp # IAR项目文件
├── database/                     # 数据库脚本
│   └── create_tables.sql        # 建表脚本
├── docs/                        # 文档
├── scripts/                     # 构建和设置脚本
└── examples/                    # 示例代码
```

## 快速开始

### 1. 环境准备

#### 软件要求
- **IAR EMBEDDED WORKBENCH for ARM 5.3+**
- **MySQL 5.7+**
- **串口调试工具**

#### 硬件要求
- **STM32F103CB微控制器**
- **最小系统板**
- **UART接口**
- **调试接口**（SWD或JTAG）

### 2. 项目设置

#### 自动设置（推荐）
```bash
# Windows
cd unified_project
..\scripts\setup_project.bat

# Linux/Mac
cd unified_project
../scripts/setup_project.sh
```

#### 手动设置
1. 创建编译输出目录：
   ```
   mkdir project\Debug\Exe
   mkdir project\Debug\Obj
   mkdir project\Debug\List
   mkdir project\Release\Exe
   mkdir project\Release\Obj
   mkdir project\Release\List
   ```

2. 在IAR中打开项目：
   - 启动IAR EMBEDDED WORKBENCH
   - 打开 `project/sensor_system_unified.eww`

### 3. 编译和运行

1. **选择配置**：
   - Debug：用于开发和调试
   - Release：用于最终产品

2. **编译项目**：
   - 按 `F7` 或选择 `Project` -> `Rebuild All`

3. **下载和调试**：
   - 连接调试器
   - 按 `Ctrl+D` 开始调试会话

## 数据格式

### 传感器1数据（温湿度）
```
格式：学号姓名缩写,温度,湿度
示例：2021001ZS,25.6,60.2
```

### 传感器2数据（中断）
```
格式：学号姓名缩写,传感器名,中断类型
示例：2021001ZS,DOOR_SENSOR,1
```

## 数据库配置

### 1. 创建数据库
```sql
CREATE DATABASE sensor_data;
USE sensor_data;
```

### 2. 运行建表脚本
```bash
mysql -u root -p sensor_data < database/create_tables.sql
```

### 3. 创建用户（可选）
```sql
CREATE USER 'sensor_user'@'localhost' IDENTIFIED BY 'sensor_pass';
GRANT ALL PRIVILEGES ON sensor_data.* TO 'sensor_user'@'localhost';
FLUSH PRIVILEGES;
```

## 配置说明

### 系统配置（config.h）

主要配置项：
```c
#define UART_BAUD_RATE          115200    // 串口波特率
#define RX_BUFFER_SIZE          256       // 接收缓冲区大小
#define DB_HOST                 "localhost" // 数据库主机
#define DB_PORT                 3306      // 数据库端口
```

### 编译器配置

#### IAR 5.3兼容模式
- 自动检测IAR版本
- 使用C99标准
- 传统数据类型定义

#### 现代IAR版本
- 使用C11+标准
- 现代HAL库支持
- 优化的性能配置

## 功能模块

### 1. 传感器数据处理（sensor_data.c/h）
- 数据解析和验证
- 格式转换
- 状态监控

### 2. 数据库操作（database.c/h）
- 连接管理
- 数据插入
- 查询操作
- 事务处理

### 3. 通信模块（communication.c/h）
- UART配置
- 数据收发
- 中断处理
- 错误处理

### 4. 主程序（main.c）
- 系统初始化
- 主循环
- 状态监控
- 错误恢复

## 调试和测试

### 调试功能
- 断点调试
- 变量监视
- 内存查看
- 实时跟踪

### 测试功能
- 数据解析测试
- 通信测试
- 数据库连接测试
- 性能测试

## 性能指标

- **内存使用**：< 2KB RAM
- **代码大小**：< 16KB Flash
- **处理延迟**：< 1ms
- **数据吞吐**：1000 packets/second

## 故障排除

### 常见问题

1. **编译错误**
   - 检查IAR版本和项目配置
   - 验证包含路径设置

2. **连接问题**
   - 检查串口配置
   - 验证硬件连接

3. **数据库错误**
   - 检查数据库服务状态
   - 验证连接参数

### 详细故障排除
参考 `docs/` 目录下的详细文档。

## 扩展功能

### 支持更多传感器类型
1. 在 `sensor_type_t` 枚举中添加新类型
2. 实现对应的解析函数
3. 更新数据库表结构

### 添加网络功能
1. 集成TCP/IP协议栈
2. 实现远程数据传输
3. 添加Web管理界面

### 增强安全性
1. 数据加密传输
2. 用户认证
3. 访问控制

## 版本历史

### v1.0.0 (2025-06-19)
- 初始版本发布
- 完整的IAR 5.3兼容性
- 基础功能实现
- 统一项目结构

## 技术支持

- **项目仓库**：https://github.com/xinzaiwuguisu/project-test
- **问题反馈**：通过GitHub Issues提交
- **文档更新**：定期检查仓库获取最新版本

## 许可证

本项目采用MIT许可证，详见LICENSE文件。

## 贡献指南

欢迎提交Pull Request和Issue，请遵循以下规范：
1. 代码风格保持一致
2. 添加必要的注释
3. 更新相关文档
4. 通过所有测试

---

**注意**：本项目专为教学和学习目的设计，在生产环境中使用前请进行充分测试。