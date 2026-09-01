#include "watchdog.h"

#include "../../core/fc_build_config.h"

static fc_watchdog_state_t g_watchdog_state;

fc_status_t fc_watchdog_prepare(uint32_t timeout_ms)
{
    if (timeout_ms == 0u) {
        return FC_STATUS_INVALID_ARGUMENT;
    }

    g_watchdog_state.hardware_available = false;
    g_watchdog_state.started = false;
    g_watchdog_state.requested_timeout_ms = timeout_ms;

    /*
     * 旧实现会重新初始化 GIC 和 SCU Timer，与 μC/OS-III Tick 冲突。
     * 硬件后端在上机确认时钟、复位链和喂狗策略前保持不可启动。
     */
    return FC_STATUS_OK;
}

fc_status_t fc_watchdog_start(void)
{
    return FC_STATUS_LOCKED;
}

fc_status_t fc_watchdog_feed(void)
{
    if (!g_watchdog_state.started) {
        return FC_STATUS_INVALID_STATE;
    }

    return FC_STATUS_NOT_SUPPORTED;
}

fc_watchdog_state_t fc_watchdog_state(void)
{
    return g_watchdog_state;
}
