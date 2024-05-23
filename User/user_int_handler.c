#include "user_int_handler.h"
#include "stm32_seq.h"

#include "user.h"
#include "user_buttons.h"

#include "user_rtc_driver.h"
#include "user_rtc.h"

void user_exti_handler(void) 
{
    while (utils_ringbuffer_getDataCount() > 0) {
        ringbuffer_button_data_t output = utils_ringbuffer_pop(&user_ringbuffer_intHandler);
        uint16_t pin = output.data;
        uint32_t ts = output.timestamp;
        button_status_e status = output.status;
        
        if (pin == user_button1.pinMask){
            buttons_update(user_button1.id, ts, status);
        }
        else if (pin == user_button2.pinMask) {
            buttons_update(user_button2.id, ts, status);
        }

    #ifdef USER_RTC_ENABLE
        else if (pin == user_rtc1.pinMask_int) {

            // handle rtc timer/counter callback

            // read interrupt flag from ss_rtc

            /*
            * if (flag == timer_flag){
            * 	// code
            * } else if (flag == counter_flag) {
            * 	// code
            * }
            */
        }
    #endif
        else {

        }

    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    button_info* btn = user_button_find(GPIO_Pin);
    ringbuffer_button_data_t input = {
        .data = GPIO_Pin,
        .timestamp = HAL_GetTick(),
        .status = HAL_GPIO_ReadPin(btn->gpio, btn->pinMask) == btn->pressedLevel ? button_status_pressed : button_status_released,
    };
    
    utils_ringbuffer_push(&user_ringbuffer_intHandler, input);

    // task is registerd in user.h
    // user_task_bitmap_gpio_int will call user_exti_handler
	UTIL_SEQ_SetTask(user_task_bitmap_gpio_int, 1);
}
