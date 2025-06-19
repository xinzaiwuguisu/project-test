/**
 * @file communication.h
 * @brief 通信模块头文件 - IAR 5.3兼容版本
 * @author OpenHands
 * @date 2025-06-19
 * @version 1.0.0
 */

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "config.h"

/* 通信状态定义 */
typedef enum {
    COMM_STATUS_IDLE = 0,
    COMM_STATUS_RECEIVING = 1,
    COMM_STATUS_TRANSMITTING = 2,
    COMM_STATUS_ERROR = 3
} comm_status_t;

/* 通信错误类型 */
typedef enum {
    COMM_ERROR_NONE = 0,
    COMM_ERROR_TIMEOUT = 1,
    COMM_ERROR_OVERRUN = 2,
    COMM_ERROR_FRAMING = 3,
    COMM_ERROR_PARITY = 4,
    COMM_ERROR_BUFFER_FULL = 5,
    COMM_ERROR_INVALID_PARAM = 6
} comm_error_t;

/* 通信配置结构 */
typedef struct {
    uint32_t baud_rate;                     /* 波特率 */
    uint8_t data_bits;                      /* 数据位 */
    uint8_t stop_bits;                      /* 停止位 */
    uint8_t parity;                         /* 校验位 */
    uint16_t rx_buffer_size;                /* 接收缓冲区大小 */
    uint16_t tx_buffer_size;                /* 发送缓冲区大小 */
    uint32_t timeout_ms;                    /* 超时时间 */
} comm_config_t;

/* 通信统计信息 */
typedef struct {
    uint32_t bytes_received;                /* 接收字节数 */
    uint32_t bytes_transmitted;             /* 发送字节数 */
    uint32_t packets_received;              /* 接收包数 */
    uint32_t packets_transmitted;           /* 发送包数 */
    uint32_t error_count;                   /* 错误计数 */
    uint32_t timeout_count;                 /* 超时计数 */
} comm_statistics_t;

/* 数据包结构 */
typedef struct {
    uint8_t* data;                          /* 数据指针 */
    uint16_t length;                        /* 数据长度 */
    uint32_t timestamp;                     /* 时间戳 */
    bool is_valid;                          /* 数据是否有效 */
} comm_packet_t;

/* 函数声明 */

/**
 * @brief 初始化通信模块
 * @param config 通信配置参数
 * @return system_status_t 初始化状态
 */
system_status_t communication_init(const comm_config_t* config);

/**
 * @brief 反初始化通信模块
 * @return system_status_t 操作状态
 */
system_status_t communication_deinit(void);

/**
 * @brief 发送数据
 * @param data 数据指针
 * @param length 数据长度
 * @return system_status_t 发送状态
 */
system_status_t communication_send(const uint8_t* data, uint16_t length);

/**
 * @brief 发送字符串
 * @param str 字符串指针
 * @return system_status_t 发送状态
 */
system_status_t communication_send_string(const char* str);

/**
 * @brief 接收数据
 * @param buffer 接收缓冲区
 * @param buffer_size 缓冲区大小
 * @param received_length 实际接收长度
 * @param timeout_ms 超时时间
 * @return system_status_t 接收状态
 */
system_status_t communication_receive(uint8_t* buffer, uint16_t buffer_size, 
                                     uint16_t* received_length, uint32_t timeout_ms);

/**
 * @brief 接收一行数据（以换行符结束）
 * @param buffer 接收缓冲区
 * @param buffer_size 缓冲区大小
 * @param timeout_ms 超时时间
 * @return system_status_t 接收状态
 */
system_status_t communication_receive_line(char* buffer, uint16_t buffer_size, uint32_t timeout_ms);

/**
 * @brief 检查是否有数据可读
 * @return bool 是否有数据
 */
bool communication_data_available(void);

/**
 * @brief 获取接收缓冲区中的数据长度
 * @return uint16_t 数据长度
 */
uint16_t communication_get_rx_data_length(void);

/**
 * @brief 清空接收缓冲区
 */
void communication_clear_rx_buffer(void);

