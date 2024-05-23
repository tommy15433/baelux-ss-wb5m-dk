#pragma once

#include "utils_config.h"
#include "stdint.h"

typedef struct {
    uint32_t data;
    uint8_t status;
    uint32_t timestamp;
} ringbuffer_button_data_t;

// typedef uint32_t ringbuffer_button_data_t;
typedef uint8_t  ringbuffer_idx_t;

typedef struct {
    ringbuffer_button_data_t buffer[UTIL_CONF_RINGBUFFER_MAX_BUFFERSIZE];
    ringbuffer_idx_t inIdx;
    ringbuffer_idx_t outIdx;
} utils_ringbuffer_t;

typedef enum {
    utils_ringbuffer_result_ok,
    utils_ringbuffer_result_maxalloc,
    utils_ringbuffer_result_notgood,
} utils_ringbuffer_result_e;

utils_ringbuffer_result_e utils_ringbuffer_alloc(utils_ringbuffer_t* handle);
void utils_ringbuffer_push(utils_ringbuffer_t* handle, ringbuffer_button_data_t data);
ringbuffer_button_data_t utils_ringbuffer_pop(utils_ringbuffer_t* handle);
uint32_t utils_ringbuffer_getDataCount(void);
