#include "pid.h"

#include <stddef.h>

/** 将值限制在闭区间。 */
static float fc_pid_clamp(float value, float minimum, float maximum)
{
    if (value < minimum) {
        return minimum;
    }
    if (value > maximum) {
        return maximum;
    }
    return value;
}

fc_status_t fc_pid_init(fc_pid_t *pid,
                        float kp,
                        float ki,
                        float kd,
                        float anti_windup_gain,
                        float output_min,
                        float output_max)
{
    if ((pid == NULL) || (ki < 0.0f) || (kd < 0.0f) ||
        (anti_windup_gain < 0.0f) || (output_min >= output_max)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->anti_windup_gain = anti_windup_gain;
    pid->output_min = output_min;
    pid->output_max = output_max;
    fc_pid_reset(pid);
    pid->initialized = 1;
    return FC_STATUS_OK;
}

fc_status_t fc_pid_update(fc_pid_t *pid,
                          float setpoint,
                          float measurement,
                          float dt_s,
                          float *output)
{
    float error;
    float derivative;
    float unrestricted;
    float limited;

    if ((pid == NULL) || (output == NULL) || (dt_s <= 0.0f) || (dt_s > 0.1f)) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    if (pid->initialized == 0) {
        return FC_STATUS_INVALID_STATE;
    }

    error = setpoint - measurement;
    derivative = -(measurement - pid->previous_measurement) / dt_s;
    unrestricted = pid->kp * error + pid->integral + pid->kd * derivative;
    limited = fc_pid_clamp(unrestricted, pid->output_min, pid->output_max);
    pid->integral += (pid->ki * error +
                      pid->anti_windup_gain * (limited - unrestricted)) * dt_s;
    pid->previous_measurement = measurement;
    *output = limited;
    return FC_STATUS_OK;
}

void fc_pid_reset(fc_pid_t *pid)
{
    if (pid != NULL) {
        pid->integral = 0.0f;
        pid->previous_measurement = 0.0f;
        pid->initialized = 0;
    }
}
