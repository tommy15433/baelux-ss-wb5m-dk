#include "user_lcd_wrapper.h"

int32_t user_lcd_DrawBitmap_wrapper(
    uint32_t device,
    uint32_t bbb,
    uint32_t ccc,
    uint8_t *ddd)
{
    return user_lcd_DrawBitmap(bbb, ccc, ddd);
}

int32_t user_lcd_FillRGBRect_wrapper(
    uint32_t device,
    uint32_t bbb,
    uint32_t ccc,
    uint8_t* ddd,
    uint32_t eee,
    uint32_t fff)
{
    return user_lcd_FillRGBRect(bbb, ccc, ddd, eee, fff);
}

int32_t user_lcd_DrawHLine_wrapper(
    uint32_t device,
    uint32_t bbb,
    uint32_t ccc,
    uint32_t ddd,
    uint32_t eee)
{
    return user_lcd_DrawHLine(bbb, ccc, ddd, eee);
}

int32_t user_lcd_DrawVLine_wrapper(
    uint32_t device,
    uint32_t bbb,
    uint32_t ccc,
    uint32_t ddd,
    uint32_t eee)
{
    return user_lcd_DrawVLine(bbb, ccc, ddd, eee);
}

int32_t user_lcd_FillRect_wrapper(
    uint32_t device,
    uint32_t bbb,
    uint32_t ccc,
    uint32_t ddd,
    uint32_t eee,
    uint32_t fff)
{
    return user_lcd_FillRect(bbb, ccc, ddd, eee, fff);
}

int32_t user_lcd_ReadPixel_wrapper(
    uint32_t device,
    uint32_t bbb,
    uint32_t ccc,
    uint32_t* ddd)
{
    return user_lcd_ReadPixel(bbb, ccc, ddd);
}

int32_t user_lcd_WritePixel_wrapper(
    uint32_t device,
    uint32_t bbb,
    uint32_t ccc,
    uint32_t ddd)
{
    return user_lcd_WritePixel(bbb, ccc, ddd);
}

int32_t user_lcd_GetXSize_wrapper(
    uint32_t device,
    uint32_t* bbb)
{
    return user_lcd_GetXSize(bbb);
}

int32_t user_lcd_GetYSize_wrapper(
    uint32_t device,
    uint32_t* bbb)
{
    return user_lcd_GetYSize(bbb);
}

int32_t user_lcd_GetPixelFormat_wrapper(
    uint32_t device,
    uint32_t* bbb)
{
    return user_lcd_GetPixelFormat(bbb);
}

/* LINK UTIL LCD */
const LCD_UTILS_Drv_t LCD_Driver =
	{
		user_lcd_DrawBitmap_wrapper,
		user_lcd_FillRGBRect_wrapper,
		user_lcd_DrawHLine_wrapper,
		user_lcd_DrawVLine_wrapper,
		user_lcd_FillRect_wrapper,
		user_lcd_ReadPixel_wrapper,
		user_lcd_WritePixel_wrapper,
		user_lcd_GetXSize_wrapper,
		user_lcd_GetYSize_wrapper,
		NULL,
		user_lcd_GetPixelFormat_wrapper};