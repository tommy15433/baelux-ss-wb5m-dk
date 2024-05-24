#pragma once

#include "PCF8563_registers.h"

#define PCF8563_I2C_ADDR7   0x51
#define PCF8563_I2C_ADDR8   0xA2

#define CTRL_BUF1   0x00
#define CTRL_BUF2   0x01

#define SECOND_DATA_BUF 0x02
#define MINUTE_DATA_BUF 0x03
#define HOUR_DATA_BUF   0x04

#define DAY_DATA_BUF    0x05
#define WEEK_DATA_BUF   0x06
#define MONTH_DATA_BUF  0x07
#define YEAR_DATA_BUF   0x08

#define MINUTE_AE_BUF   0x09
#define HOUR_AE_BUF     0x0A
#define DAY_AE_BUF      0x0B
#define WEEK_AE_BUF     0x0C

#define CLK_FRQ_BUF     0x0D
#define TIMER_CTRL_BUF  0x0E
#define COUNT_VAL_BUF   0x0F

#define changeIntToHex(dec)     ((((dec)/10) <<4) + ((dec)%10) )
#define converseIntToHex(dec)   ((((dec)>>4) *10) + ((dec)%16) )

#define changeHexToInt(hex)     ((((hex)>>4) *10 ) + ((hex)%16) )
#define converseHexToInt(hex)   ((((hex)/10) <<4 ) + ((hex)%10) )

#define TIMER_FREQUENCY_4096    0 // 4096HZ
#define TIMER_FREQUENCY_64      1 // 64HZ
#define TIMER_FREQUENCY_1       2 // 1HZ
#define TIMER_FREQUENCY_1_60    3 // 1/60Hz

#define Error_Month(dec)     (dec==4|dec==6|dec==9|dec==11)


extern struct reg_control_status_1_ reg_control_status_1;
extern struct reg_control_status_2_ reg_control_status_2;
extern struct reg_VL_seconds_       reg_VL_seconds;
extern struct reg_minutes_          reg_minutes;
extern struct reg_hours_            reg_hours;
extern struct reg_days_             reg_days;
extern struct reg_weekdays_         reg_weekdays;
extern struct reg_century_months    reg_century_months;
extern struct reg_years_            reg_years;
extern struct reg_minute_alarm_     reg_minute_alarm;
extern struct reg_hour_alarm_       reg_hour_alarm;
extern struct reg_day_alarm_        reg_day_alarm;
extern struct reg_weekday_alarm_    reg_weekday_alarm;
extern struct reg_CLKOUT_control_   reg_CLKOUT_control;
extern struct reg_timer_control_    reg_timer_control;
extern struct reg_timer_            reg_timer;

extern reg_format_t* reg_map[0x10];

