#include "health_monitor.h"

#include "../platform/platform_time.h"

typedef struct {
    volatile bool registered;
    volatile uint32_t last_heartbeat_ms;
    uint32_t timeout_ms;
    const char *name;
} fc_health_component_t;

static fc_health_component_t g_components[FC_HEALTH_MAX_COMPONENTS];

fc_status_t fc_health_init(void)
{
    uint32_t index;

    for (index = 0u; index < FC_HEALTH_MAX_COMPONENTS; ++index) {
        g_components[index].registered = false;
        g_components[index].last_heartbeat_ms = 0u;
        g_components[index].timeout_ms = 0u;
        g_components[index].name = 0;
    }

    return FC_STATUS_OK;
}

fc_status_t fc_health_register(fc_health_component_id_t component_id,
                               const char *name,
                               uint32_t timeout_ms)
{
    fc_health_component_t *component;

    if ((component_id >= FC_HEALTH_MAX_COMPONENTS) ||
        (name == 0) ||
        (timeout_ms == 0u)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }

    component = &g_components[component_id];
    if (component->registered) {
        return FC_STATUS_INVALID_STATE;
    }

    component->name = name;
    component->timeout_ms = timeout_ms;
    component->last_heartbeat_ms = fc_time_now_ms();
    component->registered = true;
    return FC_STATUS_OK;
}

fc_status_t fc_health_heartbeat(fc_health_component_id_t component_id)
{
    if ((component_id >= FC_HEALTH_MAX_COMPONENTS) ||
        !g_components[component_id].registered) {
        return FC_STATUS_INVALID_ARGUMENT;
    }

    g_components[component_id].last_heartbeat_ms = fc_time_now_ms();
    return FC_STATUS_OK;
}

bool fc_health_all_healthy(void)
{
    uint32_t index;
    uint32_t now_ms = fc_time_now_ms();

    for (index = 0u; index < FC_HEALTH_MAX_COMPONENTS; ++index) {
        if (g_components[index].registered &&
            (fc_time_elapsed_ms(g_components[index].last_heartbeat_ms, now_ms) >
             g_components[index].timeout_ms)) {
            return false;
        }
    }

    return true;
}

fc_status_t fc_health_snapshot(fc_health_component_id_t component_id,
                               fc_health_component_snapshot_t *snapshot)
{
    fc_health_component_t *component;
    uint32_t now_ms;

    if ((component_id >= FC_HEALTH_MAX_COMPONENTS) || (snapshot == 0)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }

    component = &g_components[component_id];
    if (!component->registered) {
        return FC_STATUS_INVALID_STATE;
    }

    now_ms = fc_time_now_ms();
    snapshot->registered = true;
    snapshot->age_ms = fc_time_elapsed_ms(component->last_heartbeat_ms, now_ms);
    snapshot->timeout_ms = component->timeout_ms;
    snapshot->timed_out = snapshot->age_ms > snapshot->timeout_ms;
    snapshot->name = component->name;
    return FC_STATUS_OK;
}
