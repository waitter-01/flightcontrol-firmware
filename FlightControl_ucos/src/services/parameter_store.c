#include "parameter_store.h"

#include <string.h>

static fc_parameter_t g_parameters[FC_PARAMETER_MAX_COUNT];
static size_t g_parameter_count;
static bool g_parameter_initialized;

/** 检查值是否满足参数类型对应的范围。 */
static bool fc_parameter_value_valid(const fc_parameter_t *parameter,
                                     fc_parameter_value_t value)
{
    switch (parameter->type) {
        case FC_PARAMETER_FLOAT:
            return (value.real32 >= parameter->minimum.real32) &&
                   (value.real32 <= parameter->maximum.real32);
        case FC_PARAMETER_INT32:
            return (value.int32 >= parameter->minimum.int32) &&
                   (value.int32 <= parameter->maximum.int32);
        case FC_PARAMETER_BOOL:
            return true;
        default:
            return false;
    }
}

fc_status_t fc_parameter_store_init(void)
{
    memset(g_parameters, 0, sizeof(g_parameters));
    g_parameter_count = 0u;
    g_parameter_initialized = true;
    return FC_STATUS_OK;
}

fc_status_t fc_parameter_register(const fc_parameter_t *parameter)
{
    size_t index;

    if ((parameter == NULL) ||
        (memchr(parameter->name, '\0', FC_PARAMETER_NAME_LENGTH) == NULL) ||
        (parameter->name[0] == '\0') ||
        !fc_parameter_value_valid(parameter, parameter->value)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    if (!g_parameter_initialized) {
        return FC_STATUS_INVALID_STATE;
    }
    for (index = 0u; index < g_parameter_count; ++index) {
        if (strcmp(g_parameters[index].name, parameter->name) == 0) {
            return FC_STATUS_INVALID_STATE;
        }
    }
    if (g_parameter_count >= FC_PARAMETER_MAX_COUNT) {
        return FC_STATUS_NO_RESOURCE;
    }
    g_parameters[g_parameter_count++] = *parameter;
    return FC_STATUS_OK;
}

fc_status_t fc_parameter_get(size_t index, fc_parameter_t *parameter)
{
    if ((parameter == NULL) || (index >= g_parameter_count)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    *parameter = g_parameters[index];
    return FC_STATUS_OK;
}

fc_status_t fc_parameter_set(const char *name, fc_parameter_value_t value)
{
    size_t index;

    if (name == NULL) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0u; index < g_parameter_count; ++index) {
        if (strcmp(g_parameters[index].name, name) == 0) {
            if (g_parameters[index].readonly) {
                return FC_STATUS_LOCKED;
            }
            if (!fc_parameter_value_valid(&g_parameters[index], value)) {
                return FC_STATUS_INVALID_ARGUMENT;
            }
            g_parameters[index].value = value;
            return FC_STATUS_OK;
        }
    }
    return FC_STATUS_NOT_SUPPORTED;
}

size_t fc_parameter_count(void)
{
    return g_parameter_count;
}

fc_status_t fc_parameter_persist(void)
{
    return FC_STATUS_LOCKED;
}
