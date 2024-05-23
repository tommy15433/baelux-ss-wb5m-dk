#include "user_buttons.h"

// GPIOD0 mapped to button1
#define BUTTON1_GPIO GPIOC
#define BUTTON1_PIN 12
#define BUTTON2_GPIO GPIOC
#define BUTTON2_PIN 13
#define BUTTON3_GPIO GPIOC
#define BUTTON3_PIN 4

// button id should be same as pin mask
// since gpio exti callback's parameter is called by it's pin mask
// and button.c handles the event by button's id
button_info user_button1 = {
    .gpio = BUTTON1_GPIO,
    .id = 1 << BUTTON1_PIN,
    .pin = BUTTON1_PIN,
    .pinMask = 1 << BUTTON1_PIN,
    .pressedLevel = 0};

button_info user_button2 = {
    .gpio = BUTTON2_GPIO,
    .id = 1 << BUTTON2_PIN,
    .pin = BUTTON2_PIN,
    .pinMask = 1 << BUTTON2_PIN,
    .pressedLevel = 0};

button_info* user_button_find(uint16_t pin)
{
    if (pin == 1 << BUTTON1_PIN) {
        return &user_button1;
    } else if (pin == 1 << BUTTON2_PIN) {
        return &user_button2;
    } else {
        return &user_button1;
    }
}
// button_info user_button3 = {
//     .gpio = BUTTON3_GPIO,
//     .id = 1 << BUTTON3_PIN,
//     .pin = BUTTON3_PIN,
//     .pinMask = 1 << BUTTON3_PIN,
//     .pressedLevel = 0};
