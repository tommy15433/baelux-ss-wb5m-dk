#include "user_buttons.h"
#include "buttons.h"
#include "stm32wbxx_hal.h"
#include "stdint.h"


button_status_e button_read_1(void)
{
    int status = HAL_GPIO_ReadPin(user_button1.gpio, user_button1.pinMask);

    return (status == user_button1.pressedLevel) ? button_status_pressed : button_status_released;
}

button_status_e button_read_2(void)
{
    int status = HAL_GPIO_ReadPin(user_button2.gpio, user_button2.pinMask);

    return (status == user_button2.pressedLevel) ? button_status_pressed : button_status_released;
}

// button_status_e button_read_3(void)
// {
//     int status = HAL_GPIO_ReadPin(user_button3.gpio, user_button3.pinMask);

//     return (status == user_button3.pressedLevel) ? button_status_pressed : button_status_released;
// }

uint32_t getTimeMs(void)
{
    uint32_t tick = HAL_GetTick();

    return tick;
}

button_driver_t user_button1_driver = {
    .button_getTimeStamp = getTimeMs,
    .button_getButtonState = button_read_1
    };

button_driver_t user_button2_driver = {
    .button_getTimeStamp = getTimeMs,
    .button_getButtonState = button_read_2
    };

// button_driver_t user_button3_driver = {
//     .button_getTimeStamp = getTimeMs,
//     .button_getButtonState = button_read_3
//     };
