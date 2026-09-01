#ifndef FC_DRIVER_STATS_H
#define FC_DRIVER_STATS_H

#include <stdint.h>

/** 驱动通用运行统计；计数回绕不影响故障趋势判断。 */
typedef struct {
    uint32_t transfers;
    uint32_t bytes;
    uint32_t timeouts;
    uint32_t errors;
    uint32_t overruns;
} fc_driver_stats_t;

#endif
