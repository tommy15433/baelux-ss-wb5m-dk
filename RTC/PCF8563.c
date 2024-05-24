#include "PCF8563.h"

/**
 * Register definitions 
 * reg.byte are set to random value. On initialization, read through all registers and update it
 * Bit values written below are MUST BE 0 value on either register write or read
 * 
 * reg_control_status_1.reg
 * - .rsv0, .rsv1, .rsv2
 * reg_control_status_2.reg
 * - .rsv0
 * 
*/

struct reg_control_status_1_ reg_control_status_1 = 
{ 
	.addr = 0x00, 
	.reg.byte = 0b00000000 
};
struct reg_control_status_2_ reg_control_status_2 = 
{ 
	.addr = 0x01,
	.reg.byte = 0b00000000 
};
struct reg_VL_seconds_ reg_VL_seconds = 
{ 
	.addr = 0x02,
	.reg.byte = 0b00000000 
};
struct reg_minutes_ reg_minutes = 
{ 
	.addr = 0x03,
	.reg.byte = 0b00000000 
};
struct reg_hours_ reg_hours = 
{ 
	.addr = 0x04,
	.reg.byte = 0b00000000 
};
struct reg_days_ reg_days = 
{ 
	.addr = 0x05,
	.reg.byte = 0b00000000 
};
struct reg_weekdays_ reg_weekdays = 
{ 
	.addr = 0x06,
	.reg.byte = 0b00000000 
};
struct reg_century_months reg_century_months = 
{ 
	.addr = 0x07,
	.reg.byte = 0b00000000 
};
struct reg_years_ reg_years = 
{ 
	.addr = 0x08,
	.reg.byte = 0b00000000 
};
struct reg_minute_alarm_ reg_minute_alarm = 
{ 
	.addr = 0x09,
	.reg.byte = 0b00000000 
};
struct reg_hour_alarm_ reg_hour_alarm = 
{ 
	.addr = 0x0A,
	.reg.byte = 0b00000000 
};
struct reg_day_alarm_ reg_day_alarm = 
{ 
	.addr = 0x0B,
	.reg.byte = 0b00000000 
};
struct reg_weekday_alarm_ reg_weekday_alarm = 
{ 
	.addr = 0x0C,
	.reg.byte = 0b00000000 
};
struct reg_CLKOUT_control_ reg_CLKOUT_control = 
{ 
	.addr = 0x0D,
	.reg.byte = 0b00000000 
};
struct reg_timer_control_ reg_timer_control = 
{ 
	.addr = 0x0E,
	.reg.byte = 0b00000000 
};
struct reg_timer_ reg_timer = {
    .addr = 0x0F,
    .reg.byte = 0b00000000
};

reg_format_t* reg_map[0x10] = {
    (reg_format_t*)&reg_control_status_1,
    (reg_format_t*)&reg_control_status_2,
    (reg_format_t*)&reg_VL_seconds,
    (reg_format_t*)&reg_minutes,
    (reg_format_t*)&reg_hours,
    (reg_format_t*)&reg_days,
    (reg_format_t*)&reg_weekdays,
    (reg_format_t*)&reg_century_months,
    (reg_format_t*)&reg_years,
    (reg_format_t*)&reg_minute_alarm,
    (reg_format_t*)&reg_hour_alarm,
    (reg_format_t*)&reg_day_alarm,
    (reg_format_t*)&reg_weekday_alarm,
    (reg_format_t*)&reg_CLKOUT_control,
    (reg_format_t*)&reg_timer_control,
    (reg_format_t*)&reg_timer
};
