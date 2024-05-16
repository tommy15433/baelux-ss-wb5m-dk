#pragma once

#include "stm32wb5mxx.h"
#include "buttons.h"

typedef struct {
    button_id_t id;
    
    GPIO_TypeDef* gpio;

    uint16_t pin;
    uint16_t pinMask;

    uint8_t pressedLevel;
} button_info;

extern button_info user_button1;
extern button_info user_button2;
// extern button_info user_button3;
