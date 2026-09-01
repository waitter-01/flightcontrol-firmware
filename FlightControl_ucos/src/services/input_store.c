#include "input_store.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#define FC_INPUT_SNAPSHOT_RETRIES 3u

static fc_input_snapshot_t g_snapshot;
static volatile uint32_t g_store_sequence;
static bool g_store_initialized;

/** 建立写屏障并结束一次序列锁写入。 */
static void fc_input_write_end(void)
{
    __sync_synchronize();
    ++g_store_sequence;
}

fc_status_t fc_input_store_init(void)
{
    if (g_store_initialized) {
        return FC_STATUS_INVALID_STATE;
    }
    memset(&g_snapshot, 0, sizeof(g_snapshot));
    g_store_sequence = 0u;
    g_store_initialized = true;
    return FC_STATUS_OK;
}

fc_status_t fc_input_publish_imu(const fc_imu_sample_t *sample)
{
    if (sample == NULL) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    if (!g_store_initialized) {
        return FC_STATUS_INVALID_STATE;
    }
    ++g_store_sequence;
    __sync_synchronize();
    g_snapshot.imu = *sample;
    fc_input_write_end();
    return FC_STATUS_OK;
}

fc_status_t fc_input_publish_gnss(const fc_gnss_sample_t *sample)
{
    if (sample == NULL) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    if (!g_store_initialized) {
        return FC_STATUS_INVALID_STATE;
    }
    ++g_store_sequence;
    __sync_synchronize();
    g_snapshot.gnss = *sample;
    fc_input_write_end();
    return FC_STATUS_OK;
}

fc_status_t fc_input_publish_rc(const fc_rc_sample_t *sample)
{
    if (sample == NULL) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    if (!g_store_initialized) {
        return FC_STATUS_INVALID_STATE;
    }
    ++g_store_sequence;
    __sync_synchronize();
    g_snapshot.rc = *sample;
    fc_input_write_end();
    return FC_STATUS_OK;
}

fc_status_t fc_input_snapshot(fc_input_snapshot_t *snapshot)
{
    uint32_t attempt;
    uint32_t before;
    uint32_t after;

    if (snapshot == NULL) {
        return FC_STATUS_INVALID_ARGUMENT;
    }
    if (!g_store_initialized) {
        return FC_STATUS_INVALID_STATE;
    }

    for (attempt = 0u; attempt < FC_INPUT_SNAPSHOT_RETRIES; ++attempt) {
        before = g_store_sequence;
        if ((before & 1u) != 0u) {
            continue;
        }
        __sync_synchronize();
        *snapshot = g_snapshot;
        __sync_synchronize();
        after = g_store_sequence;
        if ((before == after) && ((after & 1u) == 0u)) {
            return FC_STATUS_OK;
        }
    }
    return FC_STATUS_BUSY;
}
