#include <stdio.h>
#include "stm32_seq.h"
// hw_ts
#include "app_common.h"

#include "user.h"
#include "user_buttons_eventHandler.h"
#include "buttons.h"
#include "user_buttons.h"

#include "app_zigbee.h"
//
// button event handlers
// button events are invoked inside buttons.c automatically
// user may need to assign button drivers and call update whenever button interrupt occurs.
// those initialize sequence is called inside user.c
//

static void onButtonError(button_id_t id) {
  printf("error %d\r\n", id);

}
static void onButtonPress(button_id_t id) {
  printf("press %d\r\n", id);

//  APP_ZIGBEE_NwkStartJoin();
    UTIL_SEQ_SetTask(user_task_zigbee_nwk_join, 0);


  if (id == user_button1.id) {
//    UTIL_SEQ_SetTask(user_task_zigbee_nwk_join, 0);
  }
  else if (id == user_button2.id) {

//    UTIL_SEQ_SetTask(user_task_zigbee_nwk_join, 0);
  }
  // else if (id == user_button3.id){

  // } 
  else {

  }
}
static void onButtonRelease(button_id_t id) {
  printf("release %d\r\n", id);

  if (id == user_button1.id) {

  }
  else if (id == user_button2.id) {

  }
  // else if (id == user_button3.id){

  // } 
  else {

  }
}
static void onButtonShortPress(button_id_t id) {
  printf("shortpress %d\r\n", id);

  if (id == user_button1.id) {

  }
  else if (id == user_button2.id) {

  }
  // else if (id == user_button3.id){

  // } 
  else {

  }
}
static void onButtonLongPress(button_id_t id, long ms) {
  printf("long press %d %d [ms]\r\n", id, ms);

  if (id == user_button1.id) {

  }
  else if (id == user_button2.id) {

  }
  // else if (id == user_button3.id){

  // } 
  else {

  }
}

button_eventHandler_t user_button_eventHandler = {
		.button_onError = onButtonError,
		.button_onLongPressed = onButtonLongPress,
		.button_onPressed = onButtonPress,
		.button_onReleased = onButtonRelease,
		.button_onShortPressed = onButtonShortPress
};

// button_eventHandler_t user_button2_eventHandler = {
// 		.button_onError = onButtonError,
// 		.button_onLongPressed = onButtonLongPress,
// 		.button_onPressed = onButtonPress,
// 		.button_onReleased = onButtonRelease,
// 		.button_onShortPressed = onButtonShortPress
// };

// button_eventHandler_t user_button3_eventHandler = {
// 		.button_onError = onButtonError,
// 		.button_onLongPressed = onButtonLongPress,
// 		.button_onPressed = onButtonPress,
// 		.button_onReleased = onButtonRelease,
// 		.button_onShortPressed = onButtonShortPress
// };
