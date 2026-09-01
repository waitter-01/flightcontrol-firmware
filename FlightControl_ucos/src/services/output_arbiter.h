#ifndef FC_OUTPUT_ARBITER_H
#define FC_OUTPUT_ARBITER_H

#include <stdbool.h>
#include <stdint.h>

#include "../core/fc_status.h"
#include "../flight/hex_mixer.h"

#define FC_SERVO_OUTPUT_COUNT 8u

/** 所有执行机构的候选命令，尚未获得硬件输出许可。 */
typedef struct {
    float motor[FC_HEX_MOTOR_COUNT];
    float servo[FC_SERVO_OUTPUT_COUNT];
    uint64_t timestamp_us;
    uint32_t sequence;
} fc_actuator_command_t;

/** 执行机构仲裁状态快照。 */
typedef struct {
    fc_actuator_command_t requested;
    fc_actuator_command_t applied;
    uint32_t rejected_commands;
    bool backend_attached;
} fc_output_snapshot_t;

/** 硬件执行机构后端；实现必须同时服从板级安全门。 */
typedef fc_status_t (*fc_actuator_backend_write_t)(
    const fc_actuator_command_t *command,
    void *context);

/**
 * @brief 初始化输出仲裁器并把请求值和实际值全部清零。
 * @return 成功返回 FC_STATUS_OK。
 * @note 初始化不绑定硬件，也不会改变 GPIO、PWM 或 CAN 输出。
 */
fc_status_t fc_output_arbiter_init(void);

/**
 * @brief 绑定经过硬件专项审核的执行机构后端。
 * @param writer 固定内存回调，不得为 NULL。
 * @param context 后端上下文。
 * @return 当前构建未批准危险输出，固定返回 FC_STATUS_LOCKED。
 * @warning 只有完成断负载测试和通道映射后才能在独立变更中开放。
 */
fc_status_t fc_output_attach_backend(fc_actuator_backend_write_t writer,
                                     void *context);

/**
 * @brief 提交执行机构候选命令并执行安全仲裁。
 * @param command 六电机和舵机归一化候选值。
 * @return 未获得动力/舵机许可时返回 FC_STATUS_LOCKED，硬件不会收到命令。
 * @note 越界、过期或非有限值应由上层在提交前拒绝；本层再次检查 [0,1] 范围。
 */
fc_status_t fc_output_submit(const fc_actuator_command_t *command);

/**
 * @brief 获取最近请求、实际应用值和拒绝计数。
 * @param snapshot 接收状态快照。
 * @return 参数有效返回 FC_STATUS_OK。
 */
fc_status_t fc_output_snapshot(fc_output_snapshot_t *snapshot);

#endif
