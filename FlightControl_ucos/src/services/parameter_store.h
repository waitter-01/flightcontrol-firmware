#ifndef FC_PARAMETER_STORE_H
#define FC_PARAMETER_STORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../core/fc_status.h"

#define FC_PARAMETER_NAME_LENGTH 16u
#define FC_PARAMETER_MAX_COUNT   64u

/** 参数类型。 */
typedef enum {
    FC_PARAMETER_FLOAT = 0,
    FC_PARAMETER_INT32,
    FC_PARAMETER_BOOL
} fc_parameter_type_t;

/** 参数值。 */
typedef union {
    float real32;
    int32_t int32;
    bool boolean;
} fc_parameter_value_t;

/** 参数声明和当前值。 */
typedef struct {
    char name[FC_PARAMETER_NAME_LENGTH];
    fc_parameter_type_t type;
    fc_parameter_value_t value;
    fc_parameter_value_t minimum;
    fc_parameter_value_t maximum;
    bool readonly;
} fc_parameter_t;

/** @brief 初始化固定容量参数仓库并清空已有条目。 */
fc_status_t fc_parameter_store_init(void);

/**
 * @brief 注册一个具有类型和范围约束的参数。
 * @param parameter 参数声明，名称必须以 NUL 结尾且不可重复。
 * @return 注册成功返回 FC_STATUS_OK，容量不足返回 FC_STATUS_NO_RESOURCE。
 */
fc_status_t fc_parameter_register(const fc_parameter_t *parameter);

/**
 * @brief 按索引读取参数快照。
 * @param index 参数索引。
 * @param parameter 接收参数快照。
 * @return 成功返回 FC_STATUS_OK。
 */
fc_status_t fc_parameter_get(size_t index, fc_parameter_t *parameter);

/**
 * @brief 修改参数并执行类型、范围和只读检查。
 * @param name 参数名称。
 * @param value 新值。
 * @return 修改成功返回 FC_STATUS_OK，越界返回 FC_STATUS_INVALID_ARGUMENT。
 * @note 参数只更新 RAM；持久化接口在安全审核前保持禁用。
 */
fc_status_t fc_parameter_set(const char *name, fc_parameter_value_t value);

/** @brief 返回当前已注册参数数量。 */
size_t fc_parameter_count(void);

/**
 * @brief 请求把参数保存到非易失存储。
 * @return 当前版本固定返回 FC_STATUS_LOCKED。
 * @warning 本接口不得绕过板级 Flash 写安全域。
 */
fc_status_t fc_parameter_persist(void);

#endif
