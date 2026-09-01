#ifndef FC_PID_H
#define FC_PID_H

#include "../core/fc_status.h"

/** 带输出限幅和反算抗饱和的 PID 控制器。 */
typedef struct {
    float kp;
    float ki;
    float kd;
    float anti_windup_gain;
    float output_min;
    float output_max;
    float integral;
    float previous_measurement;
    int initialized;
} fc_pid_t;

/**
 * @brief 配置并复位 PID 控制器。
 * @return 参数范围正确返回 FC_STATUS_OK。
 */
fc_status_t fc_pid_init(fc_pid_t *pid,
                        float kp,
                        float ki,
                        float kd,
                        float anti_windup_gain,
                        float output_min,
                        float output_max);

/**
 * @brief 按测量值微分形式执行一次 PID 更新。
 * @param pid 已初始化控制器。
 * @param setpoint 目标值。
 * @param measurement 测量值。
 * @param dt_s 周期秒数，允许范围 (0, 0.1]。
 * @param output 返回限幅后的控制量。
 * @return 成功返回 FC_STATUS_OK。
 */
fc_status_t fc_pid_update(fc_pid_t *pid,
                          float setpoint,
                          float measurement,
                          float dt_s,
                          float *output);

/** @brief 清除积分和历史测量值，用于模式切换或解除控制。 */
void fc_pid_reset(fc_pid_t *pid);

#endif
