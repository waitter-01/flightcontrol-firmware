#ifndef FC_SBUS_PARSER_H
#define FC_SBUS_PARSER_H

#include <stdbool.h>
#include <stdint.h>

#include "../core/fc_data_model.h"

#define FC_SBUS_FRAME_LENGTH 25u

/** 无动态内存的 SBUS 字节流解析器。 */
typedef struct {
    uint8_t frame[FC_SBUS_FRAME_LENGTH];
    uint8_t position;
    uint32_t sequence;
} fc_sbus_parser_t;

/**
 * @brief 复位 SBUS 解析器。
 * @param parser 解析器实例。
 */
void fc_sbus_parser_reset(fc_sbus_parser_t *parser);

/**
 * @brief 输入一个 SBUS 字节并在完整帧到达时输出遥控样本。
 * @param parser 解析器实例。
 * @param byte 串口收到的字节。
 * @param timestamp_us 该字节对应的单调时间戳。
 * @param sample 完整且校验通过时接收解码结果。
 * @return 产生新样本返回 true，否则返回 false。
 * @note 支持 16 个 11 位模拟通道和 2 个数字通道；失控标志直接进入样本。
 */
bool fc_sbus_parser_push(fc_sbus_parser_t *parser,
                         uint8_t byte,
                         uint64_t timestamp_us,
                         fc_rc_sample_t *sample);

#endif
