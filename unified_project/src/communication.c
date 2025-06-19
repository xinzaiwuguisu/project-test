/**
 * @file communication.c
 * @brief 通信模块实现 - IAR 5.3兼容版本
 * @author OpenHands
 * @date 2025-06-19
 * @version 1.0.0
 */

#include "communication.h"
#include <stdarg.h>

/* 静态变量 */
static comm_status_t current_status = COMM_STATUS_IDLE;
static comm_error_t last_error = COMM_ERROR_NONE;
static comm_statistics_t statistics;
static comm_config_t current_config;

/* 缓冲区 */
static uint8_t rx_buffer[RX_BUFFER_SIZE];
static uint8_t tx_buffer[TX_BUFFER_SIZE];
static volatile uint16_t rx_head = 0;
static volatile uint16_t rx_tail = 0;
static volatile uint16_t tx_head = 0;
static volatile uint16_t tx_tail = 0;

/* 回调函数指针 */
static void (*rx_callback)(const uint8_t* data, uint16_t length) = NULL;
static void (*error_callback)(comm_error_t error) = NULL;

/* 默认通信配置 */
const comm_config_t DEFAULT_COMM_CONFIG = {
    COMM_DEFAULT_BAUD_RATE,     /* baud_rate */
    COMM_DEFAULT_DATA_BITS,     /* data_bits */
    COMM_DEFAULT_STOP_BITS,     /* stop_bits */
    COMM_DEFAULT_PARITY,        /* parity */
    RX_BUFFER_SIZE,             /* rx_buffer_size */
    TX_BUFFER_SIZE,             /* tx_buffer_size */
    COMM_DEFAULT_TIMEOUT_MS     /* timeout_ms */
};

/* 内部函数声明 */
static void set_last_error(comm_error_t error);
static uint16_t get_rx_buffer_count(void);
static uint16_t get_tx_buffer_space(void);
static bool put_rx_byte(uint8_t byte);
static bool get_rx_byte(uint8_t* byte);
static bool put_tx_byte(uint8_t byte);
static bool get_tx_byte(uint8_t* byte);
static void uart_hardware_init(const comm_config_t* config);
static void uart_enable_interrupts(void);
static void uart_disable_interrupts(void);
static uint32_t get_system_tick(void);

/**
 * @brief 初始化通信模块
 */
system_status_t communication_init(const comm_config_t* config)
{
    /* 参数检查 */
    if (!is_valid_comm_config(config)) {
        set_last_error(COMM_ERROR_INVALID_PARAM);
        return SYSTEM_ERROR;
    }
    
    /* 保存配置 */
    memcpy(&current_config, config, sizeof(comm_config_t));
    
    /* 初始化缓冲区 */
    rx_head = 0;
    rx_tail = 0;
    tx_head = 0;
    tx_tail = 0;
    memset(rx_buffer, 0, sizeof(rx_buffer));
    memset(tx_buffer, 0, sizeof(tx_buffer));
    
    /* 初始化统计信息 */
    memset(&statistics, 0, sizeof(comm_statistics_t));
    
    /* 初始化状态 */
    current_status = COMM_STATUS_IDLE;
    last_error = COMM_ERROR_NONE;
    
    /* 初始化硬件UART */
    uart_hardware_init(config);
    
    /* 启用中断 */
    uart_enable_interrupts();
    
    DEBUG_PRINT("Communication module initialized: %lu baud, %d-%d-%d", 
                config->baud_rate, config->data_bits, config->parity, config->stop_bits);
    
    return SYSTEM_OK;
}

/**
 * @brief 反初始化通信模块
 */
system_status_t communication_deinit(void)
{
    /* 禁用中断 */
    uart_disable_interrupts();
    
    /* 清空缓冲区 */
    communication_clear_rx_buffer();
    communication_clear_tx_buffer();
    
    /* 重置状态 */
    current_status = COMM_STATUS_IDLE;
    last_error = COMM_ERROR_NONE;
    
    /* 清空回调函数 */
    rx_callback = NULL;
    error_callback = NULL;
    
    DEBUG_PRINT("Communication module deinitialized");
    return SYSTEM_OK;
}

/**
 * @brief 发送数据
 */
