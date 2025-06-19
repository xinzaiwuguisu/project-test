/**
 * @file sensor_data.h
 * @brief 传感器数据处理模块头文件 - IAR 5.3兼容版本
 * @author OpenHands
 * @date 2025-06-19
 * @version 1.0.0
 */

#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include "config.h"

/* 传感器1数据结构（温湿度传感器） */
typedef struct {
    char student_id[MAX_STUDENT_ID_LEN];    /* 学号姓名缩写 */
    char sensor_name[MAX_SENSOR_NAME_LEN];  /* 传感器名称 */
    float temperature;                       /* 温度值 */
    float humidity;                         /* 湿度值 */
    sensor_status_t status;                 /* 传感器状态 */
    uint32_t timestamp;                     /* 时间戳 */
} sensor1_data_t;

/* 传感器2数据结构（中断传感器） */
typedef struct {
    char student_id[MAX_STUDENT_ID_LEN];    /* 学号姓名缩写 */
    char sensor_name[MAX_SENSOR_NAME_LEN];  /* 传感器名称 */
    interrupt_type_t interrupt_type;        /* 中断类型 */
    uint32_t interrupt_count;               /* 中断次数 */
    sensor_status_t status;                 /* 传感器状态 */
    uint32_t timestamp;                     /* 时间戳 */
} sensor2_data_t;

/* 通用传感器数据结构 */
typedef struct {
    sensor_type_t type;                     /* 传感器类型 */
    union {
        sensor1_data_t sensor1;             /* 传感器1数据 */
        sensor2_data_t sensor2;             /* 传感器2数据 */
    } data;
} sensor_data_t;

/* 数据解析结果结构 */
typedef struct {
    bool is_valid;                          /* 数据是否有效 */
    sensor_type_t type;                     /* 传感器类型 */
    char error_msg[64];                     /* 错误信息 */
} parse_result_t;

/* 函数声明 */

/**
 * @brief 初始化传感器数据处理模块
 * @return system_status_t 初始化状态
 */
system_status_t sensor_data_init(void);

/**
 * @brief 解析传感器1数据（温湿度）
 * @param data_str 输入数据字符串
 * @param sensor_data 输出传感器数据结构
 * @return parse_result_t 解析结果
 */
parse_result_t parse_sensor1_data(const char* data_str, sensor1_data_t* sensor_data);

/**
 * @brief 解析传感器2数据（中断）
 * @param data_str 输入数据字符串
 * @param sensor_data 输出传感器数据结构
 * @return parse_result_t 解析结果
 */
parse_result_t parse_sensor2_data(const char* data_str, sensor2_data_t* sensor_data);

/**
 * @brief 自动识别并解析传感器数据
 * @param data_str 输入数据字符串
 * @param sensor_data 输出传感器数据结构
 * @return parse_result_t 解析结果
 */
parse_result_t parse_sensor_data(const char* data_str, sensor_data_t* sensor_data);

/**
 * @brief 验证传感器1数据有效性
 * @param sensor_data 传感器数据结构
 * @return bool 数据是否有效
 */
bool validate_sensor1_data(const sensor1_data_t* sensor_data);

/**
 * @brief 验证传感器2数据有效性
 * @param sensor_data 传感器数据结构
 * @return bool 数据是否有效
 */
bool validate_sensor2_data(const sensor2_data_t* sensor_data);

/**
 * @brief 获取传感器状态字符串
 * @param status 传感器状态
 * @return const char* 状态字符串
 */
const char* get_sensor_status_string(sensor_status_t status);

/**
 * @brief 获取中断类型字符串
 * @param type 中断类型
 * @return const char* 类型字符串
 */
const char* get_interrupt_type_string(interrupt_type_t type);

/**
 * @brief 格式化传感器1数据为字符串
 * @param sensor_data 传感器数据结构
 * @param buffer 输出缓冲区
 * @param buffer_size 缓冲区大小
 * @return system_status_t 操作状态
 */
system_status_t format_sensor1_data(const sensor1_data_t* sensor_data, 
                                   char* buffer, size_t buffer_size);

/**
 * @brief 格式化传感器2数据为字符串
 * @param sensor_data 传感器数据结构
 * @param buffer 输出缓冲区
 * @param buffer_size 缓冲区大小
 * @return system_status_t 操作状态
 */
system_status_t format_sensor2_data(const sensor2_data_t* sensor_data, 
                                   char* buffer, size_t buffer_size);

/**
 * @brief 计算数据校验和
 * @param data 数据指针
 * @param length 数据长度
 * @return uint16_t 校验和
 */
uint16_t calculate_checksum(const uint8_t* data, size_t length);

/**
 * @brief 获取当前时间戳
 * @return uint32_t 时间戳
 */
uint32_t get_timestamp(void);

/**
 * @brief 重置传感器数据统计
 */
void reset_sensor_statistics(void);

/**
 * @brief 获取传感器数据统计信息
 * @param total_count 总数据包数
 * @param valid_count 有效数据包数
 * @param error_count 错误数据包数
 */
void get_sensor_statistics(uint32_t* total_count, uint32_t* valid_count, uint32_t* error_count);

/**
 * @brief 设置传感器数据回调函数
 * @param callback 回调函数指针
 */
void set_sensor_data_callback(void (*callback)(const sensor_data_t* data));

/* 内联函数定义 - IAR 5.3兼容 */
#ifdef IAR_LEGACY_SUPPORT
    /* IAR 5.3使用pragma inline */
    #pragma inline
    static bool is_valid_student_id(const char* student_id)
    {
        if (student_id == NULL || strlen(student_id) == 0) {
            return false;
        }
        return strlen(student_id) < MAX_STUDENT_ID_LEN;
    }
    
    #pragma inline
    static bool is_valid_sensor_name(const char* sensor_name)
    {
        if (sensor_name == NULL || strlen(sensor_name) == 0) {
            return false;
        }
        return strlen(sensor_name) < MAX_SENSOR_NAME_LEN;
    }
#else
    /* 现代编译器使用inline关键字 */
    static inline bool is_valid_student_id(const char* student_id)
    {
        if (student_id == NULL || strlen(student_id) == 0) {
            return false;
        }
        return strlen(student_id) < MAX_STUDENT_ID_LEN;
    }
    
    static inline bool is_valid_sensor_name(const char* sensor_name)
    {
        if (sensor_name == NULL || strlen(sensor_name) == 0) {
            return false;
        }
        return strlen(sensor_name) < MAX_SENSOR_NAME_LEN;
    }
#endif

/* 常量定义 */
extern const char* const SENSOR_STATUS_STRINGS[];
extern const char* const INTERRUPT_TYPE_STRINGS[];

/* 温湿度范围定义 */
#define MIN_TEMPERATURE         -40.0f
#define MAX_TEMPERATURE         85.0f
#define MIN_HUMIDITY            0.0f
#define MAX_HUMIDITY            100.0f

/* 数据格式定义 */
#define SENSOR1_DATA_FORMAT     "%[^,],%f,%f"           /* 学号,温度,湿度 */
#define SENSOR2_DATA_FORMAT     "%[^,],%[^,],%d"        /* 学号,传感器名,中断 */

/* 错误消息定义 */
#define ERROR_MSG_INVALID_FORMAT    "Invalid data format"
#define ERROR_MSG_INVALID_RANGE     "Data out of range"
#define ERROR_MSG_INVALID_ID        "Invalid student ID"
#define ERROR_MSG_INVALID_NAME      "Invalid sensor name"
#define ERROR_MSG_BUFFER_TOO_SMALL  "Buffer too small"

#endif /* SENSOR_DATA_H */