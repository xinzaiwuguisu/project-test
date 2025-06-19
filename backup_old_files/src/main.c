/**
 * @file main.c
 * @brief 传感器数据采集系统主程序 - IAR 5.3兼容版本
 * @author OpenHands
 * @date 2025-06-19
 * @version 1.0.0
 * 
 * 功能描述：
 * 1. 实时接收传感器1数据（学号姓名缩写+温度+湿度）
 * 2. 实时接收传感器2数据（学号姓名缩写+传感器名+中断）
 * 3. 将数据分别存储到MySQL数据库的不同表中
 * 4. 提供系统状态监控和错误处理
 */

#include "config.h"
#include "sensor_data.h"
#include "database.h"
#include "communication.h"

/* 全局变量 */
static bool system_running = true;
static uint32_t main_loop_count = 0;
static uint32_t last_heartbeat_time = 0;

/* 函数声明 */
static system_status_t system_init(void);
static void system_main_loop(void);
static void system_shutdown(void);
static void process_received_data(void);
static void update_system_status(void);
static void handle_system_error(system_status_t error);
static void data_received_callback(const uint8_t* data, uint16_t length);
static void communication_error_callback(comm_error_t error);
static void database_error_callback(const char* error_msg);
static void sensor_data_callback(const sensor_data_t* data);
static void print_system_info(void);
static void print_statistics(void);
static uint32_t get_uptime_seconds(void);

#ifdef IAR_LEGACY_SUPPORT
/* IAR 5.3兼容的中断服务程序声明 */
#pragma vector = USART1_IRQn
__interrupt void USART1_IRQHandler(void);
#endif

/**
 * @brief 主函数
 */
int main(void)
{
    system_status_t status;
    
    /* 系统初始化 */
    status = system_init();
    if (status != SYSTEM_OK) {
        ERROR_PRINT("System initialization failed: %d", status);
        handle_system_error(status);
        return -1;
    }
    
    /* 打印系统信息 */
    print_system_info();
    
    INFO_PRINT("Sensor data collection system started");
    INFO_PRINT("Waiting for sensor data...");
    
    /* 主循环 */
    while (system_running) {
        system_main_loop();
        
        /* 简单的任务调度延迟 */
        {
            volatile int delay;
            for (delay = 0; delay < 10000; delay++) {
                /* 空循环延迟 */
            }
        }
    }
    
    /* 系统关闭 */
    system_shutdown();
    
    INFO_PRINT("System shutdown completed");
    return 0;
}

/**
 * @brief 系统初始化
 */
static system_status_t system_init(void)
{
    system_status_t status;
    db_config_t db_config;
    comm_config_t comm_config;
    
    /* 初始化传感器数据模块 */
    status = sensor_data_init();
    if (status != SYSTEM_OK) {
        ERROR_PRINT("Sensor data module initialization failed");
        return status;
    }
    
    /* 初始化通信模块 */
    memcpy(&comm_config, &DEFAULT_COMM_CONFIG, sizeof(comm_config_t));
    status = communication_init(&comm_config);
    if (status != SYSTEM_OK) {
        ERROR_PRINT("Communication module initialization failed");
        return status;
    }
    
    /* 初始化数据库模块 */
    status = database_init();
    if (status != SYSTEM_OK) {
        ERROR_PRINT("Database module initialization failed");
        return status;
    }
    
    /* 连接数据库 */
    memcpy(&db_config, &DEFAULT_DB_CONFIG, sizeof(db_config_t));
    {
        db_result_t db_result = database_connect(&db_config);
        if (!db_result.success) {
            ERROR_PRINT("Database connection failed: %s", db_result.error_message);
            return SYSTEM_ERROR;
        }
    }
    
    /* 创建数据库表 */
    {
        db_result_t db_result = database_create_tables();
        if (!db_result.success) {
            ERROR_PRINT("Database table creation failed: %s", db_result.error_message);
            return SYSTEM_ERROR;
        }
    }
    
    /* 设置回调函数 */
    communication_set_rx_callback(data_received_callback);
    communication_set_error_callback(communication_error_callback);
    database_set_error_callback(database_error_callback);
    set_sensor_data_callback(sensor_data_callback);
    
    /* 初始化系统状态 */
    main_loop_count = 0;
    last_heartbeat_time = get_uptime_seconds();
    
    DEBUG_PRINT("System initialization completed successfully");
    return SYSTEM_OK;
}

