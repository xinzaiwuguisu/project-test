# IAR EMBEDDED WORKBENCH 项目设置指南

## 1. 创建新项目

### 步骤1：启动IAR EMBEDDED WORKBENCH
1. 打开IAR EMBEDDED WORKBENCH for ARM
2. 选择 "File" -> "New" -> "Workspace"
3. 保存工作空间为 `sensor_system.eww`

### 步骤2：创建项目
1. 在工作空间中右键点击，选择 "Add" -> "Add New Project"
2. 选择 "C" -> "main" 模板
3. 项目名称：`sensor_system`
4. 目标处理器：STM32F103CB（或根据实际硬件选择）

## 2. 项目配置

### 步骤1：添加源文件
将以下文件添加到项目中：
```
src/
├── main.c
├── sensor_data.c
├── database.c
└── communication.c
```

### 步骤2：添加头文件
将以下头文件添加到项目中：
```
src/
├── config.h
├── sensor_data.h
├── database.h
└── communication.h
```

### 步骤3：配置编译器选项

#### C/C++编译器设置：
1. 右键项目 -> "Options"
2. 选择 "C/C++ Compiler" -> "Preprocessor"
3. 在 "Defined symbols" 中添加：
   ```
   DEBUG=1
   STM32F103xB
   USE_HAL_DRIVER
   ```
4. 在 "Additional include directories" 中添加：
   ```
   $PROJ_DIR$\..\src
   ```

#### 优化设置：
1. 选择 "C/C++ Compiler" -> "Optimizations"
2. Debug配置：选择 "None"
3. Release配置：选择 "High (Speed)"

#### 调试信息：
1. 选择 "C/C++ Compiler" -> "Output"
2. 勾选 "Generate debug information"

### 步骤4：配置链接器选项

#### 链接器设置：
1. 选择 "Linker" -> "Config"
2. 选择适当的链接脚本文件（.icf文件）
3. 对于STM32F103CB，选择：
   ```
   $TOOLKIT_DIR$\config\linker\ST\stm32f103xB.icf
   ```

#### 输出设置：
1. 选择 "Linker" -> "Output"
2. 输出文件名：`sensor_system.out`
3. 勾选 "Generate linker map file"

### 步骤5：配置调试器

#### J-Link调试器设置：
1. 选择 "Debugger" -> "Setup"
2. Driver：选择 "J-Link/J-Trace"
3. 接口：选择 "SWD"

#### ST-Link调试器设置：
1. 选择 "Debugger" -> "Setup"
2. Driver：选择 "ST-LINK"
3. 接口：选择 "SWD"

## 3. 硬件配置

### 最小系统要求：
- STM32F103CB微控制器
- 8MHz外部晶振
- 复位电路
- 电源电路（3.3V）
- UART接口（用于传感器数据接收）
- 调试接口（SWD）

### 引脚配置：
```
PA9  - UART1_TX（调试输出）
PA10 - UART1_RX（传感器数据接收）
PA13 - SWDIO（调试接口）
PA14 - SWCLK（调试接口）
```

## 4. 编译和下载

### 编译项目：
1. 选择配置（Debug或Release）
2. 按F7或点击"Build"按钮
3. 检查编译输出，确保无错误

### 下载程序：
1. 连接调试器到目标板
2. 按Ctrl+D或点击"Download and Debug"
3. 程序将自动下载并开始调试

## 5. 调试技巧

### 设置断点：
1. 在代码行号左侧点击设置断点
2. 使用F5继续执行
3. 使用F10单步执行

### 查看变量：
1. 在"Watch"窗口中添加变量
2. 在"Locals"窗口查看局部变量
3. 在"Memory"窗口查看内存内容

### 串口输出：
1. 使用printf函数输出调试信息
2. 通过UART1发送到PC端串口助手
3. 波特率设置为115200

## 6. 常见问题解决

### 问题1：编译错误 "undefined symbol"
**解决方法：**
1. 检查头文件包含路径
2. 确认所有源文件都已添加到项目
3. 检查函数声明和定义是否匹配

### 问题2：下载失败
**解决方法：**
1. 检查调试器连接
2. 确认目标板电源正常
3. 检查SWD接口连接
4. 尝试擦除芯片后重新下载

### 问题3：程序运行异常
**解决方法：**
1. 检查时钟配置
2. 确认堆栈大小设置
3. 检查中断向量表
4. 使用调试器单步执行定位问题

### 问题4：UART通信异常
**解决方法：**
1. 检查波特率设置
2. 确认引脚配置正确
3. 检查硬件连接
4. 验证数据格式

## 7. 性能优化建议

### 代码优化：
1. 使用适当的数据类型
2. 避免不必要的函数调用
3. 合理使用内联函数
4. 优化循环结构

### 内存优化：
1. 合理设置缓冲区大小
2. 避免内存泄漏
3. 使用静态分配替代动态分配
4. 优化数据结构对齐

### 实时性优化：
1. 合理设置中断优先级
2. 减少中断处理时间
3. 使用DMA传输大量数据
4. 优化任务调度

## 8. 扩展功能

### 添加新传感器：
1. 在sensor_data.h中定义新的数据结构
2. 实现相应的解析函数
3. 更新数据库表结构
4. 修改主程序处理逻辑

### 网络通信：
1. 添加以太网或WiFi模块
2. 实现TCP/UDP通信协议
3. 支持远程数据传输
4. 添加网络配置界面

### 数据存储：
1. 添加SD卡存储功能
2. 实现数据压缩算法
3. 支持离线数据缓存
4. 添加数据备份机制