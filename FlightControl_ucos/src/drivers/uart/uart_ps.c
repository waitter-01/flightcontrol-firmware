#include "uart_ps.h"

#include <string.h>

fc_status_t fc_uart_ps_init(fc_uart_ps_t *uart,
                            uint32_t device_id,
                            uint32_t baud_rate,
                            uint32_t data_bits,
                            uint32_t parity,
                            uint32_t stop_bits)
{
    if ((uart == NULL) || (baud_rate == 0u)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }

    memset(uart, 0, sizeof(*uart));
    uart->handle = UARTPS_Init(device_id);
    if (uart->handle == NULL) {
        return FC_STATUS_NOT_SUPPORTED;
    }

    if ((UARTPS_DataFormatSet(uart->handle, baud_rate, data_bits, parity,
                              stop_bits) != DEF_OK) ||
        (UARTPS_Enable(uart->handle) != DEF_OK)) {
        return FC_STATUS_PLATFORM_ERROR;
    }

    uart->initialized = true;
    return FC_STATUS_OK;
}

fc_status_t fc_uart_ps_write(fc_uart_ps_t *uart,
                             const uint8_t *data,
                             size_t length)
{
    size_t index;

    if ((uart == NULL) || ((data == NULL) && (length != 0u))) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    if (!uart->initialized) {
        return FC_STATUS_INVALID_STATE;
    }

    for (index = 0u; index < length; ++index) {
        if (UARTPS_WrByte(uart->handle, (CPU_CHAR)data[index]) != DEF_OK) {
            ++uart->stats.errors;
            return FC_STATUS_PLATFORM_ERROR;
        }
    }

    ++uart->stats.transfers;
    uart->stats.bytes += (uint32_t)length;
    return FC_STATUS_OK;
}
