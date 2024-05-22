
/**
 ******************************************************************************
 * @file    stm32wb5mm_dk_lcd.c
 * @author  MCD Application Team
 * @brief   This file includes the driver for Liquid Crystal Display (LCD) module
 *          mounted on STM32WB5MM_DK evaluation board.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* File Info : -----------------------------------------------------------------
								   User NOTES
1. How To use this driver:
--------------------------
   - This driver is used to drive indirectly an LCD.
   - This driver supports the SSD1315 LCD.

2. Driver description:
---------------------
  + Initialization steps:
	 o Initialize the LCD using the user_lcd_Init() function.

	 o Select the LCD layer to be used using the user_lcd_SelectLayer() function.
	 o Enable the LCD display using the user_lcd_DisplayOn() function.
	 o Disable the LCD display using the user_lcd_DisplayOff() function.
	 o Refresh the LCD display using the user_lcd_Refresh() function.
	 o Set Page of the LCD display using the user_lcd_SetPage() function.
	 o Set Column of the LCD display using the user_lcd_SetColumn() function.
	 o Setup Scrolling of the LCD display using the user_lcd_ScrollingSetup() function.
	 o Set the display brightness using the user_lcd_SetBrightness() function.
	 o Get the display brightness using the user_lcd_GetBrightness() function.
	 o Write a pixel to the LCD memory using the user_lcd_WritePixel() function.
	 o Read a pixel from the LCD memory using the user_lcd_ReadPixel() function.
	 o Draw an horizontal line using the user_lcd_DrawHLine() function.
	 o Draw a vertical line using the user_lcd_DrawVLine() function.
	 o Draw a bitmap image using the user_lcd_DrawBitmap() function.
	 o Shift a bitmap image using the user_lcd_ShiftBitmap() function.

  + Display on LCD
	 o Clear the whole LCD using the user_lcd_Clear() function.
	 o Display a character on the specified line and column using the UTIL_LCD_DisplayChar()
	   function or a complete string line using the UTIL_LCD_DisplayStringAtLine() function.
	 o Display a string line on the specified position (x,y in pixel) and align mode
	   using the UTIL_LCD_DisplayStringAtLine() function.
	 o Draw and fill a basic shapes (dot, line, rectangle, circle, ellipse, .. bitmap, raw picture)
	   on LCD using a set of functions.
  Note:
  --------
	Regarding the "Instance" parameter, needed for all functions, it is used to select
	an LCD instance. On the STM32WB5MM_DK board, there's one instance. Then, this
	parameter should be 0.

------------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "user_lcd.h"
/**
 * macros
 */
#define POLY_X(Z) ((int32_t)((Points + (Z))->X))
#define POLY_Y(Z) ((int32_t)((Points + (Z))->Y))
#define ABS(X) (((X) > 0U) ? (X) : -(X))

/**
 * members
 */
static SSD1315_Object_t m_ssd1315_instance;
static uint32_t height;
static uint32_t width;

static SSD1315_Drv_t *LcdDrv = NULL;

/* LINK UTIL LCD */
const LCD_UTILS_Drv_t LCD_Driver =
	{
		user_lcd_DrawBitmap,
		user_lcd_FillRGBRect,
		user_lcd_DrawHLine,
		user_lcd_DrawVLine,
		user_lcd_FillRect,
		user_lcd_ReadPixel,
		user_lcd_WritePixel,
		user_lcd_GetXSize,
		user_lcd_GetYSize,
		NULL,
		user_lcd_GetPixelFormat};

/** STM32WB5MM_DK_LCD_Private_FunctionPrototypes Private Functions
 */

/**
 * @brief  Register Bus IOs for instance 0 if SSD1315 ID is OK
 * @param  Orientation
 * @retval user_lcd_status_t
 */
static user_lcd_status_t SSD1315_Probe(SSD1315_Drv_t* driver, user_lcd_orientation_t Orientation)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (SSD1315_RegisterBusIO(&m_ssd1315_instance, driver) != SSD1315_OK)
	{
		ret = user_lcd_status_ERROR_UNKNOWN_COMPONENT;
	}
	else
	{
		/* LCD Initialization */
		LcdDrv = (SSD1315_Drv_t *)&SSD1315_Driver;
		if (LcdDrv->Init(&m_ssd1315_instance, SSD1315_FORMAT_DEFAULT, Orientation) != SSD1315_OK)
		{
			ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
		}
	}

	return ret;
}

/**
 * @brief  Initializes the LCD.
 * @param  Orientation LCD_ORIENTATION_LANDSCAPE
 * @retval BSP status
 */