system_status_t communication_send(const uint8_t* data, uint16_t length)
{
    uint16_t i;
    
    /* 参数检查 */
    if (data == NULL || length == 0) {
        set_last_error(COMM_ERROR_INVALID_PARAM);
        return SYSTEM_ERROR;
    }
    
    /* 检查缓冲区空间 */
    if (get_tx_buffer_space() < length) {
        set_last_error(COMM_ERROR_BUFFER_FULL);
        return SYSTEM_ERROR;
    }
    
    /* 将数据放入发送缓冲区 */
    DISABLE_INTERRUPTS();
    for (i = 0; i < length; i++) {
        if (!put_tx_byte(data[i])) {
            ENABLE_INTERRUPTS();
            set_last_error(COMM_ERROR_BUFFER_FULL);
            return SYSTEM_ERROR;
        }
    }
    ENABLE_INTERRUPTS();
    
    /* 启动发送 */
    current_status = COMM_STATUS_TRANSMITTING;
    
#ifdef IAR_LEGACY_SUPPORT
    /* 启用TXE中断 */
    UART1_CR1 |= UART_CR1_TXEIE;
#endif
    
    /* 更新统计信息 */
    statistics.bytes_transmitted += length;
    statistics.packets_transmitted++;
    
    DEBUG_PRINT("Sending %d bytes", length);
    return SYSTEM_OK;
}

/**
 * @brief 发送字符串
 */
system_status_t communication_send_string(const char* str)
{
    if (str == NULL) {
        set_last_error(COMM_ERROR_INVALID_PARAM);
        return SYSTEM_ERROR;
    }
    
    return communication_send((const uint8_t*)str, strlen(str));
}

/**
 * @brief 接收数据
 */
system_status_t communication_receive(uint8_t* buffer, uint16_t buffer_size, 
                                     uint16_t* received_length, uint32_t timeout_ms)
{
    uint32_t start_time;
    uint16_t count = 0;
    uint8_t byte;
    
    /* 参数检查 */
    if (buffer == NULL || buffer_size == 0 || received_length == NULL) {
        set_last_error(COMM_ERROR_INVALID_PARAM);
        return SYSTEM_ERROR;
    }
    
    *received_length = 0;
    start_time = get_system_tick();
    
    /* 接收数据直到缓冲区满或超时 */
    while (count < buffer_size) {
        if (get_rx_byte(&byte)) {
            buffer[count++] = byte;
        } else {
            /* 检查超时 */
            if (timeout_ms > 0 && (get_system_tick() - start_time) > timeout_ms) {
                if (count == 0) {
                    set_last_error(COMM_ERROR_TIMEOUT);
                    return SYSTEM_TIMEOUT;
                }
                break;
            }
        }
    }
    
    *received_length = count;
    
    if (count > 0) {
        statistics.bytes_received += count;
        statistics.packets_received++;
        DEBUG_PRINT("Received %d bytes", count);
    }
    
    return SYSTEM_OK;
}

/**
 * @brief 接收一行数据
 */
system_status_t communication_receive_line(char* buffer, uint16_t buffer_size, uint32_t timeout_ms)
{
    uint32_t start_time;
    uint16_t count = 0;
    uint8_t byte;
    
    /* 参数检查 */
    if (buffer == NULL || buffer_size < 2) {
        set_last_error(COMM_ERROR_INVALID_PARAM);
        return SYSTEM_ERROR;
    }
    
    start_time = get_system_tick();
    
    /* 接收数据直到遇到换行符或缓冲区满 */
    while (count < buffer_size - 1) {
        if (get_rx_byte(&byte)) {
            if (byte == COMM_CHAR_CR || byte == COMM_CHAR_LF) {
                /* 遇到换行符，结束接收 */
                break;
            } else if (byte >= 0x20 && byte <= 0x7E) {
                /* 可打印字符 */
                buffer[count++] = (char)byte;
            }
            /* 忽略其他控制字符 */
        } else {
            /* 检查超时 */
            if (timeout_ms > 0 && (get_system_tick() - start_time) > timeout_ms) {
                if (count == 0) {
                    set_last_error(COMM_ERROR_TIMEOUT);
                    return SYSTEM_TIMEOUT;
                }
                break;
            }
        }
    }
    
    /* 添加字符串结束符 */
    buffer[count] = '\0';
    
    if (count > 0) {
        statistics.bytes_received += count;
        statistics.packets_received++;
        DEBUG_PRINT("Received line: %s", buffer);
    }
    
    return SYSTEM_OK;
}

