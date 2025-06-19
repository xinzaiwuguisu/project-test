#include "communication.h"
#include "config.h"
#include <string.h>
#include <stdio.h>

/* 通信模块实例 */
static comm_module_t comm_module;

/* 数据包分隔符 */
#define PACKET_START_MARKER    0xAA
#define PACKET_END_MARKER      0x55

/**
 * @brief 初始化通信模块
 */
void communication_init(void)
{
    memset(&comm_module, 0, sizeof(comm_module_t));
    comm_module.interface = COMM_UART;
    comm_module.status = COMM_STATUS_IDLE;
    
    /* 初始化UART */
    communication_uart_init(UART_BAUD_RATE);
    
    printf("Communication module initialized\n");
}

/**
 * @brief 初始化UART通信
 * @param baudrate 波特率
 */
void communication_uart_init(uint32_t baudrate)
{
    /* 在实际的嵌入式系统中，这里需要配置具体的UART寄存器
     * 包括：
     * - 波特率设置
     * - 数据位、停止位、校验位配置
     * - 中断使能
     * - GPIO引脚配置
     */
    
    printf("UART initialized with baudrate: %u\n", (unsigned int)baudrate);
    
    /* 模拟UART初始化完成 */
    comm_module.status = COMM_STATUS_IDLE;
}

/**
 * @brief 检查是否有数据可读
 * @return true表示有数据，false表示无数据
 */
bool communication_is_data_available(void)
{
    return (comm_module.rx_buf.count > 0);
}

/**
 * @brief 从接收缓冲区读取数据
 * @param buffer 输出缓冲区
 * @param max_length 最大读取长度
 * @return 实际读取的字节数
 */
uint16_t communication_read_data(uint8_t* buffer, uint16_t max_length)
{
    if (!buffer || max_length == 0) {
        return 0;
    }
    
    uint16_t bytes_read = 0;
    
    /* 临界区保护 */
    #ifndef TEST_MODE
    __disable_irq();
    #endif
    
    while (comm_module.rx_buf.count > 0 && bytes_read < max_length) {
        buffer[bytes_read] = comm_module.rx_buf.buffer[comm_module.rx_buf.tail];
        comm_module.rx_buf.tail = (comm_module.rx_buf.tail + 1) % RX_BUFFER_SIZE;
        comm_module.rx_buf.count--;
        bytes_read++;
    }
    
    #ifndef TEST_MODE
    __enable_irq();
    #endif
    
    return bytes_read;
}

/**
 * @brief 发送数据
 * @param data 要发送的数据
 * @param length 数据长度
 * @return true表示成功，false表示失败
 */
bool communication_send_data(const uint8_t* data, uint16_t length)
{
    if (!data || length == 0) {
        return false;
    }
    
    /* 检查发送缓冲区空间 */
    if (comm_module.tx_buf.count + length > TX_BUFFER_SIZE) {
        return false; // 缓冲区满
    }
    
    /* 临界区保护 */
    #ifndef TEST_MODE
    __disable_irq();
    #endif
    
    /* 将数据复制到发送缓冲区 */
    for (uint16_t i = 0; i < length; i++) {
        comm_module.tx_buf.buffer[comm_module.tx_buf.head] = data[i];
        comm_module.tx_buf.head = (comm_module.tx_buf.head + 1) % TX_BUFFER_SIZE;
        comm_module.tx_buf.count++;
    }
    
    /* 启动发送（在实际系统中需要启动UART发送中断） */
    /* UART_StartTransmission(); */
    
    #ifndef TEST_MODE
    __enable_irq();
    #endif
    
    comm_module.tx_count += length;
    return true;
}

/**
 * @brief UART接收中断处理函数
 * @param data 接收到的字节
 */