user_lcd_status_t user_lcd_Init(SSD1315_Drv_t* driver, user_lcd_orientation_t Orientation)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (Orientation == LCD_ORIENTATION_LANDSCAPE)
	{
		width = LCD_DEFAULT_WIDTH;
		height = LCD_DEFAULT_HEIGHT;
	}
	else
	{
		width = LCD_DEFAULT_HEIGHT;
		height = LCD_DEFAULT_WIDTH;
	}

	/* registers the function and initialize the controller */
	if (SSD1315_Probe(driver, Orientation) != user_lcd_status_ERROR_NONE)
	{
		ret = user_lcd_status_ERROR_UNKNOWN_COMPONENT;
	}

	return ret;
}

/**
 * @brief  De-Initializes the LCD resources.
 * @retval BSP status
 */
user_lcd_status_t user_lcd_DeInit(void)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->DeInit(&m_ssd1315_instance) < 0)
	{
		ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
	}
	LCD_MspDeInit();

	return ret;
}

/**
 * @brief  Gets the LCD Active LCD Pixel Format.
 * @param  PixelFormat Active LCD Pixel Format
 * @retval BSP status
 */
user_lcd_status_t user_lcd_GetPixelFormat(uint32_t *PixelFormat)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	ret = user_lcd_status_ERROR_FEATURE_NOT_SUPPORTED;

	return ret;
}

/**
 * @brief  Gets the LCD X size.
 * @param  pXSize pointer to Used LCD X size
 * @retval BSP status
 */
user_lcd_status_t user_lcd_GetXSize(uint32_t *pXSize)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->GetXSize != NULL)
	{
		if (LcdDrv->GetXSize(&m_ssd1315_instance, pXSize) < 0)
		{
			ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
		}
	}
	else
	{
		*pXSize = width;
	}

	return ret;
}

/**
 * @brief  Gets the LCD Y size.
 * @param  pYSize pointer to Used LCD Y size
 * @retval BSP status
 */
user_lcd_status_t user_lcd_GetYSize(uint32_t *pYSize)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->GetYSize != NULL)
	{
		if (LcdDrv->GetYSize(&m_ssd1315_instance, pYSize) < 0)
		{
			ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
		}
	}
	else
	{
		*pYSize = height;
	}

	return ret;
}

/**
 * @brief  Switch On the display.
 * @retval BSP status
 */
user_lcd_status_t user_lcd_DisplayOn(void)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->DisplayOn != NULL)
	{
		if (LcdDrv->DisplayOn(&m_ssd1315_instance) < 0)
		{
			ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
		}
	}
	else
	{
		ret = user_lcd_status_ERROR_FEATURE_NOT_SUPPORTED;
	}

	return ret;
}

/**
 * @brief  Switch Off the display.
 * @retval BSP status
 */
user_lcd_status_t user_lcd_DisplayOff(void)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->DisplayOff != NULL)
	{
		if (LcdDrv->DisplayOff(&m_ssd1315_instance) < 0)
		{
			ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
		}
	}
	else
	{
		ret = user_lcd_status_ERROR_FEATURE_NOT_SUPPORTED;
	}

	return ret;
}

/**
 * @brief  Refresh the display.
 * @retval BSP status
 */
user_lcd_status_t user_lcd_Refresh(void)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->Refresh != NULL)
	{
		if (LcdDrv->Refresh(&m_ssd1315_instance) < 0)
		{
			ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
		}
	}
	else
	{
		ret = user_lcd_status_ERROR_FEATURE_NOT_SUPPORTED;
	}

	return ret;
}

/**
 * @brief  Set Page.
 * @param  Page LCD Page
 * @retval BSP status
 */
user_lcd_status_t user_lcd_SetPage(uint16_t Page)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->SetPage != NULL)
	{
		if (LcdDrv->SetPage(&m_ssd1315_instance, Page) < 0)
		{
			ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
		}
	}
	else
	{
		ret = user_lcd_status_ERROR_FEATURE_NOT_SUPPORTED;
	}

	return ret;
}

/**
 * @brief  Set Column.
 * @param  Column LCD Column
 * @retval BSP status
 */
user_lcd_status_t user_lcd_SetColumn(uint16_t Column)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->SetColumn != NULL)
	{
		if (LcdDrv->SetColumn(&m_ssd1315_instance, Column) < 0)
		{
			ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
		}
	}
	else
	{
		ret = user_lcd_status_ERROR_FEATURE_NOT_SUPPORTED;
	}

	return ret;
}

