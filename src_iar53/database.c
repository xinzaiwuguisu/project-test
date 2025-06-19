/**
 * @file database.c
 * @brief 数据库操作模块实现 - IAR 5.3兼容版本
 * @author OpenHands
 * @date 2025-06-19
 * @version 1.0.0
 */

#include "database.h"

/* 静态变量 */
static db_status_t current_status = DB_STATUS_DISCONNECTED;
static char last_error_message[256] = "";
static void (*error_callback)(const char* error_msg) = NULL;
static db_config_t current_config;

/* 默认数据库配置 */
const db_config_t DEFAULT_DB_CONFIG = {
    DB_HOST,        /* host */
    DB_PORT,        /* port */
    DB_USER,        /* username */
    DB_PASSWORD,    /* password */
    DB_NAME,        /* database */
    DB_TIMEOUT      /* timeout */
};

/* 内部函数声明 */
static void set_last_error(int error_code, const char* error_msg);
static db_result_t create_error_result(int error_code, const char* error_msg);
static db_result_t create_success_result(uint32_t affected_rows, uint32_t insert_id);
static bool validate_sql_injection(const char* input);
static char* escape_string(const char* input, char* output, size_t output_size);
static void simulate_database_delay(void);

/**
 * @brief 初始化数据库模块
 */
system_status_t database_init(void)
{
    /* 初始化状态 */
    current_status = DB_STATUS_DISCONNECTED;
    memset(last_error_message, 0, sizeof(last_error_message));
    error_callback = NULL;
    
    /* 初始化默认配置 */
    memcpy(&current_config, &DEFAULT_DB_CONFIG, sizeof(db_config_t));
    
    DEBUG_PRINT("Database module initialized");
    return SYSTEM_OK;
}

/**
 * @brief 连接到数据库
 */
db_result_t database_connect(const db_config_t* config)
{
    db_result_t result;
    
    /* 参数验证 */
    if (!is_valid_db_config(config)) {
        return create_error_result(DB_ERROR_INVALID_PARAM, "Invalid database configuration");
    }
    
    /* 检查当前状态 */
    if (current_status == DB_STATUS_CONNECTED) {
        DEBUG_PRINT("Database already connected");
        return create_success_result(0, 0);
    }
    
    /* 保存配置 */
    memcpy(&current_config, config, sizeof(db_config_t));
    
    /* 模拟数据库连接过程 */
    DEBUG_PRINT("Connecting to database %s:%d", config->host, config->port);
    simulate_database_delay();
    
    /* 在实际项目中，这里应该是真实的MySQL连接代码 */
    /* 由于IAR 5.3环境限制，这里使用模拟实现 */
    
    current_status = DB_STATUS_CONNECTED;
    result = create_success_result(0, 0);
    
    INFO_PRINT("Database connected successfully");
    return result;
}

/**
 * @brief 断开数据库连接
 */
db_result_t database_disconnect(void)
{
    if (current_status == DB_STATUS_DISCONNECTED) {
        return create_success_result(0, 0);
    }
    
    /* 模拟断开连接 */
    simulate_database_delay();
    current_status = DB_STATUS_DISCONNECTED;
    
    DEBUG_PRINT("Database disconnected");
    return create_success_result(0, 0);
}

/**
 * @brief 获取数据库连接状态
 */
db_status_t database_get_status(void)
{
    return current_status;
}

/**
 * @brief 插入传感器1数据到数据库
 */
