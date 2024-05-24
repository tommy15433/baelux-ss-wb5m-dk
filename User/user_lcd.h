#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wbxx_hal.h"
#include "ssd1315.h"
#include "lcd.h"
#include "fonts.h"
#include "user_lcd_driver.h"
#include <stdint.h>
/* Include audio component Driver */

/** @addtogroup BSP
 * @{
 */

/** @addtogroup STM32WB5MM_DK
 * @{
 */

/** @defgroup STM32WB5MM_DK_LCD STM32WB5MM_DK LCD
 * @{
 */

/** @defgroup STM32WB5MM_DK_LCD_Exported_Constants Exported Constants
 * @{
 */
#define LCD_INSTANCES_NBR 1U
#define LCD_ORIENTATION_LANDSCAPE SSD1315_ORIENTATION_LANDSCAPE
#define LCD_DEFAULT_WIDTH SSD1315_LCD_PIXEL_WIDTH
#define LCD_DEFAULT_HEIGHT SSD1315_LCD_PIXEL_HEIGHT
#define LCD_COLOR_BLACK SSD1315_COLOR_BLACK
#define LCD_COLOR_WHITE SSD1315_COLOR_WHITE

        /** status
     *
     */
    typedef enum
    {

        user_lcd_status_ERROR_NONE = 0,
        user_lcd_status_ERROR_NO_INIT = -1,
        user_lcd_status_ERROR_WRONG_PARAM = -2,
        user_lcd_status_ERROR_BUSY = -3,
        user_lcd_status_ERROR_PERIPH_FAILURE = -4,
        user_lcd_status_ERROR_COMPONENT_FAILURE = -5,
        user_lcd_status_ERROR_UNKNOWN_FAILURE = -6,
        user_lcd_status_ERROR_UNKNOWN_COMPONENT = -7,
        user_lcd_status_ERROR_BUS_FAILURE = -8,
        user_lcd_status_ERROR_CLOCK_FAILURE = -9,
        user_lcd_status_ERROR_MSP_FAILURE = -10,
        user_lcd_status_ERROR_FEATURE_NOT_SUPPORTED = -11,
        user_lcd_status_ERROR_BUS_TRANSACTION_FAILURE = -100,
        user_lcd_status_ERROR_BUS_ARBITRATION_LOSS = -101,
        user_lcd_status_ERROR_BUS_ACKNOWLEDGE_FAILURE = -102,
        user_lcd_status_ERROR_BUS_PROTOCOL_FAILURE = -103,

    } user_lcd_status_t;
    typedef enum
    {
        user_lcd_orientation_landscape = SSD1315_ORIENTATION_LANDSCAPE,
    } user_lcd_orientation_t;
    /**
     * @}
     */

    /** @defgroup STM32WB5MM_DK_LCD_Exported_Types Exported Types
     * @{
     */
    typedef struct
    {
        uint32_t Width;
        uint32_t Height;
        uint32_t IsMspCallbacksValid;
    } BSP_LCD_Ctx_t;

    /**
     * @}
     */

    /** @addtogroup STM32WB5MM_DK_LCD_Private_Variables
     * @{
     */
    extern void *LcdCompObj;
    /**
     * @}
     */

    /** @defgroup STM32WB5MM_DK_LCD_Exported_Functions Exported Functions
     * @{
     */
    user_lcd_status_t user_lcd_Init(user_lcd_driver_t *driver, user_lcd_orientation_t Orientation);
    user_lcd_status_t user_lcd_DeInit(void);

    /* LCD generic APIs: Display control */
    user_lcd_status_t user_lcd_DisplayOn(void);
    user_lcd_status_t user_lcd_DisplayOff(void);
    user_lcd_status_t user_lcd_SetBrightness(uint32_t Brightness);
    user_lcd_status_t user_lcd_GetBrightness(uint32_t *Brightness);
    user_lcd_status_t user_lcd_GetXSize(uint32_t *XSize);
    user_lcd_status_t user_lcd_GetYSize(uint32_t *YSize);
    user_lcd_status_t user_lcd_GetPixelFormat(uint32_t *PixelFormat);
    user_lcd_status_t user_lcd_SetOrientation(uint32_t Orientation);
    user_lcd_status_t user_lcd_GetOrientation(uint32_t *Orientation);
    user_lcd_status_t user_lcd_Refresh(void);
    user_lcd_status_t user_lcd_SetPage(uint16_t Page);
    user_lcd_status_t user_lcd_SetColumn(uint16_t Column);
    user_lcd_status_t user_lcd_ScrollingSetup(uint16_t ScrollMode, uint16_t StartPage, uint16_t EndPage, uint16_t Frequency);
    user_lcd_status_t user_lcd_ScrollingStart(void);
    user_lcd_status_t user_lcd_ScrollingStop(void);
    user_lcd_status_t user_lcd_SetCursor(uint32_t Xpos, uint32_t Ypos);
    user_lcd_status_t user_lcd_DrawBitmap(uint32_t Xpos, uint32_t Ypos, uint8_t *pBmp);
    user_lcd_status_t user_lcd_ShiftBitmap(uint32_t Xpos, uint32_t Ypos, int16_t Xshift, int16_t Yshift, uint8_t *pBmp);
    user_lcd_status_t user_lcd_DrawHLine(uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color);
    user_lcd_status_t user_lcd_DrawVLine(uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color);
    user_lcd_status_t user_lcd_FillRect(uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t Color);
    user_lcd_status_t user_lcd_FillRGBRect(uint32_t Xpos, uint32_t Ypos, uint8_t *pData, uint32_t Width, uint32_t Height);
    user_lcd_status_t user_lcd_ReadPixel(uint32_t Xpos, uint32_t Ypos, uint32_t *Color);
    user_lcd_status_t user_lcd_WritePixel(uint32_t Xpos, uint32_t Ypos, uint32_t Color);
    user_lcd_status_t user_lcd_Clear(uint32_t Color);
    user_lcd_status_t user_lcd_SetActiveLayer(uint32_t LayerIndex);
    user_lcd_status_t user_lcd_SetPixel(uint32_t Xpos, uint32_t Ypos, uint32_t Color);
    user_lcd_status_t user_lcd_GetPixel(uint32_t Xpos, uint32_t Ypos, uint32_t *Color);

    /* LCD specific APIs */
    user_lcd_status_t user_lcd_WriteReg(uint16_t Reg, uint8_t *pData, uint16_t Length);
    user_lcd_status_t user_lcd_ReadReg(uint16_t Reg, uint8_t *pData, uint16_t Length);
    user_lcd_status_t user_lcd_SendData(uint8_t *pData, uint16_t Length);

#ifdef __cplusplus
}
#endif
