#ifndef FC_I2C_MASTER_H
#define FC_I2C_MASTER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <xiic.h>

#include "../../core/fc_status.h"
#include "../driver_stats.h"

/** AXI IIC 主机实例。 */
typedef struct {
    XIic instance;
    uintptr_t base_address;
    fc_driver_stats_t stats;
    bool initialized;
} fc_i2c_master_t;

/**
 * @brief 初始化 AXI IIC 主机，但不访问任何从设备。
 * @param bus 调用者持有的总线实例。
 * @param device_id xparameters 中的 AXI IIC 设备编号。
 * @return 成功返回 FC_STATUS_OK。
 * @note 只能在任务初始化阶段调用，不得在中断上下文调用。
 */
fc_status_t fc_i2c_master_init(fc_i2c_master_t *bus, uint16_t device_id);

/**
 * @brief 先写寄存器地址，再使用重复起始读取从设备。
 * @param bus 已初始化的 IIC 实例。
 * @param address 7 位从设备地址。
 * @param prefix 读取前发送的寄存器地址或命令。
 * @param prefix_length 命令长度，必须大于 0。
 * @param data 接收缓冲区。
 * @param data_length 期望读取的字节数。
 * @return 收发长度完全匹配返回 FC_STATUS_OK，否则返回平台错误。
 * @warning 底层 SDK 2019.1 接口为轮询实现，只允许在低频外设任务中调用。
 */
fc_status_t fc_i2c_master_write_read(fc_i2c_master_t *bus,
                                     uint8_t address,
                                     const uint8_t *prefix,
                                     size_t prefix_length,
                                     uint8_t *data,
                                     size_t data_length);

/**
 * @brief 读取 IIC 驱动累计统计。
 * @param bus 已初始化或待诊断的实例。
 * @param stats 接收统计快照。
 * @return 参数有效返回 FC_STATUS_OK。
 */
fc_status_t fc_i2c_master_stats(const fc_i2c_master_t *bus,
                                fc_driver_stats_t *stats);

#endif