db_result_t database_insert_sensor1_data(const sensor1_data_t* data)
{
    char sql[MAX_SQL_LENGTH];
    char escaped_id[MAX_STUDENT_ID_LEN * 2];
    char escaped_name[MAX_SENSOR_NAME_LEN * 2];
    
    /* 参数检查 */
    if (data == NULL) {
        return create_error_result(DB_ERROR_INVALID_PARAM, "Null sensor data");
    }
    
    /* 连接状态检查 */
    if (current_status != DB_STATUS_CONNECTED) {
        return create_error_result(DB_ERROR_CONNECTION, "Database not connected");
    }
    
    /* 数据验证 */
    if (!validate_sensor1_data(data)) {
        return create_error_result(DB_ERROR_INVALID_PARAM, "Invalid sensor data");
    }
    
    /* SQL注入防护 */
    if (!validate_sql_injection(data->student_id) || 
        !validate_sql_injection(data->sensor_name)) {
        return create_error_result(DB_ERROR_INVALID_PARAM, "Invalid characters in data");
    }
    
    /* 转义字符串 */
    escape_string(data->student_id, escaped_id, sizeof(escaped_id));
    escape_string(data->sensor_name, escaped_name, sizeof(escaped_name));
    
    /* 构建SQL语句 */
    sprintf(sql, SQL_INSERT_SENSOR1,
            escaped_id,
            escaped_name,
            data->temperature,
            data->humidity,
            get_sensor_status_string(data->status),
            data->timestamp);
    
    DEBUG_PRINT("Executing SQL: %s", sql);
    
    /* 模拟数据库插入 */
    simulate_database_delay();
    
    /* 在实际项目中，这里应该执行真实的SQL插入操作 */
    /* 模拟成功插入 */
    INFO_PRINT("Sensor1 data inserted: ID=%s, Temp=%.2f, Humid=%.2f", 
               data->student_id, data->temperature, data->humidity);
    
    return create_success_result(1, 0); /* 影响1行，插入ID由数据库自动生成 */
}

/**
 * @brief 插入传感器2数据到数据库
 */
db_result_t database_insert_sensor2_data(const sensor2_data_t* data)
{
    char sql[MAX_SQL_LENGTH];
    char escaped_id[MAX_STUDENT_ID_LEN * 2];
    char escaped_name[MAX_SENSOR_NAME_LEN * 2];
    
    /* 参数检查 */
    if (data == NULL) {
        return create_error_result(DB_ERROR_INVALID_PARAM, "Null sensor data");
    }
    
    /* 连接状态检查 */
    if (current_status != DB_STATUS_CONNECTED) {
        return create_error_result(DB_ERROR_CONNECTION, "Database not connected");
    }
    
    /* 数据验证 */
    if (!validate_sensor2_data(data)) {
        return create_error_result(DB_ERROR_INVALID_PARAM, "Invalid sensor data");
    }
    
    /* SQL注入防护 */
    if (!validate_sql_injection(data->student_id) || 
        !validate_sql_injection(data->sensor_name)) {
        return create_error_result(DB_ERROR_INVALID_PARAM, "Invalid characters in data");
    }
    
    /* 转义字符串 */
    escape_string(data->student_id, escaped_id, sizeof(escaped_id));
    escape_string(data->sensor_name, escaped_name, sizeof(escaped_name));
    
    /* 构建SQL语句 */
    sprintf(sql, SQL_INSERT_SENSOR2,
            escaped_id,
            escaped_name,
            (int)data->interrupt_type,
            data->interrupt_count,
            get_sensor_status_string(data->status),
            data->timestamp);
    
    DEBUG_PRINT("Executing SQL: %s", sql);
    
    /* 模拟数据库插入 */
    simulate_database_delay();
    
    /* 在实际项目中，这里应该执行真实的SQL插入操作 */
    /* 模拟成功插入 */
    INFO_PRINT("Sensor2 data inserted: ID=%s, Sensor=%s, IntType=%d", 
               data->student_id, data->sensor_name, data->interrupt_type);
    
    return create_success_result(1, 0); /* 影响1行，插入ID由数据库自动生成 */
}

/**
 * @brief 通用传感器数据插入
 */
db_result_t database_insert_sensor_data(const sensor_data_t* data)
{
    if (data == NULL) {
        return create_error_result(DB_ERROR_INVALID_PARAM, "Null sensor data");
    }
    
    switch (data->type) {
        case SENSOR_TYPE_TEMP_HUMIDITY:
            return database_insert_sensor1_data(&data->data.sensor1);
            
        case SENSOR_TYPE_INTERRUPT:
            return database_insert_sensor2_data(&data->data.sensor2);
            
        default:
            return create_error_result(DB_ERROR_INVALID_PARAM, "Unknown sensor type");
    }
}

/**
 * @brief 查询传感器1数据
 */
