#include "stm32wbxx_hal.h"
#include "main.h"
#include "user_lcd_driver.h"

#ifdef USER_LCD_DRIVER_LOG_ENABLE
#include "utils_print.h"
#endif

/*##################### LCD ###################################*/
/**
 * @brief  LCD Chip Select macro definition
 */
#define LCD_CS_LOW() HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_RESET)
#define LCD_CS_HIGH() HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_SET)

/**
 * @brief  LCD Control pins
 */
#define LCD_CS_PIN GPIO_PIN_0  /* PH. 0*/
#define LCD_CS_GPIO_PORT GPIOH /* GPIOH */
#define LCD_CS_GPIO_CLK_ENABLE() __HAL_RCC_GPIOH_CLK_ENABLE()
#define LCD_CS_GPIO_CLK_DISABLE() __HAL_RCC_GPIOH_CLK_DISABLE()

/**
 * @brief  LCD Reset macro definition
 */
#define LCD_RST_LOW() HAL_GPIO_WritePin(LCD_RST_GPIO_PORT, LCD_RST_PIN, GPIO_PIN_RESET)
#define LCD_RST_HIGH() HAL_GPIO_WritePin(LCD_RST_GPIO_PORT, LCD_RST_PIN, GPIO_PIN_SET)

/**
 * @brief  LCD Reset pins
 */
#define LCD_RST_PIN GPIO_PIN_8  /* PC. 08*/
#define LCD_RST_GPIO_PORT GPIOC /* GPIOC */
#define LCD_RST_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define LCD_RST_GPIO_CLK_DISABLE() __HAL_RCC_GPIOC_CLK_DISABLE()

/**
 * @brief  LCD Data/Command macro definition
 */
#define LCD_DC_LOW() HAL_GPIO_WritePin(LCD_DC_GPIO_PORT, LCD_DC_PIN, GPIO_PIN_RESET)
#define LCD_DC_HIGH() HAL_GPIO_WritePin(LCD_DC_GPIO_PORT, LCD_DC_PIN, GPIO_PIN_SET)

/**
 * @brief
 */
#define LCD_DC_PIN GPIO_PIN_9  /* PC. 09*/
#define LCD_DC_GPIO_PORT GPIOC /* GPIOC */
#define LCD_DC_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define LCD_DC_GPIO_CLK_DISABLE() __HAL_RCC_GPIOC_CLK_DISABLE()

#define LCD_HANDLE &hspi1

static user_lcd_driver_status_t spi_transmit(uint8_t *data, uint16_t size)
{
    HAL_StatusTypeDef status = HAL_OK;

    status = HAL_SPI_Transmit(LCD_HANDLE, data, size, 1000);

    if (status != HAL_OK) {

#ifdef USER_LCD_DRIVER_LOG_ENABLE
        utils_print_err("spi transmit error. HAL_Status: %x", status);
#endif
        return user_lcd_driver_status_write_error;
    }

    return status;
}
static user_lcd_driver_status_t spi_receive(uint8_t* data, uint16_t size)
{
    HAL_StatusTypeDef status = HAL_OK;

    status = HAL_SPI_Receive(LCD_HANDLE, data, size, 1000);

    if (status != HAL_OK) {

#ifdef USER_LCD_DRIVER_LOG_ENABLE
        utils_print_err("spi receive error. HAL_Status: %x", status);
#endif
        return user_lcd_driver_status_read_error;
    }

    return status;
}
int32_t user_lcd_driver_init(void)
{
    // should be initialized by code generator. otherwise write your own code
    HAL_SPI_StateTypeDef status = HAL_SPI_STATE_READY;

    if (HAL_SPI_GetState(LCD_HANDLE) != HAL_SPI_STATE_READY) {
#ifdef USER_LCD_DRIVER_LOG_ENABLE
        utils_print_err("spi receive error. HAL_Status: %x", status);
#endif
        return user_lcd_driver_status_init_fail;
    }

    LCD_RST_LOW();
    HAL_Delay(1);
    LCD_RST_HIGH();

    return user_lcd_driver_status_ok;
}
int32_t user_lcd_driver_deinit(void)
{
    return user_lcd_driver_status_ok;
}
int32_t user_lcd_driver_transmit(uint16_t addr, uint8_t *data, uint16_t size)
{
    int32_t ret = user_lcd_driver_status_ok;
    if (size == 1)
    {
        /* Reset LCD control line CS */
        LCD_CS_LOW();
        LCD_DC_LOW();
        /* Send Data */
        if (spi_transmit(data, size) != user_lcd_driver_status_ok)
        {
            ret = user_lcd_driver_status_write_error;
        }
        /* Deselect : Chip Select high */
        LCD_CS_HIGH();
    }
    else
    {
        LCD_CS_LOW();
        LCD_DC_HIGH();
        /* Send Data */
        if (spi_transmit(data, size) != user_lcd_driver_status_ok)
        {
            ret = user_lcd_driver_status_write_error;
        }
        LCD_DC_LOW();
        /* Deselect : Chip Select high */
        LCD_CS_HIGH();
    }

    return ret;
}
int32_t user_lcd_driver_receive(uint16_t addr, uint8_t *data, uint16_t size)
{
    int32_t ret = user_lcd_driver_status_ok;

    if (user_lcd_driver_transmit(addr, data, size) != user_lcd_driver_status_ok) {
        ret = user_lcd_driver_status_write_error;
    }

    LCD_CS_LOW();

    if (ret == user_lcd_driver_status_ok) {
        if (spi_receive(data, size) != user_lcd_driver_status_ok) {
            ret = user_lcd_driver_status_read_error;
        }
    }

    LCD_CS_HIGH();

    return ret;
}
// uint32_t user_lcd_driver_transceive(uint8_t* tdata, uint8_t* rdata, uint16_t size)
// {
//     return user_lcd_driver_status_ok;
// }
int32_t user_lcd_driver_getTimestampMs(void)
{
    return HAL_GetTick();
}

user_lcd_driver_t user_lcd_driver = {
    .Init = user_lcd_driver_init,
    .DeInit = user_lcd_driver_deinit,
    .WriteReg = user_lcd_driver_transmit,
    .ReadReg = user_lcd_driver_receive,
    .GetTick = user_lcd_driver_getTimestampMs,
};
