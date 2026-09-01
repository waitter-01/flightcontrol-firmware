#ifndef FC_GPIO_INPUT_H
#define FC_GPIO_INPUT_H

#include <stdbool.h>
#include <stdint.h>

#include <xgpio.h>

#include "../../core/fc_status.h"

/** 只读 AXI GPIO 输入实例。 */
typedef struct {
    XGpio instance;
    uint16_t device_id;
    uint8_t channel;
    bool initialized;
} fc_gpio_input_t;

/**
 * @brief 初始化一个只读 AXI GPIO 通道。
 *
 * @param gpio 调用者持有的 GPIO 实例。
 * @param device_id xparameters 中的 AXI GPIO 设备编号。
 * @param channel AXI GPIO 通道，允许值为 1 或 2。
 * @return 初始化成功返回 FC_STATUS_OK。
 * @note 初始化会把整个通道方向设置为输入，不会向外部引脚写数据。
 */
fc_status_t fc_gpio_input_init(fc_gpio_input_t *gpio,
                               uint16_t device_id,
                               uint8_t channel);

/**
 * @brief 读取只读 AXI GPIO 通道的当前值。
 *
 * @param gpio 已初始化的 GPIO 实例。
 * @param value 接收 32 位输入值的指针。
 * @return 成功返回 FC_STATUS_OK；未初始化或参数错误时返回相应错误码。
 * @note 本函数不修改方向寄存器和输出寄存器，可由普通任务调用。
 */
fc_status_t fc_gpio_input_read(const fc_gpio_input_t *gpio, uint32_t *value);

#endif
