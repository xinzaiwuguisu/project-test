/**
 * @file config.h
 * @brief 系统配置文件 - IAR 5.3兼容版本
 * @author OpenHands
 * @date 2025-06-19
 * @version 1.0.0
 * 
 * 适配IAR EMBEDDED WORKBENCH 5.3版本的系统配置
 */

#ifndef CONFIG_H
#define CONFIG_H

/* IAR 5.3兼容性定义 */
#ifdef IAR_VERSION_53
    #define IAR_LEGACY_SUPPORT
#endif

/* 标准库包含 - IAR 5.3兼容 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

/* IAR 5.3不完全支持stdint.h，使用传统类型定义 */
#ifdef IAR_LEGACY_SUPPORT
    typedef unsigned char       uint8_t;
    typedef unsigned short      uint16_t;
    typedef unsigned long       uint32_t;
    typedef signed char         int8_t;
    typedef signed short        int16_t;
    typedef signed long         int32_t;
    
    #ifndef NULL
        #define NULL ((void*)0)
    #endif
    
    #ifndef true
        #define true 1
    #endif
    
    #ifndef false
        #define false 0
    #endif
    
    typedef unsigned char bool;
#else
    #include <stdint.h>
    #include <stdbool.h>
#endif

/* 系统时钟配置 */
#define SYSTEM_CLOCK_FREQ       16000000UL  /* 16MHz */
#define APB1_CLOCK_FREQ         16000000UL  /* APB1时钟 */
#define APB2_CLOCK_FREQ         16000000UL  /* APB2时钟 */

/* UART配置 */
#define UART_BAUD_RATE          115200      /* 波特率 */
#define UART_DATA_BITS          8           /* 数据位 */
#define UART_STOP_BITS          1           /* 停止位 */
#define UART_PARITY             0           /* 无校验 */

/* 缓冲区大小配置 */
#define RX_BUFFER_SIZE          256         /* 接收缓冲区大小 */
#define TX_BUFFER_SIZE          512         /* 发送缓冲区大小 */
#define SENSOR_DATA_BUFFER_SIZE 128         /* 传感器数据缓冲区 */

/* 传感器配置 */
#define MAX_STUDENT_ID_LEN      20          /* 学号姓名缩写最大长度 */
#define MAX_SENSOR_NAME_LEN     16          /* 传感器名称最大长度 */
#define MAX_DATA_FIELDS         8           /* 最大数据字段数 */

/* 数据库配置 */
#define DB_HOST                 "localhost"
#define DB_PORT                 3306
#define DB_USER                 "sensor_user"
#define DB_PASSWORD             "sensor_pass"
#define DB_NAME                 "sensor_data"
#define DB_TIMEOUT              30          /* 连接超时时间(秒) */

/* 系统状态定义 */
typedef enum {
    SYSTEM_OK = 0,
    SYSTEM_ERROR = 1,
    SYSTEM_BUSY = 2,
    SYSTEM_TIMEOUT = 3
} system_status_t;

/* 传感器状态定义 */
typedef enum {
    SENSOR_STATUS_NORMAL = 0,
    SENSOR_STATUS_WARNING = 1,
    SENSOR_STATUS_ERROR = 2,
    SENSOR_STATUS_OFFLINE = 3
} sensor_status_t;

/* 传感器类型定义 */
typedef enum {
    SENSOR_TYPE_UNKNOWN = 0,
    SENSOR_TYPE_TEMP_HUMIDITY = 1,
    SENSOR_TYPE_INTERRUPT = 2
} sensor_type_t;

/* 中断类型定义 */
typedef enum {
    INTERRUPT_TYPE_NONE = 0,
    INTERRUPT_TYPE_RISING = 1,
    INTERRUPT_TYPE_FALLING = 2,
    INTERRUPT_TYPE_BOTH = 3
} interrupt_type_t;

/* 调试配置 */
#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) printf("[DEBUG] " fmt "\r\n", ##__VA_ARGS__)
    #define ERROR_PRINT(fmt, ...) printf("[ERROR] " fmt "\r\n", ##__VA_ARGS__)
    #define INFO_PRINT(fmt, ...)  printf("[INFO] " fmt "\r\n", ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...)
    #define ERROR_PRINT(fmt, ...)
    #define INFO_PRINT(fmt, ...)
#endif

