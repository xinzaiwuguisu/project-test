/**
 * @file sensor_data_examples.c
 * @brief 传感器数据使用示例
 * @author OpenHands
 * @date 2025-06-19
 */

#include "config.h"
#include "sensor_data.h"
#include "database.h"
#include "communication.h"

/**
 * @brief 示例1：解析传感器1数据（温湿度）
 */
void example_parse_sensor1_data(void)
{
    const char* test_data = "2021001ZS,25.6,60.2";
    sensor1_data_t sensor_data;
    parse_result_t result;
    
    printf("=== 示例1：解析传感器1数据 ===\n");
    printf("输入数据：%s\n", test_data);
    
    result = parse_sensor1_data(test_data, &sensor_data);
    
    if (result.is_valid) {
        printf("解析成功！\n");
        printf("学号：%s\n", sensor_data.student_id);
        printf("传感器：%s\n", sensor_data.sensor_name);
        printf("温度：%.2f°C\n", sensor_data.temperature);
        printf("湿度：%.2f%%\n", sensor_data.humidity);
        printf("状态：%s\n", get_sensor_status_string(sensor_data.status));
    } else {
        printf("解析失败：%s\n", result.error_msg);
    }
    printf("\n");
}

/**
 * @brief 示例2：解析传感器2数据（中断）
 */
void example_parse_sensor2_data(void)
{
    const char* test_data = "2021001ZS,DOOR_SENSOR,1";
    sensor2_data_t sensor_data;
    parse_result_t result;
    
    printf("=== 示例2：解析传感器2数据 ===\n");
    printf("输入数据：%s\n", test_data);
    
    result = parse_sensor2_data(test_data, &sensor_data);
    
    if (result.is_valid) {
        printf("解析成功！\n");
        printf("学号：%s\n", sensor_data.student_id);
        printf("传感器：%s\n", sensor_data.sensor_name);
        printf("中断类型：%s\n", get_interrupt_type_string(sensor_data.interrupt_type));
        printf("中断次数：%lu\n", sensor_data.interrupt_count);
        printf("状态：%s\n", get_sensor_status_string(sensor_data.status));
    } else {
        printf("解析失败：%s\n", result.error_msg);
    }
    printf("\n");
}

/**
 * @brief 示例3：自动识别传感器数据类型
 */
void example_auto_parse_sensor_data(void)
{
    const char* test_data1 = "2021002LM,23.5,55.8";
    const char* test_data2 = "2021002LM,MOTION_SENSOR,2";
    sensor_data_t sensor_data;
    parse_result_t result;
    
    printf("=== 示例3：自动识别传感器数据 ===\n");
    
    // 测试数据1
    printf("测试数据1：%s\n", test_data1);
    result = parse_sensor_data(test_data1, &sensor_data);
    
    if (result.is_valid) {
        printf("识别为传感器类型：%d\n", sensor_data.type);
        if (sensor_data.type == SENSOR_TYPE_TEMP_HUMIDITY) {
            printf("温湿度数据 - 温度：%.2f°C，湿度：%.2f%%\n",
                   sensor_data.data.sensor1.temperature,
                   sensor_data.data.sensor1.humidity);
        }
    }
    
    // 测试数据2
    printf("测试数据2：%s\n", test_data2);
    result = parse_sensor_data(test_data2, &sensor_data);
    
    if (result.is_valid) {
        printf("识别为传感器类型：%d\n", sensor_data.type);
        if (sensor_data.type == SENSOR_TYPE_INTERRUPT) {
            printf("中断数据 - 传感器：%s，类型：%s\n",
                   sensor_data.data.sensor2.sensor_name,
                   get_interrupt_type_string(sensor_data.data.sensor2.interrupt_type));
        }
    }
    printf("\n");
}

/**
 * @brief 示例4：数据库操作
 */
void example_database_operations(void)
{
    db_config_t db_config = DEFAULT_DB_CONFIG;
    db_result_t result;
    sensor1_data_t sensor1_data;
    sensor2_data_t sensor2_data;
    
    printf("=== 示例4：数据库操作 ===\n");
    
    // 初始化数据库
    database_init();
    
    // 连接数据库
    result = database_connect(&db_config);
    if (result.success) {
        printf("数据库连接成功\n");
        
        // 准备测试数据
        strcpy(sensor1_data.student_id, "EXAMPLE01");
        strcpy(sensor1_data.sensor_name, "TEMP_HUMID");
        sensor1_data.temperature = 25.0f;
        sensor1_data.humidity = 50.0f;
        sensor1_data.status = SENSOR_STATUS_NORMAL;
        sensor1_data.timestamp = get_timestamp();
        
        strcpy(sensor2_data.student_id, "EXAMPLE01");
        strcpy(sensor2_data.sensor_name, "DOOR_SENSOR");
        sensor2_data.interrupt_type = INTERRUPT_TYPE_RISING;
        sensor2_data.interrupt_count = 1;
        sensor2_data.status = SENSOR_STATUS_NORMAL;
        sensor2_data.timestamp = get_timestamp();
        
        // 插入数据
        result = database_insert_sensor1_data(&sensor1_data);
        if (result.success) {
            printf("传感器1数据插入成功\n");
        } else {
            printf("传感器1数据插入失败：%s\n", result.error_message);
        }
        
        result = database_insert_sensor2_data(&sensor2_data);
        if (result.success) {
            printf("传感器2数据插入成功\n");
        } else {
            printf("传感器2数据插入失败：%s\n", result.error_message);
        }
        
        // 断开连接
        database_disconnect();
        printf("数据库连接已断开\n");
    } else {
        printf("数据库连接失败：%s\n", result.error_message);
    }
    printf("\n");
}

