#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "hw_conf.h"

typedef struct {
    uint32_t userid;    // provided by user. returned in callback??
    uint8_t timerid;    // operation id, pointer to pTimerId on calling HW_TS_Create();
    HW_TS_Mode_t mode;
    void (*onTimerElapsedCallback)(void);
}user_hw_timer_t;