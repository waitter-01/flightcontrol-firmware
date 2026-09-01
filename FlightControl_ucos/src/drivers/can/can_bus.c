#include "can_bus.h"

#include <stddef.h>
#include <string.h>
#include <xstatus.h>

#include "../../platform/platform_time.h"

/** 在限定时间内等待控制器进入目标模式。 */
static fc_status_t fc_can_wait_mode(fc_can_bus_t *bus,
                                    uint8_t mode,
                                    uint32_t timeout_ms)
{
    uint32_t start_ms = fc_time_now_ms();

    while (XCanPs_GetMode(&bus->instance) != mode) {
        if (fc_time_elapsed_ms(start_ms, fc_time_now_ms()) >= timeout_ms) {
            ++bus->stats.timeouts;
            return FC_STATUS_TIMEOUT;
        }
    }
    return FC_STATUS_OK;
}

fc_status_t fc_can_init(fc_can_bus_t *bus,
                        uint16_t device_id,
                        uint8_t prescaler,
                        uint8_t sync_jump_width,
                        uint8_t time_segment1,
                        uint8_t time_segment2,
                        uint32_t timeout_ms)
{
    XCanPs_Config *config;
    fc_status_t status;

    if ((bus == NULL) || (timeout_ms == 0u)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }

    memset(bus, 0, sizeof(*bus));
    config = XCanPs_LookupConfig(device_id);
    if (config == NULL) {
        return FC_STATUS_NOT_SUPPORTED;
    }
    if ((XCanPs_CfgInitialize(&bus->instance, config, config->BaseAddr) != XST_SUCCESS) ||
        (XCanPs_SelfTest(&bus->instance) != XST_SUCCESS)) {
        return FC_STATUS_PLATFORM_ERROR;
    }

    XCanPs_EnterMode(&bus->instance, XCANPS_MODE_CONFIG);
    status = fc_can_wait_mode(bus, XCANPS_MODE_CONFIG, timeout_ms);
    if (status != FC_STATUS_OK) {
        return status;
    }

    XCanPs_SetBaudRatePrescaler(&bus->instance, prescaler);
    XCanPs_SetBitTiming(&bus->instance, sync_jump_width,
                        time_segment2, time_segment1);
    XCanPs_EnterMode(&bus->instance, XCANPS_MODE_NORMAL);
    status = fc_can_wait_mode(bus, XCANPS_MODE_NORMAL, timeout_ms);
    if (status == FC_STATUS_OK) {
        bus->initialized = true;
    }
    return status;
}

fc_status_t fc_can_send(fc_can_bus_t *bus,
                        uint32_t frame[FC_CAN_RAW_FRAME_WORDS],
                        uint32_t timeout_ms)
{
    uint32_t start_ms;

    if ((bus == NULL) || (frame == NULL) || (timeout_ms == 0u)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    if (!bus->initialized) {
        return FC_STATUS_INVALID_STATE;
    }

    start_ms = fc_time_now_ms();
    while (XCanPs_IsTxFifoFull(&bus->instance) == TRUE) {
        if (fc_time_elapsed_ms(start_ms, fc_time_now_ms()) >= timeout_ms) {
            ++bus->stats.timeouts;
            return FC_STATUS_TIMEOUT;
        }
    }

    if (XCanPs_Send(&bus->instance, frame) != XST_SUCCESS) {
        ++bus->stats.errors;
        return FC_STATUS_PLATFORM_ERROR;
    }
    ++bus->stats.transfers;
    bus->stats.bytes += sizeof(uint32_t) * FC_CAN_RAW_FRAME_WORDS;
    return FC_STATUS_OK;
}

fc_status_t fc_can_receive(fc_can_bus_t *bus,
                           uint32_t frame[FC_CAN_RAW_FRAME_WORDS])
{
    if ((bus == NULL) || (frame == NULL)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    if (!bus->initialized) {
        return FC_STATUS_INVALID_STATE;
    }
    if (XCanPs_IsRxEmpty(&bus->instance) == TRUE) {
        return FC_STATUS_BUSY;
    }
    if (XCanPs_Recv(&bus->instance, frame) != XST_SUCCESS) {
        ++bus->stats.errors;
        return FC_STATUS_PLATFORM_ERROR;
    }

    ++bus->stats.transfers;
    bus->stats.bytes += sizeof(uint32_t) * FC_CAN_RAW_FRAME_WORDS;
    return FC_STATUS_OK;
}
