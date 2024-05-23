#include <stdint.h>
#include "user_hw_timer.h"

user_hw_timer_t user_hw_timer_btnEvent = {
    .userid = user_hw_timer_id_btnEvent,
    .timerid = 0,
    .mode = hw_ts_SingleShot,
    .onTimerElapsedCallback = NULL,
    .elapse_ms = 0,
};


void user_hw_timer_btnEvent_start()
{
    HW_TS_Start(user_hw_timer_btnEvent.timerid, user_hw_timer_btnEvent.elapse_ms);
}
void user_hw_timer_btnEvent_restart()
{
    printf("timer restart\r\n");
    // HW_TS_Stop(user_hw_timer_btnEvent.timerid);
    user_hw_timer_btnEvent_start();

}
void user_hw_timer_btnEvent_stop()
{
    HW_TS_Stop(user_hw_timer_btnEvent.timerid);
}