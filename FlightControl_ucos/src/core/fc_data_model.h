#ifndef FC_DATA_MODEL_H
#define FC_DATA_MODEL_H

#include <stdbool.h>
#include <stdint.h>

/** 三维浮点向量；具体字段注释决定单位和坐标系。 */
typedef struct {
    float x;
    float y;
    float z;
} fc_vec3f_t;

/** 数据有效性标志。 */
typedef enum {
    FC_DATA_VALID = (1u << 0),
    FC_DATA_CALIBRATED = (1u << 1),
    FC_DATA_FAILSAFE = (1u << 2),
    FC_DATA_STALE = (1u << 3)
} fc_data_flag_t;

/** 所有异步输入共有的时间和质量信息。 */
typedef struct {
    uint64_t timestamp_us;
    uint32_t sequence;
    uint32_t flags;
} fc_sample_header_t;

/** 惯性测量样本，机体系采用前-右-下（FRD）。 */
typedef struct {
    fc_sample_header_t header;
    fc_vec3f_t angular_rate_rad_s;
    fc_vec3f_t acceleration_m_s2;
    float temperature_c;
} fc_imu_sample_t;

/** GNSS 样本，位置为 WGS84，速度为北-东-地（NED）。 */
typedef struct {
    fc_sample_header_t header;
    int32_t latitude_deg_e7;
    int32_t longitude_deg_e7;
    int32_t altitude_mm;
    fc_vec3f_t velocity_ned_m_s;
    uint8_t fix_type;
    uint8_t satellites;
} fc_gnss_sample_t;

#define FC_RC_CHANNEL_COUNT 18u

/** 遥控输入样本，通道统一归一化到 -1.0 至 1.0。 */
typedef struct {
    fc_sample_header_t header;
    float channel[FC_RC_CHANNEL_COUNT];
    bool frame_lost;
    bool failsafe;
} fc_rc_sample_t;

/** 飞控算法使用的统一输入快照。 */
typedef struct {
    fc_imu_sample_t imu;
    fc_gnss_sample_t gnss;
    fc_rc_sample_t rc;
} fc_input_snapshot_t;

#endif
