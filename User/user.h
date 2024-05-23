#pragma once

/** 
 * @file This file is for convenience for using user functions since there are too many user_&.c/h files.
 * @file write/rewrite/use user_init() function to initialize all necessary functions.
*/



#include "utils_ringbuffer.h"


// #define USER_RTC_ENABLE

// max 32
enum {
    user_task_bitmap_gpio_int = 1 << 4,
    user_task_bitmap_test = 1 << 5,
    user_task_zigbee_nwk_start = 1 << 6,
    user_task_zigbee_enter_stop = 1 << 7,
    user_task_zigbee_nwk_form = 1 << 8,
    user_task_zigbee_nwk_join = 1 << 9,
    user_task_invokeBtnEvent = 1 << 10,
    
};
extern utils_ringbuffer_t user_ringbuffer_intHandler;

void user_init(void);