#pragma once

#include "stdbool.h"
#include "stdint.h"

#define unrecognizable_timeout  1000 * 60 * 10  // 10 minutes
#define APP_ZIGBEE_SERVERLIST_MAXALLOC  100
/**
 * typedef
*/
typedef uint16_t serverlist_addr;
typedef uint32_t serverlist_time;    
typedef struct {
   serverlist_addr addr;
   serverlist_time lastUpdatedTime;

} serverlist_t;

// void app_zigbee_serverList_deleteAll(void);
// void app_zigbee_serverList_delete(serverlist_t addr);
// bool app_zigbee_serverList_add(serverlist_t addr);
// bool app_zigbee_serverList_find(serverlist_t addr);
// void app_zigbee_serverList_debug();

/**
 * @brief call this function whenever addresses are announced
 * @param addr announced address value
 * @param timestamp timestamp in millis of the announcement
 * @return true if there are no timeout devices. if false is returned, call app_zigbee_serverList_getTimeouts to get those addresses
 * 
*/
bool app_zigbee_serverList_update(serverlist_addr addr, serverlist_time timestamp);

/**
 * @brief parse timed out addresses. After calling this function, call app_zigbee_serverList_clearTimeouts() to stop traking those addresses
 * @param addresses pointer that contains address values
 * @return size of addresses. if 0 there are no timed out addresses
*/
uint16_t app_zigbee_serverList_getTimeouts(serverlist_addr* addresses);

void app_zigbee_serverList_clearTimeouts(void);

serverlist_addr app_zigbee_serverList_next();
serverlist_addr app_zigbee_serverList_cur();
