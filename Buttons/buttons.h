#pragma once

#include "button.h"

#define BUTTONS_MAX  5

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    buttons_success,
    buttons_overflow,
    buttons_idNotUnique,
    buttons_idNotValid
}buttons_result_e;


void buttons_init(button_invoke_timer_t* invokeTimer);
//void buttons_update(button_id_t id);
void buttons_update(button_id_t id, uint32_t timestamp, button_status_e status);
/**
 * @brief call this function on button timer elapsed when button_invoke_timer_t is in use.
 * 
 */
void buttons_onInvokeTimerElapsed(void);
buttons_result_e buttons_registerButton(
    button_id_t id,
    button_driver_t* driver, 
    button_eventHandler_t* handler);



#ifdef __cplusplus
}
#endif
