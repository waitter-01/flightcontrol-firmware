#include "platform_log.h"

#include <ucos_bsp.h>

static const char *fc_log_level_name(fc_log_level_t level)
{
    switch (level) {
    case FC_LOG_DEBUG:
        return "DEBUG";
    case FC_LOG_INFO:
        return "INFO";
    case FC_LOG_WARNING:
        return "WARN";
    case FC_LOG_ERROR:
        return "ERROR";
    case FC_LOG_CRITICAL:
        return "CRITICAL";
    default:
        return "UNKNOWN";
    }
}

static const char *fc_status_name(fc_status_t status)
{
    switch (status) {
    case FC_STATUS_OK:
        return "ok";
    case FC_STATUS_INVALID_ARGUMENT:
        return "invalid_argument";
    case FC_STATUS_INVALID_STATE:
        return "invalid_state";
    case FC_STATUS_TIMEOUT:
        return "timeout";
    case FC_STATUS_NOT_SUPPORTED:
        return "not_supported";
    case FC_STATUS_LOCKED:
        return "locked";
    case FC_STATUS_PLATFORM_ERROR:
        return "platform_error";
    case FC_STATUS_NO_RESOURCE:
        return "no_resource";
    default:
        return "unknown_status";
    }
}

void fc_log_write(fc_log_level_t level, const char *component, const char *message)
{
    if ((component == 0) || (message == 0)) {
        return;
    }

    UCOS_Print("[");
    UCOS_Print(fc_log_level_name(level));
    UCOS_Print("][");
    UCOS_Print(component);
    UCOS_Print("] ");
    UCOS_Print(message);
    UCOS_Print("\r\n");
}

void fc_log_status(fc_log_level_t level,
                   const char *component,
                   const char *operation,
                   fc_status_t status)
{
    if ((component == 0) || (operation == 0)) {
        return;
    }

    UCOS_Print("[");
    UCOS_Print(fc_log_level_name(level));
    UCOS_Print("][");
    UCOS_Print(component);
    UCOS_Print("] ");
    UCOS_Print(operation);
    UCOS_Print(": ");
    UCOS_Print(fc_status_name(status));
    UCOS_Print("\r\n");
}
