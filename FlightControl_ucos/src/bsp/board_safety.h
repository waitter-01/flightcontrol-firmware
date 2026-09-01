#ifndef FC_BOARD_SAFETY_H
#define FC_BOARD_SAFETY_H

#include <stdbool.h>
#include <stdint.h>

#include "../core/fc_status.h"

typedef enum {
    FC_SAFETY_BOOT_LOCKED = 0,
    FC_SAFETY_DISARMED,
    FC_SAFETY_FAULT_LOCKED
} fc_safety_state_t;

typedef enum {
    FC_OUTPUT_PROPULSION = 0,
    FC_OUTPUT_SERVO,
    FC_OUTPUT_POWER,
    FC_OUTPUT_RELAY,
    FC_OUTPUT_FLASH_WRITE,
    FC_OUTPUT_DOMAIN_COUNT
} fc_output_domain_t;

typedef enum {
    FC_SAFETY_FAULT_NONE = 0u,
    FC_SAFETY_FAULT_INITIALIZATION = (1u << 0),
    FC_SAFETY_FAULT_HEALTH_TIMEOUT = (1u << 1),
    FC_SAFETY_FAULT_EXTERNAL = (1u << 2)
} fc_safety_fault_t;

typedef struct {
    fc_safety_state_t state;
    uint32_t fault_mask;
} fc_safety_snapshot_t;

fc_status_t fc_board_safety_init(void);
fc_status_t fc_board_safety_enter_disarmed(void);
void fc_board_safety_force_lock(fc_safety_fault_t fault);
bool fc_board_safety_output_allowed(fc_output_domain_t domain);
fc_safety_snapshot_t fc_board_safety_snapshot(void);

#endif
