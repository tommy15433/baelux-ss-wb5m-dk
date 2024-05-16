#ifndef __APP_ZIGBEE_BUTTONTASKS_H_
#define __APP_ZIGBEE_BUTTONTASKS_H_

#include "app_common.h"
#include "app_entry.h"
#include "dbg_trace.h"
#include "app_zigbee.h"
#include "zigbee_interface.h"
#include "shci.h"
#include "stm_logging.h"
#include "app_conf.h"
#include "stm32wbxx_core_interface_def.h"
#include "zigbee_types.h"
#include "stm32_seq.h"


#include <assert.h>
#include "zcl/zcl.h"


#include "ee.h"
#include "hw_flash.h"
#include "zcl/general/zcl.onoff.h"


void onIndicateNextPair();
void onKickoutCurrentPair();

void app_zigbee_button1task(void);
void app_zigbee_button2task(void);
void app_zigbee_button3task(void);

#endif
