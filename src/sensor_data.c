#include "sensor_data.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* 系统时间戳计数器 */
static volatile uint32_t system_tick = 0;

/**
 * @brief 解析传感器1数据（格式：学号姓名缩写+温度+湿度）
 * @param raw_data 原始数据字符串
 * @param sensor_data 解析后的数据结构
 * @return 解析结果
 */
parse_result_t parse_sensor1_data(const char* raw_data, sensor1_data_t* sensor_data)
{
    if (!raw_data || !sensor_data) {
        return PARSE_ERROR_FORMAT;
    }
    
    char temp_buffer[MAX_DATA_LENGTH];
    strncpy(temp_buffer, raw_data, sizeof(temp_buffer) - 1);
    temp_buffer[sizeof(temp_buffer) - 1] = '\0';
    
    /* 示例数据格式: "2021001ZS,25.6,60.2" */
    char* token = strtok(temp_buffer, ",");
    if (!token) return PARSE_ERROR_FORMAT;
    
    /* 解析学号姓名缩写 */
    strncpy(sensor_data->student_id, token, sizeof(sensor_data->student_id) - 1);
    sensor_data->student_id[sizeof(sensor_data->student_id) - 1] = '\0';
    
    /* 解析温度 */
    token = strtok(NULL, ",");
    if (!token) return PARSE_ERROR_FORMAT;
    sensor_data->temperature = atof(token);
    
    /* 解析湿度 */
    token = strtok(NULL, ",");
    if (!token) return PARSE_ERROR_FORMAT;
    sensor_data->humidity = atof(token);
    
    /* 设置传感器名称和状态 */
    strcpy(sensor_data->sensor_name, SENSOR1_ID);
    sensor_data->status = SENSOR_STATUS_NORMAL;
    sensor_data->timestamp = get_system_timestamp();
    
    /* 数据有效性检查 */
    if (sensor_data->temperature < -40.0f || sensor_data->temperature > 85.0f) {
        sensor_data->status = SENSOR_STATUS_WARNING;
    }
    if (sensor_data->humidity < 0.0f || sensor_data->humidity > 100.0f) {
        sensor_data->status = SENSOR_STATUS_WARNING;
    }
    
    return PARSE_SUCCESS;
}

/**
 * @brief 解析传感器2数据（格式：学号姓名缩写+传感器名+中断）
 * @param raw_data 原始数据字符串
 * @param sensor_data 解析后的数据结构
 * @return 解析结果
 */
parse_result_t parse_sensor2_data(const char* raw_data, sensor2_data_t* sensor_data)
{
    if (!raw_data || !sensor_data) {
        return PARSE_ERROR_FORMAT;
    }
    
    char temp_buffer[MAX_DATA_LENGTH];
    strncpy(temp_buffer, raw_data, sizeof(temp_buffer) - 1);
    temp_buffer[sizeof(temp_buffer) - 1] = '\0';
    
    /* 示例数据格式: "2021001ZS,DOOR_SENSOR,1" */
    char* token = strtok(temp_buffer, ",");
    if (!token) return PARSE_ERROR_FORMAT;
    
    /* 解析学号姓名缩写 */
    strncpy(sensor_data->student_id, token, sizeof(sensor_data->student_id) - 1);
    sensor_data->student_id[sizeof(sensor_data->student_id) - 1] = '\0';
    
    /* 解析传感器名称 */
    token = strtok(NULL, ",");
    if (!token) return PARSE_ERROR_FORMAT;
    strncpy(sensor_data->sensor_name, token, sizeof(sensor_data->sensor_name) - 1);
    sensor_data->sensor_name[sizeof(sensor_data->sensor_name) - 1] = '\0';
    
    /* 解析中断类型 */
    token = strtok(NULL, ",");
    if (!token) return PARSE_ERROR_FORMAT;
    sensor_data->interrupt_type = (uint8_t)atoi(token);
    
    /* 设置状态和时间戳 */
    sensor_data->status = SENSOR_STATUS_NORMAL;
    sensor_data->timestamp = get_system_timestamp();
    sensor_data->interrupt_count = 1; // 每次接收到数据表示一次中断
    
    return PARSE_SUCCESS;
}

/**
 * @brief 获取系统时间戳
 * @return 当前时间戳
 */
uint32_t get_system_timestamp(void)
{
    return system_tick;
}

/**
 * @brief 系统时钟中断处理函数（需要在定时器中断中调用）
 */
void system_tick_handler(void)
{
    system_tick++;
}

/**
 * @brief 获取传感器状态字符串
 * @param status 传感器状态
 * @return 状态字符串
 */
const char* get_status_string(sensor_status_t status)
{
    switch (status) {
        case SENSOR_STATUS_NORMAL:  return "NORMAL";
        case SENSOR_STATUS_WARNING: return "WARNING";
        case SENSOR_STATUS_ERROR:   return "ERROR";
        case SENSOR_STATUS_OFFLINE: return "OFFLINE";
        default:                    return "UNKNOWN";
    }
}