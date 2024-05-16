#include "string.h"
#include "utils_ringbuffer.h"

static utils_ringbuffer_t m_ringbuffers[UTIL_CONF_RINGBUFFER_MAX_ALLOC];
static uint32_t m_alloc_count = 0;

utils_ringbuffer_result_e utils_ringbuffer_alloc(utils_ringbuffer_t* handle)
{
    if (m_alloc_count >= UTIL_CONF_RINGBUFFER_MAX_ALLOC) {
        return utils_ringbuffer_result_maxalloc;
    }

    uint32_t idxToAdd = m_alloc_count++;

    m_ringbuffers[idxToAdd].inIdx = 0;
    m_ringbuffers[idxToAdd].outIdx = 0;
    memset(m_ringbuffers[idxToAdd].buffer, 0, sizeof(ringbuffer_data_t) * UTIL_CONF_RINGBUFFER_MAX_BUFFERSIZE);

    handle = &m_ringbuffers[idxToAdd];

    return utils_ringbuffer_result_ok;
}

void utils_ringbuffer_push(utils_ringbuffer_t* handle, ringbuffer_data_t data)
{
    if (handle->inIdx >= UTIL_CONF_RINGBUFFER_MAX_BUFFERSIZE) {
        handle->inIdx = 0;
    }

    ringbuffer_idx_t idx2push = handle->inIdx++;

    handle->buffer[idx2push] = data;
}

ringbuffer_data_t utils_ringbuffer_pop(utils_ringbuffer_t* handle)
{
    ringbuffer_idx_t idx2pop = handle->outIdx;

    if (++handle->outIdx >= UTIL_CONF_RINGBUFFER_MAX_BUFFERSIZE) {
        handle->outIdx = 0;
    }

    return handle->buffer[idx2pop];
}

