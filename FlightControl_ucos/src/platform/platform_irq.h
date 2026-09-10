#ifndef FC_PLATFORM_IRQ_H
#define FC_PLATFORM_IRQ_H

#include <stdbool.h>
#include <stdint.h>

#include <cpu.h>

#include "../core/fc_status.h"

/** v1.45 统一采用 Xilinx 单参数中断回调签名。 */
typedef void (*fc_irq_handler_t)(void *context);

/** 中断触发方式。 */
typedef enum {
    FC_IRQ_LEVEL_TRIGGERED = 0,
    FC_IRQ_EDGE_TRIGGERED
} fc_irq_trigger_t;

/** 中断注册参数。 */
typedef struct {
    CPU_INT32U interrupt_id;
    CPU_INT32U priority;
    uint8_t cpu_target_mask;
    fc_irq_trigger_t trigger;
    fc_irq_handler_t handler;
    void *context;
} fc_irq_config_t;

/**
 * @brief 将外设中断挂接到 μC/OS-III 已初始化的 GIC。
 *
 * @param config 中断号、优先级、目标 CPU、触发方式、单参数回调及上下文。
 * @return 注册并使能成功返回 FC_STATUS_OK，否则返回平台错误。
 * @warning 该接口不会重新初始化 GIC；回调运行于中断上下文，不得阻塞、动态分配内存或写日志。
 */
fc_status_t fc_irq_register(const fc_irq_config_t *config);

/**
 * @brief 禁用指定中断源。
 *
 * @param interrupt_id GIC 中断号。
 * @return 成功返回 FC_STATUS_OK，否则返回平台错误。
 * @note 本函数只屏蔽中断源，不销毁 μC/OS-III 的全局中断控制器。
 */
fc_status_t fc_irq_disable(uint32_t interrupt_id);

#endif
