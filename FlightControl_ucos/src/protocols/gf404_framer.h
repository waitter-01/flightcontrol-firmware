#ifndef FC_GF404_FRAMER_H
#define FC_GF404_FRAMER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define FC_GF404_MAX_PAYLOAD 156u

/** GF404 帧的协议层结果；载荷解码与字节流分帧相互独立。 */
typedef struct {
    uint8_t message_id;
    uint8_t payload[FC_GF404_MAX_PAYLOAD];
    size_t payload_length;
    uint64_t timestamp_us;
} fc_gf404_frame_t;

/** GF404 有界状态机。 */
typedef struct {
    uint8_t buffer[FC_GF404_MAX_PAYLOAD + 5u];
    size_t position;
    size_t expected_length;
} fc_gf404_framer_t;

/** @brief 清空 GF404 分帧状态；可在串口错误或链路切换后调用。 */
void fc_gf404_framer_reset(fc_gf404_framer_t *framer);

/**
 * @brief 输入一个字节并解析 `55 AA 长度 ID 数据 XOR` 帧。
 * @param framer 分帧器实例。
 * @param byte 输入字节。
 * @param timestamp_us 单调时间戳。
 * @param frame 完整帧输出。
 * @return 完整且异或校验通过返回 true。
 * @note 长度超过固定上限立即丢弃，不会发生缓冲区越界。
 */
bool fc_gf404_framer_push(fc_gf404_framer_t *framer,
                          uint8_t byte,
                          uint64_t timestamp_us,
                          fc_gf404_frame_t *frame);

#endif
