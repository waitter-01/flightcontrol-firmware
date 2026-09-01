#ifndef FC_INPUT_STORE_H
#define FC_INPUT_STORE_H

#include "../core/fc_data_model.h"
#include "../core/fc_status.h"

/**
 * @brief 初始化固定内存输入快照仓库。
 * @return 首次初始化返回 FC_STATUS_OK，重复初始化返回状态错误。
 * @note 仓库不使用堆内存；发布者和读取者可以位于不同任务。
 */
fc_status_t fc_input_store_init(void);

/**
 * @brief 发布最新 IMU 样本。
 * @param sample 已填写单位、坐标系、时间戳和有效标志的样本。
 * @return 成功返回 FC_STATUS_OK。
 */
fc_status_t fc_input_publish_imu(const fc_imu_sample_t *sample);

/** @brief 发布最新 GNSS 样本；参数和返回值约束同 IMU 发布接口。 */
fc_status_t fc_input_publish_gnss(const fc_gnss_sample_t *sample);

/** @brief 发布最新遥控样本；参数和返回值约束同 IMU 发布接口。 */
fc_status_t fc_input_publish_rc(const fc_rc_sample_t *sample);

/**
 * @brief 获取三个输入主题的一致性快照。
 * @param snapshot 接收快照的固定内存对象。
 * @return 成功返回 FC_STATUS_OK；写入竞争持续发生时返回 FC_STATUS_BUSY。
 * @note 使用序列锁复制，不阻塞实时控制任务，最多重试三次。
 */
fc_status_t fc_input_snapshot(fc_input_snapshot_t *snapshot);

#endif
