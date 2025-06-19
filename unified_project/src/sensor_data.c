/**
 * @file sensor_data.c
 * @brief 传感器数据处理模块实现 - IAR 5.3兼容版本
 * @author OpenHands
 * @date 2025-06-19
 * @version 1.0.0
 */

#include "sensor_data.h"

/* 静态变量 */
static uint32_t total_data_count = 0;
static uint32_t valid_data_count = 0;
static uint32_t error_data_count = 0;
static void (*data_callback)(const sensor_data_t* data) = NULL;

/* 常量字符串数组 */
const char* const SENSOR_STATUS_STRINGS[] = {
    "NORMAL",
    "WARNING", 
    "ERROR",
    "OFFLINE"
};

const char* const INTERRUPT_TYPE_STRINGS[] = {
    "NONE",
    "RISING",
    "FALLING",
    "BOTH"
};

/* 内部函数声明 */
static bool is_numeric_string(const char* str);
static float parse_float_safe(const char* str);
static int parse_int_safe(const char* str);
static void trim_whitespace(char* str);
static sensor_status_t determine_sensor1_status(const sensor1_data_t* data);
static sensor_status_t determine_sensor2_status(const sensor2_data_t* data);

/**
 * @brief 初始化传感器数据处理模块
 */
system_status_t sensor_data_init(void)
{
    /* 重置统计信息 */
    total_data_count = 0;
    valid_data_count = 0;
    error_data_count = 0;
    data_callback = NULL;
    
    DEBUG_PRINT("Sensor data module initialized");
    return SYSTEM_OK;
}

/**
 * @brief 解析传感器1数据（温湿度）
 */
parse_result_t parse_sensor1_data(const char* data_str, sensor1_data_t* sensor_data)
{
    parse_result_t result;
    char temp_str[32], humid_str[32];
    char input_copy[128];
    char* token;
    int field_count = 0;
    
    /* 初始化结果 */
    result.is_valid = false;
    result.type = SENSOR_TYPE_TEMP_HUMIDITY;
    strcpy(result.error_msg, "");
    
    /* 参数检查 */
    if (data_str == NULL || sensor_data == NULL) {
        strcpy(result.error_msg, "Null pointer parameter");
        return result;
    }
    
    /* 复制输入字符串以避免修改原始数据 */
    if (strlen(data_str) >= sizeof(input_copy)) {
        strcpy(result.error_msg, ERROR_MSG_BUFFER_TOO_SMALL);
        return result;
    }
    strcpy(input_copy, data_str);
    
    /* 初始化传感器数据 */
    memset(sensor_data, 0, sizeof(sensor1_data_t));
    strcpy(sensor_data->sensor_name, "TEMP_HUMIDITY");
    
    /* 解析数据字段 */
    token = strtok(input_copy, ",");
    while (token != NULL && field_count < 3) {
        trim_whitespace(token);
        
        switch (field_count) {
            case 0: /* 学号姓名缩写 */
                if (strlen(token) >= MAX_STUDENT_ID_LEN) {
                    strcpy(result.error_msg, ERROR_MSG_INVALID_ID);
                    return result;
                }
                strcpy(sensor_data->student_id, token);
                break;
                
            case 1: /* 温度 */
                if (!is_numeric_string(token)) {
                    strcpy(result.error_msg, ERROR_MSG_INVALID_FORMAT);
                    return result;
                }
                sensor_data->temperature = parse_float_safe(token);
                break;
                
            case 2: /* 湿度 */
                if (!is_numeric_string(token)) {
                    strcpy(result.error_msg, ERROR_MSG_INVALID_FORMAT);
                    return result;
                }
                sensor_data->humidity = parse_float_safe(token);
                break;
        }
        
        field_count++;
        token = strtok(NULL, ",");
    }
    
    /* 检查字段数量 */
    if (field_count != 3) {
        strcpy(result.error_msg, ERROR_MSG_INVALID_FORMAT);
        return result;
    }
    
    /* 验证数据范围 */
    if (!validate_sensor1_data(sensor_data)) {
        strcpy(result.error_msg, ERROR_MSG_INVALID_RANGE);
        return result;
    }
    
    /* 设置时间戳和状态 */
    sensor_data->timestamp = get_timestamp();
    sensor_data->status = determine_sensor1_status(sensor_data);
    
    result.is_valid = true;
    return result;
}

/**
 * @brief 解析传感器2数据（中断）
 */
