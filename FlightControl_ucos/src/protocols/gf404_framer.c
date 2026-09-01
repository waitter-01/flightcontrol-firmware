#include "gf404_framer.h"

#include <string.h>

void fc_gf404_framer_reset(fc_gf404_framer_t *framer)
{
    if (framer != NULL) {
        memset(framer, 0, sizeof(*framer));
    }
}

bool fc_gf404_framer_push(fc_gf404_framer_t *framer,
                          uint8_t byte,
                          uint64_t timestamp_us,
                          fc_gf404_frame_t *frame)
{
    size_t index;
    uint8_t checksum = 0u;

    if ((framer == NULL) || (frame == NULL)) {
        return false;
    }
    if ((framer->position == 0u) && (byte != 0x55u)) {
        return false;
    }
    if ((framer->position == 1u) && (byte != 0xaau)) {
        framer->position = (byte == 0x55u) ? 1u : 0u;
        return false;
    }

    framer->buffer[framer->position++] = byte;
    if (framer->position == 3u) {
        framer->expected_length = (size_t)byte + 3u;
        if ((framer->expected_length < 5u) ||
            (framer->expected_length > sizeof(framer->buffer))) {
            framer->position = 0u;
            return false;
        }
    }
    if ((framer->expected_length == 0u) ||
        (framer->position < framer->expected_length)) {
        return false;
    }

    for (index = 0u; index + 1u < framer->expected_length; ++index) {
        checksum ^= framer->buffer[index];
    }
    if (checksum != framer->buffer[framer->expected_length - 1u]) {
        framer->position = 0u;
        framer->expected_length = 0u;
        return false;
    }

    frame->message_id = framer->buffer[3];
    frame->payload_length = framer->expected_length - 5u;
    memcpy(frame->payload, &framer->buffer[4], frame->payload_length);
    frame->timestamp_us = timestamp_us;
    framer->position = 0u;
    framer->expected_length = 0u;
    return true;
}
