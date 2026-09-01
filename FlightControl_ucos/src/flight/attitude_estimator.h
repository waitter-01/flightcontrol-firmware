#ifndef FC_ATTITUDE_ESTIMATOR_H
#define FC_ATTITUDE_ESTIMATOR_H

#include <stdbool.h>

#include "../core/fc_data_model.h"
#include "../core/fc_status.h"

/** 单位四元数，表示从 FRD 机体系到 NED 导航系的旋转。 */
typedef struct {
    float w;
    float x;
    float y;
    float z;
} fc_quaternion_t;

/** 基于陀螺积分和加速度重力校正的姿态估计器。 */
typedef struct {
    fc_quaternion_t attitude;
    float accel_correction_gain;
    bool initialized;
} fc_attitude_estimator_t;

/**
 * @brief 初始化姿态估计器为水平朝北姿态。
 * @param estimator 估计器实例。
 * @param accel_correction_gain 加速度重力校正比例增益，必须非负。
 * @return 参数有效返回 FC_STATUS_OK。
 */
fc_status_t fc_attitude_estimator_init(fc_attitude_estimator_t *estimator,
                                       float accel_correction_gain);

/**
 * @brief 使用一个 IMU 样本更新姿态四元数。
 * @param estimator 已初始化的估计器。
 * @param sample FRD、SI 单位且有效的 IMU 样本。
 * @param dt_s 更新周期，单位秒，允许范围 (0, 0.05]。
 * @return 更新成功返回 FC_STATUS_OK；无效或异常样本返回数据错误。
 * @note 磁航向未接入时偏航只由陀螺积分，长期会漂移。
 */
fc_status_t fc_attitude_estimator_update(fc_attitude_estimator_t *estimator,
                                         const fc_imu_sample_t *sample,
                                         float dt_s);

#endif
