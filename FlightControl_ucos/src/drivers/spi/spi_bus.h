#ifndef FC_SPI_BUS_H
#define FC_SPI_BUS_H

#include <stddef.h>
#include <stdint.h>

#include "../../core/fc_status.h"

/**
 * @brief 执行一次全双工 SPI 传输。
 * @param tx 发送缓冲区。
 * @param rx 接收缓冲区。
 * @param length 传输字节数。
 * @return 当前 HDF 未提供通用 SPI 控制器，固定返回 FC_STATUS_NOT_SUPPORTED。
 * @note QSPI Flash 不作为传感器 SPI 使用，避免总线归属和片选冲突。
 */
fc_status_t fc_spi_transfer(const uint8_t *tx, uint8_t *rx, size_t length);

#endif