/**
 * @brief 清空发送缓冲区
 */
void communication_clear_tx_buffer(void);

/**
 * @brief 获取通信状态
 * @return comm_status_t 通信状态
 */
comm_status_t communication_get_status(void);

/**
 * @brief 获取最后一次错误
 * @return comm_error_t 错误类型
 */
comm_error_t communication_get_last_error(void);

/**
 * @brief 获取通信统计信息
 * @param stats 统计信息结构指针
 */
void communication_get_statistics(comm_statistics_t* stats);

/**
 * @brief 重置通信统计信息
 */
void communication_reset_statistics(void);

/**
 * @brief 设置数据接收回调函数
 * @param callback 回调函数指针
 */
void communication_set_rx_callback(void (*callback)(const uint8_t* data, uint16_t length));

/**
 * @brief 设置错误回调函数
 * @param callback 回调函数指针
 */
void communication_set_error_callback(void (*callback)(comm_error_t error));

/**
 * @brief 启用/禁用中断
 * @param enable 是否启用
 */
void communication_enable_interrupt(bool enable);

/**
 * @brief UART中断服务程序
 */
void communication_uart_isr(void);

/**
 * @brief 处理接收到的数据包
 */
void communication_process_rx_data(void);

/**
 * @brief 发送格式化字符串
 * @param format 格式字符串
 * @param ... 可变参数
 * @return system_status_t 发送状态
 */
system_status_t communication_printf(const char* format, ...);

/**
 * @brief 配置硬件UART
 * @param config 配置参数
 * @return system_status_t 配置状态
 */
system_status_t communication_configure_uart(const comm_config_t* config);

/**
 * @brief 启动DMA传输（如果支持）
 * @param data 数据指针
 * @param length 数据长度
 * @return system_status_t 启动状态
 */
system_status_t communication_start_dma_tx(const uint8_t* data, uint16_t length);

/**
 * @brief 停止DMA传输
 * @return system_status_t 停止状态
 */
system_status_t communication_stop_dma_tx(void);

/* 内联函数 - IAR 5.3兼容 */
#ifdef IAR_LEGACY_SUPPORT
    #pragma inline
    static bool is_valid_comm_config(const comm_config_t* config)
    {
        if (config == NULL) return false;
        if (config->baud_rate == 0) return false;
        if (config->data_bits < 5 || config->data_bits > 9) return false;
        if (config->stop_bits == 0 || config->stop_bits > 2) return false;
        if (config->rx_buffer_size == 0 || config->tx_buffer_size == 0) return false;
        return true;
    }
    
    #pragma inline
    static uint8_t calculate_parity(const uint8_t* data, uint16_t length)
    {
        uint8_t parity = 0;
        uint16_t i;
        for (i = 0; i < length; i++) {
            parity ^= data[i];
        }
        return parity;
    }
#else
    static inline bool is_valid_comm_config(const comm_config_t* config)
    {
        if (config == NULL) return false;
        if (config->baud_rate == 0) return false;
        if (config->data_bits < 5 || config->data_bits > 9) return false;
        if (config->stop_bits == 0 || config->stop_bits > 2) return false;
        if (config->rx_buffer_size == 0 || config->tx_buffer_size == 0) return false;
        return true;
    }
    
    static inline uint8_t calculate_parity(const uint8_t* data, uint16_t length)
    {
        uint8_t parity = 0;
        uint16_t i;
        for (i = 0; i < length; i++) {
            parity ^= data[i];
        }
        return parity;
    }
#endif

/* 常量定义 */
#define COMM_DEFAULT_BAUD_RATE      115200
#define COMM_DEFAULT_DATA_BITS      8
#define COMM_DEFAULT_STOP_BITS      1
#define COMM_DEFAULT_PARITY         0
#define COMM_DEFAULT_TIMEOUT_MS     5000
#define COMM_MAX_PACKET_SIZE        256
#define COMM_LINE_ENDING            "\r\n"

/* 校验位定义 */
#define COMM_PARITY_NONE            0
#define COMM_PARITY_ODD             1
#define COMM_PARITY_EVEN            2