/**
 * @brief 系统主循环
 */
static void system_main_loop(void)
{
    /* 增加循环计数 */
    main_loop_count++;
    
    /* 处理接收到的数据 */
    process_received_data();
    
    /* 处理通信模块的接收数据 */
    communication_process_rx_data();
    
    /* 更新系统状态 */
    update_system_status();
    
    /* 定期打印统计信息 */
    if (main_loop_count % 10000 == 0) {
        print_statistics();
    }
    
    /* 心跳检测 */
    {
        uint32_t current_time = get_uptime_seconds();
        if (current_time - last_heartbeat_time >= 60) { /* 每60秒一次心跳 */
            INFO_PRINT("System heartbeat - Uptime: %lu seconds", current_time);
            last_heartbeat_time = current_time;
        }
    }
}

/**
 * @brief 系统关闭
 */
static void system_shutdown(void)
{
    /* 断开数据库连接 */
    {
        db_result_t result = database_disconnect();
        if (!result.success) {
            ERROR_PRINT("Database disconnect failed: %s", result.error_message);
        }
    }
    
    /* 关闭通信模块 */
    communication_deinit();
    
    /* 打印最终统计信息 */
    print_statistics();
    
    DEBUG_PRINT("System shutdown completed");
}

/**
 * @brief 处理接收到的数据
 */
static void process_received_data(void)
{
    char line_buffer[128];
    system_status_t status;
    
    /* 检查是否有数据可读 */
    if (!communication_data_available()) {
        return;
    }
    
    /* 接收一行数据 */
    status = communication_receive_line(line_buffer, sizeof(line_buffer), 1000);
    if (status == SYSTEM_OK && strlen(line_buffer) > 0) {
        sensor_data_t sensor_data;
        parse_result_t parse_result;
        
        DEBUG_PRINT("Received data: %s", line_buffer);
        
        /* 解析传感器数据 */
        parse_result = parse_sensor_data(line_buffer, &sensor_data);
        
        if (parse_result.is_valid) {
            /* 存储到数据库 */
            db_result_t db_result = database_insert_sensor_data(&sensor_data);
            
            if (db_result.success) {
                INFO_PRINT("Data stored successfully");
            } else {
                ERROR_PRINT("Database insert failed: %s", db_result.error_message);
            }
        } else {
            ERROR_PRINT("Data parse failed: %s", parse_result.error_msg);
        }
    } else if (status == SYSTEM_TIMEOUT) {
        /* 超时是正常的，不需要报错 */
    } else {
        ERROR_PRINT("Data receive failed: %d", status);
    }
}

/**
 * @brief 更新系统状态
 */
static void update_system_status(void)
{
    /* 检查通信状态 */
    comm_status_t comm_status = communication_get_status();
    if (comm_status == COMM_STATUS_ERROR) {
        comm_error_t error = communication_get_last_error();
        ERROR_PRINT("Communication error detected: %d", error);
    }
    
    /* 检查数据库状态 */
    db_status_t db_status = database_get_status();
    if (db_status == DB_STATUS_ERROR) {
        ERROR_PRINT("Database error detected: %s", database_get_last_error());
    }
    
    /* 检查内存使用情况（简化实现） */
    /* 在实际项目中，这里可以检查堆栈使用情况 */
}

/**
 * @brief 处理系统错误
 */
static void handle_system_error(system_status_t error)
{
    ERROR_PRINT("System error occurred: %d", error);
    
    switch (error) {
        case SYSTEM_ERROR:
            /* 尝试重启相关模块 */
            break;
            
        case SYSTEM_TIMEOUT:
            /* 重置超时计数器 */
            break;
            
        default:
            /* 其他错误处理 */
            break;
    }
    
    /* 在严重错误情况下，可以选择重启系统 */
    /* system_running = false; */
}

/**
 * @brief 数据接收回调函数
 */
static void data_received_callback(const uint8_t* data, uint16_t length)
{
    if (data != NULL && length > 0) {
        DEBUG_PRINT("Data received callback: %d bytes", length);
        /* 数据已经在主循环中处理，这里只做统计 */
    }
}

