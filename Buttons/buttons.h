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

void buttons_init(void);
void buttons_update(button_id_t id);
buttons_result_e buttons_registerButton(button_id_t id, button_driver_t* driver, button_eventHandler_t* handler);


#ifdef __cplusplus
}
#endif