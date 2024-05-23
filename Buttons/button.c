#include "button.h"


button_invoke_timer_t* m_invokeTimer = NULL;
bool m_isTimerUsed = false;
static button_t* m_lastShortKeyhandle = NULL;
// static
static void invokeEvent(button_t* handle, button_event_e event, long elapsedMs){
    switch (event) {
        default:
        case button_event_none:

        break;

        case button_event_pressed:
            if (handle->m_events->button_onPressed != NULL){
                handle->m_events->button_onPressed(handle->id);
            }
        break;

        case button_event_released:
            if (handle->m_events->button_onReleased != NULL){
                handle->m_events->button_onReleased(handle->id);
            }
        break;

        case button_event_short:
            if (handle->m_events->button_onShortPressed != NULL){
                handle->m_events->button_onShortPressed(handle->id);
            }

        break;

        case button_event_short_multi:
            if (handle->m_events->button_onShortMultiPressed != NULL) {
                handle->m_events->button_onShortMultiPressed(handle->id, handle->cnt);
            }
        break;
        case button_event_long:
            if (handle->m_events->button_onLongPressed != NULL){
                handle->m_events->button_onLongPressed(handle->id, elapsedMs);
            }
        break;

        case button_event_error:
            if(handle->m_events->button_onError != NULL){
                handle->m_events->button_onError(handle->id);
            }
        break;
    }
}

static void reset(button_t* handle){
    handle->m_statePre = button_status_released;
    handle->m_pressed_elapsed_ms = 0;
    handle->m_initialPressedTime = 0;
    handle->cnt = 0;
}


// public
void button_init(button_t* handle){
    handle->id = ID_NOT_MAPPED;
    handle->m_errorTimeout = 10000;
    handle->m_initialPressedTime = 0;
    handle->m_pressed_elapsed_ms = 0;
    handle->m_shortPressTimeout = 500;
    handle->m_statePre = button_status_released;
}

void button_setInvokeTimer(button_invoke_timer_t* invokeTimer)
{
    if (invokeTimer != NULL)
    {
        m_isTimerUsed = true;
        m_invokeTimer = invokeTimer;
    }
}

void button_setDriver(button_t* handle, button_driver_t* driver){
    handle->m_driver = driver;
}
void button_registerEvent(button_t* handle, button_eventHandler_t* eventHandler){
    handle->m_events = eventHandler;
}

void button_setId(button_t* handle, button_id_t id){
    handle->id = id;
}
void button_update(button_t* handle, uint32_t timestamp, button_status_e status){

   button_status_e curState;
   long curTimestamp = 0; 
    if (timestamp == 0) {
        // use driver
        curTimestamp = handle->m_driver->button_getTimeStamp();
    } else {
        curTimestamp = timestamp;
    }

    if (status == button_status_unknown) {
        // use driver
        curState = handle->m_driver->button_getButtonState();
    } else {
        curState = status;
    }

    if (m_isTimerUsed) {
        m_invokeTimer->button_invoke_timer_stop();
    }

    if (curState == button_status_pressed){
        // released -> pressed
        if (handle->m_statePre == button_status_released){
            // initial pressed
            handle->m_initialPressedTime = curTimestamp;
            handle->m_pressed_elapsed_ms = 0;

            handle->m_statePre = curState;

            handle->cnt++;
            invokeEvent(handle, button_event_pressed, 0);
        }
        // pressed -> pressed
        else{
            // accumulate 
            handle->m_pressed_elapsed_ms = curTimestamp - handle->m_initialPressedTime;

            // nothing
            if (handle->m_pressed_elapsed_ms > handle->m_errorTimeout){
                invokeEvent(handle, button_event_error, 0);
                reset(handle);
            }
        }

    }else{
        // pressed -> released occurrence
    	handle->m_pressed_elapsed_ms = curTimestamp - handle->m_initialPressedTime;

        if (handle->m_statePre == button_status_pressed){
            // invoke event
            if (handle->m_pressed_elapsed_ms < handle->m_shortPressTimeout){
                if (m_isTimerUsed) {
                    m_lastShortKeyhandle = handle;
                    m_invokeTimer->button_invoke_timer_restart();
                } else {
                    invokeEvent(handle, button_event_short, handle->m_pressed_elapsed_ms);
                }
            }else{
                invokeEvent(handle, button_event_long, handle->m_pressed_elapsed_ms);
                handle->cnt = 0;
            }

            handle->m_statePre = curState;

            invokeEvent(handle, button_event_released, 0);
        }
        // released -> released
        else{
            
        }
    }

}
uint8_t button_getId(button_t* handle){
    return handle->id;
}
void button_setShortPressTimeout(button_t* handle, long ms){
    handle->m_shortPressTimeout = ms;
}

void button_onInvokeTimerElapsed(void)
{
    // currently applied only for short multiple keys
    if (m_lastShortKeyhandle != NULL) {
        invokeEvent(m_lastShortKeyhandle, button_event_short_multi, m_lastShortKeyhandle->m_pressed_elapsed_ms);
        m_lastShortKeyhandle->cnt = 0;
    }
}