/**
 * @brief 检查是否有数据可读
 */
bool communication_data_available(void)
{
    return (get_rx_buffer_count() > 0);
}

/**
 * @brief 获取接收缓冲区中的数据长度
 */
uint16_t communication_get_rx_data_length(void)
{
    return get_rx_buffer_count();
}

/**
 * @brief 清空接收缓冲区
 */
void communication_clear_rx_buffer(void)
{
    DISABLE_INTERRUPTS();
    rx_head = 0;
    rx_tail = 0;
    ENABLE_INTERRUPTS();
    
    DEBUG_PRINT("RX buffer cleared");
}

/**
 * @brief 清空发送缓冲区
 */
void communication_clear_tx_buffer(void)
{
    DISABLE_INTERRUPTS();
    tx_head = 0;
    tx_tail = 0;
    ENABLE_INTERRUPTS();
    
    DEBUG_PRINT("TX buffer cleared");
}

/**
 * @brief 获取通信状态
 */
comm_status_t communication_get_status(void)
{
    return current_status;
}

/**
 * @brief 获取最后一次错误
 */
comm_error_t communication_get_last_error(void)
{
    return last_error;
}

/**
 * @brief 获取通信统计信息
 */
void communication_get_statistics(comm_statistics_t* stats)
{
    if (stats != NULL) {
        memcpy(stats, &statistics, sizeof(comm_statistics_t));
    }
}

/**
 * @brief 重置通信统计信息
 */
void communication_reset_statistics(void)
{
    memset(&statistics, 0, sizeof(comm_statistics_t));
    DEBUG_PRINT("Communication statistics reset");
}

/**
 * @brief 设置数据接收回调函数
 */
void communication_set_rx_callback(void (*callback)(const uint8_t* data, uint16_t length))
{
    rx_callback = callback;
}

/**
 * @brief 设置错误回调函数
 */
void communication_set_error_callback(void (*callback)(comm_error_t error))
{
    error_callback = callback;
}

/**
 * @brief 启用/禁用中断
 */
void communication_enable_interrupt(bool enable)
{
    if (enable) {
        uart_enable_interrupts();
    } else {
        uart_disable_interrupts();
    }
}

/**
 * @brief UART中断服务程序
 */
void communication_uart_isr(void)
{
#ifdef IAR_LEGACY_SUPPORT
    uint32_t status = UART1_SR;
    uint8_t data;
    
    /* 接收中断 */
    if (status & UART_SR_RXNE) {
        data = (uint8_t)UART1_DR;
        if (!put_rx_byte(data)) {
            /* 接收缓冲区满 */
            set_last_error(COMM_ERROR_BUFFER_FULL);
        }
        current_status = COMM_STATUS_RECEIVING;
    }
    
    /* 发送中断 */
    if (status & UART_SR_TXE) {
        if (get_tx_byte(&data)) {
            UART1_DR = data;
        } else {
            /* 发送缓冲区空，禁用TXE中断 */
            UART1_CR1 &= ~UART_CR1_TXEIE;
            current_status = COMM_STATUS_IDLE;
        }
    }
    
    /* 错误处理 */
    if (status & UART_SR_ORE) {
        set_last_error(COMM_ERROR_OVERRUN);
        /* 清除错误标志 */
        data = (uint8_t)UART1_DR;
    }
    
    if (status & UART_SR_FE) {
        set_last_error(COMM_ERROR_FRAMING);
    }
    
    if (status & UART_SR_PE) {
        set_last_error(COMM_ERROR_PARITY);
    }
#endif
}

/**
 * @brief 处理接收到的数据包
 */
void communication_process_rx_data(void)
{
    uint8_t temp_buffer[COMM_MAX_PACKET_SIZE];
    uint16_t length = 0;
    
    /* 检查是否有完整的数据包 */
    if (get_rx_buffer_count() > 0) {
        /* 简单的数据包处理：读取所有可用数据 */
        while (length < sizeof(temp_buffer) && get_rx_byte(&temp_buffer[length])) {
            length++;
        }
        
        if (length > 0 && rx_callback != NULL) {
            rx_callback(temp_buffer, length);
        }
    }
}

