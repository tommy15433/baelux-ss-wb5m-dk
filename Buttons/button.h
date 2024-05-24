#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define ID_NOT_MAPPED 0xffff

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t button_id_t;

typedef enum {
    button_status_pressed = 0,
    button_status_released,
    button_status_unknown,
}button_status_e;

typedef enum {
    button_event_none,
    button_event_pressed,
    button_event_released,
    button_event_short,
    button_event_short_multi,
    button_event_long,
    button_event_error
}button_event_e;

/**
 * @brief driver function for reading gpio level of a button
 * @return status_e (0 if pressed, 1 if released)
*/
typedef button_status_e (*button_readState)(void);
/**
 * @brief driver function for getting system millis
 * @return current timestamp in millis
*/
typedef uint32_t (*button_getTimestamp)(void);
/**
 * @brief callback function of triggered event_e
 * @return event_e
*/
typedef button_event_e (*button_eventCb)(void);

// button event callbacks
typedef void (*button_onPressed)(button_id_t id);
typedef void (*button_onReleased)(button_id_t id);
typedef void (*button_onShortPressed)(button_id_t id);
typedef void (*button_onShortMultiPressed)(button_id_t id, int cnt);
typedef void (*button_onLongPressed)(button_id_t id, long pressedTimeMs);
typedef void (*button_onError)(button_id_t id);

typedef struct {
    button_onPressed button_onPressed;
    button_onReleased button_onReleased;
    button_onShortPressed button_onShortPressed;
    button_onShortMultiPressed button_onShortMultiPressed;
    button_onLongPressed button_onLongPressed;
    button_onError button_onError;
}button_eventHandler_t;

typedef struct {
    button_getTimestamp button_getTimeStamp;
    button_readState button_getButtonState;
}button_driver_t;

typedef struct 
{
    void (*button_invoke_timer_start)(void);
    void (*button_invoke_timer_stop)(void);
    void (*button_invoke_timer_restart)(void);
} button_invoke_timer_t;

typedef struct button{
    button_id_t id;
    
    button_status_e m_statePre;

    long m_shortPressTimeout;
    long m_errorTimeout;
    
    long m_initialPressedTime;

    long m_pressed_elapsed_ms;

    button_driver_t* m_driver;
    button_eventHandler_t* m_events;

    // internal use
    int cnt;
}button_t;

void button_init(button_t* handle);
void button_setInvokeTimer(button_invoke_timer_t* invokeTimer);
void button_setDriver(button_t* button, button_driver_t* driver);
void button_registerEvent(button_t* button, button_eventHandler_t* eventCb);

/**
 * @brief Set short press timeout value.
 * Releasing button after this timeout will considered as long press event.
 * Default 500ms
 * @param ms timeout value in millis
*/
void button_setShortPressTimeout(button_t* handle, long ms);
void button_setId(button_t* handle, button_id_t id);
uint8_t button_getId(button_t* handle);
void button_update(button_t* handle, uint32_t timestamp, button_status_e status);

void button_onInvokeTimerElapsed(void);

#ifdef __cplusplus
}
#endif
        
    
