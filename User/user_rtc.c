#include <stddef.h>

#include "user.h"

#ifdef USER_RTC_ENABLE

#include "user_rtc.h"
#include "PCF8563.h"

#define REG0    ((reg_format_t*)&reg_control_status_1)
#define REG1    ((reg_format_t*)&reg_control_status_2)
#define REG2    ((reg_format_t*)&reg_VL_seconds)
#define REG3    ((reg_format_t*)&reg_minutes)
#define REG4    ((reg_format_t*)&reg_hours)
#define REG5    ((reg_format_t*)&reg_days)
#define REG6    ((reg_format_t*)&reg_weekdays)
#define REG7    ((reg_format_t*)&reg_century_months)
#define REG8    ((reg_format_t*)&reg_years)
#define REG9    ((reg_format_t*)&reg_minute_alarm)
#define REGA    ((reg_format_t*)&reg_hour_alarm)
#define REGB    ((reg_format_t*)&reg_day_alarm)
#define REGC    ((reg_format_t*)&reg_weekday_alarm)
#define REGD    ((reg_format_t*)&reg_CLKOUT_control)
#define REGE    ((reg_format_t*)&reg_timer_control)
#define REGF    ((reg_format_t*)&reg_timer)


/**
 * 
*/
static user_rtc_handle_t* m_driver = NULL;
/**
 * 
*/
static uint8_t readRegister(reg_format_t* reg)
{
    bool ret = m_driver->i2c_read(PCF8563_I2C_ADDR7, reg->addr, &reg->reg.byte, 1);
    if (ret == false){
        return 0xff;
    }else{
        return reg->reg.byte;
    }
}
/**
 * 
*/
static bool writeRegister(reg_format_t* reg, uint8_t data)
{
    return m_driver->i2c_write(PCF8563_I2C_ADDR7, reg->addr, &reg->reg.byte, 1);
}
/**
 * 
*/
static void PCF8563_Init(void)
{
    writeRegister(REG0, 0x00);  //basic setting
    writeRegister(REG1, 0x00);  //Disable INT

    user_rtc_Timer_Disable();
    user_rtc_Alarm_Disable();
}
/**
 * 
*/
static bool user_rtc_verify(void)
{
    readRegister(REG0);

    return (reg_control_status_1.reg.byte == 0xff) ? false : true;
}
/**
 * 
*/
user_rtc_inttype_t ss_rtc_getInterruptType(void) {
    // read flag
}

/******************************************************************************
function: Set  Time
parameter:
    hour:0~23
    minute:0~60
    second:0~60
Info:
******************************************************************************/
/**
 * @brief set rtc time
 * @param hour 0~23
 * @param minute 0~60
 * @param second 0~60
*/
void user_rtc_Set_Time(int hour, int minute, int second)
{
    if(hour>=0){
        hour = changeIntToHex(hour%60);
        writeRegister(REG4, hour);
    }
    if(minute>=0){
        minute = changeIntToHex(minute%60);
        writeRegister(REG9, minute);
    }
    if(second>=0){
        second = changeIntToHex(second%60);
        writeRegister(REG2, second);
    }
}
/**
 * @brief
 * @param year 1990 ~ 2099
 * @param months 1 ~ 12
 * @param days 0 ~ 31
*/
void user_rtc_Set_Days(int year, int months, int days)
{
    if(days >= 0 && days <= 31){
        days = changeIntToHex(days);
        writeRegister(REG5, days);
    }
    if(months>=0 && months <= 12 ){
        months = changeIntToHex(months);
        writeRegister(REG7, (readRegister(REG7) & 0x80) |  months);
    }
    
    if(year>=1900 && year<2000){
        
        writeRegister(REG7, readRegister(REG7) | 0x80);
        /*7  century; this bit is toggled when the years register
            overflows from 99 to 00
            0 indicates the century is 20xx
            1 indicates the century is 19xx
        */
        writeRegister(REG8, year%100);

    }else if(year>= 2000 && year < 3000){
        writeRegister(REG7, readRegister(REG7) & 0x7F);
        writeRegister(REG8, year%100);
    }
}
/**
 * @param buf data buffer
*/
void user_rtc_Get_Days(uint8_t *buf)
{
    buf[0] = readRegister(REG5) & 0x3f;
    buf[1] = readRegister(REG7) & 0x1f;
    buf[2] = readRegister(REG8) & 0xff;
    
    buf[0] = changeHexToInt(buf[0]);
    buf[1] = changeHexToInt(buf[1]);
	
    if(readRegister(REG7) & 0x80){
        buf[3] = 19;
    }else{
        buf[3] = 20;
    }
}
/**
 * @param WeekData 0 ~ 7
*/
void user_rtc_Set_WeekData(int WeekData)
{
    if(WeekData <= 7){
        writeRegister(REG6, WeekData);
    }
}
/**
 * @param buf data buffer
*/
void user_rtc_Get_Time(uint8_t *buf)
{
    buf[0] = readRegister(REG2) & 0x7f; //get second data
    buf[1] = readRegister(REG3) & 0x7f; //get minute data
    buf[2] = readRegister(REG4) & 0x3f; //get hour data

    buf[0] = changeHexToInt(buf[0]);
    buf[1] = changeHexToInt(buf[1]);
    buf[2] = changeHexToInt(buf[2]);
}


