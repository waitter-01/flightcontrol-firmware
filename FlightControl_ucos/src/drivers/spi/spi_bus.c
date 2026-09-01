#include "spi_bus.h"

fc_status_t fc_spi_transfer(const uint8_t *tx, uint8_t *rx, size_t length)
{
    if (((tx == NULL) && (length != 0u)) ||
        ((rx == NULL) && (length != 0u))) {
        return FC_STATUS_INVALID_ARGUMENT;
    }

    return FC_STATUS_NOT_SUPPORTED;
}
