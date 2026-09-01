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
    FC_SAFETY_FAULT_EXTERNAL = (1u << 2),
    FC_SAFETY_FAULT_OUTPUT_BACKEND = (1u << 3)
} fc_safety_fault_t;

typedef struct {
    fc_safety_state_t state;
    uint32_t fault_mask;
} fc_safety_snapshot_t;

/** @brief 初始化安全状态为 BOOT_LOCKED，并清空故障位。 */
fc_status_t fc_board_safety_init(void);
/** @brief 基础服务正常后进入 DISARMED；危险输出仍全部禁止。 */
fc_status_t fc_board_safety_enter_disarmed(void);
/** @brief 锁存故障并立即进入 FAULT_LOCKED，仅允许向更安全状态转换。 */
void fc_board_safety_force_lock(fc_safety_fault_t fault);
/** @brief 查询输出域许可；当前构建对所有域固定返回 false。 */
bool fc_board_safety_output_allowed(fc_output_domain_t domain);
/** @brief 返回安全状态和故障掩码的一致性快照。 */
fc_safety_snapshot_t fc_board_safety_snapshot(void);

#endif
