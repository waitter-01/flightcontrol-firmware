#include "dangerous_operations.h"

#include <stdbool.h>

fc_status_t fc_power_channel_set(uint8_t channel, bool enabled)
{
    (void)channel;
    (void)enabled;
    return FC_STATUS_LOCKED;
}

fc_status_t fc_relay_set(uint8_t channel, bool enabled)
{
    (void)channel;
    (void)enabled;
    return FC_STATUS_LOCKED;
}

fc_status_t fc_flash_write(uint32_t address,
                           const uint8_t *data,
                           size_t length)
{
    (void)address;
    (void)data;
    (void)length;
    return FC_STATUS_LOCKED;
}

fc_status_t fc_flash_erase(uint32_t address, size_t length)
{
    (void)address;
    (void)length;
    return FC_STATUS_LOCKED;
}
