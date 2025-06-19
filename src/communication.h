#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

/* 通信接口类型 */
typedef enum {
    COMM_UART = 0,
    COMM_SPI,
    COMM_I2C,
    COMM_ETHERNET
} comm_interface_t;

/* 通信状态 */
typedef enum {
    COMM_STATUS_IDLE = 0,
    COMM_STATUS_RECEIVING,
    COMM_STATUS_PROCESSING,
    COMM_STATUS_ERROR
} comm_status_t;

/* 接收缓冲区结构 */
typedef struct {
    uint8_t buffer[RX_BUFFER_SIZE];
    uint16_t head;
    uint16_t tail;
    uint16_t count;
    bool overflow;
} rx_buffer_t;

/* 发送缓冲区结构 */
typedef struct {
    uint8_t buffer[TX_BUFFER_SIZE];
    uint16_t head;
    uint16_t tail;
    uint16_t count;
} tx_buffer_t;

/* 通信模块结构 */
typedef struct {
    comm_interface_t interface;
    comm_status_t status;
    rx_buffer_t rx_buf;
    tx_buffer_t tx_buf;
    uint32_t rx_count;
    uint32_t tx_count;
    uint32_t error_count;
} comm_module_t;

/* 函数声明 */
void communication_init(void);
void communication_uart_init(uint32_t baudrate);
bool communication_is_data_available(void);
uint16_t communication_read_data(uint8_t* buffer, uint16_t max_length);
bool communication_send_data(const uint8_t* data, uint16_t length);
void communication_process_rx_interrupt(uint8_t data);
void communication_process_tx_interrupt(void);
comm_status_t communication_get_status(void);
void communication_clear_buffers(void);

/* 数据包处理函数 */
bool communication_parse_packet(const uint8_t* data, uint16_t length, char* output, uint16_t output_size);
uint8_t communication_calculate_checksum(const uint8_t* data, uint16_t length);

#endif /* COMMUNICATION_H */