parse_result_t parse_sensor2_data(const char* data_str, sensor2_data_t* sensor_data)
{
    parse_result_t result;
    char input_copy[128];
    char* token;
    int field_count = 0;
    
    /* 初始化结果 */
    result.is_valid = false;
    result.type = SENSOR_TYPE_INTERRUPT;
    strcpy(result.error_msg, "");
    
    /* 参数检查 */
    if (data_str == NULL || sensor_data == NULL) {
        strcpy(result.error_msg, "Null pointer parameter");
        return result;
    }
    
    /* 复制输入字符串 */
    if (strlen(data_str) >= sizeof(input_copy)) {
        strcpy(result.error_msg, ERROR_MSG_BUFFER_TOO_SMALL);
        return result;
    }
    strcpy(input_copy, data_str);
    
    /* 初始化传感器数据 */
    memset(sensor_data, 0, sizeof(sensor2_data_t));
    
    /* 解析数据字段 */
    token = strtok(input_copy, ",");
    while (token != NULL && field_count < 3) {
        trim_whitespace(token);
        
        switch (field_count) {
            case 0: /* 学号姓名缩写 */
                if (strlen(token) >= MAX_STUDENT_ID_LEN) {
                    strcpy(result.error_msg, ERROR_MSG_INVALID_ID);
                    return result;
                }
                strcpy(sensor_data->student_id, token);
                break;
                
            case 1: /* 传感器名称 */
                if (strlen(token) >= MAX_SENSOR_NAME_LEN) {
                    strcpy(result.error_msg, ERROR_MSG_INVALID_NAME);
                    return result;
                }
                strcpy(sensor_data->sensor_name, token);
                break;
                
            case 2: /* 中断值 */
                if (!is_numeric_string(token)) {
                    strcpy(result.error_msg, ERROR_MSG_INVALID_FORMAT);
                    return result;
                }
                {
                    int interrupt_val = parse_int_safe(token);
                    if (interrupt_val < 0 || interrupt_val > 3) {
                        strcpy(result.error_msg, ERROR_MSG_INVALID_RANGE);
                        return result;
                    }
                    sensor_data->interrupt_type = (interrupt_type_t)interrupt_val;
                    sensor_data->interrupt_count = (interrupt_val > 0) ? 1 : 0;
                }
                break;
        }
        
        field_count++;
        token = strtok(NULL, ",");
    }
    
    /* 检查字段数量 */
    if (field_count != 3) {
        strcpy(result.error_msg, ERROR_MSG_INVALID_FORMAT);
        return result;
    }
    
    /* 验证数据 */
    if (!validate_sensor2_data(sensor_data)) {
        strcpy(result.error_msg, ERROR_MSG_INVALID_RANGE);
        return result;
    }
    
    /* 设置时间戳和状态 */
    sensor_data->timestamp = get_timestamp();
    sensor_data->status = determine_sensor2_status(sensor_data);
    
    result.is_valid = true;
    return result;
}

/**
 * @brief 自动识别并解析传感器数据
 */
parse_result_t parse_sensor_data(const char* data_str, sensor_data_t* sensor_data)
{
    parse_result_t result;
    char input_copy[128];
    int comma_count = 0;
    int i;
    
    /* 参数检查 */
    if (data_str == NULL || sensor_data == NULL) {
        result.is_valid = false;
        strcpy(result.error_msg, "Null pointer parameter");
        return result;
    }
    
    /* 统计逗号数量来判断数据类型 */
    for (i = 0; data_str[i] != '\0'; i++) {
        if (data_str[i] == ',') {
            comma_count++;
        }
    }
    
    /* 更新统计 */
    total_data_count++;
    
    if (comma_count == 2) {
        /* 检查第二个字段是否为数字（温度） */
        strcpy(input_copy, data_str);
        {
            char* token = strtok(input_copy, ",");
            if (token != NULL) {
                token = strtok(NULL, ",");
                if (token != NULL && is_numeric_string(token)) {
                    /* 传感器1数据 */
                    sensor_data->type = SENSOR_TYPE_TEMP_HUMIDITY;
                    result = parse_sensor1_data(data_str, &sensor_data->data.sensor1);
                } else {
                    /* 传感器2数据 */
                    sensor_data->type = SENSOR_TYPE_INTERRUPT;
                    result = parse_sensor2_data(data_str, &sensor_data->data.sensor2);
                }
            } else {
                result.is_valid = false;
                strcpy(result.error_msg, ERROR_MSG_INVALID_FORMAT);
            }
        }
    } else {
        result.is_valid = false;
        strcpy(result.error_msg, ERROR_MSG_INVALID_FORMAT);
    }
    
    /* 更新统计 */
    if (result.is_valid) {
        valid_data_count++;
        
        /* 调用回调函数 */
        if (data_callback != NULL) {
            data_callback(sensor_data);
        }
    } else {
        error_data_count++;
    }
    
    return result;
}

