#pragma once
#pragma pack(1)

#include <stdint.h>
#include <stdbool.h>

#define changeIntToHex(dec)     ((((dec)/10) <<4) + ((dec)%10) )
#define converseIntToHex(dec)   ((((dec)>>4) *10) + ((dec)%16) )

#define changeHexToInt(hex)     ((((hex)>>4) *10 ) + ((hex)%16) )
#define converseHexToInt(hex)   ((((hex)/10) <<4 ) + ((hex)%10) )

#define TIMER_FREQUENCY_4096    0 // 4096HZ
#define TIMER_FREQUENCY_64      1 // 64HZ
#define TIMER_FREQUENCY_1       2 // 1HZ
#define TIMER_FREQUENCY_1_60    3 // 1/60Hz

#define Error_Month(dec)     (dec==4|dec==6|dec==9|dec==11)

struct reg_control_status_1_ {
    uint8_t addr;
    union {
        uint8_t byte;
        struct {
            uint8_t rsv0 : 3;
            uint8_t TESTC : 1;
            uint8_t rsv1 : 1;
            uint8_t STOP : 1;
            uint8_t rsv2 : 1;
            uint8_t TEST1 : 1;
        };
    } reg;
};

struct reg_control_status_2_ {
    uint8_t addr;
    union {
        uint8_t byte;
        struct {
            uint8_t TIE : 1;
            uint8_t AIE : 1;
            uint8_t TF : 1;
            uint8_t AF : 1;
            uint8_t TI_TP : 1;
            uint8_t rsv0 : 3;
        };
    } reg;
};

struct reg_VL_seconds_ {
    uint8_t addr;
    union {
        uint8_t byte;
        struct {
            uint8_t seconds_bcd : 7;
            uint8_t VL : 1;
        };
    } reg;
    
};

struct reg_minutes_ {
    uint8_t addr;
    union {
        uint8_t byte;
        struct {
            uint8_t minutes_bcd : 7;
            uint8_t rsv0 : 1;
        };
    } reg;
};

struct reg_hours_ {
    uint8_t addr;
    union {
        uint8_t byte;
        struct {
            uint8_t hours_bcd : 6;
            uint8_t rsv0 : 2;
        };
    } reg;
};

struct reg_days_ {
    uint8_t addr;
    union {
        uint8_t byte;
        struct {
            uint8_t days_bcd : 6;
            uint8_t rsv0 : 2;
        };
    } reg;
};

struct reg_weekdays_ {
    uint8_t addr;
    union {
        uint8_t byte;
        struct {
            uint8_t weekdays_bcd : 2;
            uint8_t rsv0 : 6;
        };
    } reg;
};

struct reg_century_months {
    uint8_t addr;
    union {
        uint8_t byte;
        struct {
            uint8_t months_bcd : 3;
            uint8_t rsv0 : 5;
        };
    } reg;
};

struct reg_years_ {
    uint8_t addr;
    union { 
        uint8_t byte;
        struct {
            uint8_t years_bcd : 8;
        };
    } reg;
};

struct reg_minute_alarm_ {
    uint8_t addr;
    union {
        uint8_t byte;
        struct {
            uint8_t minute_alarm_bcd : 7;
            uint8_t AE : 1;
        };
    } reg;
};

struct reg_hour_alarm_ {
    uint8_t addr;
    union {
        uint8_t byte;
        struct {
            uint8_t hour_alarm_bcd : 6;
            uint8_t rsv : 1;
            uint8_t AE : 1;
        };

    } reg;
};

struct reg_day_alarm_ {
    uint8_t addr;
    union {
        uint8_t byte;
        struct {
            uint8_t day_alarm_bcd : 6;
            uint8_t rsv : 1;
            uint8_t AE : 1;
        };
    } reg;
    
};

struct reg_weekday_alarm_  {
    uint8_t addr;
    union {
        uint8_t byte;
        struct {
            uint8_t weekday_amarm_bcd : 3;
            uint8_t rsv : 4;
            uint8_t AE : 1;
        };
    } reg;
};

struct reg_CLKOUT_control_ {
    uint8_t addr;
    union {
        uint8_t byte;
        struct {
            uint8_t FD0 : 1;
            uint8_t FD1 : 1;
            uint8_t rsv : 5;
            uint8_t FE : 1;
        };
    } reg;
};

struct reg_timer_control_ {
    uint8_t addr;
    union {
        uint8_t byte;
        struct {
            uint8_t TD0 : 1;
            uint8_t TD1 : 1;
            uint8_t rsv : 5;
            uint8_t TE : 1;
        };
    } reg;
};

struct reg_timer_ {
    uint8_t addr;
    union  {
        uint8_t byte;
    } reg;
};

typedef struct {
    uint8_t addr;
    union {
        uint8_t byte;
        struct {
            uint8_t bit0 : 1;
            uint8_t bit1 : 1;
            uint8_t bit2 : 1;
            uint8_t bit3 : 1;
            uint8_t bit4 : 1;
            uint8_t bit5 : 1;
            uint8_t bit6 : 1;
            uint8_t bit7 : 1;
        };
    } reg;
} reg_format_t;
