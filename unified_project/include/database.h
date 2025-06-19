/**
 * @file database.h
 * @brief 数据库操作模块头文件 - IAR 5.3兼容版本
 * @author OpenHands
 * @date 2025-06-19
 * @version 1.0.0
 */

#ifndef DATABASE_H
#define DATABASE_H

#include "config.h"
#include "sensor_data.h"

/* 数据库连接状态 */
typedef enum {
    DB_STATUS_DISCONNECTED = 0,
    DB_STATUS_CONNECTED = 1,
    DB_STATUS_ERROR = 2,
    DB_STATUS_BUSY = 3
} db_status_t;

/* 数据库操作结果 */
typedef struct {
    bool success;                       /* 操作是否成功 */
    int error_code;                     /* 错误代码 */
    char error_message[128];            /* 错误信息 */
    uint32_t affected_rows;             /* 影响的行数 */
    uint32_t insert_id;                 /* 插入记录的ID */
} db_result_t;

/* 数据库连接参数 */
typedef struct {
    char host[64];                      /* 主机地址 */
    uint16_t port;                      /* 端口号 */
    char username[32];                  /* 用户名 */
    char password[32];                  /* 密码 */
    char database[32];                  /* 数据库名 */
    uint32_t timeout;                   /* 连接超时时间 */
} db_config_t;

/* 查询结果结构 */
typedef struct {
    uint32_t row_count;                 /* 行数 */
    uint32_t column_count;              /* 列数 */
    char** data;                        /* 数据指针 */
    char** column_names;                /* 列名 */
} db_query_result_t;

/* 函数声明 */

/**
 * @brief 初始化数据库模块
 * @return system_status_t 初始化状态
 */
system_status_t database_init(void);

/**
 * @brief 连接到数据库
 * @param config 数据库配置参数
 * @return db_result_t 连接结果
 */
db_result_t database_connect(const db_config_t* config);

/**
 * @brief 断开数据库连接
 * @return db_result_t 断开结果
 */
db_result_t database_disconnect(void);

/**
 * @brief 获取数据库连接状态
 * @return db_status_t 连接状态
 */
db_status_t database_get_status(void);

/**
 * @brief 插入传感器1数据到数据库
 * @param data 传感器1数据
 * @return db_result_t 插入结果
 */
db_result_t database_insert_sensor1_data(const sensor1_data_t* data);

/**
 * @brief 插入传感器2数据到数据库
 * @param data 传感器2数据
 * @return db_result_t 插入结果
 */
db_result_t database_insert_sensor2_data(const sensor2_data_t* data);

/**
 * @brief 通用传感器数据插入
 * @param data 传感器数据
 * @return db_result_t 插入结果
 */
db_result_t database_insert_sensor_data(const sensor_data_t* data);

/**
 * @brief 查询传感器1数据
 * @param student_id 学号（可为NULL查询所有）
 * @param limit 限制返回行数（0表示无限制）
 * @return db_query_result_t 查询结果
 */
db_query_result_t database_query_sensor1_data(const char* student_id, uint32_t limit);

/**
 * @brief 查询传感器2数据
 * @param student_id 学号（可为NULL查询所有）
 * @param limit 限制返回行数（0表示无限制）
 * @return db_query_result_t 查询结果
 */
db_query_result_t database_query_sensor2_data(const char* student_id, uint32_t limit);

/**
 * @brief 执行自定义SQL查询
 * @param sql SQL语句
 * @return db_query_result_t 查询结果
 */
db_query_result_t database_execute_query(const char* sql);

/**
 * @brief 执行SQL更新语句
 * @param sql SQL语句
 * @return db_result_t 执行结果
 */
db_result_t database_execute_update(const char* sql);

/**
 * @brief 释放查询结果内存
 * @param result 查询结果指针
 */
void database_free_query_result(db_query_result_t* result);

/**
 * @brief 开始事务
 * @return db_result_t 操作结果
 */
db_result_t database_begin_transaction(void);

/**
 * @brief 提交事务
 * @return db_result_t 操作结果
 */
db_result_t database_commit_transaction(void);

/**
 * @brief 回滚事务
 * @return db_result_t 操作结果
 */
db_result_t database_rollback_transaction(void);

/**
 * @brief 检查数据库表是否存在
 * @param table_name 表名
 * @return bool 表是否存在
 */
bool database_table_exists(const char* table_name);

/**
 * @brief 创建数据库表
 * @return db_result_t 创建结果
 */
db_result_t database_create_tables(void);

/**
 * @brief 获取数据库统计信息
 * @param sensor1_count 传感器1数据条数
 * @param sensor2_count 传感器2数据条数
 * @return db_result_t 操作结果
 */
db_result_t database_get_statistics(uint32_t* sensor1_count, uint32_t* sensor2_count);

/**
 * @brief 清理过期数据
 * @param days_old 保留天数
 * @return db_result_t 清理结果
 */
db_result_t database_cleanup_old_data(uint32_t days_old);

/**
 * @brief 备份数据库
 * @param backup_path 备份文件路径
 * @return db_result_t 备份结果
 */
db_result_t database_backup(const char* backup_path);

