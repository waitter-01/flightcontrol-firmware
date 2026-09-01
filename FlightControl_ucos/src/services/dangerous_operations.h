#ifndef FC_DANGEROUS_OPERATIONS_H
#define FC_DANGEROUS_OPERATIONS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../core/fc_status.h"

/**
 * @brief 请求改变配电通道状态。
 * @return 当前版本固定返回 FC_STATUS_LOCKED，且不访问 GPIO。
 */
fc_status_t fc_power_channel_set(uint8_t channel, bool enabled);

/**
 * @brief 请求改变继电器状态。
 * @return 当前版本固定返回 FC_STATUS_LOCKED，且不访问 GPIO。
 */
fc_status_t fc_relay_set(uint8_t channel, bool enabled);

/**
 * @brief 请求写入非易失存储。
 * @return 当前版本固定返回 FC_STATUS_LOCKED，且不执行 QSPI 传输。
 */
fc_status_t fc_flash_write(uint32_t address,
                           const uint8_t *data,
                           size_t length);

/**
 * @brief 请求擦除非易失存储区域。
 * @return 当前版本固定返回 FC_STATUS_LOCKED，且不执行 QSPI 传输。
 */
fc_status_t fc_flash_erase(uint32_t address, size_t length);

#endif
