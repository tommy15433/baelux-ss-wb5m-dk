#include "app_zigbee_customlib.h"
#include "app_zigbee.h"
#include "app_zigbee_serverList.h"

static struct ZigBeeT *zb;
addr_maps_t azcl_addr_maps = {0};


//static void permitJoinReqCb(struct ZbZdoNwkUpdateNotifyT *reqPtr, void *cb_arg){
////	  APP_ZIGBEE_Commissioning_Client_SetNwkCfg_cmd(nwk_config.currentConfig, &nwk_config.config[nwk_config.currentConfig]);
//	APP_DBG("permitJoinReqCb\r\n");
//}

static void permitJoinReqCb(struct ZbZdoPermitJoinRspT *rsp, void *cb_arg) {
	APP_DBG("permitJoinReqCb\r\n");
}
void azcl_init(struct ZigBeeT *_zb){
	zb = _zb;
}


void azcl_announce()
{
	struct ZbZdoDeviceAnnceT t = {
		.capability = 1,
		.extAddr = azcl_getAddress(),	// ext address (which seems like the network address)
		.nwkAddr = ZbShortAddress(zb)	// should be my address
	};
	ZbZdoDeviceAnnce(zb, &t);
}
/*
 * duration: [0]: disable, otherwise [second] value
 */
void azcl_broadcastPermitJoinRequest(uint8_t duration){
	struct ZbZdoPermitJoinReqT t;

	// set destination address 0xffff (or 0xfffc) to broadcast
	t.destAddr = 0xfffc;

	// max duration
	t.duration = duration;

	enum ZbStatusCodeT status = ZbZdoPermitJoinReq(zb, &t, permitJoinReqCb, NULL);
	if (status != ZB_STATUS_SUCCESS){
		APP_DBG("Permit join request failed %d\r\n", status);
	}
}

void azcl_broadcaseLeaveRequest(){
	struct ZbZdoLeaveReqT req;

	// Mgmt_Leave_req data structure
	// [64bit] 	| [7:2] [1] 			[0]
	// dev addr | rsv   remove children rejoin

	req.destAddr = 0xfffc;
//	req.deviceAddr = 0x0080e1250007e132;
	req.deviceAddr = azcl_getAddress();
	req.flags = 0b00000000;

	printf("leave reuqest %lld\r\n", req.deviceAddr);
	// callback
	// void (*callback)(struct ZbZdoLeaveRspT *rsp, void *cb_arg)
	enum ZbStatusCodeT status = ZbZdoLeaveReq(zb, &req, NULL, NULL);
	if (status != ZB_STATUS_SUCCESS){ 
		// do something
	}
}
void azcl_setShortAddress(uint16_t addr)
{
    uint16_t nwkAddr = addr;

    (void)ZbNwkSet(zb, ZB_NWK_NIB_ID_NetworkAddress, &nwkAddr, sizeof(nwkAddr));
}
uint16_t azcl_getShortAddress()
{
	return ZbShortAddress(zb);
}
uint64_t azcl_getAddress() {
	// ZbChangeExtAddr(zb, 0x1234);
	return ZbExtendedAddress(zb);
	// return ZbShortAddress(zb);
}

int zdoMsgCallback(struct ZigBeeT *zb, struct ZbZdoDeviceAnnceT *annce, uint8_t seqno, void *arg){

	static int cnt = 0;

	APP_DBG("[%d], 0x%2x", cnt++, annce->nwkAddr);

	app_zigbee_serverList_update(annce->nwkAddr, HAL_GetTick());

	return 0;
}

void azcl_registerZdoMsg() {

	ZbZdoDeviceAnnceFilterRegister(zb, zdoMsgCallback, NULL);
}

uint16_t azcl_getParentAddr() 
{
	struct ZbNwkNeighborT table = {0};
	ZbNwkGet(zb, ZB_NWK_NIB_ID_NeighborTable, &table, sizeof(table));

	return table.nwkAddr;
}

void azcl_test() {
	// for (int i = 0; i < azcl_addr_maps.count; i++){
	// 	APP_DBG("addr found: %.4x", azcl_addr_maps.maps[i]);
	// }
	// struct ZbZdoDeviceAnnceT t = {
	// 	.capability = 1,
	// 	.extAddr = azcl_getAddress(),
	// 	.nwkAddr = ZbShortAddress(zb)
	// };
	// ZbZdoDeviceAnnce(zb, &t);

	// // send only 2bytes to parent test
	// uint8_t msg[2] = {'a', 'b'};
	// APP_ZIGBEE_Send2bytes(msg, m_parent, ZB_APSDE_ADDRMODE_SHORT);
	// // ZbNwkGetParentShortAddr is undefined in zigbee core STM
	// // uint16_t pa = ZbNwkGetParentShortAddr(zb);


	// uint16_t pa = ZbNwkAddrLookupNwk(zb, azcl_getAddress());
	// APP_DBG("parent: %x", pa);


	struct ZbNwkNeighborT table = {0};
	ZbNwkGet(zb, ZB_NWK_NIB_ID_NeighborTable, &table, sizeof(table));
	printf("%x %llx %x\r\n", table.nwkAddr, table.extAddr, table.relationship);
}

void azcl_restore_nwk_addr(void) {

	uint32_t len = cache_persistent_data.U32_data[0] + 4 ;

	APP_DBG("Restoring network addresses");

	bool is_fe = false;
	int found = 0;

	// reset
	azcl_addr_maps.count = 0;

	// find 0xfeff, for parsing network addresses
	for (int i = 0; i < len; i++){

		if (is_fe == false){
			if (cache_persistent_data.U8_data[i] == 0xfe){
				is_fe = true;
			}else{
				is_fe = false;
			}
		}else{
			if (cache_persistent_data.U8_data[i] == 0xff){
				// previous 2nd and 3rb bytes are the shorten nwk addr
				if (i > 3){
					found = (cache_persistent_data.U8_data[i-2] << 8) |
							(cache_persistent_data.U8_data[i-3] << 0);

					APP_DBG("nwk_addr: %.4x", found);
					azcl_addr_maps.maps[azcl_addr_maps.count++].nwk_addr = found;
				}
			}
			is_fe = false;
		}
	}

}

void azcl_setBroadcastTimeout(uint32_t seconds){
	uint32_t bcast_timeout = seconds;
	ZbNwkSet(zb, ZB_NWK_NIB_ID_NetworkBroadcastDeliveryTime, &bcast_timeout, sizeof(bcast_timeout));
}

void azcl_addGroup(uint8_t endpoint, uint16_t groupAddress) {

	struct ZbApsmeAddGroupReqT req;
	struct ZbApsmeAddGroupConfT conf;

	memset(&req, 0, sizeof(req));
	req.endpt = endpoint;
	req.groupAddr = groupAddress;
	ZbApsmeAddGroupReq(zb, &req, &conf);
}

void azcl_removeGroup(uint8_t endpoint, uint16_t groupAddress) {
	
	struct ZbApsmeRemoveGroupReqT req;
	struct ZbApsmeRemoveGroupConfT conf;

	memset(&req, 0, sizeof(req));	
	req.endpt = endpoint;
	req.groupAddr = groupAddress;

	ZbApsmeRemoveGroupReq(zb, &req, &conf);
}

void azcl_removeAllGroup(uint8_t endpoint){

	struct ZbApsmeRemoveAllGroupsReqT req;
	struct ZbApsmeRemoveAllGroupsConfT cfg;

	memset(&req, 0, sizeof(req));
	req.endpt = endpoint;
	ZbApsmeRemoveAllGroupsReq(zb, &req, &cfg);
}
