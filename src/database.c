#include "database.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* 数据库连接实例 */
static db_connection_t db_conn;

/* SQL查询缓冲区 */
static char sql_buffer[512];

/**
 * @brief 初始化数据库模块
 * @return 操作结果
 */
db_result_t database_init(void)
{
    /* 初始化连接参数 */
    strcpy(db_conn.host, DB_HOST);
    db_conn.port = DB_PORT;
    strcpy(db_conn.username, DB_USER);
    strcpy(db_conn.password, DB_PASSWORD);
    strcpy(db_conn.database, DB_NAME);
    db_conn.status = DB_STATUS_DISCONNECTED;
    
    return DB_SUCCESS;
}

/**
 * @brief 连接到数据库
 * @return 操作结果
 */
db_result_t database_connect(void)
{
    /* 在实际的嵌入式系统中，这里需要实现具体的MySQL连接逻辑
     * 由于嵌入式系统资源限制，通常通过以下方式之一实现：
     * 1. 使用轻量级MySQL客户端库
     * 2. 通过串口或网络发送数据到上位机处理
     * 3. 使用HTTP API方式发送数据
     */
    
    printf("Connecting to database: %s@%s:%d/%s\n", 
           db_conn.username, db_conn.host, db_conn.port, db_conn.database);
    
    /* 模拟连接过程 */
    db_conn.status = DB_STATUS_CONNECTED;
    
    return DB_SUCCESS;
}

/**
 * @brief 断开数据库连接
 */
void database_disconnect(void)
{
    if (db_conn.status == DB_STATUS_CONNECTED) {
        printf("Disconnecting from database...\n");
        db_conn.status = DB_STATUS_DISCONNECTED;
    }
}

/**
 * @brief 插入传感器1数据到数据库
 * @param data 传感器1数据
 * @return 操作结果
 */
db_result_t database_insert_sensor1_data(const sensor1_data_t* data)
{
    if (!data || db_conn.status != DB_STATUS_CONNECTED) {
        return DB_ERROR_CONNECTION;
    }
    
    /* 构建SQL插入语句 */
    snprintf(sql_buffer, sizeof(sql_buffer),
        "INSERT INTO %s (student_id, sensor_name, temperature, humidity, status, timestamp) "
        "VALUES ('%s', '%s', %.2f, %.2f, '%s', %u)",
        TABLE_SENSOR1,
        data->student_id,
        data->sensor_name,
        data->temperature,
        data->humidity,
        get_status_string(data->status),
        data->timestamp
    );
    
    printf("SQL: %s\n", sql_buffer);
    
    /* 在实际实现中，这里需要执行SQL语句 */
    /* 模拟执行成功 */
    printf("Sensor1 data inserted successfully\n");
    
    return DB_SUCCESS;
}

/**
 * @brief 插入传感器2数据到数据库
 * @param data 传感器2数据
 * @return 操作结果
 */
db_result_t database_insert_sensor2_data(const sensor2_data_t* data)
{
    if (!data || db_conn.status != DB_STATUS_CONNECTED) {
        return DB_ERROR_CONNECTION;
    }
    
    /* 构建SQL插入语句 */
    snprintf(sql_buffer, sizeof(sql_buffer),
        "INSERT INTO %s (student_id, sensor_name, interrupt_type, interrupt_count, status, timestamp) "
        "VALUES ('%s', '%s', %u, %u, '%s', %u)",
        TABLE_SENSOR2,
        data->student_id,
        data->sensor_name,
        data->interrupt_type,
        data->interrupt_count,
        get_status_string(data->status),
        data->timestamp
    );
    
    printf("SQL: %s\n", sql_buffer);
    
    /* 在实际实现中，这里需要执行SQL语句 */
    /* 模拟执行成功 */
    printf("Sensor2 data inserted successfully\n");
    
    return DB_SUCCESS;
}

/**
 * @brief 获取数据库连接状态
 * @return 连接状态
 */
db_status_t database_get_status(void)
{
    return db_conn.status;
}

/**
 * @brief 获取错误信息字符串
 * @param result 错误代码
 * @return 错误信息字符串
 */
const char* database_get_error_string(db_result_t result)
{
    switch (result) {
        case DB_SUCCESS:         return "Success";
        case DB_ERROR_CONNECTION: return "Connection error";
        case DB_ERROR_QUERY:     return "Query error";
        case DB_ERROR_MEMORY:    return "Memory error";
        case DB_ERROR_TIMEOUT:   return "Timeout error";
        default:                 return "Unknown error";
    }
}

/**
 * @brief 查询传感器1最新数据
 * @param data 输出数据结构
 * @param student_id 学号姓名缩写
 * @return 操作结果
 */
db_result_t database_query_sensor1_latest(sensor1_data_t* data, const char* student_id)
{
    if (!data || !student_id || db_conn.status != DB_STATUS_CONNECTED) {
        return DB_ERROR_CONNECTION;
    }
    
    /* 构建SQL查询语句 */
    snprintf(sql_buffer, sizeof(sql_buffer),
        "SELECT * FROM %s WHERE student_id='%s' ORDER BY timestamp DESC LIMIT 1",
        TABLE_SENSOR1, student_id
    );
    
    printf("Query SQL: %s\n", sql_buffer);
    
    /* 在实际实现中，这里需要执行查询并填充data结构 */
    
    return DB_SUCCESS;
}

/**
 * @brief 查询传感器2最新数据
 * @param data 输出数据结构
 * @param student_id 学号姓名缩写
 * @return 操作结果
 */
db_result_t database_query_sensor2_latest(sensor2_data_t* data, const char* student_id)
{
    if (!data || !student_id || db_conn.status != DB_STATUS_CONNECTED) {
        return DB_ERROR_CONNECTION;
    }
    
    /* 构建SQL查询语句 */
    snprintf(sql_buffer, sizeof(sql_buffer),
        "SELECT * FROM %s WHERE student_id='%s' ORDER BY timestamp DESC LIMIT 1",
        TABLE_SENSOR2, student_id
    );
    
    printf("Query SQL: %s\n", sql_buffer);
    
    /* 在实际实现中，这里需要执行查询并填充data结构 */
    
    return DB_SUCCESS;
}