db_query_result_t database_query_sensor1_data(const char* student_id, uint32_t limit)
{
    db_query_result_t result;
    char sql[MAX_SQL_LENGTH];
    
    /* 初始化结果 */
    memset(&result, 0, sizeof(db_query_result_t));
    
    /* 连接状态检查 */
    if (current_status != DB_STATUS_CONNECTED) {
        set_last_error(DB_ERROR_CONNECTION, "Database not connected");
        return result;
    }
    
    /* 构建SQL语句 */
    if (student_id != NULL && strlen(student_id) > 0) {
        if (!validate_sql_injection(student_id)) {
            set_last_error(DB_ERROR_INVALID_PARAM, "Invalid student ID");
            return result;
        }
        sprintf(sql, SQL_SELECT_SENSOR1_BY_ID, student_id);
    } else {
        strcpy(sql, SQL_SELECT_SENSOR1_ALL);
    }
    
    /* 添加限制条件 */
    if (limit > 0) {
        char limit_clause[32];
        sprintf(limit_clause, " LIMIT %lu", limit);
        strcat(sql, limit_clause);
    }
    
    DEBUG_PRINT("Executing query: %s", sql);
    
    /* 模拟数据库查询 */
    simulate_database_delay();
    
    /* 在实际项目中，这里应该执行真实的SQL查询操作 */
    /* 模拟查询结果 */
    result.row_count = 0;
    result.column_count = 7; /* id, student_id, sensor_name, temperature, humidity, status, timestamp */
    result.data = NULL;
    result.column_names = NULL;
    
    INFO_PRINT("Sensor1 data query completed: %lu rows", result.row_count);
    return result;
}

/**
 * @brief 查询传感器2数据
 */
db_query_result_t database_query_sensor2_data(const char* student_id, uint32_t limit)
{
    db_query_result_t result;
    char sql[MAX_SQL_LENGTH];
    
    /* 初始化结果 */
    memset(&result, 0, sizeof(db_query_result_t));
    
    /* 连接状态检查 */
    if (current_status != DB_STATUS_CONNECTED) {
        set_last_error(DB_ERROR_CONNECTION, "Database not connected");
        return result;
    }
    
    /* 构建SQL语句 */
    if (student_id != NULL && strlen(student_id) > 0) {
        if (!validate_sql_injection(student_id)) {
            set_last_error(DB_ERROR_INVALID_PARAM, "Invalid student ID");
            return result;
        }
        sprintf(sql, SQL_SELECT_SENSOR2_BY_ID, student_id);
    } else {
        strcpy(sql, SQL_SELECT_SENSOR2_ALL);
    }
    
    /* 添加限制条件 */
    if (limit > 0) {
        char limit_clause[32];
        sprintf(limit_clause, " LIMIT %lu", limit);
        strcat(sql, limit_clause);
    }
    
    DEBUG_PRINT("Executing query: %s", sql);
    
    /* 模拟数据库查询 */
    simulate_database_delay();
    
    /* 在实际项目中，这里应该执行真实的SQL查询操作 */
    /* 模拟查询结果 */
    result.row_count = 0;
    result.column_count = 7; /* id, student_id, sensor_name, interrupt_type, interrupt_count, status, timestamp */
    result.data = NULL;
    result.column_names = NULL;
    
    INFO_PRINT("Sensor2 data query completed: %lu rows", result.row_count);
    return result;
}

/**
 * @brief 执行自定义SQL查询
 */
db_query_result_t database_execute_query(const char* sql)
{
    db_query_result_t result;
    
    /* 初始化结果 */
    memset(&result, 0, sizeof(db_query_result_t));
    
    /* 参数检查 */
    if (sql == NULL || strlen(sql) == 0) {
        set_last_error(DB_ERROR_INVALID_PARAM, "Empty SQL statement");
        return result;
    }
    
    /* 连接状态检查 */
    if (current_status != DB_STATUS_CONNECTED) {
        set_last_error(DB_ERROR_CONNECTION, "Database not connected");
        return result;
    }
    
    DEBUG_PRINT("Executing custom query: %s", sql);
    
    /* 模拟数据库查询 */
    simulate_database_delay();
    
    /* 在实际项目中，这里应该执行真实的SQL查询操作 */
    
    return result;
}

/**
 * @brief 执行SQL更新语句
 */
db_result_t database_execute_update(const char* sql)
{
    /* 参数检查 */
    if (sql == NULL || strlen(sql) == 0) {
        return create_error_result(DB_ERROR_INVALID_PARAM, "Empty SQL statement");
    }
    
    /* 连接状态检查 */
    if (current_status != DB_STATUS_CONNECTED) {
        return create_error_result(DB_ERROR_CONNECTION, "Database not connected");
    }
    
    DEBUG_PRINT("Executing update: %s", sql);
    
    /* 模拟数据库更新 */
    simulate_database_delay();
    
    /* 在实际项目中，这里应该执行真实的SQL更新操作 */
    
    return create_success_result(0, 0);
}