/**
 * @brief 发送格式化字符串
 */
system_status_t communication_printf(const char* format, ...)
{
    char buffer[256];
    va_list args;
    int length;
    
    if (format == NULL) {
        set_last_error(COMM_ERROR_INVALID_PARAM);
        return SYSTEM_ERROR;
    }
    
    /* 格式化字符串 */
    va_start(args, format);
    length = vsprintf(buffer, format, args);
    va_end(args);
    
    if (length < 0 || length >= sizeof(buffer)) {
        set_last_error(COMM_ERROR_INVALID_PARAM);
        return SYSTEM_ERROR;
    }
    
    return communication_send((const uint8_t*)buffer, (uint16_t)length);
}

/**
 * @brief 配置硬件UART
 */
system_status_t communication_configure_uart(const comm_config_t* config)
{
    if (!is_valid_comm_config(config)) {
        set_last_error(COMM_ERROR_INVALID_PARAM);
        return SYSTEM_ERROR;
    }
    
    /* 保存新配置 */
    memcpy(&current_config, config, sizeof(comm_config_t));
    
    /* 重新初始化硬件 */
    uart_hardware_init(config);
    
    DEBUG_PRINT("UART reconfigured: %lu baud", config->baud_rate);
    return SYSTEM_OK;
}

/**
 * @brief 启动DMA传输
 */
system_status_t communication_start_dma_tx(const uint8_t* data, uint16_t length)
{
    /* IAR 5.3环境下的简化DMA实现 */
    if (data == NULL || length == 0) {
        set_last_error(COMM_ERROR_INVALID_PARAM);
        return SYSTEM_ERROR;
    }
    
    /* 在实际项目中，这里应该配置DMA控制器 */
    DEBUG_PRINT("DMA TX started: %d bytes", length);
    
    /* 回退到普通发送 */
    return communication_send(data, length);
}

/**
 * @brief 停止DMA传输
 */
system_status_t communication_stop_dma_tx(void)
{
    /* 在实际项目中，这里应该停止DMA传输 */
    DEBUG_PRINT("DMA TX stopped");
    return SYSTEM_OK;
}

/* 内部函数实现 */

/**
 * @brief 设置最后一次错误
 */
static void set_last_error(comm_error_t error)
{
    last_error = error;
    statistics.error_count++;
    
    if (error_callback != NULL) {
        error_callback(error);
    }
    
    switch (error) {
        case COMM_ERROR_TIMEOUT:
            statistics.timeout_count++;
            ERROR_PRINT("Communication timeout");
            break;
        case COMM_ERROR_OVERRUN:
            ERROR_PRINT("Communication overrun");
            break;
        case COMM_ERROR_FRAMING:
            ERROR_PRINT("Communication framing error");
            break;
        case COMM_ERROR_PARITY:
            ERROR_PRINT("Communication parity error");
            break;
        case COMM_ERROR_BUFFER_FULL:
            ERROR_PRINT("Communication buffer full");
            break;
        case COMM_ERROR_INVALID_PARAM:
            ERROR_PRINT("Communication invalid parameter");
            break;
        default:
            break;
    }
}

/**
 * @brief 获取接收缓冲区数据数量
 */
static uint16_t get_rx_buffer_count(void)
{
    uint16_t head, tail;
    
    DISABLE_INTERRUPTS();
    head = rx_head;
    tail = rx_tail;
    ENABLE_INTERRUPTS();
    
    if (head >= tail) {
        return head - tail;
    } else {
        return (RX_BUFFER_SIZE - tail) + head;
    }
}

/**
 * @brief 获取发送缓冲区可用空间
 */
static uint16_t get_tx_buffer_space(void)
{
    uint16_t head, tail;
    
    DISABLE_INTERRUPTS();
    head = tx_head;
    tail = tx_tail;
    ENABLE_INTERRUPTS();
    
    if (head >= tail) {
        return (TX_BUFFER_SIZE - 1) - (head - tail);
    } else {
        return (tail - head) - 1;
    }
}

