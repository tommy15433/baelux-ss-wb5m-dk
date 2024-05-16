#include "user_int_handler.h"
#include "stm32_seq.h"

#include "user.h"
#include "user_buttons.h"

#include "user_rtc_driver.h"
#include "user_rtc.h"

void user_exti_handler(void) 
{
    uint16_t pin = (uint16_t)utils_ringbuffer_pop(&user_ringbuffer_intHandler);

    if (pin == user_button1.pinMask){
        buttons_update(user_button1.id);
    }
    else if (pin == user_button2.pinMask) {
        buttons_update(user_button2.id);
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

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    printf("exticb\r\n");
    utils_ringbuffer_push(&user_ringbuffer_intHandler, (ringbuffer_data_t)GPIO_Pin);

    // task is registerd in user.h
    // user_task_bitmap_gpio_int will call user_exti_handler
	UTIL_SEQ_SetTask(user_task_bitmap_gpio_int, 1);
}
