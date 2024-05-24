#pragma once


#include <stdint.h>
#include <stdbool.h>
#include "stm32wb5mxx.h"
#include "stm32wbxx_hal.h"
#include "stm32wbxx_hal_gpio.h"

#include "user.h"
#ifdef USER_RTC_ENABLE
#include "user_rtc.h"
#include "main.h"
/**
 * @file RTC controller driver file
*/

#define USER_RTC_I2C_H  &hi2c1
#define USER_RTC_INT_GPIO       GPIOD
#define USER_RTC_INT_PIN    1

typedef struct {

    GPIO_TypeDef* gpio_int;

    uint16_t pin_int;
    uint16_t pinMask_int;

    I2C_HandleTypeDef* hi2c;

    user_rtc_handle_t user_rtc_h;
    // bool (*i2c_read)(uint8_t sla_7bit, uint8_t addr, uint8_t* data, uint8_t size);
    // bool (*i2c_write)(uint8_t sla_7bit, uint8_t addr, uint8_t* data, uint8_t size);
} user_rtc_driver_t;

extern user_rtc_driver_t user_rtc1;

#endif