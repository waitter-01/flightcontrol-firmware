#ifndef FC_BOARD_RESOURCES_H
#define FC_BOARD_RESOURCES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../core/fc_status.h"

/** 板级资源的访问风险等级。 */
typedef enum {
    FC_RESOURCE_READ_ONLY = 0,
    FC_RESOURCE_COMMUNICATION,
    FC_RESOURCE_DANGEROUS_OUTPUT,
    FC_RESOURCE_NONVOLATILE_WRITE
} fc_resource_risk_t;

/** 从当前 HDF/xparameters 固化得到的板级硬件资源。 */
typedef struct {
    const char *name;
    uint16_t device_id;
    uintptr_t base_address;
    uintptr_t high_address;
    int16_t interrupt_id;
    fc_resource_risk_t risk;
    bool mapping_confirmed;
} fc_board_resource_t;

/**
 * @brief 返回板级硬件资源数量。
 *
 * @return 当前资源表中的条目数。
 * @note 本函数只访问只读常量，可在任务上下文调用。
 */
size_t fc_board_resource_count(void);

/**
 * @brief 按索引读取板级硬件资源。
 *
 * @param index 资源索引，范围为 0 到 fc_board_resource_count()-1。
 * @param resource 接收资源描述的指针。
 * @return 成功返回 FC_STATUS_OK；参数或索引无效时返回错误码。
 * @note 本函数不会访问硬件寄存器，也不会改变任何输出。
 */
fc_status_t fc_board_resource_get(size_t index,
                                  const fc_board_resource_t **resource);

#endif
