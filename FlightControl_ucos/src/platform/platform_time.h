#ifndef FC_PLATFORM_TIME_H
#define FC_PLATFORM_TIME_H

#include <stdint.h>

#include "../core/fc_status.h"

typedef uint64_t fc_time_us_t;

fc_status_t fc_time_init(void);
fc_time_us_t fc_time_now_us(void);
uint32_t fc_time_now_ms(void);
uint32_t fc_time_elapsed_ms(uint32_t start_ms, uint32_t now_ms);

#endif
