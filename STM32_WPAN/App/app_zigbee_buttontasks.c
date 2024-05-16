#include "app_zigbee_buttontasks.h"

#include "app_zigbee_cluster_client.h"
#include "app_zigbee_customlib.h"
#include "app_zigbee_nvm.h"

#include "app_zigbee_clientList.h"
#include "main.h"
#include "app_zigbee_cmd.h"

// example
// pair and leave network
static void indicateNextPair()
{
	APP_ZIGBEE_Indicate_c2s();
}
static void kickoutCurrentPair()
{
	APP_ZIGBEE_Leave_c2s();
}

// pair and leave handler function
void onIndicateNextPair()
{
	printf("this device is indicated\r\n");
}
void onKickoutCurrentPair()
{
	printf("this device is being kicked out\r\n");

	azcl_broadcaseLeaveRequest();
	APP_ZIGBEE_NVM_Erase();
}

// group addressing
static void leaveAllGroup()
{
	// device function
	// stops receiving every packet assigned to group address
	azcl_removeAllGroup(1);
}
static void joinToGroup(uint8_t endpoint, uint8_t groupAddr)
{
	// device function
	// make device to receive packet from endpoint in groupaddress
	azcl_addGroup(endpoint, groupAddr);
}

void app_zigbee_button1task(void){
	APP_DBG("SW1 PUSHED : Sending customls");
#ifdef CFG_REMOTE
	// azcl_setShortAddress(0x1000);
	// // calling persist save after changing address will restore the address from the flash
	// APP_ZIGBEE_persist_save();
	// azcl_test();

	APP_ZIGBEE_Indicate_c2s();
// enum ZbStatusCodeT ZB_WARN_UNUSED ZbZdoNwkUpdateReq(struct ZigBeeT *zb, struct ZbZdoNwkUpdateReqT *req,
//     void (*callback)(struct ZbZdoNwkUpdateNotifyT *reqPtr, void *cb_arg), void *arg);
// ZbZdoNwkUpdateReq();
 
		
	// static int cnt = 0;


	// static int value = 0;
	// uint8_t buffer[2];
	// buffer[0] = value;
	// buffer[1] = value / 128;

	// uint8_t grpAddr = 2;
	// if (cnt++ % 2 == 0) {
	// 	APP_ZIGBEE_Broadcast2bytes(buffer);
	// } else {
	// 	azcl_test();
	// }
	// // APP_ZIGBEE_Groupsend2bytes(buffer, grpAddr);
	// APP_ZIGBEE_exit_sleep();

	// value++;

#else
	azcl_test();
	// APP_ZIGBEE_NVM_Erase();
	// azcl_broadcaseLeaveRequest();
#endif

}
void app_zigbee_button2task(void){
	// printf("%x\r\n", azcl_getAddress());
	// azcl_test();
	// uint16_t shortaddress = azcl_getShortAddress();
	// printf("short address: %x\r\n", shortaddress);
	// return;

#ifdef CFG_REMOTE
	// APP_ZIGBEE_BroadcastPermitJoin();

	// APP_ZIGBEE_BroadcastExit();

	// indicateNextPair();
	kickoutCurrentPair();	
	// APP_ZIGBEE_Indicate(app_zigbee_clientList_next());

	uint16_t addr = azcl_getShortAddress();
	printf("short address: %x\r\n", addr);
#else
	// group address testing

	uint16_t shortaddress = azcl_getShortAddress();
	printf("short address: %x\r\n", shortaddress);

	static uint16_t ga = 1;

	if (ga == 1) {
		APP_DBG("remove all groups and add 1");
		azcl_removeAllGroup(1);
		azcl_addGroup(1, ga);
		ga = 2;
	}else {
		APP_DBG("remove all gorups and add 2");
		azcl_removeAllGroup(1);
		azcl_addGroup(1, ga);
		ga = 1;
	}
#endif

}
void app_zigbee_button3task(void){
#ifdef CFG_REMOTE
	// azcl_test();
	APP_ZIGBEE_SetNwkTask(ZbStartTypeJoin);
	// kickoutCurrentPair();
#else
	static bool shouldForm = false;
	// uint64_t addr = azcl_getAddress();
	// uint32_t* pAddr = (uint32_t*)&addr;
	// APP_DBG("address: %d %d", (uint32_t)*pAddr, (uint32_t)(*(pAddr + 1)));

	// on first try, join. if fails then always try to form
	if (shouldForm){
		APP_DBG("network form start");
		APP_ZIGBEE_SetNwkTask(ZbStartTypeForm);
	}else {
		APP_DBG("network join start");
		APP_ZIGBEE_SetNwkTask(ZbStartTypeJoin);
		shouldForm = true;
	}
#endif

}
