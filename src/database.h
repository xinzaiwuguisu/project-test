#ifndef DATABASE_H
#define DATABASE_H

#include <stdint.h>
#include <stdbool.h>
#include "sensor_data.h"

/* 数据库连接状态 */
typedef enum {
    DB_STATUS_DISCONNECTED = 0,
    DB_STATUS_CONNECTED,
    DB_STATUS_ERROR
} db_status_t;

/* 数据库操作结果 */
typedef enum {
    DB_SUCCESS = 0,
    DB_ERROR_CONNECTION,
    DB_ERROR_QUERY,
    DB_ERROR_MEMORY,
    DB_ERROR_TIMEOUT
} db_result_t;

/* 数据库连接结构 */
typedef struct {
    char host[64];
    uint16_t port;
    char username[32];
    char password[32];
    char database[32];
    db_status_t status;
} db_connection_t;

/* 函数声明 */
db_result_t database_init(void);
db_result_t database_connect(void);
void database_disconnect(void);
db_result_t database_insert_sensor1_data(const sensor1_data_t* data);
db_result_t database_insert_sensor2_data(const sensor2_data_t* data);
db_status_t database_get_status(void);
const char* database_get_error_string(db_result_t result);

/* 数据库查询函数 */
db_result_t database_query_sensor1_latest(sensor1_data_t* data, const char* student_id);
db_result_t database_query_sensor2_latest(sensor2_data_t* data, const char* student_id);

#endif /* DATABASE_H */