/******************************************************************************
function: Set alarm parameters
parameter:
    hour: 0~23
    minute: 0~59
Info:
******************************************************************************/
void user_rtc_Set_Alarm(int hour, int minute)
{
    if(minute >= 0){
        minute = changeIntToHex(minute);
        writeRegister(REG9, minute);
    }
    
    if(hour >= 0){
        hour = changeIntToHex(hour); 
        writeRegister(REGA, hour);
    }
 
}
/**
 * @param
*/
void user_rtc_Alarm_Enable(void)
{
    writeRegister(REG1, readRegister(REG1) | 0x02);
    writeRegister(REG9, readRegister(REG9) & 0x7f);
    writeRegister(REGA, readRegister(REGA) & 0x7f);
    writeRegister(REGB, 0x80);//关闭
    writeRegister(REGC, 0x80);//关闭
}

/**
 * 
*/
void user_rtc_Alarm_Disable(void)
{
    writeRegister(REG1, readRegister(REG1) & 0xfd);
    writeRegister(REG9, readRegister(REG9) | 0x80);
    writeRegister(REGA, readRegister(REGA) | 0x80);
    writeRegister(REGB, 0x80);//关闭
    writeRegister(REGC, 0x80);//关闭
}
/**
 * @param Timer_Frequency user_rtc_timer_frequency_t 
 * @param Value total cycle = Value / Frequency(value of Timer_Frequency)
 * @note     
    REGE//0x0E
    TIMER_FREQUENCY_4096    0 // 4096HZ      MAX  0.062 second
    TIMER_FREQUENCY_64      1 // 64HZ        MAX  3.98 second
    TIMER_FREQUENCY_1       2 // 1HZ         MAX  255 second
    TIMER_FREQUENCY_1_60    3 // 1/60Hz      MAX  255 minute
*/
void user_rtc_Set_Timer(user_rtc_timer_frequency_t Timer_Frequency ,uint8_t Value)
{
    if (Timer_Frequency < user_rtc_timer_frequency_4096 || 
        Timer_Frequency > user_rtc_timer_frequency_1_60) {
            // error
            return;
        }
    // writeRegister(REGE, readRegister(REGE) & 0x7f);
    writeRegister(REGF, Value);
    writeRegister(REGE, ((readRegister(REGE)) & 0xfc) | Timer_Frequency);

    // original code
    // writeRegister(REGF, Value);
    // if(Timer_Frequency == 4096){
    //     writeRegister(REGE, ((readRegister(REGE)) & 0xfc) | TIMER_FREQUENCY_4096);
    // }else if(Timer_Frequency == 64){
    //     writeRegister(REGE, ((readRegister(REGE)) & 0xfc) | TIMER_FREQUENCY_64);
    // }else if(Timer_Frequency == 1){
    //     writeRegister(REGE, ((readRegister(REGE)) & 0xfc) | TIMER_FREQUENCY_1);
    // }else if(Timer_Frequency == 0){// 1/60
    //     writeRegister(REGE, ((readRegister(REGE)) & 0xfc) | TIMER_FREQUENCY_1_60);
    // }else{
    //     printf("Set Timer Error\r\n");
    // }
}
/**
 * @brief Enable timer
 * @param
*/
void user_rtc_Timer_Enable(void)
{
    writeRegister(REG1, readRegister(REG1) | 0x01);
    writeRegister(REGE, readRegister(REGE) | 0x80);
}

