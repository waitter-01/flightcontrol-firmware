#ifndef FC_HEALTH_MONITOR_H
#define FC_HEALTH_MONITOR_H

#include <stdbool.h>
#include <stdint.h>

#include "../core/fc_status.h"

#define FC_HEALTH_MAX_COMPONENTS 8u

typedef uint8_t fc_health_component_id_t;

typedef struct {
    bool registered;
    bool timed_out;
    uint32_t age_ms;
    uint32_t timeout_ms;
    const char *name;
} fc_health_component_snapshot_t;

fc_status_t fc_health_init(void);
fc_status_t fc_health_register(fc_health_component_id_t component_id,
                               const char *name,
                               uint32_t timeout_ms);
fc_status_t fc_health_heartbeat(fc_health_component_id_t component_id);
bool fc_health_all_healthy(void);
fc_status_t fc_health_snapshot(fc_health_component_id_t component_id,
                               fc_health_component_snapshot_t *snapshot);

#endif
