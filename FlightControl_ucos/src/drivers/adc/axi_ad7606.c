#include "axi_ad7606.h"

#include <stddef.h>
#include <string.h>
#include <xil_io.h>

fc_status_t fc_axi_ad7606_init(fc_axi_ad7606_t *adc, uintptr_t base_address)
{
    if ((adc == NULL) || ((base_address & 0x3u) != 0u)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }

    memset(adc, 0, sizeof(*adc));
    adc->base_address = base_address;
    adc->initialized = true;
    return FC_STATUS_OK;
}

fc_status_t fc_axi_ad7606_read(fc_axi_ad7606_t *adc,
                               uint32_t samples[FC_AD7606_CHANNEL_COUNT])
{
    uint32_t index;

    if ((adc == NULL) || (samples == NULL)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    if (!adc->initialized) {
        return FC_STATUS_INVALID_STATE;
    }

    for (index = 0u; index < FC_AD7606_CHANNEL_COUNT; ++index) {
        samples[index] = Xil_In32(adc->base_address + (index * 4u));
    }

    ++adc->stats.transfers;
    adc->stats.bytes += FC_AD7606_CHANNEL_COUNT * sizeof(samples[0]);
    return FC_STATUS_OK;
}