/**
 * @brief Disable timer
*/
void user_rtc_Timer_Disable(void)
{
    writeRegister(REG1, readRegister(REG1) & 0xfe);
    writeRegister(REGE, readRegister(REGE) & 0x7f);
}
/**
 * @brief Clear alarm interrupt flag
*/
void user_rtc_Cleare_AF_Flag(void)
{
    writeRegister(REG1, readRegister(REG1) & 0xf7);
}
/**
 * @brief Clear timer interrupt flag
*/
void user_rtc_Cleare_TF_Flag(void)
{
    writeRegister(REG1, readRegister(REG1) & 0xfB);
}
/**
 * @brief Get flag
 * @return 1: AF alarm 2: TF timer 3: AF and TF
*/
uint8_t user_rtc_Get_Flag(void)
{
    uint8_t temp = 0;
    if (readRegister(REG1) & 0x08) {
        temp = temp | 0x01;
    }
    if(readRegister(REG1) & 0x04){
        temp = temp | 0x02;
    }
    return temp;
    
}
/******************************************************************************
function: Set timer register
parameter:
    Timer_Frequency : Choose the corresponding frequency
                    32768   :327.68KHz
                    1024    :1024Hz
                    32      :32Hz
                    1       :1Hz
    Value           : Value
                    Total cycle = Value/TIMER_FREQUENCY
Info:

******************************************************************************/
/**
 * @brief Set timer register
 * @param Frequency user_rtc_clkout_frequency_t
*/
void user_rtc_CLKOUT_FrequencyHZ(user_rtc_clkout_frequency_t Frequency)
{
    if (Frequency < user_rtc_clkout_frequency_32768 ||
        Frequency > user_rtc_clkout_frequency_1)  {
        
        m_driver->print(__func__);
            return;
        }

    writeRegister(REGD, (readRegister(REGD) & 0xfC) | Frequency);

    // if(Frequency == 32768){
    //     writeRegister(REGD, (readRegister(REGD) & 0xfC) | 0x00);
    // }else if(Frequency == 1024){
    //     writeRegister(REGD, (readRegister(REGD) & 0xfC) | 0x01);
    // }else if(Frequency == 32){
    //     writeRegister(REGD, (readRegister(REGD) & 0xfC) | 0x02);
    // }else if(Frequency == 1){
    //     writeRegister(REGD, (readRegister(REGD) & 0xfC) | 0x03);
    // }else {
    //     printf("Set CLKOUT requency Selection Error\r\n");
    // }
}
/**
 * @brief Enable clkout
*/
void user_rtc_CLKOUT_Enable(void)
{
    writeRegister(REGD, readRegister(REGD) | 0x80);
}
/**
 * @brief Disable clkout
*/
void user_rtc_CLKOUT_Disable(void)
{
    writeRegister(REGD, readRegister(REGD) & 0x7f);
}

/**
 * @brief init rtc
*/
bool user_rtc_init(user_rtc_handle_t* driver){
    m_driver = driver;

    if (user_rtc_verify() == false){
        return false;
    }else {
        PCF8563_Init();
    }

    return true;
}

#endif