/* 硬件抽象层定义 - IAR 5.3兼容 */
#ifdef IAR_LEGACY_SUPPORT
    /* 简化的硬件定义，避免复杂的HAL库依赖 */
    #define UART1_BASE              0x40013800UL
    #define GPIOA_BASE              0x40010800UL
    #define RCC_BASE                0x40021000UL
    
    /* 寄存器访问宏 */
    #define REG32(addr)             (*(volatile uint32_t *)(addr))
    #define REG16(addr)             (*(volatile uint16_t *)(addr))
    #define REG8(addr)              (*(volatile uint8_t *)(addr))
    
    /* 位操作宏 */
    #define BIT_SET(reg, bit)       ((reg) |= (1UL << (bit)))
    #define BIT_CLEAR(reg, bit)     ((reg) &= ~(1UL << (bit)))
    #define BIT_READ(reg, bit)      (((reg) >> (bit)) & 1UL)
    
    /* 中断控制 - 简化版本 */
    #define DISABLE_INTERRUPTS()    __disable_interrupt()
    #define ENABLE_INTERRUPTS()     __enable_interrupt()
#else
    /* 现代HAL库支持 */
    #include "stm32f1xx_hal.h"
    #define DISABLE_INTERRUPTS()    __disable_irq()
    #define ENABLE_INTERRUPTS()     __enable_irq()
#endif

/* 内存管理配置 */
#define HEAP_SIZE               2048        /* 堆大小 */
#define STACK_SIZE              1024        /* 栈大小 */

/* 定时器配置 */
#define TIMER_PRESCALER         1000        /* 定时器预分频 */
#define TIMER_PERIOD            1000        /* 定时器周期 */

/* 看门狗配置 */
#define WATCHDOG_TIMEOUT        5000        /* 看门狗超时时间(ms) */

/* 系统版本信息 */
#define FIRMWARE_VERSION_MAJOR  1
#define FIRMWARE_VERSION_MINOR  0
#define FIRMWARE_VERSION_PATCH  0
#define FIRMWARE_BUILD_DATE     __DATE__
#define FIRMWARE_BUILD_TIME     __TIME__

/* 功能开关 */
#define ENABLE_WATCHDOG         1           /* 启用看门狗 */
#define ENABLE_DEBUG_OUTPUT     1           /* 启用调试输出 */
#define ENABLE_ERROR_RECOVERY   1           /* 启用错误恢复 */
#define ENABLE_DATA_VALIDATION  1           /* 启用数据验证 */

/* 性能配置 */
#define MAX_PROCESSING_TIME_MS  100         /* 最大处理时间 */
#define MAX_RETRY_COUNT         3           /* 最大重试次数 */
#define COMMUNICATION_TIMEOUT   5000        /* 通信超时时间(ms) */

/* 数据格式配置 */
#define FLOAT_PRECISION         2           /* 浮点数精度 */
#define TIMESTAMP_FORMAT        "%Y-%m-%d %H:%M:%S"

/* 错误代码定义 */
#define ERROR_NONE              0x00
#define ERROR_INVALID_PARAM     0x01
#define ERROR_BUFFER_OVERFLOW   0x02
#define ERROR_COMMUNICATION    0x03
#define ERROR_DATABASE          0x04
#define ERROR_SENSOR            0x05
#define ERROR_TIMEOUT           0x06
#define ERROR_MEMORY            0x07
#define ERROR_HARDWARE          0x08

/* 工具宏定义 */
#define ARRAY_SIZE(arr)         (sizeof(arr) / sizeof((arr)[0]))
#define MIN(a, b)               ((a) < (b) ? (a) : (b))
#define MAX(a, b)               ((a) > (b) ? (a) : (b))
#define CLAMP(val, min, max)    (MIN(MAX(val, min), max))

/* 字符串处理宏 - IAR 5.3兼容 */
#ifdef IAR_LEGACY_SUPPORT
    #define SAFE_STRCPY(dst, src, size) do { \
        strncpy(dst, src, size - 1); \
        dst[size - 1] = '\0'; \
    } while(0)
    
    #define SAFE_STRCAT(dst, src, size) do { \
        size_t len = strlen(dst); \
        if (len < size - 1) { \
            strncat(dst, src, size - len - 1); \
        } \
    } while(0)
#else
    #define SAFE_STRCPY(dst, src, size) strcpy_s(dst, size, src)
    #define SAFE_STRCAT(dst, src, size) strcat_s(dst, size, src)
#endif

#endif /* CONFIG_H */