/**
 * @brief 通信错误回调函数
 */
static void communication_error_callback(comm_error_t error)
{
    ERROR_PRINT("Communication error callback: %d", error);
    
    /* 根据错误类型采取相应措施 */
    switch (error) {
        case COMM_ERROR_BUFFER_FULL:
            /* 清空缓冲区 */
            communication_clear_rx_buffer();
            break;
            
        case COMM_ERROR_TIMEOUT:
            /* 重置通信模块 */
            break;
            
        default:
            break;
    }
}

/**
 * @brief 数据库错误回调函数
 */
static void database_error_callback(const char* error_msg)
{
    ERROR_PRINT("Database error callback: %s", error_msg ? error_msg : "Unknown error");
    
    /* 尝试重新连接数据库 */
    {
        db_result_t result = database_disconnect();
        if (result.success) {
            result = database_connect(&DEFAULT_DB_CONFIG);
            if (result.success) {
                INFO_PRINT("Database reconnected successfully");
            }
        }
    }
}

/**
 * @brief 传感器数据回调函数
 */
static void sensor_data_callback(const sensor_data_t* data)
{
    if (data == NULL) {
        return;
    }
    
    /* 根据传感器类型打印信息 */
    switch (data->type) {
        case SENSOR_TYPE_TEMP_HUMIDITY:
            INFO_PRINT("Sensor1 data: ID=%s, Temp=%.2f, Humid=%.2f", 
                      data->data.sensor1.student_id,
                      data->data.sensor1.temperature,
                      data->data.sensor1.humidity);
            break;
            
        case SENSOR_TYPE_INTERRUPT:
            INFO_PRINT("Sensor2 data: ID=%s, Sensor=%s, IntType=%d", 
                      data->data.sensor2.student_id,
                      data->data.sensor2.sensor_name,
                      data->data.sensor2.interrupt_type);
            break;
            
        default:
            DEBUG_PRINT("Unknown sensor type: %d", data->type);
            break;
    }
}

/**
 * @brief 打印系统信息
 */
static void print_system_info(void)
{
    INFO_PRINT("========================================");
    INFO_PRINT("  Sensor Data Collection System v%d.%d.%d", 
               FIRMWARE_VERSION_MAJOR, FIRMWARE_VERSION_MINOR, FIRMWARE_VERSION_PATCH);
    INFO_PRINT("  Build Date: %s %s", FIRMWARE_BUILD_DATE, FIRMWARE_BUILD_TIME);
    INFO_PRINT("  IAR Version: 5.3 Compatible");
    INFO_PRINT("  Target: STM32F103CB");
    INFO_PRINT("  System Clock: %lu Hz", SYSTEM_CLOCK_FREQ);
    INFO_PRINT("  UART Baud Rate: %lu", UART_BAUD_RATE);
    INFO_PRINT("========================================");
}

/**
 * @brief 打印统计信息
 */
static void print_statistics(void)
{
    uint32_t sensor_total, sensor_valid, sensor_error;
    comm_statistics_t comm_stats;
    uint32_t sensor1_count, sensor2_count;
    
    /* 获取传感器数据统计 */
    get_sensor_statistics(&sensor_total, &sensor_valid, &sensor_error);
    
    /* 获取通信统计 */
    communication_get_statistics(&comm_stats);
    
    /* 获取数据库统计 */
    {
        db_result_t result = database_get_statistics(&sensor1_count, &sensor2_count);
        if (!result.success) {
            sensor1_count = 0;
            sensor2_count = 0;
        }
    }
    
    INFO_PRINT("=== System Statistics ===");
    INFO_PRINT("Main Loop Count: %lu", main_loop_count);
    INFO_PRINT("Uptime: %lu seconds", get_uptime_seconds());
    INFO_PRINT("Sensor Data - Total: %lu, Valid: %lu, Error: %lu", 
               sensor_total, sensor_valid, sensor_error);
    INFO_PRINT("Communication - RX: %lu bytes, TX: %lu bytes, Errors: %lu", 
               comm_stats.bytes_received, comm_stats.bytes_transmitted, comm_stats.error_count);
    INFO_PRINT("Database - Sensor1: %lu records, Sensor2: %lu records", 
               sensor1_count, sensor2_count);
    INFO_PRINT("========================");
}