/**
 * @brief 恢复数据库
 * @param backup_path 备份文件路径
 * @return db_result_t 恢复结果
 */
db_result_t database_restore(const char* backup_path);

/**
 * @brief 设置数据库错误回调函数
 * @param callback 回调函数指针
 */
void database_set_error_callback(void (*callback)(const char* error_msg));

/**
 * @brief 获取最后一次错误信息
 * @return const char* 错误信息
 */
const char* database_get_last_error(void);

/* 内联函数 - IAR 5.3兼容 */
#ifdef IAR_LEGACY_SUPPORT
    #pragma inline
    static bool is_valid_db_config(const db_config_t* config)
    {
        if (config == NULL) return false;
        if (strlen(config->host) == 0) return false;
        if (config->port == 0) return false;
        if (strlen(config->username) == 0) return false;
        if (strlen(config->database) == 0) return false;
        return true;
    }
#else
    static inline bool is_valid_db_config(const db_config_t* config)
    {
        if (config == NULL) return false;
        if (strlen(config->host) == 0) return false;
        if (config->port == 0) return false;
        if (strlen(config->username) == 0) return false;
        if (strlen(config->database) == 0) return false;
        return true;
    }
#endif

/* 常量定义 */
#define MAX_SQL_LENGTH          1024        /* 最大SQL语句长度 */
#define MAX_QUERY_RESULTS       1000        /* 最大查询结果数 */
#define DB_RETRY_COUNT          3           /* 数据库重试次数 */
#define DB_RETRY_DELAY_MS       1000        /* 重试延迟时间 */

/* SQL语句模板 */
#define SQL_INSERT_SENSOR1 \
    "INSERT INTO sensor1_data (student_id, sensor_name, temperature, humidity, status, timestamp) " \
    "VALUES ('%s', '%s', %.2f, %.2f, '%s', %lu)"

#define SQL_INSERT_SENSOR2 \
    "INSERT INTO sensor2_data (student_id, sensor_name, interrupt_type, interrupt_count, status, timestamp) " \
    "VALUES ('%s', '%s', %d, %lu, '%s', %lu)"

#define SQL_SELECT_SENSOR1_ALL \
    "SELECT * FROM sensor1_data ORDER BY created_at DESC"

#define SQL_SELECT_SENSOR1_BY_ID \
    "SELECT * FROM sensor1_data WHERE student_id = '%s' ORDER BY created_at DESC"

#define SQL_SELECT_SENSOR2_ALL \
    "SELECT * FROM sensor2_data ORDER BY created_at DESC"

#define SQL_SELECT_SENSOR2_BY_ID \
    "SELECT * FROM sensor2_data WHERE student_id = '%s' ORDER BY created_at DESC"

#define SQL_COUNT_SENSOR1 \
    "SELECT COUNT(*) FROM sensor1_data"

#define SQL_COUNT_SENSOR2 \
    "SELECT COUNT(*) FROM sensor2_data"

#define SQL_CREATE_SENSOR1_TABLE \
    "CREATE TABLE IF NOT EXISTS sensor1_data (" \
    "id INT AUTO_INCREMENT PRIMARY KEY, " \
    "student_id VARCHAR(20) NOT NULL, " \
    "sensor_name VARCHAR(16) NOT NULL, " \
    "temperature DECIMAL(5,2) NOT NULL, " \
    "humidity DECIMAL(5,2) NOT NULL, " \
    "status VARCHAR(10) NOT NULL, " \
    "timestamp INT UNSIGNED NOT NULL, " \
    "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, " \
    "INDEX idx_student_id (student_id), " \
    "INDEX idx_timestamp (timestamp)" \
    ")"

#define SQL_CREATE_SENSOR2_TABLE \
    "CREATE TABLE IF NOT EXISTS sensor2_data (" \
    "id INT AUTO_INCREMENT PRIMARY KEY, " \
    "student_id VARCHAR(20) NOT NULL, " \
    "sensor_name VARCHAR(16) NOT NULL, " \
    "interrupt_type TINYINT NOT NULL, " \
    "interrupt_count INT UNSIGNED NOT NULL, " \
    "status VARCHAR(10) NOT NULL, " \
    "timestamp INT UNSIGNED NOT NULL, " \
    "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, " \
    "INDEX idx_student_id (student_id), " \
    "INDEX idx_sensor_name (sensor_name), " \
    "INDEX idx_timestamp (timestamp)" \
    ")"

/* 错误代码定义 */
#define DB_ERROR_NONE           0
#define DB_ERROR_CONNECTION     1001
#define DB_ERROR_QUERY          1002
#define DB_ERROR_INSERT         1003
#define DB_ERROR_UPDATE         1004
#define DB_ERROR_DELETE         1005
#define DB_ERROR_TRANSACTION    1006
#define DB_ERROR_TIMEOUT        1007
#define DB_ERROR_MEMORY         1008
#define DB_ERROR_INVALID_PARAM  1009
#define DB_ERROR_TABLE_NOT_EXIST 1010

/* 默认配置 */
extern const db_config_t DEFAULT_DB_CONFIG;

#endif /* DATABASE_H */