/**
 * @brief 向接收缓冲区放入一个字节
 */
static bool put_rx_byte(uint8_t byte)
{
    uint16_t next_head = (rx_head + 1) % RX_BUFFER_SIZE;
    
    if (next_head == rx_tail) {
        /* 缓冲区满 */
        return false;
    }
    
    rx_buffer[rx_head] = byte;
    rx_head = next_head;
    return true;
}

/**
 * @brief 从接收缓冲区取出一个字节
 */
static bool get_rx_byte(uint8_t* byte)
{
    if (rx_tail == rx_head) {
        /* 缓冲区空 */
        return false;
    }
    
    *byte = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % RX_BUFFER_SIZE;
    return true;
}

/**
 * @brief 向发送缓冲区放入一个字节
 */
static bool put_tx_byte(uint8_t byte)
{
    uint16_t next_head = (tx_head + 1) % TX_BUFFER_SIZE;
    
    if (next_head == tx_tail) {
        /* 缓冲区满 */
        return false;
    }
    
    tx_buffer[tx_head] = byte;
    tx_head = next_head;
    return true;
}

/**
 * @brief 从发送缓冲区取出一个字节
 */
static bool get_tx_byte(uint8_t* byte)
{
    if (tx_tail == tx_head) {
        /* 缓冲区空 */
        return false;
    }
    
    *byte = tx_buffer[tx_tail];
    tx_tail = (tx_tail + 1) % TX_BUFFER_SIZE;
    return true;
}

/**
 * @brief 初始化UART硬件
 */
static void uart_hardware_init(const comm_config_t* config)
{
#ifdef IAR_LEGACY_SUPPORT
    uint32_t brr_value;
    
    /* 计算波特率寄存器值 */
    brr_value = (SYSTEM_CLOCK_FREQ + config->baud_rate / 2) / config->baud_rate;
    
    /* 禁用UART */
    UART1_CR1 &= ~UART_CR1_UE;
    
    /* 配置波特率 */
    UART1_BRR = brr_value;
    
    /* 配置数据位、校验位、停止位 */
    UART1_CR1 &= ~(UART_CR1_M | UART_CR1_PCE | UART_CR1_PS);
    if (config->data_bits == 9) {
        UART1_CR1 |= UART_CR1_M;
    }
    if (config->parity != COMM_PARITY_NONE) {
        UART1_CR1 |= UART_CR1_PCE;
        if (config->parity == COMM_PARITY_ODD) {
            UART1_CR1 |= UART_CR1_PS;
        }
    }
    
    /* 配置停止位 */
    UART1_CR2 &= ~(3 << 12);
    if (config->stop_bits == 2) {
        UART1_CR2 |= (2 << 12);
    }
    
    /* 启用发送器和接收器 */
    UART1_CR1 |= (UART_CR1_TE | UART_CR1_RE);
    
    /* 启用UART */
    UART1_CR1 |= UART_CR1_UE;
#endif
    
    DEBUG_PRINT("UART hardware initialized");
}

/**
 * @brief 启用UART中断
 */
static void uart_enable_interrupts(void)
{
#ifdef IAR_LEGACY_SUPPORT
    /* 启用接收中断 */
    UART1_CR1 |= UART_CR1_RXNEIE;
    
    /* 启用错误中断 */
    UART1_CR3 |= (1 << 0); /* EIE - Error interrupt enable */
#endif
    
    DEBUG_PRINT("UART interrupts enabled");
}

/**
 * @brief 禁用UART中断
 */
static void uart_disable_interrupts(void)
{
#ifdef IAR_LEGACY_SUPPORT
    /* 禁用所有UART中断 */
    UART1_CR1 &= ~(UART_CR1_RXNEIE | UART_CR1_TXEIE | UART_CR1_TCIE | UART_CR1_IDLEIE | UART_CR1_PEIE);
    UART1_CR3 &= ~(1 << 0); /* EIE */
#endif
    
    DEBUG_PRINT("UART interrupts disabled");
}

/**
 * @brief 获取系统滴答计数
 */
static uint32_t get_system_tick(void)
{
    /* 简化的时间实现 */
    static uint32_t tick_count = 0;
    return ++tick_count;
}