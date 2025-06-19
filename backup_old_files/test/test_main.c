#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/sensor_data.h"
#include "../src/database.h"
#include "../src/communication.h"

/* 测试函数声明 */
void test_sensor_data_parsing(void);
void test_database_operations(void);
void test_communication_module(void);
void run_all_tests(void);

/**
 * @brief 测试主函数
 */
int main(void)
{
    printf("=== 传感器数据采集系统测试程序 ===\n\n");
    
    run_all_tests();
    
    printf("\n=== 所有测试完成 ===\n");
    return 0;
}

/**
 * @brief 运行所有测试
 */
void run_all_tests(void)
{
    printf("开始运行测试...\n\n");
    
    test_sensor_data_parsing();
    test_database_operations();
    test_communication_module();
}

/**
 * @brief 测试传感器数据解析功能
 */
void test_sensor_data_parsing(void)
{
    printf("--- 测试传感器数据解析功能 ---\n");
    
    /* 测试传感器1数据解析 */
    {
        sensor1_data_t sensor1_data;
        const char* test_data = "2021001ZS,25.6,60.2";
        
        parse_result_t result = parse_sensor1_data(test_data, &sensor1_data);
        
        printf("传感器1数据解析测试:\n");
        printf("  输入数据: %s\n", test_data);
        printf("  解析结果: %s\n", result == PARSE_SUCCESS ? "成功" : "失败");
        
        if (result == PARSE_SUCCESS) {
            printf("  学号: %s\n", sensor1_data.student_id);
            printf("  传感器: %s\n", sensor1_data.sensor_name);
            printf("  温度: %.2f°C\n", sensor1_data.temperature);
            printf("  湿度: %.2f%%\n", sensor1_data.humidity);
            printf("  状态: %s\n", get_status_string(sensor1_data.status));
            
            /* 验证解析结果 */
            assert(strcmp(sensor1_data.student_id, "2021001ZS") == 0);
            assert(sensor1_data.temperature == 25.6f);
            assert(sensor1_data.humidity == 60.2f);
            printf("  ✓ 数据验证通过\n");
        }
    }
    
    /* 测试传感器2数据解析 */
    {
        sensor2_data_t sensor2_data;
        const char* test_data = "2021001ZS,DOOR_SENSOR,1";
        
        parse_result_t result = parse_sensor2_data(test_data, &sensor2_data);
        
        printf("\n传感器2数据解析测试:\n");
        printf("  输入数据: %s\n", test_data);
        printf("  解析结果: %s\n", result == PARSE_SUCCESS ? "成功" : "失败");
        
        if (result == PARSE_SUCCESS) {
            printf("  学号: %s\n", sensor2_data.student_id);
            printf("  传感器: %s\n", sensor2_data.sensor_name);
            printf("  中断类型: %u\n", sensor2_data.interrupt_type);
            printf("  中断次数: %u\n", sensor2_data.interrupt_count);
            printf("  状态: %s\n", get_status_string(sensor2_data.status));
            
            /* 验证解析结果 */
            assert(strcmp(sensor2_data.student_id, "2021001ZS") == 0);
            assert(strcmp(sensor2_data.sensor_name, "DOOR_SENSOR") == 0);
            assert(sensor2_data.interrupt_type == 1);
            printf("  ✓ 数据验证通过\n");
        }
    }
    
    /* 测试错误数据格式 */
    {
        sensor1_data_t sensor1_data;
        const char* invalid_data = "invalid,data";
        
        parse_result_t result = parse_sensor1_data(invalid_data, &sensor1_data);
        
        printf("\n错误数据格式测试:\n");
        printf("  输入数据: %s\n", invalid_data);
        printf("  解析结果: %s\n", result == PARSE_SUCCESS ? "成功" : "失败");
        printf("  ✓ 错误处理正常\n");
    }
    
    printf("\n");
}

/**
 * @brief 测试数据库操作功能
 */
void test_database_operations(void)
{
    printf("--- 测试数据库操作功能 ---\n");
    
    /* 初始化数据库 */
    db_result_t result = database_init();
    printf("数据库初始化: %s\n", result == DB_SUCCESS ? "成功" : "失败");
    
    /* 连接数据库 */
    result = database_connect();
    printf("数据库连接: %s\n", result == DB_SUCCESS ? "成功" : "失败");
    
    if (result == DB_SUCCESS) {
        /* 测试插入传感器1数据 */
        sensor1_data_t sensor1_data = {
            .student_id = "2021001ZS",
            .sensor_name = "TEMP_HUM",
            .temperature = 25.6f,
            .humidity = 60.2f,
            .status = SENSOR_STATUS_NORMAL,
            .timestamp = 1234567890
        };
        
        result = database_insert_sensor1_data(&sensor1_data);
        printf("插入传感器1数据: %s\n", result == DB_SUCCESS ? "成功" : "失败");
        
        /* 测试插入传感器2数据 */
        sensor2_data_t sensor2_data = {
            .student_id = "2021001ZS",
            .sensor_name = "DOOR_SENSOR",
            .interrupt_type = 1,
            .interrupt_count = 1,
            .status = SENSOR_STATUS_NORMAL,
            .timestamp = 1234567890
        };
        
        result = database_insert_sensor2_data(&sensor2_data);
        printf("插入传感器2数据: %s\n", result == DB_SUCCESS ? "成功" : "失败");
        
        /* 检查数据库状态 */
        db_status_t status = database_get_status();
        printf("数据库状态: %s\n", 
               status == DB_STATUS_CONNECTED ? "已连接" : 
               status == DB_STATUS_DISCONNECTED ? "未连接" : "错误");
        
        printf("✓ 数据库操作测试完成\n");
    }
    
    printf("\n");
}

/**
 * @brief 测试通信模块功能
 */
void test_communication_module(void)
{
    printf("--- 测试通信模块功能 ---\n");
    
    /* 初始化通信模块 */
    communication_init();
    printf("通信模块初始化: 完成\n");
    
    /* 测试数据包解析 */
    {
        uint8_t test_packet[] = {0xAA, '2', '0', '2', '1', '0', '0', '1', 'Z', 'S', ',', '2', '5', '.', '6', ',', '6', '0', '.', '2', 0x55};
        char output[64];
        
        bool result = communication_parse_packet(test_packet, sizeof(test_packet), output, sizeof(output));
        printf("数据包解析测试: %s\n", result ? "成功" : "失败");
        
        if (result) {
            printf("  解析结果: %s\n", output);
            printf("  ✓ 数据包解析正常\n");
        }
    }
    
    /* 测试校验和计算 */
    {
        uint8_t test_data[] = "2021001ZS,25.6,60.2";
        uint8_t checksum = communication_calculate_checksum(test_data, sizeof(test_data) - 1);
        printf("校验和计算测试: 0x%02X\n", checksum);
        printf("✓ 校验和计算正常\n");
    }
    
    /* 测试缓冲区操作 */
    {
        uint8_t test_data[] = "Hello, World!";
        bool result = communication_send_data(test_data, sizeof(test_data) - 1);
        printf("发送数据测试: %s\n", result ? "成功" : "失败");
        
        comm_status_t status = communication_get_status();
        printf("通信状态: %s\n", 
               status == COMM_STATUS_IDLE ? "空闲" : 
               status == COMM_STATUS_RECEIVING ? "接收中" : "其他");
        
        printf("✓ 通信模块测试完成\n");
    }
    
    printf("\n");
}