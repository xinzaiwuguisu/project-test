# 传感器数据采集系统

## 项目概述

本项目是一个基于IAR EMBEDDED WORKBENCH的嵌入式传感器数据采集系统，能够实时接收两种不同类型的传感器数据并存储到MySQL数据库中。

### 功能特性

1. **传感器1数据接收**：接收温湿度传感器数据（格式：学号姓名缩写+温度+湿度）
2. **传感器2数据接收**：接收中断传感器数据（格式：学号姓名缩写+传感器名+中断）
3. **数据库存储**：将不同传感器数据分别存储到对应的数据表中
4. **实时处理**：支持实时数据接收和处理
5. **状态监控**：监控传感器状态和系统运行状态

## 项目结构

```
project-test/
├── src/                    # 源代码目录
│   ├── main.c             # 主程序文件
│   ├── config.h           # 系统配置文件
│   ├── sensor_data.h      # 传感器数据结构定义
│   ├── sensor_data.c      # 传感器数据处理实现
│   ├── database.h         # 数据库接口定义
│   ├── database.c         # 数据库操作实现
│   ├── communication.h    # 通信接口定义
│   └── communication.c    # 通信模块实现
├── project/               # IAR项目文件
│   ├── sensor_system.ewp  # IAR项目文件
│   └── sensor_system.eww  # IAR工作空间文件
├── database/              # 数据库脚本
│   └── create_tables.sql  # 数据库建表脚本
├── Makefile              # 构建脚本
└── README_SENSOR_SYSTEM.md # 项目说明文档
```

## 数据格式

### 传感器1数据格式（温湿度传感器）
```
学号姓名缩写,温度,湿度
示例：2021001ZS,25.6,60.2
```

### 传感器2数据格式（中断传感器）
```
学号姓名缩写,传感器名,中断类型
示例：2021001ZS,DOOR_SENSOR,1
```

## 数据库表结构

### sensor1_data表（传感器1数据）
| 字段名 | 类型 | 说明 |
|--------|------|------|
| id | INT | 主键ID |
| student_id | VARCHAR(20) | 学号姓名缩写 |
| sensor_name | VARCHAR(16) | 传感器名称 |
| temperature | DECIMAL(5,2) | 温度值 |
| humidity | DECIMAL(5,2) | 湿度值 |
| status | VARCHAR(10) | 传感器状态 |
| timestamp | INT UNSIGNED | 时间戳 |
| created_at | TIMESTAMP | 记录创建时间 |

### sensor2_data表（传感器2数据）
| 字段名 | 类型 | 说明 |
|--------|------|------|
| id | INT | 主键ID |
| student_id | VARCHAR(20) | 学号姓名缩写 |
| sensor_name | VARCHAR(16) | 传感器名称 |
| interrupt_type | TINYINT | 中断类型 |
| interrupt_count | INT UNSIGNED | 中断次数 |
| status | VARCHAR(10) | 传感器状态 |
| timestamp | INT UNSIGNED | 时间戳 |
| created_at | TIMESTAMP | 记录创建时间 |

## 安装和配置

### 1. 环境要求

- IAR EMBEDDED WORKBENCH 8.0或更高版本
- MySQL 5.7或更高版本
- STM32F103系列微控制器（或兼容的ARM Cortex-M3处理器）

### 2. 数据库配置

1. 安装MySQL数据库
2. 执行数据库建表脚本：
```bash
mysql -u root -p < database/create_tables.sql
```

### 3. 项目配置

1. 打开IAR EMBEDDED WORKBENCH
2. 打开工作空间文件：`project/sensor_system.eww`
3. 根据实际硬件配置修改`src/config.h`中的参数
4. 编译并下载到目标硬件

### 4. 硬件连接

- UART接口用于接收传感器数据
- 根据实际需求配置GPIO、定时器等外设

## 使用说明

### 1. 系统启动

系统启动后会自动初始化各个模块：
- 通信模块初始化
- 数据库连接初始化
- 传感器数据处理模块初始化

