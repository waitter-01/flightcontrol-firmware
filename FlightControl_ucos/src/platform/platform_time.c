#include "platform_time.h"

#include <limits.h>

#include <xtime_l.h>

static XTime g_start_count;
static int g_time_initialized;

fc_status_t fc_time_init(void)
{
    XTime_GetTime(&g_start_count);
    g_time_initialized = 1;
    return FC_STATUS_OK;
}

fc_time_us_t fc_time_now_us(void)
{
    XTime current_count;
    XTime elapsed_count;
    XTime whole_seconds;
    XTime remaining_counts;

    if (!g_time_initialized) {
        return 0u;
    }

    XTime_GetTime(&current_count);
    elapsed_count = current_count - g_start_count;
    whole_seconds = elapsed_count / (XTime)COUNTS_PER_SECOND;
    remaining_counts = elapsed_count % (XTime)COUNTS_PER_SECOND;

    return (fc_time_us_t)(whole_seconds * 1000000u) +
           (fc_time_us_t)((remaining_counts * 1000000u) /
                          (XTime)COUNTS_PER_SECOND);
}

uint32_t fc_time_now_ms(void)
{
    return (uint32_t)(fc_time_now_us() / 1000u);
}

uint32_t fc_time_elapsed_ms(uint32_t start_ms, uint32_t now_ms)
{
    return now_ms - start_ms;
}