/**
 * @brief 释放查询结果内存
 */
void database_free_query_result(db_query_result_t* result)
{
    if (result == NULL) {
        return;
    }
    
    /* 在实际项目中，这里应该释放查询结果分配的内存 */
    
    memset(result, 0, sizeof(db_query_result_t));
}

/**
 * @brief 开始事务
 */
db_result_t database_begin_transaction(void)
{
    if (current_status != DB_STATUS_CONNECTED) {
        return create_error_result(DB_ERROR_CONNECTION, "Database not connected");
    }
    
    DEBUG_PRINT("Beginning transaction");
    return database_execute_update("BEGIN");
}

/**
 * @brief 提交事务
 */
db_result_t database_commit_transaction(void)
{
    if (current_status != DB_STATUS_CONNECTED) {
        return create_error_result(DB_ERROR_CONNECTION, "Database not connected");
    }
    
    DEBUG_PRINT("Committing transaction");
    return database_execute_update("COMMIT");
}

/**
 * @brief 回滚事务
 */
db_result_t database_rollback_transaction(void)
{
    if (current_status != DB_STATUS_CONNECTED) {
        return create_error_result(DB_ERROR_CONNECTION, "Database not connected");
    }
    
    DEBUG_PRINT("Rolling back transaction");
    return database_execute_update("ROLLBACK");
}

/**
 * @brief 检查数据库表是否存在
 */
bool database_table_exists(const char* table_name)
{
    char sql[MAX_SQL_LENGTH];
    db_query_result_t result;
    
    if (table_name == NULL || current_status != DB_STATUS_CONNECTED) {
        return false;
    }
    
    sprintf(sql, "SHOW TABLES LIKE '%s'", table_name);
    result = database_execute_query(sql);
    
    bool exists = (result.row_count > 0);
    database_free_query_result(&result);
    
    return exists;
}

/**
 * @brief 创建数据库表
 */
db_result_t database_create_tables(void)
{
    db_result_t result;
    
    if (current_status != DB_STATUS_CONNECTED) {
        return create_error_result(DB_ERROR_CONNECTION, "Database not connected");
    }
    
    /* 创建传感器1数据表 */
    result = database_execute_update(SQL_CREATE_SENSOR1_TABLE);
    if (!result.success) {
        return result;
    }
    
    /* 创建传感器2数据表 */
    result = database_execute_update(SQL_CREATE_SENSOR2_TABLE);
    if (!result.success) {
        return result;
    }
    
    INFO_PRINT("Database tables created successfully");
    return create_success_result(0, 0);
}

/**
 * @brief 获取数据库统计信息
 */
db_result_t database_get_statistics(uint32_t* sensor1_count, uint32_t* sensor2_count)
{
    db_query_result_t result;
    
    if (current_status != DB_STATUS_CONNECTED) {
        return create_error_result(DB_ERROR_CONNECTION, "Database not connected");
    }
    
    /* 查询传感器1数据数量 */
    if (sensor1_count != NULL) {
        result = database_execute_query(SQL_COUNT_SENSOR1);
        *sensor1_count = 0; /* 在实际项目中从查询结果中获取 */
        database_free_query_result(&result);
    }
    
    /* 查询传感器2数据数量 */
    if (sensor2_count != NULL) {
        result = database_execute_query(SQL_COUNT_SENSOR2);
        *sensor2_count = 0; /* 在实际项目中从查询结果中获取 */
        database_free_query_result(&result);
    }
    
    return create_success_result(0, 0);
}

/**
 * @brief 清理过期数据
 */
db_result_t database_cleanup_old_data(uint32_t days_old)
{
    char sql[MAX_SQL_LENGTH];
    db_result_t result;
    
    if (current_status != DB_STATUS_CONNECTED) {
        return create_error_result(DB_ERROR_CONNECTION, "Database not connected");
    }
    
    /* 删除过期的传感器1数据 */
    sprintf(sql, "DELETE FROM sensor1_data WHERE created_at < DATE_SUB(NOW(), INTERVAL %lu DAY)", days_old);
    result = database_execute_update(sql);
    if (!result.success) {
        return result;
    }
    
    /* 删除过期的传感器2数据 */
    sprintf(sql, "DELETE FROM sensor2_data WHERE created_at < DATE_SUB(NOW(), INTERVAL %lu DAY)", days_old);
    result = database_execute_update(sql);
    
    INFO_PRINT("Old data cleanup completed: %lu days", days_old);
    return result;
}