/**
 * @brief 验证传感器1数据有效性
 */
bool validate_sensor1_data(const sensor1_data_t* sensor_data)
{
    if (sensor_data == NULL) {
        return false;
    }
    
    /* 检查学号 */
    if (!is_valid_student_id(sensor_data->student_id)) {
        return false;
    }
    
    /* 检查温度范围 */
    if (sensor_data->temperature < MIN_TEMPERATURE || 
        sensor_data->temperature > MAX_TEMPERATURE) {
        return false;
    }
    
    /* 检查湿度范围 */
    if (sensor_data->humidity < MIN_HUMIDITY || 
        sensor_data->humidity > MAX_HUMIDITY) {
        return false;
    }
    
    return true;
}

/**
 * @brief 验证传感器2数据有效性
 */
bool validate_sensor2_data(const sensor2_data_t* sensor_data)
{
    if (sensor_data == NULL) {
        return false;
    }
    
    /* 检查学号 */
    if (!is_valid_student_id(sensor_data->student_id)) {
        return false;
    }
    
    /* 检查传感器名称 */
    if (!is_valid_sensor_name(sensor_data->sensor_name)) {
        return false;
    }
    
    /* 检查中断类型 */
    if (sensor_data->interrupt_type < INTERRUPT_TYPE_NONE || 
        sensor_data->interrupt_type > INTERRUPT_TYPE_BOTH) {
        return false;
    }
    
    return true;
}

/**
 * @brief 获取传感器状态字符串
 */
const char* get_sensor_status_string(sensor_status_t status)
{
    if (status >= 0 && status < ARRAY_SIZE(SENSOR_STATUS_STRINGS)) {
        return SENSOR_STATUS_STRINGS[status];
    }
    return "UNKNOWN";
}

/**
 * @brief 获取中断类型字符串
 */
const char* get_interrupt_type_string(interrupt_type_t type)
{
    if (type >= 0 && type < ARRAY_SIZE(INTERRUPT_TYPE_STRINGS)) {
        return INTERRUPT_TYPE_STRINGS[type];
    }
    return "UNKNOWN";
}

/**
 * @brief 格式化传感器1数据为字符串
 */
system_status_t format_sensor1_data(const sensor1_data_t* sensor_data, 
                                   char* buffer, size_t buffer_size)
{
    if (sensor_data == NULL || buffer == NULL || buffer_size == 0) {
        return SYSTEM_ERROR;
    }
    
    /* IAR 5.3兼容的sprintf使用 */
    {
        int ret = sprintf(buffer, 
            "ID:%s,Sensor:%s,Temp:%.2f,Humid:%.2f,Status:%s,Time:%lu",
            sensor_data->student_id,
            sensor_data->sensor_name,
            sensor_data->temperature,
            sensor_data->humidity,
            get_sensor_status_string(sensor_data->status),
            sensor_data->timestamp);
        
        if (ret < 0 || (size_t)ret >= buffer_size) {
            return SYSTEM_ERROR;
        }
    }
    
    return SYSTEM_OK;
}

/**
 * @brief 格式化传感器2数据为字符串
 */
system_status_t format_sensor2_data(const sensor2_data_t* sensor_data, 
                                   char* buffer, size_t buffer_size)
{
    if (sensor_data == NULL || buffer == NULL || buffer_size == 0) {
        return SYSTEM_ERROR;
    }
    
    /* IAR 5.3兼容的sprintf使用 */
    {
        int ret = sprintf(buffer,
            "ID:%s,Sensor:%s,IntType:%s,Count:%lu,Status:%s,Time:%lu",
            sensor_data->student_id,
            sensor_data->sensor_name,
            get_interrupt_type_string(sensor_data->interrupt_type),
            sensor_data->interrupt_count,
            get_sensor_status_string(sensor_data->status),
            sensor_data->timestamp);
        
        if (ret < 0 || (size_t)ret >= buffer_size) {
            return SYSTEM_ERROR;
        }
    }
    
    return SYSTEM_OK;
}