/**
 * @brief  Scrolling Setup.
 * @param  ScrollMode LCD Scroll Mode: SSD1315_SCROLL_RIGHT or SSD1315_SCROLL_LEFT
 * @param  StartPage LCD Start page  for scrolling: 0..7
 * @param  EndPage LCD End page for scrolling: This must be larger or equal to StartLine 0..7
 * @param  Frequency LCD Frequency: SSD1315_SCROLL_FREQ_2FRAMES to SSD1315_SCROLL_FREQ_256FRAMES
 * @retval BSP status
 */
user_lcd_status_t user_lcd_ScrollingSetup(uint16_t ScrollMode, uint16_t StartPage, uint16_t EndPage, uint16_t Frequency)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->ScrollingSetup != NULL)
	{
		if (LcdDrv->ScrollingSetup(&m_ssd1315_instance, ScrollMode, StartPage, EndPage, Frequency) < 0)
		{
			ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
		}
	}
	else
	{
		ret = user_lcd_status_ERROR_FEATURE_NOT_SUPPORTED;
	}

	return ret;
}

/**
 * @brief  Scrolling Start.
 * @retval BSP status
 */
user_lcd_status_t user_lcd_ScrollingStart(void)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->ScrollingStart != NULL)
	{
		if (LcdDrv->ScrollingStart(&m_ssd1315_instance) < 0)
		{
			ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
		}
	}
	else
	{
		ret = user_lcd_status_ERROR_FEATURE_NOT_SUPPORTED;
	}

	return ret;
}

/**
 * @brief  Scrolling Stop.
 * @retval BSP status
 */
user_lcd_status_t user_lcd_ScrollingStop(void)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->ScrollingStop != NULL)
	{
		if (LcdDrv->ScrollingStop(&m_ssd1315_instance) < 0)
		{
			ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
		}
	}
	else
	{
		ret = user_lcd_status_ERROR_FEATURE_NOT_SUPPORTED;
	}

	return ret;
}

/**
 * @brief  Set the brightness value
 * @param  Brightness [00: Min (black), 100 Max]
 * @retval BSP status
 */
user_lcd_status_t user_lcd_SetBrightness(uint32_t Brightness)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->SetBrightness != NULL)
	{
		if (LcdDrv->SetBrightness(&m_ssd1315_instance, Brightness) < 0)
		{
			ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
		}
	}
	else
	{
		ret = user_lcd_status_ERROR_FEATURE_NOT_SUPPORTED;
	}

	return ret;
}

/**
 * @brief  Get the brightness value
 * @param  Brightness [00: Min (black), 100 Max]
 * @retval BSP status
 */
user_lcd_status_t user_lcd_GetBrightness(uint32_t *Brightness)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->GetBrightness != NULL)
	{
		if (LcdDrv->GetBrightness(&m_ssd1315_instance, Brightness) < 0)
		{
			ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
		}
	}
	else
	{
		ret = user_lcd_status_ERROR_FEATURE_NOT_SUPPORTED;
	}

	return ret;
}

/**
 * @brief  Set the LCD Orientation.
 * @param  Orientation LCD orientation to set
 * @retval BSP status
 */
user_lcd_status_t user_lcd_SetOrientation(uint32_t Orientation)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->SetOrientation != NULL)
	{
		if (LcdDrv->SetOrientation(&m_ssd1315_instance, Orientation) < 0)
		{
			ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
		}
	}
	else
	{
		ret = user_lcd_status_ERROR_FEATURE_NOT_SUPPORTED;
	}

	return ret;
}

/**
 * @brief  Get the LCD orientation.
 * @param  Orientation LCD Orientation used
 * @retval BSP status
 */
user_lcd_status_t user_lcd_GetOrientation(uint32_t *Orientation)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->GetOrientation != NULL)
	{
		if (LcdDrv->GetOrientation(&m_ssd1315_instance, Orientation) < 0)
		{
			ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
		}
	}
	else
	{
		ret = user_lcd_status_ERROR_FEATURE_NOT_SUPPORTED;
	}

	return ret;
}

/**
 * @brief  Get the LCD orientation.
 * @param  Xpos to set the cursor
 * @param  Ypos to set the cursor
 * @retval BSP status
 */
user_lcd_status_t user_lcd_SetCursor(uint32_t Xpos, uint32_t Ypos)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->SetCursor != NULL)
	{
		if (LcdDrv->SetCursor(&m_ssd1315_instance, Xpos, Ypos) < 0)
		{
			ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
		}
	}
	else
	{
		ret = user_lcd_status_ERROR_FEATURE_NOT_SUPPORTED;
	}

	return ret;
}