/**
 * @brief 备份数据库
 */
db_result_t database_backup(const char* backup_path)
{
    if (backup_path == NULL) {
        return create_error_result(DB_ERROR_INVALID_PARAM, "Invalid backup path");
    }
    
    if (current_status != DB_STATUS_CONNECTED) {
        return create_error_result(DB_ERROR_CONNECTION, "Database not connected");
    }
    
    /* 在实际项目中，这里应该实现真实的数据库备份功能 */
    DEBUG_PRINT("Database backup to: %s", backup_path);
    
    return create_success_result(0, 0);
}

/**
 * @brief 恢复数据库
 */
db_result_t database_restore(const char* backup_path)
{
    if (backup_path == NULL) {
        return create_error_result(DB_ERROR_INVALID_PARAM, "Invalid backup path");
    }
    
    if (current_status != DB_STATUS_CONNECTED) {
        return create_error_result(DB_ERROR_CONNECTION, "Database not connected");
    }
    
    /* 在实际项目中，这里应该实现真实的数据库恢复功能 */
    DEBUG_PRINT("Database restore from: %s", backup_path);
    
    return create_success_result(0, 0);
}

/**
 * @brief 设置数据库错误回调函数
 */
void database_set_error_callback(void (*callback)(const char* error_msg))
{
    error_callback = callback;
}

/**
 * @brief 获取最后一次错误信息
 */
const char* database_get_last_error(void)
{
    return last_error_message;
}

/* 内部函数实现 */

/**
 * @brief 设置最后一次错误信息
 */
static void set_last_error(int error_code, const char* error_msg)
{
    if (error_msg != NULL) {
        SAFE_STRCPY(last_error_message, error_msg, sizeof(last_error_message));
    } else {
        sprintf(last_error_message, "Database error %d", error_code);
    }
    
    /* 调用错误回调 */
    if (error_callback != NULL) {
        error_callback(last_error_message);
    }
    
    ERROR_PRINT("Database error: %s", last_error_message);
}

/**
 * @brief 创建错误结果
 */
static db_result_t create_error_result(int error_code, const char* error_msg)
{
    db_result_t result;
    
    result.success = false;
    result.error_code = error_code;
    result.affected_rows = 0;
    result.insert_id = 0;
    
    if (error_msg != NULL) {
        SAFE_STRCPY(result.error_message, error_msg, sizeof(result.error_message));
    } else {
        sprintf(result.error_message, "Error %d", error_code);
    }
    
    set_last_error(error_code, error_msg);
    return result;
}

/**
 * @brief 创建成功结果
 */
static db_result_t create_success_result(uint32_t affected_rows, uint32_t insert_id)
{
    db_result_t result;
    
    result.success = true;
    result.error_code = DB_ERROR_NONE;
    result.affected_rows = affected_rows;
    result.insert_id = insert_id;
    strcpy(result.error_message, "");
    
    return result;
}

/**
 * @brief 验证SQL注入
 */
static bool validate_sql_injection(const char* input)
{
    const char* dangerous_chars = "';\"\\";
    int i, j;
    
    if (input == NULL) {
        return false;
    }
    
    /* 检查危险字符 */
    for (i = 0; input[i] != '\0'; i++) {
        for (j = 0; dangerous_chars[j] != '\0'; j++) {
            if (input[i] == dangerous_chars[j]) {
                return false;
            }
        }
    }
    
    return true;
}

/**
 * @brief 转义字符串
 */
static char* escape_string(const char* input, char* output, size_t output_size)
{
    size_t i, j = 0;
    
    if (input == NULL || output == NULL || output_size == 0) {
        return NULL;
    }
    
    for (i = 0; input[i] != '\0' && j < output_size - 1; i++) {
        if (input[i] == '\'' || input[i] == '\"' || input[i] == '\\') {
            if (j < output_size - 2) {
                output[j++] = '\\';
                output[j++] = input[i];
            }
        } else {
            output[j++] = input[i];
        }
    }
    
    output[j] = '\0';
    return output;
}

/**
 * @brief 模拟数据库延迟
 */
static void simulate_database_delay(void)
{
    /* 在实际项目中，这里不需要延迟 */
    /* 这里只是为了模拟数据库操作的时间消耗 */
    volatile int i;
    for (i = 0; i < 1000; i++) {
        /* 空循环模拟延迟 */
    }
}