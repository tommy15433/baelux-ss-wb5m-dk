#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "hw_conf.h"
#include "hw_if.h"
#include "app_conf.h"

#define HW_TS_250MS (250 * 1000 / CFG_TS_TICK_VAL)
#define HW_TS_1SEC (1 * 1000 * 1000 / CFG_TS_TICK_VAL) /* 1s */

enum user_hw_timer_id_e{
    user_hw_timer_id_start = 4,
    user_hw_timer_id_btnEvent,

} ;
typedef struct {
    uint32_t userid;    // provided by user. returned in callback??
    uint8_t timerid;    // operation id, pointer to pTimerId on calling HW_TS_Create();
    HW_TS_Mode_t mode;
    void (*onTimerElapsedCallback)(void);

    uint32_t elapse_ms;
}user_hw_timer_t;

extern user_hw_timer_t user_hw_timer_btnEvent;

void user_hw_timer_btnEvent_start();
void user_hw_timer_btnEvent_restart();
void user_hw_timer_btnEvent_stop();
