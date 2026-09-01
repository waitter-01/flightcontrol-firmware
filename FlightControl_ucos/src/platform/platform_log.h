#ifndef FC_PLATFORM_LOG_H
#define FC_PLATFORM_LOG_H

#include "../core/fc_status.h"

typedef enum {
    FC_LOG_DEBUG = 0,
    FC_LOG_INFO,
    FC_LOG_WARNING,
    FC_LOG_ERROR,
    FC_LOG_CRITICAL
} fc_log_level_t;

void fc_log_write(fc_log_level_t level, const char *component, const char *message);
void fc_log_status(fc_log_level_t level,
                   const char *component,
                   const char *operation,
                   fc_status_t status);

#endif
