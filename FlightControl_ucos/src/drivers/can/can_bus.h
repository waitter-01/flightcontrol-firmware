#ifndef FC_CAN_BUS_H
#define FC_CAN_BUS_H

#include <stdbool.h>
#include <stdint.h>

#include <xcanps.h>

#include "../../core/fc_status.h"
#include "../driver_stats.h"

#define FC_CAN_RAW_FRAME_WORDS 4u

/** PS CAN 轮询驱动实例。 */
typedef struct {
    XCanPs instance;
    fc_driver_stats_t stats;
    bool initialized;
} fc_can_bus_t;

/**
 * @brief 初始化 PS CAN 并进入正常模式。
 * @param bus 调用者持有的 CAN 实例。
 * @param device_id CAN0 或 CAN1 的设备编号。
 * @param prescaler 波特率预分频值。
 * @param sync_jump_width 同步跳转宽度。
 * @param time_segment1 位时间段 1。
 * @param time_segment2 位时间段 2。
 * @param timeout_ms 模式切换最大等待时间。
 * @return 初始化成功返回 FC_STATUS_OK，超时返回 FC_STATUS_TIMEOUT。
 * @warning 位时序必须结合 100 MHz CAN 时钟和目标总线速率计算并复核。
 */
fc_status_t fc_can_init(fc_can_bus_t *bus,
                        uint16_t device_id,
                        uint8_t prescaler,
                        uint8_t sync_jump_width,
                        uint8_t time_segment1,
                        uint8_t time_segment2,
                        uint32_t timeout_ms);

/**
 * @brief 在限定时间内发送一个 Xilinx 原始 CAN 帧。
 * @param bus 已初始化的 CAN 实例。
 * @param frame 四个 32 位字组成的 XCanPs 帧。
 * @param timeout_ms 等待发送 FIFO 空位的最大时间。
 * @return 成功返回 FC_STATUS_OK，FIFO 持续占满返回 FC_STATUS_TIMEOUT。
 */
fc_status_t fc_can_send(fc_can_bus_t *bus,
                        uint32_t frame[FC_CAN_RAW_FRAME_WORDS],
                        uint32_t timeout_ms);

/**
 * @brief 非阻塞读取一个 Xilinx 原始 CAN 帧。
 * @param bus 已初始化的 CAN 实例。
 * @param frame 接收四个 32 位字的缓冲区。
 * @return 有帧返回 FC_STATUS_OK，无帧返回 FC_STATUS_BUSY。
 */
fc_status_t fc_can_receive(fc_can_bus_t *bus,
                           uint32_t frame[FC_CAN_RAW_FRAME_WORDS]);

#endif