/* 流控制定义 */
#define COMM_FLOW_CONTROL_NONE      0
#define COMM_FLOW_CONTROL_RTS_CTS   1
#define COMM_FLOW_CONTROL_XON_XOFF  2

/* 特殊字符定义 */
#define COMM_CHAR_STX               0x02    /* 文本开始 */
#define COMM_CHAR_ETX               0x03    /* 文本结束 */
#define COMM_CHAR_EOT               0x04    /* 传输结束 */
#define COMM_CHAR_ACK               0x06    /* 确认 */
#define COMM_CHAR_NAK               0x15    /* 否认 */
#define COMM_CHAR_CR                0x0D    /* 回车 */
#define COMM_CHAR_LF                0x0A    /* 换行 */

/* 错误消息定义 */
#define COMM_ERROR_MSG_TIMEOUT      "Communication timeout"
#define COMM_ERROR_MSG_OVERRUN      "Buffer overrun"
#define COMM_ERROR_MSG_FRAMING      "Framing error"
#define COMM_ERROR_MSG_PARITY       "Parity error"
#define COMM_ERROR_MSG_BUFFER_FULL  "Buffer full"
#define COMM_ERROR_MSG_INVALID_PARAM "Invalid parameter"

/* 默认配置 */
extern const comm_config_t DEFAULT_COMM_CONFIG;

/* 硬件相关定义 - IAR 5.3兼容 */
#ifdef IAR_LEGACY_SUPPORT
    /* UART寄存器定义 */
    #define UART1_DR                (*(volatile uint32_t*)(UART1_BASE + 0x04))
    #define UART1_BRR               (*(volatile uint32_t*)(UART1_BASE + 0x08))
    #define UART1_CR1               (*(volatile uint32_t*)(UART1_BASE + 0x0C))
    #define UART1_CR2               (*(volatile uint32_t*)(UART1_BASE + 0x10))
    #define UART1_CR3               (*(volatile uint32_t*)(UART1_BASE + 0x14))
    #define UART1_SR                (*(volatile uint32_t*)(UART1_BASE + 0x00))
    
    /* UART状态位定义 */
    #define UART_SR_TXE             (1 << 7)    /* 发送数据寄存器空 */
    #define UART_SR_TC              (1 << 6)    /* 传输完成 */
    #define UART_SR_RXNE            (1 << 5)    /* 读数据寄存器非空 */
    #define UART_SR_IDLE            (1 << 4)    /* 检测到空闲线路 */
    #define UART_SR_ORE             (1 << 3)    /* 溢出错误 */
    #define UART_SR_NE              (1 << 2)    /* 噪声错误 */
    #define UART_SR_FE              (1 << 1)    /* 帧错误 */
    #define UART_SR_PE              (1 << 0)    /* 校验错误 */
    
    /* UART控制位定义 */
    #define UART_CR1_UE             (1 << 13)   /* UART使能 */
    #define UART_CR1_M              (1 << 12)   /* 字长 */
    #define UART_CR1_WAKE           (1 << 11)   /* 唤醒方法 */
    #define UART_CR1_PCE            (1 << 10)   /* 校验控制使能 */
    #define UART_CR1_PS             (1 << 9)    /* 校验选择 */
    #define UART_CR1_PEIE           (1 << 8)    /* PE中断使能 */
    #define UART_CR1_TXEIE          (1 << 7)    /* TXE中断使能 */
    #define UART_CR1_TCIE           (1 << 6)    /* 传输完成中断使能 */
    #define UART_CR1_RXNEIE         (1 << 5)    /* RXNE中断使能 */
    #define UART_CR1_IDLEIE         (1 << 4)    /* IDLE中断使能 */
    #define UART_CR1_TE             (1 << 3)    /* 发送器使能 */
    #define UART_CR1_RE             (1 << 2)    /* 接收器使能 */
    #define UART_CR1_RWU            (1 << 1)    /* 接收器唤醒 */
    #define UART_CR1_SBK            (1 << 0)    /* 发送中断 */
#endif

#endif /* COMMUNICATION_H */