### 2. 数据接收

系统通过UART接口实时接收传感器数据，支持以下数据包格式：
- 起始标记：0xAA
- 数据内容：传感器数据字符串
- 结束标记：0x55

### 3. 数据处理流程

1. 接收原始数据包
2. 解析数据包格式
3. 根据数据格式判断传感器类型
4. 解析具体的传感器数据
5. 验证数据有效性
6. 存储到对应的数据库表

### 4. 状态监控

系统提供实时状态监控功能：
- 系统运行状态
- 数据库连接状态
- 通信模块状态
- 传感器数据统计

## 配置参数

### 系统配置（config.h）

```c
#define SYSTEM_CLOCK_FREQ       16000000UL  // 系统时钟频率
#define UART_BAUD_RATE         115200       // UART波特率
#define RX_BUFFER_SIZE         256          // 接收缓冲区大小
#define TX_BUFFER_SIZE         512          // 发送缓冲区大小
```

### 数据库配置

```c
#define DB_HOST                "localhost"   // 数据库主机
#define DB_PORT                3306          // 数据库端口
#define DB_USER                "sensor_user" // 数据库用户名
#define DB_PASSWORD            "sensor_pass" // 数据库密码
#define DB_NAME                "sensor_data" // 数据库名称
```

## 编译和构建

### 使用IAR EMBEDDED WORKBENCH

1. 打开项目文件：`project/sensor_system.ewp`
2. 选择Debug或Release配置
3. 点击Build按钮编译项目

### 使用Makefile（可选）

```bash
# 编译项目
make all

# 清理构建文件
make clean

# 重新构建
make rebuild

# 显示项目信息
make info
```

## 调试和测试

### 1. 调试模式

在Debug配置下编译，可以使用IAR的调试功能：
- 设置断点
- 单步执行
- 查看变量值
- 监控内存使用

### 2. 测试数据

系统内置测试数据生成功能，会定期生成模拟的传感器数据用于测试。

### 3. 日志输出

系统通过printf输出详细的运行日志，包括：
- 数据接收日志
- 数据解析结果
- 数据库操作状态
- 错误信息

## 故障排除

### 常见问题

1. **数据库连接失败**
   - 检查数据库服务是否启动
   - 验证连接参数是否正确
   - 确认用户权限设置

2. **数据接收异常**
   - 检查UART配置参数
   - 验证硬件连接
   - 检查数据包格式

3. **内存不足**
   - 调整缓冲区大小
   - 优化内存使用
   - 检查内存泄漏

### 错误代码

| 错误代码 | 说明 | 解决方法 |
|----------|------|----------|
| PARSE_ERROR_FORMAT | 数据格式错误 | 检查传感器数据格式 |
| DB_ERROR_CONNECTION | 数据库连接错误 | 检查数据库配置 |
| COMM_STATUS_ERROR | 通信错误 | 检查硬件连接 |

## 扩展功能

### 1. 添加新的传感器类型

1. 在`sensor_data.h`中定义新的数据结构
2. 在`sensor_data.c`中实现数据解析函数
3. 在`database.c`中添加数据库操作函数
4. 更新主程序中的数据处理逻辑

### 2. 网络通信支持

可以扩展通信模块支持以太网或WiFi通信：
1. 添加网络协议栈
2. 实现TCP/UDP通信接口
3. 支持远程数据传输

### 3. 数据加密

为提高数据安全性，可以添加数据加密功能：
1. 实现AES加密算法
2. 添加数字签名验证
3. 支持安全密钥管理

## 技术支持

如有技术问题，请参考：
1. IAR EMBEDDED WORKBENCH用户手册
2. STM32F103参考手册
3. MySQL官方文档

## 版本历史

- v1.0.0：初始版本，支持基本的传感器数据采集和存储功能
- 后续版本将根据需求添加更多功能

## 许可证

本项目仅供学习和研究使用。