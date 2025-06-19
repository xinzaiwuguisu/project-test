#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

/* 传感器状态枚举 */
typedef enum {
    SENSOR_STATUS_NORMAL = 0,
    SENSOR_STATUS_WARNING,
    SENSOR_STATUS_ERROR,
    SENSOR_STATUS_OFFLINE
} sensor_status_t;

/* 传感器1数据结构（温湿度传感器） */
typedef struct {
    char student_id[MAX_STUDENT_ID_LENGTH];
    char sensor_name[16];
    float temperature;
    float humidity;
    sensor_status_t status;
    uint32_t timestamp;
} sensor1_data_t;

/* 传感器2数据结构（中断传感器） */
typedef struct {
    char student_id[MAX_STUDENT_ID_LENGTH];
    char sensor_name[16];
    uint8_t interrupt_type;
    uint32_t interrupt_count;
    sensor_status_t status;
    uint32_t timestamp;
} sensor2_data_t;

/* 数据解析结果 */
typedef enum {
    PARSE_SUCCESS = 0,
    PARSE_ERROR_FORMAT,
    PARSE_ERROR_LENGTH,
    PARSE_ERROR_CHECKSUM
} parse_result_t;

/* 函数声明 */
parse_result_t parse_sensor1_data(const char* raw_data, sensor1_data_t* sensor_data);
parse_result_t parse_sensor2_data(const char* raw_data, sensor2_data_t* sensor_data);
uint32_t get_system_timestamp(void);
const char* get_status_string(sensor_status_t status);

#endif /* SENSOR_DATA_H */