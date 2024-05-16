#pragma once

#include <stdint.h>
#include <stdbool.h>

#define USER_RTC_PRINT_ERR true

typedef enum {
    user_rtc_inttype_timer,
    user_rtc_inttype_counter,
} user_rtc_inttype_t;

typedef enum {
    user_rtc_timer_frequency_4096 = 0,  // 4096 Hz
    user_rtc_timer_frequency_64 = 1,    // 64 Hz
    user_rtc_timer_frequency_1 = 2,     // 1 Hz
    user_rtc_timer_frequency_1_60 = 3   // 1/60 Hz
} user_rtc_timer_frequency_t;

typedef enum {
    user_rtc_clkout_frequency_32768 = 0,
    user_rtc_clkout_frequency_1024 = 1,
    user_rtc_clkout_frequency_32 = 2,
    user_rtc_clkout_frequency_1 = 3
} user_rtc_clkout_frequency_t;

typedef struct {

    int (*i2c_read)(uint8_t sla, uint8_t addr, uint8_t* data, uint8_t data_size);
    int (*i2c_write)(uint8_t sla, uint8_t addr, uint8_t* data, uint8_t data_size);
    void (*print)(const char*);
} user_rtc_handle_t;

bool user_rtc_init(user_rtc_handle_t* handle);

// user should call this callback on interrupt handler
// or user can clear the flags manually
void user_rtc_onTimerCallback(void);
void user_rtc_onCounterCallback(void);

void user_rtc_onI2cReadRequest(uint8_t* data, uint8_t size);
void user_rtc_onI2cWriteRequest(uint8_t* data, uint8_t size);

void user_rtc_Set_Time(int hour, int minute, int second);
void user_rtc_Get_Time(uint8_t *buf);

void user_rtc_Set_Days(int year, int months, int days);
void user_rtc_Get_Days(uint8_t *buf);

void user_rtc_Set_WeekData(int WeekData);

void user_rtc_Set_Alarm(int hour, int minute);
void user_rtc_Set_Timer(user_rtc_timer_frequency_t Timer_frequency ,uint8_t value);

void user_rtc_Alarm_Enable(void);
void user_rtc_Alarm_Disable(void);
void user_rtc_Timer_Enable(void);
void user_rtc_Timer_Disable(void);

void user_rtc_Cleare_AF_Flag(void);//Alarm
void user_rtc_Cleare_TF_Flag(void);//Timer

uint8_t user_rtc_Get_Flag(void);

void user_rtc_CLKOUT_FrequencyHZ(user_rtc_clkout_frequency_t Frequency);

void user_rtc_CLKOUT_Disable(void);
void user_rtc_CLKOUT_Enable(void);