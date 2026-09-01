#ifndef FC_UART_PL_H
#define FC_UART_PL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../../core/fc_status.h"
#include "../driver_stats.h"

/** 四通道 PL UART 的单通道轮询实例。 */
typedef struct {
    uintptr_t channel_base;
    fc_driver_stats_t stats;
    bool initialized;
} fc_uart_pl_t;

/**
 * @brief 配置 PL UART 单通道的波特率和校验方式。
 * @param uart 调用者持有的实例。
 * @param channel_base 具体通道寄存器基地址。
 * @param clock_hz UART IP 输入时钟，当前 HDF 预期为 100 MHz。
 * @param baud_rate 目标波特率。
 * @param parity_enable 是否启用校验。
 * @param odd_parity 启用校验时选择奇校验，否则为偶校验。
 * @return 配置有效返回 FC_STATUS_OK。
 * @warning 本函数会写通信 IP 寄存器，但不会操作动力、配电或执行机构。
 */
fc_status_t fc_uart_pl_init(fc_uart_pl_t *uart,
                            uintptr_t channel_base,
                            uint32_t clock_hz,
                            uint32_t baud_rate,
                            bool parity_enable,
                            bool odd_parity);

/**
 * @brief 非阻塞读取 PL UART FIFO 中已有数据。
 * @param uart 已初始化实例。
 * @param data 接收缓冲区。
 * @param capacity 缓冲区容量。
 * @param received 返回实际读取长度。
 * @return 成功返回 FC_STATUS_OK；当前无数据也返回成功且长度为 0。
 */
fc_status_t fc_uart_pl_read(fc_uart_pl_t *uart,
                            uint8_t *data,
                            size_t capacity,
                            size_t *received);

/**
 * @brief 尽可能向 PL UART FIFO 写入数据。
 * @param uart 已初始化实例。
 * @param data 发送数据。
 * @param length 请求长度。
 * @param sent 返回实际写入长度。
 * @return 成功返回 FC_STATUS_OK；FIFO 空间不足时只写可容纳部分。
 */
fc_status_t fc_uart_pl_write(fc_uart_pl_t *uart,
                             const uint8_t *data,
                             size_t length,
                             size_t *sent);

#endif
