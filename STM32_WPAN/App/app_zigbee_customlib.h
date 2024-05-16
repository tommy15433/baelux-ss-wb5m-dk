#ifndef __APP_ZIGBEE_CUSTOMLIB_H_
#define __APP_ZIGBEE_CUSTOMLIB_H_

// need app_common.h and dbg_trace.h to use stm_logging
#include "app_common.h"
#include "dbg_trace.h"
#include "stm_logging.h"
#include "zcl/zcl.h"

#define ADDR_MAX_SIZE	30

typedef struct {
	uint64_t ieee_addr;
	uint16_t nwk_addr;
}addr_map_t;

typedef struct {
	addr_map_t maps[ADDR_MAX_SIZE];
	int count;
}addr_maps_t;

extern addr_maps_t azcl_addr_maps;

void azcl_init(struct ZigBeeT *_zb);

// broadcast permit join request to make routers to join the network
void azcl_broadcastPermitJoinRequest(uint8_t duration);
void azcl_broadcaseLeaveRequest();

void azcl_setShortAddress(uint16_t addr);
uint64_t azcl_getAddress();
uint16_t azcl_getShortAddress();

uint16_t azcl_getParentAddr();

void azcl_announce();

void azcl_registerZdoMsg();
void azcl_restore_nwk_addr(void);
void azcl_setBroadcastTimeout(uint32_t seconds);

void azcl_test();

void azcl_addGroup(uint8_t endpoint, uint16_t groupAddress);
void azcl_removeGroup(uint8_t endpoint, uint16_t groupAddress);
void azcl_removeAllGroup(uint8_t endpoint);


#endif
