#include "hex_mixer.h"

#include <stddef.h>
#include <stdint.h>

/** 将电机命令整体缩放到 0 至 1，同时保留相对控制差。 */
static void fc_hex_normalize(float motor[FC_HEX_MOTOR_COUNT])
{
    uint32_t index;
    float minimum = motor[0];
    float maximum = motor[0];
    float range;

    for (index = 1u; index < FC_HEX_MOTOR_COUNT; ++index) {
        if (motor[index] < minimum) minimum = motor[index];
        if (motor[index] > maximum) maximum = motor[index];
    }
    if (minimum < 0.0f) {
        for (index = 0u; index < FC_HEX_MOTOR_COUNT; ++index) motor[index] -= minimum;
        maximum -= minimum;
    }
    if (maximum > 1.0f) {
        range = maximum;
        for (index = 0u; index < FC_HEX_MOTOR_COUNT; ++index) motor[index] /= range;
    }
}

fc_status_t fc_hex_mixer_allocate(const fc_control_demand_t *demand,
                                  float motor[FC_HEX_MOTOR_COUNT])
{
    static const float roll[FC_HEX_MOTOR_COUNT] =
        {0.0f, 0.8660254f, 0.8660254f, 0.0f, -0.8660254f, -0.8660254f};
    static const float pitch[FC_HEX_MOTOR_COUNT] =
        {-1.0f, -0.5f, 0.5f, 1.0f, 0.5f, -0.5f};
    static const float yaw[FC_HEX_MOTOR_COUNT] =
        {1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f};
    uint32_t index;

    if ((demand == NULL) || (motor == NULL) ||
        (demand->thrust < 0.0f) || (demand->thrust > 1.0f)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0u; index < FC_HEX_MOTOR_COUNT; ++index) {
        motor[index] = demand->thrust + demand->roll * roll[index] +
                       demand->pitch * pitch[index] + demand->yaw * yaw[index];
    }
    fc_hex_normalize(motor);
    return FC_STATUS_OK;
}
