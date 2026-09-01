#ifndef FC_HEX_MIXER_H
#define FC_HEX_MIXER_H

#include "../core/fc_status.h"

#define FC_HEX_MOTOR_COUNT 6u

/** 六旋翼归一化控制需求。 */
typedef struct {
    float thrust;
    float roll;
    float pitch;
    float yaw;
} fc_control_demand_t;

/**
 * @brief 按 X/Y 平面等间隔六电机布局执行控制分配。
 * @param demand 推力 [0,1] 以及滚转、俯仰、偏航归一化需求。
 * @param motor 输出六个 [0,1] 电机命令；顺序为机头开始顺时针每 60 度。
 * @return 参数有效返回 FC_STATUS_OK。
 * @note 偏航方向按相邻电机交替设置，实机必须核对旋向和电机编号。
 */
fc_status_t fc_hex_mixer_allocate(const fc_control_demand_t *demand,
                                  float motor[FC_HEX_MOTOR_COUNT]);

#endif
