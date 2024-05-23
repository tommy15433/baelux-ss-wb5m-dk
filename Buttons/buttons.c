#include "buttons.h"
#include <stdbool.h>

button_t m_buttons[BUTTONS_MAX];
button_id_t idx2IdMap[BUTTONS_MAX];
uint8_t mInitCount;

uint16_t id2idx(button_id_t id) {
    for (int i = 0; i < BUTTONS_MAX; i++) {
        if (idx2IdMap[i] == id){
            return i;
        }   
    }

    return ID_NOT_MAPPED;    
}
button_id_t idx2id(uint8_t idx) {
    if (idx > BUTTONS_MAX){
        return ID_NOT_MAPPED;
    }

    return idx2IdMap[idx];
}

buttons_result_e verifyId(button_id_t id){
    if (id == ID_NOT_MAPPED){
        return buttons_idNotValid;
    }else{
        for (int i = 0; i < BUTTONS_MAX; i++){
            if (idx2IdMap[i] == id){
                return buttons_idNotUnique;
            }
        }
    }

    return buttons_success;
}

void idx2IdMapper(button_id_t id, uint8_t idx){
    idx2IdMap[idx] = id;
}

//
// public
//
void buttons_init(button_invoke_timer_t* invokeTimer) {
    mInitCount = 0;
    for (int i = 0; i < BUTTONS_MAX; i++){
        idx2IdMap[i] = ID_NOT_MAPPED;
        button_init(&m_buttons[i]);
    }

    button_setInvokeTimer(invokeTimer);
}
void buttons_update(button_id_t id, uint32_t timestamp, button_status_e status)
{

    uint16_t idx = id2idx(id);
    if (idx == ID_NOT_MAPPED) {
        return;
    }
    
    // set timestamp to 0 to use driver to fetch timestamp
    // set status to button_status_unknown to use driver to read gpio status
    button_update(&m_buttons[idx], timestamp, status);
}
buttons_result_e buttons_registerButton(button_id_t id, button_driver_t* driver, button_eventHandler_t* handler){
    if (mInitCount > BUTTONS_MAX){
        return buttons_overflow;
    }
    if (verifyId(id) != buttons_success){
        // check unique || not available value
        return verifyId(id);
    }

    uint8_t curIdx = mInitCount++;

    button_setDriver(&m_buttons[curIdx], driver);
    button_registerEvent(&m_buttons[curIdx], handler);
    button_setId(&m_buttons[curIdx], id);

    idx2IdMapper(id, curIdx);

    return buttons_success;
}

void buttons_onInvokeTimerElapsed(void){
    button_onInvokeTimerElapsed();
}