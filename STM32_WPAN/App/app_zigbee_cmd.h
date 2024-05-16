#pragma once

#include "app_zigbee.h"

void APP_ZIGBEE_Indicate_s2s(uint16_t addr);
void APP_ZIGBEE_Indicate_c2s(void);
void APP_ZIGBEE_Leave_c2s(void);
void APP_ZIGBEE_Leave_s2s(uint16_t addr);
void APP_ZIGBEE_Broadcast2byte_c2s(uint8_t b1, uint8_t b2);