/**
 * @brief  Draws a bitmap picture loaded in the internal Flash in currently active layer.
 * @param  Xpos Bmp X position in the LCD
 * @param  Ypos Bmp Y position in the LCD
 * @param  pBmp Pointer to Bmp picture address in the internal Flash
 * @retval BSP status
 */
user_lcd_status_t user_lcd_DrawBitmap(uint32_t Xpos, uint32_t Ypos, uint8_t *pBmp)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->DrawBitmap != NULL)
	{
		/* Draw the bitmap on LCD */
		if (LcdDrv->DrawBitmap(&m_ssd1315_instance, Xpos, Ypos, pBmp) < 0)
		{
			ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
		}
	}
	else
	{
		ret = user_lcd_status_ERROR_FEATURE_NOT_SUPPORTED;
	}

	return ret;
}

/**
 * @brief  Draws Shift bitmap picture loaded in the internal Flash in currently active layer.
 * @param  Xpos Bmp X position in the LCD
 * @param  Ypos Bmp Y position in the LCD
 * @param  Xshift  specifies number of pixel to shift on X position.
 * @param  Yshift  specifies number of pixel to shift on Y position.
 * @param  pBmp Pointer to Bmp picture address in the internal Flash
 * @retval BSP status
 */
user_lcd_status_t user_lcd_ShiftBitmap(uint32_t Xpos, uint32_t Ypos, int16_t Xshift, int16_t Yshift, uint8_t *pBmp)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->ShiftBitmap != NULL)
	{
		/* shift bitmap on LCD */
		if (LcdDrv->ShiftBitmap(&m_ssd1315_instance, Xpos, Ypos, Xshift, Yshift, pBmp) < 0)
		{
			ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
		}
	}
	else
	{
		ret = user_lcd_status_ERROR_FEATURE_NOT_SUPPORTED;
	}

	return ret;
}

/**
 * @brief  Fill a rectangle with a BitMap on LCD.
 * @param  pData Pointer to RGB line data
 * @param  Xpos X position.
 * @param  Ypos Y position.
 * @param  Width width of the rectangle to fill.
 * @param  Height height of the rectangle to fill.
 * @retval BSP status.
 */
user_lcd_status_t user_lcd_FillRGBRect(uint32_t Xpos, uint32_t Ypos, uint8_t *pData, uint32_t Width, uint32_t Height)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->FillRGBRect != NULL)
	{
		/* shift bitmap on LCD */
		if (LcdDrv->FillRGBRect(&m_ssd1315_instance, Xpos, Ypos, pData, Width, Height) < 0)
		{
			ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
		}
	}
	else
	{
		ret = user_lcd_status_ERROR_FEATURE_NOT_SUPPORTED;
	}

	return ret;
}

/**
 * @brief  Draws an horizontal line
 * @param  Xpos X position
 * @param  Ypos Y position
 * @param  Length Line length
 * @param  Color Line color
 * @retval BSP status
 */
user_lcd_status_t user_lcd_DrawHLine(uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->DrawHLine != NULL)
	{
		/* Draw the horizontal line on LCD */
		if (LcdDrv->DrawHLine(&m_ssd1315_instance, Xpos, Ypos, Length, Color) < 0)
		{
			ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
		}
	}
	else
	{
		ret = user_lcd_status_ERROR_FEATURE_NOT_SUPPORTED;
	}

	return ret;
}

/**
 * @brief  Draws a vertical line
 * @param  Xpos X position
 * @param  Ypos Y position
 * @param  Length Line length
 * @param  Color Line color
 * @retval BSP status
 */
user_lcd_status_t user_lcd_DrawVLine(uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->DrawVLine != NULL)
	{
		/* Draw the vertical line on LCD */
		if (LcdDrv->DrawVLine(&m_ssd1315_instance, Xpos, Ypos, Length, Color) < 0)
		{
			ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
		}
	}
	else
	{
		ret = user_lcd_status_ERROR_FEATURE_NOT_SUPPORTED;
	}

	return ret;
}

/**
 * @brief  Draws a full rectangle in currently active layer.
 * @param  Xpos X position
 * @param  Ypos Y position
 * @param  Width Rectangle width
 * @param  Height Rectangle height
 * @param  Color Color of rectangle
 * @retval BSP status
 */
user_lcd_status_t user_lcd_FillRect(uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t Color)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->FillRect(&m_ssd1315_instance, Xpos, Ypos, Width, Height, Color) < 0)
	{
		ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
	}

	return ret;
}

