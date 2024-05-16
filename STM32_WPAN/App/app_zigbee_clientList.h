#include "stdint.h"
#include "stdbool.h"

/**
 *  user configuration
 * 
 * 
*/
#define APP_ZIGBEE_CLIENTLIST_MAXALLOC  100
/**
 * typedef
*/
typedef uint16_t app_zigbee_clientList_Address_t;

void app_zigbee_clientList_deleteAll(void);
void app_zigbee_clientList_delete(app_zigbee_clientList_Address_t addr);
bool app_zigbee_clientList_add(app_zigbee_clientList_Address_t addr);
bool app_zigbee_clientList_find(app_zigbee_clientList_Address_t addr);
void app_zigbee_clientList_debug();
app_zigbee_clientList_Address_t app_zigbee_clientList_next();
app_zigbee_clientList_Address_t app_zigbee_clientList_cur();