/**
 * @brief 获取系统运行时间
 */
static uint32_t get_uptime_seconds(void)
{
    /* 简化的运行时间实现 */
    static uint32_t start_time = 0;
    static uint32_t current_time = 0;
    
    if (start_time == 0) {
        start_time = current_time;
    }
    
    current_time++;
    return (current_time - start_time) / 1000; /* 假设每毫秒调用一次 */
}

#ifdef IAR_LEGACY_SUPPORT
/**
 * @brief UART1中断服务程序 - IAR 5.3兼容
 */
#pragma vector = USART1_IRQn
__interrupt void USART1_IRQHandler(void)
{
    communication_uart_isr();
}
#endif

/* 测试函数（仅在调试模式下编译） */
#ifdef DEBUG

/**
 * @brief 测试传感器数据解析
 */
static void test_sensor_data_parsing(void)
{
    const char* test_data1 = "2021001ZS,25.6,60.2";
    const char* test_data2 = "2021001ZS,DOOR_SENSOR,1";
    sensor_data_t sensor_data;
    parse_result_t result;
    
    DEBUG_PRINT("Testing sensor data parsing...");
    
    /* 测试传感器1数据 */
    result = parse_sensor_data(test_data1, &sensor_data);
    if (result.is_valid) {
        DEBUG_PRINT("Sensor1 test passed: ID=%s, Temp=%.2f, Humid=%.2f", 
                    sensor_data.data.sensor1.student_id,
                    sensor_data.data.sensor1.temperature,
                    sensor_data.data.sensor1.humidity);
    } else {
        ERROR_PRINT("Sensor1 test failed: %s", result.error_msg);
    }
    
    /* 测试传感器2数据 */
    result = parse_sensor_data(test_data2, &sensor_data);
    if (result.is_valid) {
        DEBUG_PRINT("Sensor2 test passed: ID=%s, Sensor=%s, IntType=%d", 
                    sensor_data.data.sensor2.student_id,
                    sensor_data.data.sensor2.sensor_name,
                    sensor_data.data.sensor2.interrupt_type);
    } else {
        ERROR_PRINT("Sensor2 test failed: %s", result.error_msg);
    }
}

/**
 * @brief 测试数据库操作
 */
static void test_database_operations(void)
{
    sensor1_data_t sensor1_data;
    sensor2_data_t sensor2_data;
    db_result_t result;
    
    DEBUG_PRINT("Testing database operations...");
    
    /* 准备测试数据 */
    strcpy(sensor1_data.student_id, "TEST001");
    strcpy(sensor1_data.sensor_name, "TEST_TEMP");
    sensor1_data.temperature = 25.0f;
    sensor1_data.humidity = 50.0f;
    sensor1_data.status = SENSOR_STATUS_NORMAL;
    sensor1_data.timestamp = get_timestamp();
    
    strcpy(sensor2_data.student_id, "TEST001");
    strcpy(sensor2_data.sensor_name, "TEST_INT");
    sensor2_data.interrupt_type = INTERRUPT_TYPE_RISING;
    sensor2_data.interrupt_count = 1;
    sensor2_data.status = SENSOR_STATUS_NORMAL;
    sensor2_data.timestamp = get_timestamp();
    
    /* 测试插入操作 */
    result = database_insert_sensor1_data(&sensor1_data);
    if (result.success) {
        DEBUG_PRINT("Sensor1 database insert test passed");
    } else {
        ERROR_PRINT("Sensor1 database insert test failed: %s", result.error_message);
    }
    
    result = database_insert_sensor2_data(&sensor2_data);
    if (result.success) {
        DEBUG_PRINT("Sensor2 database insert test passed");
    } else {
        ERROR_PRINT("Sensor2 database insert test failed: %s", result.error_message);
    }
}

/**
 * @brief 运行所有测试
 */
static void run_all_tests(void)
{
    DEBUG_PRINT("Running system tests...");
    test_sensor_data_parsing();
    test_database_operations();
    DEBUG_PRINT("System tests completed");
}

#endif /* DEBUG */