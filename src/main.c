#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "config.h"
#include "sensor_data.h"
#include "database.h"
#include "communication.h"

/* 系统状态 */
typedef enum {
    SYSTEM_INIT = 0,
    SYSTEM_RUNNING,
    SYSTEM_ERROR,
    SYSTEM_SHUTDOWN
} system_state_t;

/* 全局变量 */
static system_state_t system_state = SYSTEM_INIT;
static uint8_t rx_buffer[RX_BUFFER_SIZE];
static char packet_buffer[MAX_DATA_LENGTH];

/* 函数声明 */
static void system_init(void);
static void system_main_loop(void);
static void process_sensor_data(void);
static void handle_sensor1_data(const char* data_str);
static void handle_sensor2_data(const char* data_str);
static void system_error_handler(void);
static void print_system_status(void);

/**
 * @brief 主函数
 * @return 程序退出码
 */
int main(void)
{
    printf("=== 传感器数据采集系统启动 ===\n");
    printf("学号姓名缩写: %s\n", STUDENT_ID);
    printf("系统时钟频率: %lu Hz\n", SYSTEM_CLOCK_FREQ);
    printf("UART波特率: %d\n", UART_BAUD_RATE);
    printf("================================\n\n");
    
    /* 系统初始化 */
    system_init();
    
    if (system_state == SYSTEM_ERROR) {
        printf("系统初始化失败！\n");
        return -1;
    }
    
    printf("系统初始化完成，开始运行...\n\n");
    system_state = SYSTEM_RUNNING;
    
    /* 主循环 */
    system_main_loop();
    
    printf("系统正常退出\n");
    return 0;
}

/**
 * @brief 系统初始化
 */
static void system_init(void)
{
    printf("正在初始化系统组件...\n");
    
    /* 初始化通信模块 */
    communication_init();
    
    /* 初始化数据库模块 */
    if (database_init() != DB_SUCCESS) {
        printf("数据库初始化失败！\n");
        system_state = SYSTEM_ERROR;
        return;
    }
    
    /* 连接数据库 */
    if (database_connect() != DB_SUCCESS) {
        printf("数据库连接失败！\n");
        system_state = SYSTEM_ERROR;
        return;
    }
    
    printf("所有组件初始化完成\n");
}

/**
 * @brief 系统主循环
 */
static void system_main_loop(void)
{
    uint32_t loop_count = 0;
    
    while (system_state == SYSTEM_RUNNING) {
        /* 处理传感器数据 */
        process_sensor_data();
        
        /* 定期打印系统状态 */
        if (++loop_count % 1000 == 0) {
            print_system_status();
        }
        
        /* 模拟一些延时 */
        for (volatile int i = 0; i < 10000; i++);
        
        /* 检查系统状态 */
        if (database_get_status() == DB_STATUS_ERROR) {
            printf("数据库连接错误，尝试重连...\n");
            if (database_connect() != DB_SUCCESS) {
                system_state = SYSTEM_ERROR;
                break;
            }
        }
        
        /* 模拟接收一些测试数据 */
        static uint32_t test_counter = 0;
        if (++test_counter % 5000 == 0) {
            /* 模拟传感器1数据 */
            char test_data1[] = "2021001ZS,25.6,60.2";
            handle_sensor1_data(test_data1);
            
            /* 模拟传感器2数据 */
            char test_data2[] = "2021001ZS,DOOR_SENSOR,1";
            handle_sensor2_data(test_data2);
        }
        
        /* 退出条件（在实际系统中可能是外部信号） */
        if (loop_count > 50000) {
            printf("达到最大循环次数，系统退出\n");
            break;
        }
    }
    
    if (system_state == SYSTEM_ERROR) {
        system_error_handler();
    }
    
    /* 清理资源 */
    database_disconnect();
}

/**
 * @brief 处理传感器数据
 */
static void process_sensor_data(void)
{
    if (!communication_is_data_available()) {
        return;
    }
    
    /* 读取接收到的数据 */
    uint16_t bytes_read = communication_read_data(rx_buffer, sizeof(rx_buffer));
    if (bytes_read == 0) {
        return;
    }
    
    /* 解析数据包 */
    if (communication_parse_packet(rx_buffer, bytes_read, packet_buffer, sizeof(packet_buffer))) {
        printf("接收到数据包: %s\n", packet_buffer);
        
        /* 根据数据格式判断传感器类型 */
        /* 简单的判断方法：统计逗号数量 */
        int comma_count = 0;
        for (int i = 0; packet_buffer[i] != '\0'; i++) {
            if (packet_buffer[i] == ',') {
                comma_count++;
            }
        }
        
        if (comma_count == 2) {
            /* 可能是传感器1数据（学号,温度,湿度）或传感器2数据（学号,传感器名,中断） */
            /* 进一步判断第二个字段是否为数字 */
            char* second_field = strchr(packet_buffer, ',');
            if (second_field) {
                second_field++; // 跳过逗号
                char* third_field = strchr(second_field, ',');
                if (third_field) {
                    *third_field = '\0'; // 临时截断
                    
                    /* 检查第二个字段是否为数字（温度） */
                    bool is_numeric = true;
                    for (int i = 0; second_field[i] != '\0'; i++) {
                        if (!(second_field[i] >= '0' && second_field[i] <= '9') && 
                            second_field[i] != '.' && second_field[i] != '-') {
                            is_numeric = false;
                            break;
                        }
                    }
                    
                    *third_field = ','; // 恢复逗号
                    
                    if (is_numeric) {
                        handle_sensor1_data(packet_buffer);
                    } else {
                        handle_sensor2_data(packet_buffer);
                    }
                }
            }
        }
    }
}

