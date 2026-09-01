#include "board_safety.h"

#include "../core/fc_build_config.h"

static volatile fc_safety_state_t g_safety_state = FC_SAFETY_BOOT_LOCKED;
static volatile uint32_t g_safety_fault_mask = FC_SAFETY_FAULT_NONE;
static volatile bool g_safety_initialized;

fc_status_t fc_board_safety_init(void)
{
    if (g_safety_initialized) {
        return FC_STATUS_INVALID_STATE;
    }

    g_safety_fault_mask = FC_SAFETY_FAULT_NONE;
    g_safety_state = FC_SAFETY_BOOT_LOCKED;
    g_safety_initialized = true;
    return FC_STATUS_OK;
}

fc_status_t fc_board_safety_enter_disarmed(void)
{
    if (!g_safety_initialized ||
        (g_safety_state != FC_SAFETY_BOOT_LOCKED) ||
        (g_safety_fault_mask != FC_SAFETY_FAULT_NONE)) {
        return FC_STATUS_INVALID_STATE;
    }

    g_safety_state = FC_SAFETY_DISARMED;
    return FC_STATUS_OK;
}

void fc_board_safety_force_lock(fc_safety_fault_t fault)
{
    g_safety_initialized = true;
    g_safety_fault_mask |= (uint32_t)fault;
    g_safety_state = FC_SAFETY_FAULT_LOCKED;
}

bool fc_board_safety_output_allowed(fc_output_domain_t domain)
{
    if (domain >= FC_OUTPUT_DOMAIN_COUNT) {
        return false;
    }

    /* 本阶段没有“已解锁”状态，所有危险输出始终被软件门禁止。 */
    return false;
}

fc_safety_snapshot_t fc_board_safety_snapshot(void)
{
    fc_safety_snapshot_t snapshot;

    snapshot.state = g_safety_state;
    snapshot.fault_mask = g_safety_fault_mask;
    return snapshot;
}
