#ifndef FC_MAVLINK_ENDPOINT_H
#define FC_MAVLINK_ENDPOINT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../../../FlightControl/src/mavlink/common/mavlink.h"
#include "../bsp/board_safety.h"
#include "../core/fc_status.h"

/** MAVLink 2 流式接收端。 */
typedef struct {
    mavlink_message_t message;
    mavlink_status_t status;
    uint32_t received_messages;
    uint32_t parse_errors;
} fc_mavlink_endpoint_t;

/** @brief 清空 MAVLink 解析状态和统计。 */
void fc_mavlink_endpoint_init(fc_mavlink_endpoint_t *endpoint);

/**
 * @brief 输入一个链路字节并在完整消息到达时返回消息指针。
 * @param endpoint 接收端实例。
 * @param channel MAVLink 通道编号。
 * @param byte 输入字节。
 * @param message 完整消息到达时指向实例内部消息对象。
 * @return 产生完整消息返回 true，否则返回 false。
 * @note 返回的消息指针在下一次成功解析后失效，调用者应立即处理或复制。
 */
bool fc_mavlink_endpoint_push(fc_mavlink_endpoint_t *endpoint,
                              mavlink_channel_t channel,
                              uint8_t byte,
                              const mavlink_message_t **message);

/**
 * @brief 生成 QGroundControl 可识别的飞控心跳包。
 * @param system_id MAVLink 系统编号。
 * @param component_id MAVLink 组件编号，通常为 MAV_COMP_ID_AUTOPILOT1。
 * @param safety 当前板级安全快照。
 * @param buffer 接收序列化数据，容量至少 MAVLINK_MAX_PACKET_LEN。
 * @param length 返回实际包长。
 * @return 成功返回 FC_STATUS_OK。
 * @note 未实现 ARMED 状态时始终不上报 SAFETY_ARMED 标志。
 */
fc_status_t fc_mavlink_build_heartbeat(uint8_t system_id,
                                       uint8_t component_id,
                                       fc_safety_snapshot_t safety,
                                       uint8_t *buffer,
                                       size_t *length);

/**
 * @brief 生成标准 SYS_STATUS 消息。
 * @param system_id MAVLink 系统编号。
 * @param component_id MAVLink 组件编号。
 * @param voltage_mv 电池或主电源电压，未知时传 UINT16_MAX。
 * @param current_ca 电流，单位厘安，未知时传 -1。
 * @param remaining_percent 剩余电量百分比，未知时传 -1。
 * @param buffer 接收序列化数据。
 * @param length 返回实际包长。
 * @return 成功返回 FC_STATUS_OK。
 */
fc_status_t fc_mavlink_build_sys_status(uint8_t system_id,
                                        uint8_t component_id,
                                        uint16_t voltage_mv,
                                        int16_t current_ca,
                                        int8_t remaining_percent,
                                        uint8_t *buffer,
                                        size_t *length);

#endif
