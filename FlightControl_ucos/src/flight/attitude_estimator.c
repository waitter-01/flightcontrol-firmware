#include "attitude_estimator.h"

#include <stddef.h>
#include <stdint.h>

/** 使用两次牛顿迭代计算正数的平方根倒数，避免平台数学库依赖。 */
static float fc_inverse_sqrt(float value)
{
    union {
        float real;
        uint32_t bits;
    } conversion;
    float half;

    if (value <= 0.0f) {
        return 0.0f;
    }
    half = 0.5f * value;
    conversion.real = value;
    conversion.bits = 0x5f3759dfu - (conversion.bits >> 1u);
    conversion.real *= 1.5f - (half * conversion.real * conversion.real);
    conversion.real *= 1.5f - (half * conversion.real * conversion.real);
    return conversion.real;
}

/** 将姿态四元数归一化并拒绝数值退化。 */
static fc_status_t fc_quaternion_normalize(fc_quaternion_t *quaternion)
{
    float inverse_norm = fc_inverse_sqrt(quaternion->w * quaternion->w +
                                         quaternion->x * quaternion->x +
                                         quaternion->y * quaternion->y +
                                         quaternion->z * quaternion->z);
    if (inverse_norm == 0.0f) {
        return FC_STATUS_DATA_INVALID;
    }
    quaternion->w *= inverse_norm;
    quaternion->x *= inverse_norm;
    quaternion->y *= inverse_norm;
    quaternion->z *= inverse_norm;
    return FC_STATUS_OK;
}

fc_status_t fc_attitude_estimator_init(fc_attitude_estimator_t *estimator,
                                       float accel_correction_gain)
{
    if ((estimator == NULL) || (accel_correction_gain < 0.0f)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    estimator->attitude.w = 1.0f;
    estimator->attitude.x = 0.0f;
    estimator->attitude.y = 0.0f;
    estimator->attitude.z = 0.0f;
    estimator->accel_correction_gain = accel_correction_gain;
    estimator->initialized = true;
    return FC_STATUS_OK;
}

fc_status_t fc_attitude_estimator_update(fc_attitude_estimator_t *estimator,
                                         const fc_imu_sample_t *sample,
                                         float dt_s)
{
    fc_quaternion_t *q;
    float inverse_accel_norm;
    float ax;
    float ay;
    float az;
    float gravity_x;
    float gravity_y;
    float gravity_z;
    float error_x;
    float error_y;
    float error_z;
    float gx;
    float gy;
    float gz;
    float half_dt;
    float dw;
    float dx;
    float dy;
    float dz;

    if ((estimator == NULL) || (sample == NULL) ||
        (dt_s <= 0.0f) || (dt_s > 0.05f)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    if (!estimator->initialized) {
        return FC_STATUS_INVALID_STATE;
    }
    if ((sample->header.flags & FC_DATA_VALID) == 0u) {
        return FC_STATUS_DATA_INVALID;
    }

    q = &estimator->attitude;
    ax = sample->acceleration_m_s2.x;
    ay = sample->acceleration_m_s2.y;
    az = sample->acceleration_m_s2.z;
    inverse_accel_norm = fc_inverse_sqrt(ax * ax + ay * ay + az * az);
    gx = sample->angular_rate_rad_s.x;
    gy = sample->angular_rate_rad_s.y;
    gz = sample->angular_rate_rad_s.z;

    if (inverse_accel_norm > 0.0f) {
        ax *= inverse_accel_norm;
        ay *= inverse_accel_norm;
        az *= inverse_accel_norm;
        gravity_x = 2.0f * (q->x * q->z - q->w * q->y);
        gravity_y = 2.0f * (q->w * q->x + q->y * q->z);
        gravity_z = q->w * q->w - q->x * q->x - q->y * q->y + q->z * q->z;
        error_x = ay * gravity_z - az * gravity_y;
        error_y = az * gravity_x - ax * gravity_z;
        error_z = ax * gravity_y - ay * gravity_x;
        gx += estimator->accel_correction_gain * error_x;
        gy += estimator->accel_correction_gain * error_y;
        gz += estimator->accel_correction_gain * error_z;
    }

    half_dt = 0.5f * dt_s;
    dw = (-q->x * gx - q->y * gy - q->z * gz) * half_dt;
    dx = ( q->w * gx + q->y * gz - q->z * gy) * half_dt;
    dy = ( q->w * gy - q->x * gz + q->z * gx) * half_dt;
    dz = ( q->w * gz + q->x * gy - q->y * gx) * half_dt;
    q->w += dw;
    q->x += dx;
    q->y += dy;
    q->z += dz;
    return fc_quaternion_normalize(q);
}
