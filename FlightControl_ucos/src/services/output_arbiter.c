#include "output_arbiter.h"

#include <stddef.h>
#include <string.h>

#include "../bsp/board_safety.h"

static fc_output_snapshot_t g_output;
static fc_actuator_backend_write_t g_backend_writer;
static void *g_backend_context;
static bool g_output_initialized;

/** 检查所有归一化输出是否处于闭区间 [0,1]。 */
static bool fc_output_command_valid(const fc_actuator_command_t *command)
{
    uint32_t index;

    for (index = 0u; index < FC_HEX_MOTOR_COUNT; ++index) {
        if ((command->motor[index] < 0.0f) || (command->motor[index] > 1.0f)) {
            return false;
        }
    }
    for (index = 0u; index < FC_SERVO_OUTPUT_COUNT; ++index) {
        if ((command->servo[index] < 0.0f) || (command->servo[index] > 1.0f)) {
            return false;
        }
    }
    return true;
}

fc_status_t fc_output_arbiter_init(void)
{
    memset(&g_output, 0, sizeof(g_output));
    g_backend_writer = NULL;
    g_backend_context = NULL;
    g_output_initialized = true;
    return FC_STATUS_OK;
}

fc_status_t fc_output_attach_backend(fc_actuator_backend_write_t writer,
                                     void *context)
{
    if (writer == NULL) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    (void)context;
    return FC_STATUS_LOCKED;
}

fc_status_t fc_output_submit(const fc_actuator_command_t *command)
{
    if (command == NULL) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    if (!g_output_initialized) {
        return FC_STATUS_INVALID_STATE;
    }
    if (!fc_output_command_valid(command)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }

    g_output.requested = *command;
    if (!fc_board_safety_output_allowed(FC_OUTPUT_PROPULSION) ||
        !fc_board_safety_output_allowed(FC_OUTPUT_SERVO) ||
        (g_backend_writer == NULL)) {
        ++g_output.rejected_commands;
        memset(&g_output.applied, 0, sizeof(g_output.applied));
        return FC_STATUS_LOCKED;
    }

    if (g_backend_writer(command, g_backend_context) != FC_STATUS_OK) {
        fc_board_safety_force_lock(FC_SAFETY_FAULT_OUTPUT_BACKEND);
        memset(&g_output.applied, 0, sizeof(g_output.applied));
        return FC_STATUS_PLATFORM_ERROR;
    }
    g_output.applied = *command;
    return FC_STATUS_OK;
}

fc_status_t fc_output_snapshot(fc_output_snapshot_t *snapshot)
{
    if (snapshot == NULL) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    *snapshot = g_output;
    return FC_STATUS_OK;
}