void communication_process_rx_interrupt(uint8_t data)
{
    /* 检查接收缓冲区是否满 */
    if (comm_module.rx_buf.count >= RX_BUFFER_SIZE) {
        comm_module.rx_buf.overflow = true;
        comm_module.error_count++;
        return;
    }
    
    /* 将数据存入接收缓冲区 */
    comm_module.rx_buf.buffer[comm_module.rx_buf.head] = data;
    comm_module.rx_buf.head = (comm_module.rx_buf.head + 1) % RX_BUFFER_SIZE;
    comm_module.rx_buf.count++;
    
    comm_module.rx_count++;
    comm_module.status = COMM_STATUS_RECEIVING;
}

/**
 * @brief UART发送中断处理函数
 */
void communication_process_tx_interrupt(void)
{
    if (comm_module.tx_buf.count > 0) {
        /* 发送下一个字节 */
        uint8_t data = comm_module.tx_buf.buffer[comm_module.tx_buf.tail];
        comm_module.tx_buf.tail = (comm_module.tx_buf.tail + 1) % TX_BUFFER_SIZE;
        comm_module.tx_buf.count--;
        
        /* 在实际系统中，这里需要将data写入UART数据寄存器 */
        /* UART_SendByte(data); */
        
        if (comm_module.tx_buf.count == 0) {
            /* 发送完成，禁用发送中断 */
            /* UART_DisableTransmitInterrupt(); */
            comm_module.status = COMM_STATUS_IDLE;
        }
    }
}

/**
 * @brief 获取通信状态
 * @return 当前通信状态
 */
comm_status_t communication_get_status(void)
{
    return comm_module.status;
}

/**
 * @brief 清空通信缓冲区
 */
void communication_clear_buffers(void)
{
    #ifndef TEST_MODE
    __disable_irq();
    #endif
    
    memset(&comm_module.rx_buf, 0, sizeof(rx_buffer_t));
    memset(&comm_module.tx_buf, 0, sizeof(tx_buffer_t));
    
    #ifndef TEST_MODE
    __enable_irq();
    #endif
    
    printf("Communication buffers cleared\n");
}

/**
 * @brief 解析数据包
 * @param data 原始数据
 * @param length 数据长度
 * @param output 输出字符串
 * @param output_size 输出缓冲区大小
 * @return true表示解析成功，false表示失败
 */
bool communication_parse_packet(const uint8_t* data, uint16_t length, char* output, uint16_t output_size)
{
    if (!data || !output || length < 3 || output_size == 0) {
        return false;
    }
    
    /* 查找数据包起始标记 */
    uint16_t start_pos = 0;
    bool found_start = false;
    
    for (uint16_t i = 0; i < length; i++) {
        if (data[i] == PACKET_START_MARKER) {
            start_pos = i + 1;
            found_start = true;
            break;
        }
    }
    
    if (!found_start) {
        return false;
    }
    
    /* 查找数据包结束标记 */
    uint16_t end_pos = 0;
    bool found_end = false;
    
    for (uint16_t i = start_pos; i < length; i++) {
        if (data[i] == PACKET_END_MARKER) {
            end_pos = i;
            found_end = true;
            break;
        }
    }
    
    if (!found_end || end_pos <= start_pos) {
        return false;
    }
    
    /* 提取数据内容 */
    uint16_t data_length = end_pos - start_pos;
    if (data_length >= output_size) {
        data_length = output_size - 1;
    }
    
    memcpy(output, &data[start_pos], data_length);
    output[data_length] = '\0';
    
    /* 验证校验和（如果需要） */
    uint8_t calculated_checksum = communication_calculate_checksum(&data[start_pos], data_length);
    
    return true;
}

/**
 * @brief 计算校验和
 * @param data 数据
 * @param length 数据长度
 * @return 校验和
 */
uint8_t communication_calculate_checksum(const uint8_t* data, uint16_t length)
{
    uint8_t checksum = 0;
    
    for (uint16_t i = 0; i < length; i++) {
        checksum ^= data[i];
    }
    
    return checksum;
}