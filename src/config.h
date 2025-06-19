#ifndef CONFIG_H
#define CONFIG_H

/* 系统配置 */
#define SYSTEM_CLOCK_FREQ       16000000UL  // 16MHz
#define UART_BAUD_RATE         115200

/* 传感器配置 */
#define SENSOR1_ID             "TEMP_HUM"
#define SENSOR2_ID             "INTERRUPT"
#define MAX_DATA_LENGTH        128
#define MAX_STUDENT_ID_LENGTH  20

/* 数据库配置 */
#define DB_HOST                "localhost"
#define DB_PORT                3306
#define DB_USER                "sensor_user"
#define DB_PASSWORD            "sensor_pass"
#define DB_NAME                "sensor_data"

/* 表名定义 */
#define TABLE_SENSOR1          "sensor1_data"
#define TABLE_SENSOR2          "sensor2_data"

/* 缓冲区大小 */
#define RX_BUFFER_SIZE         256
#define TX_BUFFER_SIZE         512

/* 学号姓名缩写示例 */
#define STUDENT_ID             "2021001ZS"  // 示例：2021001张三

#endif /* CONFIG_H */