/**
 * @brief 示例5：通信模块使用
 */
void example_communication_usage(void)
{
    comm_config_t comm_config = DEFAULT_COMM_CONFIG;
    system_status_t status;
    char test_message[] = "Hello, Sensor System!";
    
    printf("=== 示例5：通信模块使用 ===\n");
    
    // 初始化通信模块
    status = communication_init(&comm_config);
    if (status == SYSTEM_OK) {
        printf("通信模块初始化成功\n");
        printf("波特率：%lu\n", comm_config.baud_rate);
        printf("数据位：%d\n", comm_config.data_bits);
        printf("停止位：%d\n", comm_config.stop_bits);
        
        // 发送测试消息
        status = communication_send_string(test_message);
        if (status == SYSTEM_OK) {
            printf("测试消息发送成功：%s\n", test_message);
        } else {
            printf("测试消息发送失败\n");
        }
        
        // 获取统计信息
        comm_statistics_t stats;
        communication_get_statistics(&stats);
        printf("发送字节数：%lu\n", stats.bytes_transmitted);
        printf("发送包数：%lu\n", stats.packets_transmitted);
        
        // 反初始化
        communication_deinit();
        printf("通信模块已关闭\n");
    } else {
        printf("通信模块初始化失败\n");
    }
    printf("\n");
}

/**
 * @brief 示例6：完整的数据处理流程
 */
void example_complete_data_flow(void)
{
    const char* incoming_data[] = {
        "2021003WH,26.8,65.3",
        "2021003WH,WINDOW_SENSOR,1",
        "2021004XY,22.1,45.7",
        "INVALID_DATA",
        "2021004XY,MOTION_SENSOR,2"
    };
    
    int data_count = sizeof(incoming_data) / sizeof(incoming_data[0]);
    int i;
    
    printf("=== 示例6：完整数据处理流程 ===\n");
    
    // 初始化所有模块
    sensor_data_init();
    database_init();
    
    // 连接数据库
    db_config_t db_config = DEFAULT_DB_CONFIG;
    db_result_t db_result = database_connect(&db_config);
    
    if (db_result.success) {
        printf("系统初始化完成，开始处理数据...\n\n");
        
        // 处理每条数据
        for (i = 0; i < data_count; i++) {
            sensor_data_t sensor_data;
            parse_result_t parse_result;
            
            printf("处理数据 %d: %s\n", i + 1, incoming_data[i]);
            
            // 解析数据
            parse_result = parse_sensor_data(incoming_data[i], &sensor_data);
            
            if (parse_result.is_valid) {
                printf("  解析成功，类型：%d\n", sensor_data.type);
                
                // 存储到数据库
                db_result = database_insert_sensor_data(&sensor_data);
                if (db_result.success) {
                    printf("  数据库存储成功\n");
                } else {
                    printf("  数据库存储失败：%s\n", db_result.error_message);
                }
            } else {
                printf("  解析失败：%s\n", parse_result.error_msg);
            }
            printf("\n");
        }
        
        // 获取统计信息
        uint32_t total, valid, error;
        get_sensor_statistics(&total, &valid, &error);
        printf("处理统计：\n");
        printf("  总数据包：%lu\n", total);
        printf("  有效数据包：%lu\n", valid);
        printf("  错误数据包：%lu\n", error);
        
        // 断开数据库连接
        database_disconnect();
    } else {
        printf("数据库连接失败，无法演示完整流程\n");
    }
    printf("\n");
}

/**
 * @brief 运行所有示例
 */
void run_all_examples(void)
{
    printf("========================================\n");
    printf("  传感器数据采集系统 - 使用示例\n");
    printf("========================================\n\n");
    
    example_parse_sensor1_data();
    example_parse_sensor2_data();
    example_auto_parse_sensor_data();
    example_database_operations();
    example_communication_usage();
    example_complete_data_flow();
    
    printf("========================================\n");
    printf("  所有示例运行完成\n");
    printf("========================================\n");
}

/* 如果作为独立程序运行 */
#ifdef EXAMPLE_MAIN
int main(void)
{
    run_all_examples();
    return 0;
}
#endif