#include <stdio.h>

#include "user.h"
#include "stm32_seq.h"

// includes for interrupt handler. 
// used for buttons, rtc interrupt pin
#include "user_int_handler.h"

// includes for buttons
#include "buttons.h"
#include "user_buttons.h"
#include "user_buttons_driver.h"
#include "user_buttons_eventHandler.h"

// includes for rtc
#include "user_rtc_driver.h"

// include for lcd
#include "user_lcd.h"
#include "user_lcd_driver.h"
#include "user_lcd_wrapper.h"
#include "stm32_lcd.h"
#include "stlogo.h"
// include for zigbee
#include "app_zigbee.h"

#include "user_hw_timer.h"

// for error message
#include "utils_print.h"

// private stack variable memory allocation
utils_ringbuffer_t user_ringbuffer_intHandler;
uint8_t buf[10];
// function prototype
static void user_fun_test(void);

void a4(void)
{
    APP_ZIGBEE_NwkStartJoin();
}
void a3(void)
{
    APP_ZIGBEE_NwkStartForm();
}
void a2(void)
{
    user_fun_test();
}
void a1(void)
{
    user_exti_handler();
}
// redirection of task functions.
// Written for better navigation of the source code.
void (*user_task_bitmap_gpio_int_fn)(void) = user_exti_handler;
void (*user_task_bitmap_test_fn)(void) = user_fun_test;
void (*user_task_zigbee_nwk_form_fn)(void) = APP_ZIGBEE_NwkStartForm;
void (*user_task_zigbee_nwk_join_fn)(void) = APP_ZIGBEE_NwkStartJoin;
void (*user_task_zigbee_enter_stop_fn)(void) = APP_ZIGBEE_enter_sleep;

//static void user_fun_test_rtc(void)
//{
//    user_rtc_Get_Time(buf);
//    user_rtc_Get_Days(&buf[3]);
//    printf("%d%d Year %d Month %d Day %d:%d:%d\r\n",buf[6],buf[5],buf[4],buf[3],buf[2],buf[1],buf[0]);
//}
static void user_fun_test(void)
{
	printf("test_fun\r\n");
}
static void user_rtc_init() 
{
#ifdef USER_RTC_ENABLE
    // initialize PCF8563 RTC driver and pair them with i2c driver
    printf("user_rtc_init_and_register\r\n");
    if (user_rtc_init(&user_rtc1.user_rtc_h) != true) {
        // initialization fails.
        // may occur when i2c read fails.
    }

    printf("user_rtc_init_and_register done\r\n");
#endif
}
button_invoke_timer_t buttonTimer = {
    .button_invoke_timer_start = user_hw_timer_btnEvent_start,
    .button_invoke_timer_stop = user_hw_timer_btnEvent_stop,
    .button_invoke_timer_restart = user_hw_timer_btnEvent_restart
};


static void user_button_init()
{
    // initialize buttons software and pair them with user_buttons

    user_hw_timer_btnEvent.elapse_ms = HW_TS_250MS;
    user_hw_timer_btnEvent.onTimerElapsedCallback = buttons_onInvokeTimerElapsed;
    HW_TS_Create(user_hw_timer_btnEvent.userid, &user_hw_timer_btnEvent.timerid, user_hw_timer_btnEvent.mode, user_hw_timer_btnEvent.onTimerElapsedCallback);

    buttons_init(&buttonTimer);
    if (buttons_registerButton(user_button1.id, &user_button1_driver, &user_button_eventHandler) != buttons_success) {
        // registeration failed
    }
    if (buttons_registerButton(user_button2.id, &user_button2_driver, &user_button_eventHandler) != buttons_success) {

    }
    // if (buttons_registerButton(user_button3.id, &user_button3_driver, &user_button3_eventHandler) != buttons_success) {

    // }
}
static void user_lcd_init()
{
   user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

   ret = user_lcd_Init(&user_lcd_driver, user_lcd_orientation_landscape);

   if (ret != user_lcd_status_ERROR_NONE) {
       // throw error
        utils_print_err("user lcd init failed %x", ret);
   }

    UTIL_LCD_SetFuncDriver(&LCD_Driver);
    UTIL_LCD_SetDevice(0);

    user_lcd_Clear(SSD1315_COLOR_BLACK);
    user_lcd_DisplayOn();
    user_lcd_Refresh();

    UTIL_LCD_SetFont(&Font12);
    UTIL_LCD_SetTextColor(SSD1315_COLOR_WHITE);
    UTIL_LCD_SetBackColor(SSD1315_COLOR_BLACK);
    user_lcd_Clear(SSD1315_COLOR_BLACK);
    user_lcd_Refresh();
    /* Display ST Logo */
    user_lcd_Clear(SSD1315_COLOR_BLACK);
    user_lcd_Refresh();
    user_lcd_DrawBitmap( 0, 0, (uint8_t *)stlogo);
    user_lcd_Refresh();
    HAL_Delay(2000);
}
static void user_hwtimer_init(void)
{
	// button event timer for handling multiple click events
	// since buttons.c library only notifies press, release, shortkey, longkey, error
}

static void user_qtask_init(void)
{
    // declare ringbuffers for their usage
    if (utils_ringbuffer_alloc(&user_ringbuffer_intHandler) != utils_ringbuffer_result_ok){

    }

    // UTIL_SEQ_Init();
    // register tasks
    UTIL_SEQ_RegTask(user_task_bitmap_gpio_int , 0, user_task_bitmap_gpio_int_fn);
    UTIL_SEQ_RegTask(user_task_bitmap_test, 0, user_task_bitmap_test_fn);

    UTIL_SEQ_RegTask(user_task_zigbee_enter_stop, 0, user_task_zigbee_enter_stop_fn);
    UTIL_SEQ_RegTask(user_task_zigbee_nwk_join, 0, user_task_zigbee_nwk_join_fn);
    UTIL_SEQ_RegTask(user_task_zigbee_nwk_form, 0, user_task_zigbee_nwk_form_fn);
}

void user_init(void)
{
    user_qtask_init();

    user_button_init();
    
    // user_rtc_init();

    // user_hwtimer_init();

    user_lcd_init();
}

