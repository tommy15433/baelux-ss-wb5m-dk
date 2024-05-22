#pragma once

#include "ssd1315.h"
#include <stdint.h>
#include <stdbool.h>

#define USER_LCD_DRIVER_LOG_ENABLE  1

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
    user_lcd_driver_status_fail = SSD1315_ERROR,
    user_lcd_driver_status_ok = SSD1315_OK,
    user_lcd_driver_status_init_fail,
    user_lcd_driver_status_write_error,
    user_lcd_driver_status_read_error,

} user_lcd_driver_status_t;

typedef SSD1315_IO_t user_lcd_driver_t;

// typedef struct 
// {
//     user_lcd_driver_status_t (*init)(void);
//     user_lcd_driver_status_t (*deinit)(void);
//     user_lcd_driver_status_t (*transmit)(uint8_t* data, uint16_t size);
//     user_lcd_driver_status_t (*receive)(uint8_t* data, uint16_t size);
//     user_lcd_driver_status_t (*transceive)(uint8_t* tdata, uint8_t* rdata, uint16_t size);
//     uint32_t (*getTimestampMs)(void);

// } user_lcd_driver_t;

/**
 * @brief pass this driver as SSD1315_IO_t to SSD1315_RegisterBusIO(SSD1315_Object_t *pObj, SSD1315_IO_t *pIO)
 *          and SSD1315_Object_t will be initialized automatically
*/
extern user_lcd_driver_t user_lcd_driver;

#ifdef __cplusplus
}
#endif