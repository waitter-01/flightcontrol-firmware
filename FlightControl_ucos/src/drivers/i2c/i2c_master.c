#include "i2c_master.h"

#include <limits.h>
#include <string.h>
#include <xstatus.h>

fc_status_t fc_i2c_master_init(fc_i2c_master_t *bus, uint16_t device_id)
{
    XIic_Config *config;

    if (bus == NULL) {
        return FC_STATUS_INVALID_ARGUMENT;
    }

    memset(bus, 0, sizeof(*bus));
    config = XIic_LookupConfig(device_id);
    if (config == NULL) {
        return FC_STATUS_NOT_SUPPORTED;
    }

    if (XIic_Initialize(&bus->instance, device_id) != XST_SUCCESS) {
        return FC_STATUS_PLATFORM_ERROR;
    }

    bus->base_address = config->BaseAddress;
    bus->initialized = true;
    return FC_STATUS_OK;
}

fc_status_t fc_i2c_master_write_read(fc_i2c_master_t *bus,
                                     uint8_t address,
                                     const uint8_t *prefix,
                                     size_t prefix_length,
                                     uint8_t *data,
                                     size_t data_length)
{
    unsigned sent;
    unsigned received;

    if ((bus == NULL) || (prefix == NULL) || (data == NULL) ||
        (prefix_length == 0u) || (data_length == 0u) ||
        (prefix_length > UINT8_MAX) || (data_length > UINT8_MAX) ||
        (address > 0x7fu)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }

    if (!bus->initialized) {
        return FC_STATUS_INVALID_STATE;
    }

    sent = XIic_Send(bus->base_address, address, (uint8_t *)prefix,
                     (uint8_t)prefix_length, XIIC_REPEATED_START);
    if (sent != prefix_length) {
        ++bus->stats.errors;
        return FC_STATUS_PLATFORM_ERROR;
    }

    received = XIic_Recv(bus->base_address, address, data,
                         (uint8_t)data_length, XIIC_STOP);
    if (received != data_length) {
        ++bus->stats.errors;
        return FC_STATUS_PLATFORM_ERROR;
    }

    ++bus->stats.transfers;
    bus->stats.bytes += (uint32_t)(prefix_length + data_length);
    return FC_STATUS_OK;
}

fc_status_t fc_i2c_master_stats(const fc_i2c_master_t *bus,
                                fc_driver_stats_t *stats)
{
    if ((bus == NULL) || (stats == NULL)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }

    *stats = bus->stats;
    return FC_STATUS_OK;
}
