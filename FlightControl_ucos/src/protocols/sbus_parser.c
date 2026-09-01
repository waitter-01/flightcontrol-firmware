#include "sbus_parser.h"

#include <stddef.h>
#include <string.h>

#define FC_SBUS_START_BYTE 0x0fu
#define FC_SBUS_END_BYTE   0x00u
#define FC_SBUS_MIN_VALUE  172u
#define FC_SBUS_MAX_VALUE  1811u

/** 将 SBUS 原始通道限制并归一化到 -1 至 1。 */
static float fc_sbus_normalize(uint16_t raw)
{
    if (raw < FC_SBUS_MIN_VALUE) {
        raw = FC_SBUS_MIN_VALUE;
    } else if (raw > FC_SBUS_MAX_VALUE) {
        raw = FC_SBUS_MAX_VALUE;
    }
    return ((float)(raw - FC_SBUS_MIN_VALUE) * 2.0f /
            (float)(FC_SBUS_MAX_VALUE - FC_SBUS_MIN_VALUE)) - 1.0f;
}

void fc_sbus_parser_reset(fc_sbus_parser_t *parser)
{
    if (parser != NULL) {
        memset(parser, 0, sizeof(*parser));
    }
}

bool fc_sbus_parser_push(fc_sbus_parser_t *parser,
                         uint8_t byte,
                         uint64_t timestamp_us,
                         fc_rc_sample_t *sample)
{
    uint32_t bit_index;
    uint32_t channel;
    uint16_t raw;

    if ((parser == NULL) || (sample == NULL)) {
        return false;
    }
    if ((parser->position == 0u) && (byte != FC_SBUS_START_BYTE)) {
        return false;
    }

    parser->frame[parser->position++] = byte;
    if (parser->position < FC_SBUS_FRAME_LENGTH) {
        return false;
    }
    parser->position = 0u;
    if ((parser->frame[0] != FC_SBUS_START_BYTE) ||
        (parser->frame[24] != FC_SBUS_END_BYTE)) {
        return false;
    }

    memset(sample, 0, sizeof(*sample));
    for (channel = 0u; channel < 16u; ++channel) {
        bit_index = channel * 11u;
        raw = (uint16_t)((parser->frame[1u + (bit_index / 8u)] |
                         ((uint32_t)parser->frame[2u + (bit_index / 8u)] << 8u) |
                         ((uint32_t)parser->frame[3u + (bit_index / 8u)] << 16u)) >>
                        (bit_index % 8u)) & 0x07ffu;
        sample->channel[channel] = fc_sbus_normalize(raw);
    }
    sample->channel[16] = ((parser->frame[23] & (1u << 0)) != 0u) ? 1.0f : -1.0f;
    sample->channel[17] = ((parser->frame[23] & (1u << 1)) != 0u) ? 1.0f : -1.0f;
    sample->frame_lost = (parser->frame[23] & (1u << 2)) != 0u;
    sample->failsafe = (parser->frame[23] & (1u << 3)) != 0u;
    sample->header.timestamp_us = timestamp_us;
    sample->header.sequence = ++parser->sequence;
    sample->header.flags = sample->failsafe ? FC_DATA_FAILSAFE : FC_DATA_VALID;
    return true;
}
