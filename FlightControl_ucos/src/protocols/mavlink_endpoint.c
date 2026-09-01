#include "mavlink_endpoint.h"

#include <string.h>

void fc_mavlink_endpoint_init(fc_mavlink_endpoint_t *endpoint)
{
    if (endpoint != NULL) {
        memset(endpoint, 0, sizeof(*endpoint));
    }
}

bool fc_mavlink_endpoint_push(fc_mavlink_endpoint_t *endpoint,
                              mavlink_channel_t channel,
                              uint8_t byte,
                              const mavlink_message_t **message)
{
    uint8_t parsed;

    if ((endpoint == NULL) || (message == NULL)) {
        return false;
    }
    parsed = mavlink_parse_char(channel, byte, &endpoint->message,
                                &endpoint->status);
    endpoint->parse_errors = endpoint->status.parse_error;
    if (parsed == 0u) {
        return false;
    }
    ++endpoint->received_messages;
    *message = &endpoint->message;
    return true;
}

fc_status_t fc_mavlink_build_heartbeat(uint8_t system_id,
                                       uint8_t component_id,
                                       fc_safety_snapshot_t safety,
                                       uint8_t *buffer,
                                       size_t *length)
{
    mavlink_message_t message;
    uint8_t system_status;
    uint8_t base_mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;

    if ((buffer == NULL) || (length == NULL)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    system_status = (safety.state == FC_SAFETY_FAULT_LOCKED)
                        ? MAV_STATE_CRITICAL
                        : MAV_STATE_STANDBY;
    mavlink_msg_heartbeat_pack(system_id, component_id, &message,
                               MAV_TYPE_HEXAROTOR,
                               MAV_AUTOPILOT_GENERIC,
                               base_mode, 0u, system_status);
    *length = mavlink_msg_to_send_buffer(buffer, &message);
    return FC_STATUS_OK;
}

fc_status_t fc_mavlink_build_sys_status(uint8_t system_id,
                                        uint8_t component_id,
                                        uint16_t voltage_mv,
                                        int16_t current_ca,
                                        int8_t remaining_percent,
                                        uint8_t *buffer,
                                        size_t *length)
{
    mavlink_message_t message;

    if ((buffer == NULL) || (length == NULL)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    mavlink_msg_sys_status_pack(system_id, component_id, &message,
                                0u, 0u, 0u, 0u,
                                voltage_mv, current_ca, remaining_percent,
                                0u, 0u, 0u, 0u, 0u, 0u,
                                0u, 0u, 0u);
    *length = mavlink_msg_to_send_buffer(buffer, &message);
    return FC_STATUS_OK;
}
