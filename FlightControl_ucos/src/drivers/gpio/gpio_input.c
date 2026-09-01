#include "gpio_input.h"

#include <xstatus.h>

fc_status_t fc_gpio_input_init(fc_gpio_input_t *gpio,
                               uint16_t device_id,
                               uint8_t channel)
{
    int result;

    if ((gpio == NULL) || ((channel != 1u) && (channel != 2u))) {
        return FC_STATUS_INVALID_ARGUMENT;
    }

    gpio->initialized = false;
    result = XGpio_Initialize(&gpio->instance, device_id);
    if (result != XST_SUCCESS) {
        return FC_STATUS_PLATFORM_ERROR;
    }

    XGpio_SetDataDirection(&gpio->instance, channel, UINT32_MAX);
    gpio->device_id = device_id;
    gpio->channel = channel;
    gpio->initialized = true;
    return FC_STATUS_OK;
}

fc_status_t fc_gpio_input_read(const fc_gpio_input_t *gpio, uint32_t *value)
{
    if ((gpio == NULL) || (value == NULL)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }

    if (!gpio->initialized) {
        return FC_STATUS_INVALID_STATE;
    }

    *value = XGpio_DiscreteRead((XGpio *)&gpio->instance, gpio->channel);
    return FC_STATUS_OK;
}
