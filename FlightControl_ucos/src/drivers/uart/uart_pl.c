#include "uart_pl.h"

#include <string.h>
#include <xil_io.h>

#define FC_PL_UART_CFG_OFFSET       0x00u
#define FC_PL_UART_TX_COUNT_OFFSET  0x04u
#define FC_PL_UART_RX_COUNT_OFFSET  0x08u
#define FC_PL_UART_TX_DATA_OFFSET   0x0cu
#define FC_PL_UART_RX_DATA_OFFSET   0x10u
#define FC_PL_UART_FIFO_CAPACITY    4096u
#define FC_PL_UART_COUNT_MASK       0x0fffu
#define FC_PL_UART_ENABLE_MASK      0x80000000u
#define FC_PL_UART_PARITY_MASK      0x40000000u
#define FC_PL_UART_ODD_MASK         0x20000000u

fc_status_t fc_uart_pl_init(fc_uart_pl_t *uart,
                            uintptr_t channel_base,
                            uint32_t clock_hz,
                            uint32_t baud_rate,
                            bool parity_enable,
                            bool odd_parity)
{
    uint32_t divisor;
    uint32_t config;

    if ((uart == NULL) || ((channel_base & 0x3u) != 0u) ||
        (clock_hz == 0u) || (baud_rate == 0u) || (baud_rate > clock_hz)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }

    divisor = (clock_hz / baud_rate) - 1u;
    config = FC_PL_UART_ENABLE_MASK | divisor;
    if (parity_enable) {
        config |= FC_PL_UART_PARITY_MASK;
        if (odd_parity) {
            config |= FC_PL_UART_ODD_MASK;
        }
    }

    memset(uart, 0, sizeof(*uart));
    uart->channel_base = channel_base;
    Xil_Out32(channel_base + FC_PL_UART_CFG_OFFSET, config);
    uart->initialized = true;
    return FC_STATUS_OK;
}

fc_status_t fc_uart_pl_read(fc_uart_pl_t *uart,
                            uint8_t *data,
                            size_t capacity,
                            size_t *received)
{
    size_t available;
    size_t index;

    if ((uart == NULL) || (received == NULL) ||
        ((data == NULL) && (capacity != 0u))) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    if (!uart->initialized) {
        return FC_STATUS_INVALID_STATE;
    }

    available = Xil_In32(uart->channel_base + FC_PL_UART_RX_COUNT_OFFSET) &
                FC_PL_UART_COUNT_MASK;
    if (available > capacity) {
        available = capacity;
    }
    for (index = 0u; index < available; ++index) {
        data[index] = (uint8_t)Xil_In32(uart->channel_base +
                                        FC_PL_UART_RX_DATA_OFFSET);
    }

    *received = available;
    ++uart->stats.transfers;
    uart->stats.bytes += (uint32_t)available;
    return FC_STATUS_OK;
}

fc_status_t fc_uart_pl_write(fc_uart_pl_t *uart,
                             const uint8_t *data,
                             size_t length,
                             size_t *sent)
{
    size_t used;
    size_t available;
    size_t index;

    if ((uart == NULL) || (sent == NULL) ||
        ((data == NULL) && (length != 0u))) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    if (!uart->initialized) {
        return FC_STATUS_INVALID_STATE;
    }

    used = Xil_In32(uart->channel_base + FC_PL_UART_TX_COUNT_OFFSET) &
           FC_PL_UART_COUNT_MASK;
    available = (used < FC_PL_UART_FIFO_CAPACITY)
                    ? (FC_PL_UART_FIFO_CAPACITY - used)
                    : 0u;
    if (available > length) {
        available = length;
    }
    for (index = 0u; index < available; ++index) {
        Xil_Out32(uart->channel_base + FC_PL_UART_TX_DATA_OFFSET, data[index]);
    }

    *sent = available;
    ++uart->stats.transfers;
    uart->stats.bytes += (uint32_t)available;
    return FC_STATUS_OK;
}
