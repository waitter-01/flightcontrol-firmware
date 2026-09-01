#ifndef FC_AXI_AD7606_H
#define FC_AXI_AD7606_H

#include <stdbool.h>
#include <stdint.h>

#include "../../core/fc_status.h"
#include "../driver_stats.h"

#define FC_AD7606_CHANNEL_COUNT 4u

/** AD7606 自定义 AXI IP 的只读实例。 */
typedef struct {
    uintptr_t base_address;
    fc_driver_stats_t stats;
    bool initialized;
} fc_axi_ad7606_t;

/**
 * @brief 绑定 AD7606 AXI 基地址，不启动转换也不写寄存器。
 * @param adc 调用者持有的实例。
 * @param base_address HDF 中的 AXI 基地址，必须按 4 字节对齐。
 * @return 地址有效返回 FC_STATUS_OK。
 */
fc_status_t fc_axi_ad7606_init(fc_axi_ad7606_t *adc, uintptr_t base_address);

/**
 * @brief 读取 IP 暴露的四个 32 位采样寄存器。
 * @param adc 已绑定的 ADC 实例。
 * @param samples 接收四路原始寄存器值的数组。
 * @return 成功返回 FC_STATUS_OK。
 * @note 原始值的有效位、量程和采样同步关系必须在上机时确认。
 */
fc_status_t fc_axi_ad7606_read(fc_axi_ad7606_t *adc,
                               uint32_t samples[FC_AD7606_CHANNEL_COUNT]);

#endif
