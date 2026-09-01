#include "platform_irq.h"

#include <stddef.h>
#include <ucos_int.h>

#define FC_IRQ_MAX_ID        159u
#define FC_IRQ_MAX_PRIORITY  255u

fc_status_t fc_irq_register(const fc_irq_config_t *config)
{
    CPU_BOOLEAN result;
    UCOS_INT_TYPE trigger;

    if ((config == NULL) || (config->handler == NULL) ||
        (config->interrupt_id > FC_IRQ_MAX_ID) ||
        (config->priority > FC_IRQ_MAX_PRIORITY) ||
        (config->cpu_target_mask == 0u)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }

    trigger = (config->trigger == FC_IRQ_EDGE_TRIGGERED)
                  ? UCOS_INT_TYPE_EDGE
                  : UCOS_INT_TYPE_LEVEL;

    result = UCOS_IntTypeSet(config->interrupt_id, trigger);
    if (result != DEF_OK) {
        return FC_STATUS_PLATFORM_ERROR;
    }

    result = UCOS_IntVectSet(config->interrupt_id,
                             config->priority,
                             config->cpu_target_mask,
                             config->handler,
                             config->context);
    if (result != DEF_OK) {
        return FC_STATUS_PLATFORM_ERROR;
    }

    return (UCOS_IntSrcEn(config->interrupt_id) == DEF_OK)
               ? FC_STATUS_OK
               : FC_STATUS_PLATFORM_ERROR;
}

fc_status_t fc_irq_disable(uint32_t interrupt_id)
{
    if (interrupt_id > FC_IRQ_MAX_ID) {
        return FC_STATUS_INVALID_ARGUMENT;
    }

    return (UCOS_IntSrcDis(interrupt_id) == DEF_OK)
               ? FC_STATUS_OK
               : FC_STATUS_PLATFORM_ERROR;
}