/**
 * @brief 处理传感器1数据（温湿度传感器）
 * @param data_str 数据字符串
 */
static void handle_sensor1_data(const char* data_str)
{
    sensor1_data_t sensor_data;
    
    parse_result_t result = parse_sensor1_data(data_str, &sensor_data);
    if (result == PARSE_SUCCESS) {
        printf("传感器1数据解析成功:\n");
        printf("  学号: %s\n", sensor_data.student_id);
        printf("  传感器: %s\n", sensor_data.sensor_name);
        printf("  温度: %.2f°C\n", sensor_data.temperature);
        printf("  湿度: %.2f%%\n", sensor_data.humidity);
        printf("  状态: %s\n", get_status_string(sensor_data.status));
        printf("  时间戳: %u\n", sensor_data.timestamp);
        
        /* 存储到数据库 */
        db_result_t db_result = database_insert_sensor1_data(&sensor_data);
        if (db_result == DB_SUCCESS) {
            printf("传感器1数据已存储到数据库\n");
        } else {
            printf("传感器1数据存储失败: %s\n", database_get_error_string(db_result));
        }
    } else {
        printf("传感器1数据解析失败，错误码: %d\n", result);
    }
    
    printf("---\n");
}

/**
 * @brief 处理传感器2数据（中断传感器）
 * @param data_str 数据字符串
 */
static void handle_sensor2_data(const char* data_str)
{
    sensor2_data_t sensor_data;
    
    parse_result_t result = parse_sensor2_data(data_str, &sensor_data);
    if (result == PARSE_SUCCESS) {
        printf("传感器2数据解析成功:\n");
        printf("  学号: %s\n", sensor_data.student_id);
        printf("  传感器: %s\n", sensor_data.sensor_name);
        printf("  中断类型: %u\n", sensor_data.interrupt_type);
        printf("  中断次数: %u\n", sensor_data.interrupt_count);
        printf("  状态: %s\n", get_status_string(sensor_data.status));
        printf("  时间戳: %u\n", sensor_data.timestamp);
        
        /* 存储到数据库 */
        db_result_t db_result = database_insert_sensor2_data(&sensor_data);
        if (db_result == DB_SUCCESS) {
            printf("传感器2数据已存储到数据库\n");
        } else {
            printf("传感器2数据存储失败: %s\n", database_get_error_string(db_result));
        }
    } else {
        printf("传感器2数据解析失败，错误码: %d\n", result);
    }
    
    printf("---\n");
}

/**
 * @brief 系统错误处理
 */
static void system_error_handler(void)
{
    printf("系统进入错误状态，正在处理...\n");
    
    /* 清空通信缓冲区 */
    communication_clear_buffers();
    
    /* 尝试重连数据库 */
    database_disconnect();
    if (database_connect() == DB_SUCCESS) {
        printf("数据库重连成功，系统恢复运行\n");
        system_state = SYSTEM_RUNNING;
    } else {
        printf("数据库重连失败，系统无法恢复\n");
        system_state = SYSTEM_SHUTDOWN;
    }
}

/**
 * @brief 打印系统状态
 */
static void print_system_status(void)
{
    printf("\n=== 系统状态 ===\n");
    printf("系统状态: %s\n", 
           system_state == SYSTEM_RUNNING ? "运行中" : 
           system_state == SYSTEM_ERROR ? "错误" : "其他");
    printf("数据库状态: %s\n", 
           database_get_status() == DB_STATUS_CONNECTED ? "已连接" : 
           database_get_status() == DB_STATUS_DISCONNECTED ? "未连接" : "错误");
    printf("通信状态: %s\n", 
           communication_get_status() == COMM_STATUS_IDLE ? "空闲" : 
           communication_get_status() == COMM_STATUS_RECEIVING ? "接收中" : "其他");
    printf("系统时间戳: %u\n", get_system_timestamp());
    printf("===============\n\n");
}

/**
 * @brief 系统时钟中断处理函数（需要在定时器中断中调用）
 * 这个函数应该在系统定时器中断服务程序中被调用
 */
void SysTick_Handler(void)
{
    system_tick_handler();
}