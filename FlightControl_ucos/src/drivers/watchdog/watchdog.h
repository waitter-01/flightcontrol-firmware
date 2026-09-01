#ifndef FC_WATCHDOG_H
#define FC_WATCHDOG_H

#include <stdbool.h>
#include <stdint.h>

#include "../../core/fc_status.h"

typedef struct {
    bool hardware_available;
    bool started;
    uint32_t requested_timeout_ms;
} fc_watchdog_state_t;

fc_status_t fc_watchdog_prepare(uint32_t timeout_ms);
fc_status_t fc_watchdog_start(void);
fc_status_t fc_watchdog_feed(void);
fc_watchdog_state_t fc_watchdog_state(void);

#endif
