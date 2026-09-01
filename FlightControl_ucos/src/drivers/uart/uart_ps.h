#ifndef FC_UART_PS_H
#define FC_UART_PS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ucos_uartps.h>

#include "../../core/fc_status.h"
#include "../driver_stats.h"

/** Micrium PS UART 适配实例。 */
typedef struct {
    UARTPS_HANDLE handle;
    fc_driver_stats_t stats;
    bool initialized;
} fc_uart_ps_t;

/**
 * @brief 使用 Micrium BSP 初始化并配置 PS UART。
 * @param uart 调用者持有的实例。
 * @param device_id PS UART 设备编号。
 * @param baud_rate 波特率。
 * @param data_bits Micrium `UARTPS_FORMAT_*_BITS` 值。
 * @param parity Micrium `UARTPS_FORMAT_*_PARITY` 值。
 * @param stop_bits Micrium `UARTPS_FORMAT_*_STOP_BIT` 值。
 * @return 初始化成功返回 FC_STATUS_OK。
 * @warning 不得对当前控制台 UART 重复初始化；链路归属由系统配置统一决定。
 */
fc_status_t fc_uart_ps_init(fc_uart_ps_t *uart,
                            uint32_t device_id,
                            uint32_t baud_rate,
                            uint32_t data_bits,
                            uint32_t parity,
                            uint32_t stop_bits);

/**
 * @brief 发送固定长度数据。
 * @param uart 已初始化的 UART 实例。
 * @param data 待发送数据。
 * @param length 数据长度。
 * @return 全部写入返回 FC_STATUS_OK。
 * @note 运行于任务上下文，Micrium 驱动内部使用锁和信号量。
 */
fc_status_t fc_uart_ps_write(fc_uart_ps_t *uart,
                             const uint8_t *data,
                             size_t length);

#endif