/**
 * @brief  clear the LCD in currently active layer.
 * @param  Color to set
 * @retval BSP status
 */
user_lcd_status_t user_lcd_Clear(uint32_t Color)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->FillRect(&m_ssd1315_instance, 1, 0, width, height, Color) < 0)
	{
		ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
	}

	return ret;
}

/**
 * @brief  Reads a LCD pixel color.
 * @param  Xpos X position
 * @param  Ypos Y position
 * @param  Color pointer to RGB pixel color
 * @retval BSP status
 */
user_lcd_status_t user_lcd_ReadPixel(uint32_t Xpos, uint32_t Ypos, uint32_t *Color)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->GetPixel(&m_ssd1315_instance, Xpos, Ypos, Color) < 0)
	{
		ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
	}

	return ret;
}

/**
 * @brief  Writes a LCD pixel.
 * @param  Xpos X position
 * @param  Ypos Y position
 * @param  Color RGB pixel color
 * @retval BSP status
 */
user_lcd_status_t user_lcd_WritePixel(uint32_t Xpos, uint32_t Ypos, uint32_t Color)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	if (LcdDrv->SetPixel(&m_ssd1315_instance, Xpos, Ypos, Color) < 0)
	{
		ret = user_lcd_status_ERROR_COMPONENT_FAILURE;
	}

	return ret;
}

/**
 * @brief  Writes register on LCD register.
 * @param  Reg Register to be written
 * @param  pData pointer to the read data from LCD SRAM.
 * @param  Length length of data be read from the LCD SRAM
 * @retval BSP status
 */
user_lcd_status_t user_lcd_WriteReg(uint16_t Reg, uint8_t *pData, uint16_t Length)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;

	/* Send Data */
	if ((ret == user_lcd_status_ERROR_NONE) && (Length > 0U))
	{
		if (user_lcd_SendData(pData, Length) != user_lcd_status_ERROR_NONE)
		{
			ret = user_lcd_status_ERROR_BUS_FAILURE;
		}
	}

	return ret;
}

/**
 * @brief  Send data to select the LCD SRAM.
 * @param  pData pointer to data to write to LCD SRAM.
 * @param  Length length of data to write to LCD SRAM
 * @retval Error status
 */
user_lcd_status_t user_lcd_SendData(uint8_t *pData, uint16_t Length)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;
	if (Length == 1)
	{
		/* Reset LCD control line CS */
		LCD_CS_LOW();
		LCD_DC_LOW();
		/* Send Data */
		if (BSP_SPI1_Send(pData, Length) != user_lcd_status_ERROR_NONE)
		{
			ret = user_lcd_status_ERROR_BUS_FAILURE;
		}
		/* Deselect : Chip Select high */
		LCD_CS_HIGH();
	}
	else
	{
		LCD_CS_LOW();
		LCD_DC_HIGH();
		/* Send Data */
		if (BSP_SPI1_Send(pData, Length) != user_lcd_status_ERROR_NONE)
		{
			ret = user_lcd_status_ERROR_BUS_FAILURE;
		}
		LCD_DC_LOW();
		/* Deselect : Chip Select high */
		LCD_CS_HIGH();
	}

	return ret;
}

/**
 * @brief  Read data from LCD data register.
 * @param  Reg Register to be read
 * @param  pData pointer to the read data from LCD SRAM.
 * @param  Length length of data be read from the LCD SRAM
 * @retval BSP status
 */
user_lcd_status_t user_lcd_ReadReg(uint16_t Reg, uint8_t *pData, uint16_t Length)
{
	user_lcd_status_t ret = user_lcd_status_ERROR_NONE;
	UNUSED(Length);

	/* Send Reg value to Read */
	if (user_lcd_WriteReg(Reg, pData, 0) != user_lcd_status_ERROR_NONE)
	{
		ret = user_lcd_status_ERROR_BUS_FAILURE;
	}
	/* Reset LCD control line(/CS) and Send command */
	LCD_CS_LOW();

	if (ret == user_lcd_status_ERROR_NONE)
	{
		if (BSP_SPI1_Recv(pData, 2) != user_lcd_status_ERROR_NONE)
		{
			ret = user_lcd_status_ERROR_BUS_FAILURE;
		}
	}
	/* Deselect : Chip Select high */
	LCD_CS_HIGH();

	return ret;
}

/**
 * @}
 */

/** @defgroup STM32WB5MM_DK_LCD_Private_Functions Private Functions
 * @{
 */

/*******************************************************************************
 ********************************* LINK LCD ***********************************/

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
