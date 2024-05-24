
#include <stdio.h>

#include "stm32wb5mxx.h"
#include "stm32wbxx_hal.h"
#include "user_rtc_driver.h"

#ifdef USER_RTC_ENABLE

#include "user_rtc.h"
#include "utils_config.h"
/**
 * @brief i2c read when using 7bit slave address form
*/
bool user_i2c_read_rtc_7bit(uint8_t sla7bit, uint8_t da, uint8_t* data, uint8_t size) 
{
	if (HAL_I2C_Mem_Read(USER_RTC_I2C_H ,(sla7bit << 1), da, 1, data, size, 1000) != HAL_OK){
        return false;     
    }
    // need delay?
    // HAL_Delay(0);

    return true;
}

/**
 * @brief i2c write when using 7bit slave address form
*/
bool user_i2c_write_rtc_7bit(uint8_t sla7bit, uint8_t da, uint8_t* data, uint8_t size) 
{
    if (HAL_I2C_Mem_Write(USER_RTC_I2C_H, (sla7bit << 1), da, 1, data, size, 1000) != HAL_OK) {
        return false;
    }
    // need delay?
    // HAL_Delay(0);

    return true;
}

void print(const char* msg)
{
#if (USER_RTC_PRINT_ERR == true)
    printf(ANSI_COLOR_RED "%s\r\n" ANSI_COLOR_RESET, msg);
#endif
}
user_rtc_driver_t user_rtc1 = {
    .gpio_int = USER_RTC_INT_GPIO,
    .pin_int = USER_RTC_INT_PIN,
    .pinMask_int = 1 << USER_RTC_INT_PIN,
    .hi2c = &hi2c1,
    .user_rtc_h = {
        .i2c_read = user_i2c_read_rtc_7bit,
        .i2c_write = user_i2c_write_rtc_7bit,
        .print = print
    }
};

#endif