/**
 * @brief 计算数据校验和
 */
uint16_t calculate_checksum(const uint8_t* data, size_t length)
{
    uint16_t checksum = 0;
    size_t i;
    
    if (data == NULL) {
        return 0;
    }
    
    for (i = 0; i < length; i++) {
        checksum += data[i];
    }
    
    return checksum;
}

/**
 * @brief 获取当前时间戳
 */
uint32_t get_timestamp(void)
{
    /* 简化的时间戳实现 - 使用系统滴答计数 */
    static uint32_t tick_count = 0;
    return ++tick_count;
}

/**
 * @brief 重置传感器数据统计
 */
void reset_sensor_statistics(void)
{
    total_data_count = 0;
    valid_data_count = 0;
    error_data_count = 0;
}

/**
 * @brief 获取传感器数据统计信息
 */
void get_sensor_statistics(uint32_t* total_count, uint32_t* valid_count, uint32_t* error_count)
{
    if (total_count != NULL) {
        *total_count = total_data_count;
    }
    if (valid_count != NULL) {
        *valid_count = valid_data_count;
    }
    if (error_count != NULL) {
        *error_count = error_data_count;
    }
}

/**
 * @brief 设置传感器数据回调函数
 */
void set_sensor_data_callback(void (*callback)(const sensor_data_t* data))
{
    data_callback = callback;
}

/* 内部函数实现 */

/**
 * @brief 检查字符串是否为数字
 */
static bool is_numeric_string(const char* str)
{
    int i = 0;
    bool has_dot = false;
    
    if (str == NULL || str[0] == '\0') {
        return false;
    }
    
    /* 允许负号 */
    if (str[0] == '-') {
        i = 1;
    }
    
    for (; str[i] != '\0'; i++) {
        if (str[i] == '.') {
            if (has_dot) {
                return false; /* 多个小数点 */
            }
            has_dot = true;
        } else if (str[i] < '0' || str[i] > '9') {
            return false;
        }
    }
    
    return i > (str[0] == '-' ? 1 : 0); /* 确保有数字 */
}

/**
 * @brief 安全的浮点数解析
 */
static float parse_float_safe(const char* str)
{
    if (str == NULL) {
        return 0.0f;
    }
    
    /* IAR 5.3兼容的atof使用 */
    return (float)atof(str);
}

/**
 * @brief 安全的整数解析
 */
static int parse_int_safe(const char* str)
{
    if (str == NULL) {
        return 0;
    }
    
    return atoi(str);
}

/**
 * @brief 去除字符串首尾空白字符
 */
static void trim_whitespace(char* str)
{
    char* start;
    char* end;
    size_t len;
    
    if (str == NULL) {
        return;
    }
    
    /* 去除开头空白 */
    start = str;
    while (*start == ' ' || *start == '\t' || *start == '\r' || *start == '\n') {
        start++;
    }
    
    /* 计算长度 */
    len = strlen(start);
    if (len == 0) {
        str[0] = '\0';
        return;
    }
    
    /* 去除结尾空白 */
    end = start + len - 1;
    while (end > start && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) {
        end--;
    }
    
    /* 复制结果 */
    len = end - start + 1;
    memmove(str, start, len);
    str[len] = '\0';
}

/**
 * @brief 确定传感器1状态
 */
static sensor_status_t determine_sensor1_status(const sensor1_data_t* data)
{
    if (data == NULL) {
        return SENSOR_STATUS_ERROR;
    }
    
    /* 检查温度警告范围 */
    if (data->temperature < -20.0f || data->temperature > 60.0f) {
        return SENSOR_STATUS_WARNING;
    }
    
    /* 检查湿度警告范围 */
    if (data->humidity < 10.0f || data->humidity > 90.0f) {
        return SENSOR_STATUS_WARNING;
    }
    
    return SENSOR_STATUS_NORMAL;
}

/**
 * @brief 确定传感器2状态
 */
static sensor_status_t determine_sensor2_status(const sensor2_data_t* data)
{
    if (data == NULL) {
        return SENSOR_STATUS_ERROR;
    }
    
    /* 根据中断类型确定状态 */
    if (data->interrupt_type == INTERRUPT_TYPE_NONE) {
        return SENSOR_STATUS_NORMAL;
    } else {
        return SENSOR_STATUS_WARNING; /* 有中断发生 */
    }
}