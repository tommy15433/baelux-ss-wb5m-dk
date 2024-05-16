
/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    App/app_zigbee.c
 * @author  MCD Application Team
 * @brief   Zigbee Application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_common.h"
#include "app_entry.h"
#include "dbg_trace.h"
#include "app_zigbee.h"
#include "zigbee_interface.h"
#include "shci.h"
#include "stm_logging.h"
#include "app_conf.h"
#include "stm32wbxx_core_interface_def.h"
#include "zigbee_types.h"
#include "stm32_seq.h"

/* Private includes -----------------------------------------------------------*/
#include <assert.h>
#include "zcl/zcl.h"

/* USER CODE BEGIN Includes */
#include "hw_flash.h"
#include "stm32_lpm.h"
#include "zcl/general/zcl.onoff.h"

#include "app_zigbee_buttontasks.h"
#include "app_zigbee_nvm.h"

#include "app_zigbee_cluster_client.h"
#include "app_zigbee_cluster_server.h"
#include "app_zigbee_customlib.h"

#include "app_zigbee_utils.h"
#include "app_zigbee_clientList.h"
#include "app_zigbee_serverList.h"
#include "user.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_STARTUP_FAIL_DELAY               500U
#define CHANNEL                                     11
#define ZED_SLEEP_TIME_30S                           1 /* 30s sleep time unit */

#define SW1_ENDPOINT                                1

/* USER CODE BEGIN PD */

#define DEV_ENDPOINT  2

#define CFG_NVM_DIAG 1U

#define SW1_GROUP_ADDR 0x0001

#define HW_TS_1S (1 * 1000 * 1000 / CFG_TS_TICK_VAL) /* 1s */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* External definition -------------------------------------------------------*/
enum ZbStatusCodeT ZbStartupWait(struct ZigBeeT *zb, struct ZbStartupT *config);

/* USER CODE BEGIN ED */

static void APP_ZIGBEE_ConfigGroupAddr(void);

void APP_ZIGBEE_sleep_mode_entry(void);
void APP_ZIGBEE_sleep_timer_enter();
void APP_ZIGBEE_enter_sleep(void);
void APP_ZIGBEE_exit_sleep(void);
void APP_ZIGBEE_prevent_nwk_nnt_timeout(void);
/* USER CODE END ED */

/* Private function prototypes -----------------------------------------------*/
static void APP_ZIGBEE_StackLayersInit(void);
static void APP_ZIGBEE_ConfigEndpoints(void);
static void APP_ZIGBEE_NwkForm(void);

static void APP_ZIGBEE_TraceError(const char *pMess, uint32_t ErrCode);
static void APP_ZIGBEE_CheckWirelessFirmwareInfo(void);

static void Wait_Getting_Ack_From_M0(void);
static void Receive_Ack_From_M0(void);
static void Receive_Notification_From_M0(void);

static void APP_ZIGBEE_ProcessNotifyM0ToM4(void);
static void APP_ZIGBEE_ProcessRequestM0ToM4(void);

/* USER CODE BEGIN PFP */

static bool APP_ZIGBEE_persist_load(void);
bool APP_ZIGBEE_persist_save(void);
static void APP_ZIGBEE_persist_delete(void);
static void APP_ZIGBEE_persist_notify_cb(struct ZigBeeT *zb, void *cbarg);
static enum ZbStatusCodeT APP_ZIGBEE_ZbStartupPersist(struct ZigBeeT *zb);
static void APP_ZIGBEE_PersistCompleted_callback(enum ZbStatusCodeT status, void *arg);
static enum ZclStatusCodeT APP_ZIGBEE_RestoreClusterAttr(struct ZbZclClusterT *clusterPtr);

void APP_ZIGBEE_SetNwkTask(enum ZbStartType type);
static void APP_ZIGBEE_NwkStart();

void APP_ZIGBEE_Groupsend2bytes(uint8_t *b, uint8_t groupAddr);
void APP_ZIGBEE_Send2bytes(uint8_t *b, uint16_t groupAddr, enum ZbApsAddrModeT mode);
void APP_ZIGBEE_Broadcast2bytes(uint8_t* b);

void APP_ZIGBEE_ConfigGA_Remote();
static void APP_ZIGBEE_ConfigSettings(void);
#ifdef CFG_REMOTE

void app_zigbee_cluster_client_callback(struct ZbZclCommandRspT *rsp, void *arg);

#else

void app_zigbee_cluster_device_tx_callback(struct ZbZclCommandRspT *rsp, void *arg);

enum ZclStatusCodeT device_tx_callback(
    struct ZbZclClusterT *clusterPtr,
    struct set_custom_ls_command_req_t *cmd_req,
    struct ZbZclAddrInfoT *src_info,
    void *arg);

enum ZclStatusCodeT server_cmd_callback(
    struct ZbZclClusterT *clusterPtr,
    struct set_custom_ls_command_req_t *cmd_req,
    struct ZbZclAddrInfoT *src_info,
    void *arg);

struct zb_cluster_server_callback_t callback_device_tx = {
    .set_custom_ls_command = device_tx_callback};
struct zb_cluster_server_callback_t callback = {
    .set_custom_ls_command = server_cmd_callback};


struct zb_cluster_server_callback_t callback_tx = {
    .set_custom_ls_command = device_tx_callback};
#endif

static void APP_ZIGBEE_ConfigSettings(void) {
    
    azcl_setBroadcastTimeout(3);

    azcl_registerZdoMsg();

    // add address of self to serverlist list
    uint16_t addr = azcl_getShortAddress();
    app_zigbee_serverList_update(addr, HAL_GetTick());

    azcl_announce();

    #ifdef CFG_REMOTE
    // add group address 1 and 2 for remote
    // for target, address switches between 1 and 2 inside buttontask.c
    APP_ZIGBEE_ConfigGA_Remote();
    #endif
}

/* USER CODE END PFP */

/* Private variables ---------------------------------------------------------*/
static TL_CmdPacket_t   *p_ZIGBEE_otcmdbuffer;
static TL_EvtPacket_t   *p_ZIGBEE_notif_M0_to_M4;
static TL_EvtPacket_t   *p_ZIGBEE_request_M0_to_M4;
static __IO uint32_t    CptReceiveNotifyFromM0 = 0;
static __IO uint32_t    CptReceiveRequestFromM0 = 0;

PLACE_IN_SECTION("MB_MEM1") ALIGN(4) static TL_ZIGBEE_Config_t ZigbeeConfigBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static TL_CmdPacket_t ZigbeeOtCmdBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t ZigbeeNotifRspEvtBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t ZigbeeNotifRequestBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];

struct zigbee_app_info
{
  bool has_init;
  struct ZigBeeT *zb;
  enum ZbStartType startupControl;
  enum ZbStatusCodeT join_status;
  uint32_t join_delay;
  bool init_after_join;

};
static struct zigbee_app_info zigbee_app_info;

/* USER CODE BEGIN PV */

// callback
static app_zigbee_logic_callbacks_t* m_logicCallbacks = NULL;

// CLUSTERS

clusterId_t cluster_remote_cmd_id;
clusterId_t cluster_device_cmd_id;
clusterId_t cluster_device_tx_id;
clusterId_t cluster_device_rx_id;

struct ZbZclClusterT *zigbee_cluster;
bool freshstart = false;
static int persistNumWrites = 0;
int forceJoin = 0;
uint16_t m_groupAddress[5];
uint8_t m_groupAddressSize = 0;
uint16_t m_parent = 0x0000;

static enum ZbStartType m_nwkJoinType;

// persist cache
__attribute__((section(".noinit"))) union cache cache_persistent_data;
__attribute__((section(".noinit"))) union cache cache_diag_reference;

// timer for sleep
static uint8_t TS_SLEEP_ID1;
static uint8_t TS_NNT_TIMEOUT_ID1;

// timer for server which is parent to manage serverlist
static uint8_t TS_SERVER_PARENT_ID1;

// flag that indicates this device is server parent
static bool m_flag_parent = false;

/* USER CODE END PV */
/* Functions Definition ------------------------------------------------------*/

/**
 * @brief  Zigbee application initialization
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_Init(void)
{
  SHCI_CmdStatus_t ZigbeeInitStatus;

  APP_DBG("APP_ZIGBEE_Init");

  /* Check the compatibility with the Coprocessor Wireless Firmware loaded */
  APP_ZIGBEE_CheckWirelessFirmwareInfo();

  /* Register cmdbuffer */
  APP_ZIGBEE_RegisterCmdBuffer(&ZigbeeOtCmdBuffer);

  /* Init config buffer and call TL_ZIGBEE_Init */
  APP_ZIGBEE_TL_INIT();

  /* Register task */
  /* Create the different tasks */
  UTIL_SEQ_RegTask(1U << (uint32_t)CFG_TASK_NOTIFY_FROM_M0_TO_M4, UTIL_SEQ_RFU, APP_ZIGBEE_ProcessNotifyM0ToM4);
  UTIL_SEQ_RegTask(1U << (uint32_t)CFG_TASK_REQUEST_FROM_M0_TO_M4, UTIL_SEQ_RFU, APP_ZIGBEE_ProcessRequestM0ToM4);

  /* Task associated with network creation process */
  UTIL_SEQ_RegTask(1U << CFG_TASK_ZIGBEE_NETWORK_FORM, UTIL_SEQ_RFU, APP_ZIGBEE_NwkForm);

  /* USER CODE BEGIN APP_ZIGBEE_INIT */

  /* NVM Init */
#if CFG_NVM
  APP_ZIGBEE_NVM_Init();
#endif

  // button tasks init
  // UTIL_SEQ_RegTask(1U << CFG_TASK_BUTTON_1, UTIL_SEQ_RFU, app_zigbee_button1task);
  // UTIL_SEQ_RegTask(1U << CFG_TASK_BUTTON_2, UTIL_SEQ_RFU, app_zigbee_button2task);
  // UTIL_SEQ_RegTask(1U << CFG_TASK_BUTTON_3, UTIL_SEQ_RFU, app_zigbee_button3task);
  // UTIL_SEQ_RegTask(1U << CFG_TASK_ENTER_STOP, UTIL_SEQ_RFU, APP_ZIGBEE_enter_sleep);

  // // nwk start task init
  // UTIL_SEQ_RegTask(1U << CFG_TASK_ZIGBEE_NETWORK_START, UTIL_SEQ_RFU, APP_ZIGBEE_NwkStart);
  
  /* USER CODE END APP_ZIGBEE_INIT */

  /* Start the Zigbee on the CPU2 side */
  ZigbeeInitStatus = SHCI_C2_ZIGBEE_Init();
  /* Prevent unused argument(s) compilation warning */
  UNUSED(ZigbeeInitStatus);

  /* Initialize Zigbee stack layers */
  APP_ZIGBEE_StackLayersInit();

}

/**
 * @brief  Initialize Zigbee stack layers
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_StackLayersInit(void)
{
  APP_DBG("APP_ZIGBEE_StackLayersInit");

  zigbee_app_info.zb = ZbInit(0U, NULL, NULL);
  assert(zigbee_app_info.zb != NULL);

  /* Create the endpoint and cluster(s) */
  APP_ZIGBEE_ConfigEndpoints();

  /* USER CODE BEGIN APP_ZIGBEE_StackLayersInit */

#if CFG_NVM

  ZbPersistNotifyRegister(zigbee_app_info.zb, NULL, NULL);
  enum ZbStatusCodeT status = APP_ZIGBEE_ZbStartupPersist(zigbee_app_info.zb);

  if (status == ZB_STATUS_SUCCESS)
  {
    /* no fresh stratup need anymore */
    
    ZbSetLogging(zigbee_app_info.zb, ZB_LOG_MASK_LEVEL_5, NULL);
    freshstart = false;
    APP_DBG("ZbStartupPersist: SUCCESS, restarted from persistence");
    zigbee_app_info.startupControl = ZbStartTypeForm;

    APP_ZIGBEE_ConfigSettings(); 
  }
  else
  {

    /* Start-up form persistence failed perform a fresh ZbStartup */
    APP_DBG("ZbStartupPersist: FAILED to restart from persistence with status: 0x%02x", status);

    /* Configure the joining parameters */
    zigbee_app_info.join_status = (enum ZbStatusCodeT)0x01; /* init to error status */
    zigbee_app_info.join_delay = HAL_GetTick();             /* now */

#ifdef CFG_REMOTE
    //		zigbee_app_info.startupControl = ZbStartTypeForm;
    zigbee_app_info.startupControl = ZbStartTypeJoin;
#else
    zigbee_app_info.startupControl = ZbStartTypeJoin;
#endif
    /* Initialization Complete */
    zigbee_app_info.has_init = true;
  }

  azcl_init(zigbee_app_info.zb);
  APP_ZIGBEE_sleep_mode_entry();

  APP_ZIGBEE_prevent_nwk_nnt_timeout();

  if (m_logicCallbacks != NULL){
    m_logicCallbacks->onNetworkInitSuccess();
  }else{
    APP_DBG("logic callbacks are not registered");
  }

  // return from the function to block from accessing below generated code
  return;
#else

    /* Configure the joining parameters */
    zigbee_app_info.join_status = (enum ZbStatusCodeT)0x01; /* init to error status */
    zigbee_app_info.join_delay = HAL_GetTick();             /* now */

    zigbee_app_info.startupControl = ZbStartTypeJoin;

    /* Initialization Complete */
    zigbee_app_info.has_init = true;
  

  azcl_init(zigbee_app_info.zb);
  APP_ZIGBEE_sleep_mode_entry();

  APP_ZIGBEE_prevent_nwk_nnt_timeout();

  if (m_logicCallbacks != NULL){
    m_logicCallbacks->onNetworkInitSuccess();
  }else{
    APP_DBG("logic callbacks are not registered");
  }

  // return from the function to block from accessing below generated code
  return;
#endif

  /* USER CODE END APP_ZIGBEE_StackLayersInit */

  /* Configure the joining parameters */
  zigbee_app_info.join_status = (enum ZbStatusCodeT) 0x01; /* init to error status */
  zigbee_app_info.join_delay = HAL_GetTick(); /* now */
  zigbee_app_info.startupControl = ZbStartTypeJoin;

  /* Initialization Complete */
  zigbee_app_info.has_init = true;

  /* run the task */
  UTIL_SEQ_SetTask(1U << CFG_TASK_ZIGBEE_NETWORK_FORM, CFG_SCH_PRIO_0);
}

/**
 * @brief  Configure Zigbee application endpoints
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ConfigEndpoints(void)
{
  struct ZbApsmeAddEndpointReqT req;
  struct ZbApsmeAddEndpointConfT conf;

  memset(&req, 0, sizeof(req));

  /* Endpoint: SW1_ENDPOINT */
  req.profileId = ZCL_PROFILE_HOME_AUTOMATION;
  req.deviceId = ZCL_DEVICE_ONOFF_SWITCH;
  req.endpoint = SW1_ENDPOINT;
  ZbZclAddEndpoint(zigbee_app_info.zb, &req, &conf);
  assert(conf.status == ZB_STATUS_SUCCESS);

  /* USER CODE BEGIN CONFIG_ENDPOINT */

  APP_DBG("APP_ZIGBEE_Allocate");
#ifdef CFG_REMOTE
  cluster_remote_cmd_id = zb_cluster_client_alloc(zigbee_app_info.zb, SW1_ENDPOINT);
  zb_cluster_client_setOnResponseCallback(cluster_remote_cmd_id, app_zigbee_cluster_client_callback);
#else

  cluster_device_cmd_id = zb_cluster_server_alloc(zigbee_app_info.zb, SW1_ENDPOINT, &callback, NULL);
  cluster_device_tx_id = zb_cluster_client_alloc(zigbee_app_info.zb, DEV_ENDPOINT);
  zb_cluster_client_setOnResponseCallback(cluster_device_tx_id, app_zigbee_cluster_device_tx_callback);
  cluster_device_rx_id = zb_cluster_server_alloc(zigbee_app_info.zb, DEV_ENDPOINT, &callback_device_tx, NULL);

#endif

  APP_DBG("APP_ZIGBEE_Allocate done");
  /* USER CODE END CONFIG_ENDPOINT */
}

/**
 * @brief  Handle Zigbee network forming and joining
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_NwkForm(void)
{
  if ((zigbee_app_info.join_status != ZB_STATUS_SUCCESS) && (HAL_GetTick() >= zigbee_app_info.join_delay))
  {
    struct ZbStartupT config;
    enum ZbStatusCodeT status;

    /* Configure Zigbee Logging */
    ZbSetLogging(zigbee_app_info.zb, ZB_LOG_MASK_LEVEL_5, NULL);

    /* Attempt to join a zigbee network */
    ZbStartupConfigGetProDefaults(&config);

    /* Set the distributed network */
    APP_DBG("Network config : APP_STARTUP_DISTRIBUTED");
    config.startupControl = zigbee_app_info.startupControl;

    /* Set the TC address to be distributed. */
    config.security.trustCenterAddress = ZB_DISTRIBUTED_TC_ADDR;

    /* Using the Uncertified Distributed Global Key (d0:d1:d2:d3:d4:d5:d6:d7:d8:d9:da:db:dc:dd:de:df) */
    memcpy(config.security.distributedGlobalKey, sec_key_distrib_uncert, ZB_SEC_KEYSIZE);

    config.channelList.count = 1;
    config.channelList.list[0].page = 0;
    config.channelList.list[0].channelMask = 1 << CHANNEL; /*Channel in use */

    /* For Distributed and Touchlink network in RFD application, End device configuration has to be set */
    config.capability &= ~(MCP_ASSOC_CAP_RXONIDLE | MCP_ASSOC_CAP_DEV_TYPE | MCP_ASSOC_CAP_ALT_COORD);
    config.endDeviceTimeout=ZED_SLEEP_TIME_30S;

    /* Using ZbStartupWait (blocking) */
    status = ZbStartupWait(zigbee_app_info.zb, &config);

    APP_DBG("ZbStartup Callback (status = 0x%02x)", status);
    zigbee_app_info.join_status = status;

    if (status == ZB_STATUS_SUCCESS)
    {
      zigbee_app_info.join_delay = 0U;
      zigbee_app_info.init_after_join = true;
      APP_DBG("Startup done !\n");
      /* USER CODE BEGIN 0 */

      // persist used code
      /* Register Persistent data change notification */
#ifdef CFG_NVM
      ZbPersistNotifyRegister(zigbee_app_info.zb, APP_ZIGBEE_persist_notify_cb, NULL);
      /* Call the callback once here to save persistence data */
      APP_ZIGBEE_persist_notify_cb(zigbee_app_info.zb, NULL);
#endif
      APP_ZIGBEE_sleep_mode_entry();
      /* USER CODE END 0 */
    }
    else
    {
      APP_DBG("Startup failed, attempting again after a short delay (%d ms)", APP_ZIGBEE_STARTUP_FAIL_DELAY);
      zigbee_app_info.join_delay = HAL_GetTick() + APP_ZIGBEE_STARTUP_FAIL_DELAY;
      /* USER CODE BEGIN 1 */

      // no network to join, it becomes network former
#ifndef CFG_REMOTE
      zigbee_app_info.startupControl = ZbStartTypeForm;
#endif
      APP_DBG("Startup failed, attempting as ZbStartTypeForm again after a short delay (%d ms)", APP_ZIGBEE_STARTUP_FAIL_DELAY);
      zigbee_app_info.join_delay = HAL_GetTick() + APP_ZIGBEE_STARTUP_FAIL_DELAY;

      /* USER CODE END 1 */
    }
  }

  /* If Network forming/joining was not successful reschedule the current task to retry the process */
  if (zigbee_app_info.join_status != ZB_STATUS_SUCCESS)
  {
    UTIL_SEQ_SetTask(1U << CFG_TASK_ZIGBEE_NETWORK_FORM, CFG_SCH_PRIO_0);
  }
  /* USER CODE BEGIN NW_FORM */
  else
  {
    /* Assign ourselves to the group addresses */
    APP_ZIGBEE_ConfigGroupAddr();

    /* Since we're using group addressing (broadcast), shorten the broadcast timeout */

    azcl_setBroadcastTimeout(0);
    azcl_registerZdoMsg();
    forceJoin = 0;
  }
  /* USER CODE END NW_FORM */
}

/*************************************************************
 * ZbStartupWait Blocking Call
 *************************************************************/
struct ZbStartupWaitInfo
{
  bool active;
  enum ZbStatusCodeT status;
};

static void ZbStartupWaitCb(enum ZbStatusCodeT status, void *cb_arg)
{
  struct ZbStartupWaitInfo *info = cb_arg;

  info->status = status;
  info->active = false;
  UTIL_SEQ_SetEvt(EVENT_ZIGBEE_STARTUP_ENDED);
}

enum ZbStatusCodeT ZbStartupWait(struct ZigBeeT *zb, struct ZbStartupT *config)
{
  struct ZbStartupWaitInfo *info;
  enum ZbStatusCodeT status;

  info = malloc(sizeof(struct ZbStartupWaitInfo));
  if (info == NULL)
  {
    return ZB_STATUS_ALLOC_FAIL;
  }
  memset(info, 0, sizeof(struct ZbStartupWaitInfo));

  info->active = true;
  status = ZbStartup(zb, config, ZbStartupWaitCb, info);
  if (status != ZB_STATUS_SUCCESS)
  {
    free(info);
    return status;
  }

  UTIL_SEQ_WaitEvt(EVENT_ZIGBEE_STARTUP_ENDED);
  status = info->status;
  free(info);
  return status;
}

/**
 * @brief  Trace the error or the warning reported.
 * @param  ErrId :
 * @param  ErrCode
 * @retval None
 */
void APP_ZIGBEE_Error(uint32_t ErrId, uint32_t ErrCode)
{
  switch (ErrId)
  {
    default:
      APP_ZIGBEE_TraceError("ERROR Unknown ", 0);
      break;
  }
}

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/**
 * @brief  Warn the user that an error has occurred.
 *
 * @param  pMess  : Message associated to the error.
 * @param  ErrCode: Error code associated to the module (Zigbee or other module if any)
 * @retval None
 */
static void APP_ZIGBEE_TraceError(const char *pMess, uint32_t ErrCode)
{
  APP_DBG("**** Fatal error = %s (Err = %d)", pMess, ErrCode);
  /* USER CODE BEGIN TRACE_ERROR */
  /* USER CODE END TRACE_ERROR */

}

/**
 * @brief Check if the Coprocessor Wireless Firmware loaded supports Zigbee
 *        and display associated information
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_CheckWirelessFirmwareInfo(void)
{
  WirelessFwInfo_t wireless_info_instance;
  WirelessFwInfo_t *p_wireless_info = &wireless_info_instance;

  if (SHCI_GetWirelessFwInfo(p_wireless_info) != SHCI_Success)
  {
    APP_ZIGBEE_Error((uint32_t)ERR_ZIGBEE_CHECK_WIRELESS, (uint32_t)ERR_INTERFACE_FATAL);
  }
  else
  {
    APP_DBG("**********************************************************");
    APP_DBG("WIRELESS COPROCESSOR FW:");
    /* Print version */
    APP_DBG("VERSION ID = %d.%d.%d", p_wireless_info->VersionMajor, p_wireless_info->VersionMinor, p_wireless_info->VersionSub);

    switch (p_wireless_info->StackType)
    {
      case INFO_STACK_TYPE_ZIGBEE_FFD:
        APP_DBG("FW Type : FFD Zigbee stack");
        break;

      case INFO_STACK_TYPE_ZIGBEE_RFD:
        APP_DBG("FW Type : RFD Zigbee stack");
        break;

      default:
        /* No Zigbee device supported ! */
        APP_ZIGBEE_Error((uint32_t)ERR_ZIGBEE_CHECK_WIRELESS, (uint32_t)ERR_INTERFACE_FATAL);
        break;
    }

    /* print the application name */
    char *__PathProject__ = (strstr(__FILE__, "Zigbee") ? strstr(__FILE__, "Zigbee") + 7 : __FILE__);
    char *pdel = NULL;
    if((strchr(__FILE__, '/')) == NULL)
    {
      pdel = strchr(__PathProject__, '\\');
    }
    else
    {
      pdel = strchr(__PathProject__, '/');
    }

    int index = (int)(pdel - __PathProject__);
    APP_DBG("Application flashed: %*.*s", index, index, __PathProject__);

    /* print channel */
    APP_DBG("Channel used: %d", CHANNEL);
    /* print Link Key */
    APP_DBG("Link Key: %.16s", sec_key_ha);
    /* print Link Key value hex */
    char Z09_LL_string[ZB_SEC_KEYSIZE*3+1];
    Z09_LL_string[0] = 0;
    for (int str_index = 0; str_index < ZB_SEC_KEYSIZE; str_index++)
    {
      sprintf(&Z09_LL_string[str_index*3], "%02x ", sec_key_ha[str_index]);
    }

    APP_DBG("Link Key value: %s", Z09_LL_string);
    /* print clusters allocated */
    APP_DBG("Clusters allocated are:");
    APP_DBG("**********************************************************");
  }
}

/*************************************************************
 *
 * WRAP FUNCTIONS
 *
 *************************************************************/

void APP_ZIGBEE_RegisterCmdBuffer(TL_CmdPacket_t *p_buffer)
{
  p_ZIGBEE_otcmdbuffer = p_buffer;
}

Zigbee_Cmd_Request_t * ZIGBEE_Get_OTCmdPayloadBuffer(void)
{
  return (Zigbee_Cmd_Request_t *)p_ZIGBEE_otcmdbuffer->cmdserial.cmd.payload;
}

Zigbee_Cmd_Request_t * ZIGBEE_Get_OTCmdRspPayloadBuffer(void)
{
  return (Zigbee_Cmd_Request_t *)((TL_EvtPacket_t *)p_ZIGBEE_otcmdbuffer)->evtserial.evt.payload;
}

Zigbee_Cmd_Request_t * ZIGBEE_Get_NotificationPayloadBuffer(void)
{
  return (Zigbee_Cmd_Request_t *)(p_ZIGBEE_notif_M0_to_M4)->evtserial.evt.payload;
}

Zigbee_Cmd_Request_t * ZIGBEE_Get_M0RequestPayloadBuffer(void)
{
  return (Zigbee_Cmd_Request_t *)(p_ZIGBEE_request_M0_to_M4)->evtserial.evt.payload;
}

/**
 * @brief  This function is used to transfer the commands from the M4 to the M0.
 *
 * @param   None
 * @return  None
 */
void ZIGBEE_CmdTransfer(void)
{
  Zigbee_Cmd_Request_t *cmd_req = (Zigbee_Cmd_Request_t *)p_ZIGBEE_otcmdbuffer->cmdserial.cmd.payload;

  /* Zigbee OT command cmdcode range 0x280 .. 0x3DF = 352 */
  p_ZIGBEE_otcmdbuffer->cmdserial.cmd.cmdcode = 0x280U;
  /* Size = otCmdBuffer->Size (Number of OT cmd arguments : 1 arg = 32bits so multiply by 4 to get size in bytes)
   * + ID (4 bytes) + Size (4 bytes) */
  p_ZIGBEE_otcmdbuffer->cmdserial.cmd.plen = 8U + (cmd_req->Size * 4U);

  TL_ZIGBEE_SendM4RequestToM0();

  /* Wait completion of cmd */
  Wait_Getting_Ack_From_M0();
}

/**
 * @brief  This function is called when the M0+ acknowledge the fact that it has received a Cmd
 *
 *
 * @param   Otbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void TL_ZIGBEE_CmdEvtReceived(TL_EvtPacket_t *Otbuffer)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Otbuffer);

  Receive_Ack_From_M0();
}

/**
 * @brief  This function is called when notification from M0+ is received.
 *
 * @param   Notbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void TL_ZIGBEE_NotReceived(TL_EvtPacket_t *Notbuffer)
{
  p_ZIGBEE_notif_M0_to_M4 = Notbuffer;

  Receive_Notification_From_M0();
}

/**
 * @brief  This function is called before sending any ot command to the M0
 *         core. The purpose of this function is to be able to check if
 *         there are no notifications coming from the M0 core which are
 *         pending before sending a new ot command.
 * @param  None
 * @retval None
 */
void Pre_ZigbeeCmdProcessing(void)
{
  UTIL_SEQ_WaitEvt(EVENT_SYNCHRO_BYPASS_IDLE);
}

/**
 * @brief  This function waits for getting an acknowledgment from the M0.
 *
 * @param  None
 * @retval None
 */
static void Wait_Getting_Ack_From_M0(void)
{
  UTIL_SEQ_WaitEvt(EVENT_ACK_FROM_M0_EVT);
}

/**
 * @brief  Receive an acknowledgment from the M0+ core.
 *         Each command send by the M4 to the M0 are acknowledged.
 *         This function is called under interrupt.
 * @param  None
 * @retval None
 */
static void Receive_Ack_From_M0(void)
{
  UTIL_SEQ_SetEvt(EVENT_ACK_FROM_M0_EVT);
}

/**
 * @brief  Receive a notification from the M0+ through the IPCC.
 *         This function is called under interrupt.
 * @param  None
 * @retval None
 */
static void Receive_Notification_From_M0(void)
{
  CptReceiveNotifyFromM0++;
  UTIL_SEQ_SetTask(1U << (uint32_t)CFG_TASK_NOTIFY_FROM_M0_TO_M4, CFG_SCH_PRIO_0);
}

/**
 * @brief  This function is called when a request from M0+ is received.
 *
 * @param   Notbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void TL_ZIGBEE_M0RequestReceived(TL_EvtPacket_t *Reqbuffer)
{
  p_ZIGBEE_request_M0_to_M4 = Reqbuffer;

  CptReceiveRequestFromM0++;
  UTIL_SEQ_SetTask(1U << (uint32_t)CFG_TASK_REQUEST_FROM_M0_TO_M4, CFG_SCH_PRIO_0);
}

/**
 * @brief Perform initialization of TL for Zigbee.
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_TL_INIT(void)
{
  ZigbeeConfigBuffer.p_ZigbeeOtCmdRspBuffer = (uint8_t *)&ZigbeeOtCmdBuffer;
  ZigbeeConfigBuffer.p_ZigbeeNotAckBuffer = (uint8_t *)ZigbeeNotifRspEvtBuffer;
  ZigbeeConfigBuffer.p_ZigbeeNotifRequestBuffer = (uint8_t *)ZigbeeNotifRequestBuffer;
  TL_ZIGBEE_Init(&ZigbeeConfigBuffer);
}

/**
 * @brief Process the messages coming from the M0.
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ProcessNotifyM0ToM4(void)
{
  if (CptReceiveNotifyFromM0 != 0)
  {
    /* Reset counter */
    CptReceiveNotifyFromM0 = 0;
    Zigbee_CallBackProcessing();
  }
}

/**
 * @brief Process the requests coming from the M0.
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ProcessRequestM0ToM4(void)
{
  if (CptReceiveRequestFromM0 != 0)
  {
    CptReceiveRequestFromM0 = 0;
    Zigbee_M0RequestProcessing();
  }
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

static void APP_ZIGBEE_ConfigGroupAddr(void)
{
  struct ZbApsmeAddGroupReqT req;
  struct ZbApsmeAddGroupConfT conf;

  memset(&req, 0, sizeof(req));
  req.endpt = SW1_ENDPOINT;
  req.groupAddr = SW1_GROUP_ADDR;
  ZbApsmeAddGroupReq(zigbee_app_info.zb, &req, &conf);

} /* APP_ZIGBEE_ConfigGroupAddr */

static enum ZclStatusCodeT APP_ZIGBEE_RestoreClusterAttr(struct ZbZclClusterT *clusterPtr)
{
  uint8_t attrVal;

  if (ZbZclAttrRead(clusterPtr, ZCL_ONOFF_ATTR_ONOFF, NULL,
                    &attrVal, sizeof(attrVal), false) != ZCL_STATUS_SUCCESS)
  {
    return ZCL_STATUS_FAILURE;
  }
  if (attrVal)
  {
    APP_DBG("RESTORE LED_RED TO ON");
  }
  else
  {
    APP_DBG("RESTORE LED_RED TO OFF");
  }

  return ZCL_STATUS_SUCCESS;
} /* APP_ZIGBEE_RestoreClusterAttr */

/**
 * @brief  notify to save persistent data callback
 * @param  zb: Zigbee device object pointer, cbarg: callback arg pointer
 * @retval None
 */
void APP_ZIGBEE_persist_notify_cb(struct ZigBeeT *zb, void *cbarg)
{
  APP_DBG("Notification to save persistent data requested from stack");
  /* Save the persistent data */
  azcl_broadcastPermitJoinRequest(0x80);
  APP_ZIGBEE_persist_save();

  //	APP_ZIGBEE_enter_sleep();
}

/**
 * @brief  Start Zigbee Network from persistent data
 * @param  zb: Zigbee device object pointer
 * @retval Zigbee stack Status code
 */
enum ZbStatusCodeT APP_ZIGBEE_ZbStartupPersist(struct ZigBeeT *zb)
{
  bool read_status;
  enum ZbStatusCodeT status = ZB_STATUS_SUCCESS;

  /* Restore persistence */
  read_status = APP_ZIGBEE_persist_load();

  if (read_status)
  {
    /* Make sure the EPID is cleared, before we are allowed to restore persistence */
    uint64_t epid = 0U;
    ZbNwkSet(zb, ZB_NWK_NIB_ID_ExtendedPanId, &epid, sizeof(uint64_t));

    /* Start-up from persistence */
    APP_DBG("APP_ZIGBEE_ZbStartupPersist: restoring stack persistence");
    status = ZbStartupPersist(zb, &cache_persistent_data.U8_data[4], cache_persistent_data.U32_data[0], NULL, APP_ZIGBEE_PersistCompleted_callback, NULL);
  }
  else
  {
    /* Failed to restart from persistence */
    APP_DBG("APP_ZIGBEE_ZbStartupPersist: no persistence data to restore");
    status = ZB_STATUS_ALLOC_FAIL;
  }

  /* Only for debug purpose, depending of persistent data, following traces
     could display bytes that are irrelevants to on off cluster */
  if (status == ZB_STATUS_SUCCESS)
  {
    /* read the last bytes of data where the ZCL on off persistent data shall be*/
    uint32_t len = cache_persistent_data.U32_data[0] + 4;
    APP_DBG("ClusterID %02x %02x", cache_persistent_data.U8_data[len - 9], cache_persistent_data.U8_data[len - 10]);
    APP_DBG("Endpoint %02x %02x", cache_persistent_data.U8_data[len - 7], cache_persistent_data.U8_data[len - 8]);
    APP_DBG("Direction %02x", cache_persistent_data.U8_data[len - 6]);
    APP_DBG("AttrID %02x %02x", cache_persistent_data.U8_data[len - 4], cache_persistent_data.U8_data[len - 5]);
    APP_DBG("Len %02x %02x", cache_persistent_data.U8_data[len - 2], cache_persistent_data.U8_data[len - 3]);
    APP_DBG("Value %02x", cache_persistent_data.U8_data[len - 1]);
  }

  return status;
} /* APP_ZIGBEE_ZbStartupPersist */

/**
 * @brief  timer callback to wait end of restore cluster persistence form M0
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_PersistCompleted_callback(enum ZbStatusCodeT status, void *arg)
{
  if (status == ZB_WPAN_STATUS_SUCCESS)
  {
    APP_DBG("Persist complete callback entered with SUCCESS");
    /* Restore the on/off value based on persitence loaded */
    if (APP_ZIGBEE_RestoreClusterAttr(zigbee_cluster) == ZCL_STATUS_SUCCESS)
    {
      APP_DBG("Read back OnOff cluster attribute : SUCCESS");
    }
    else
    {
      APP_DBG("Read back OnOff cluster attribute : FAILED");
    }
  }
  else
  {
    APP_DBG("Error in persist complete callback %x", status);
  }
  /* STEP3 - Activate back the persistent notofacation */
  /* Register Persistent data change notification */
  ZbPersistNotifyRegister(zigbee_app_info.zb, APP_ZIGBEE_persist_notify_cb, NULL);

  /* Call the callback once here to save persistence data */
  APP_ZIGBEE_persist_notify_cb(zigbee_app_info.zb, NULL);
} /* APP_ZIGBEE_PersistCompleted_callback */

/**
 * @brief  Load persistent data
 * @param  None
 * @retval true if success, false if fail
 */
bool APP_ZIGBEE_persist_load(void)
{
#ifdef CFG_NVM
  APP_DBG("Retrieving persistent data from FLASH");
  return APP_ZIGBEE_NVM_Read(cache_persistent_data.U32_data);
#else
  /* Check length range */
  if ((cache_persistent_data.U32_data[0] == 0) ||
      (cache_persistent_data.U32_data[0] > ST_PERSIST_MAX_ALLOC_SZ))
  {
    APP_DBG("No data or too large length : %d", cache_persistent_data.U32_data[0]);
    return false;
  }
  return true;
#endif /* CFG_NVM */
} /* APP_ZIGBEE_persist_load */

/**
 * @brief  Save persistent data
 * @param  None
 * @retval true if success , false if fail
 */
bool APP_ZIGBEE_persist_save(void)
{
  uint32_t len;

  /* Clear the RAM cache before saving */
  memset(cache_persistent_data.U8_data, 0x00, ST_PERSIST_MAX_ALLOC_SZ);

  /* Call the satck API t get current persistent data */
  len = ZbPersistGet(zigbee_app_info.zb, 0, 0);
  /* Check Length range */
  if (len == 0U)
  {
    /* If the persistence length was zero then no data available. */
    APP_DBG("APP_ZIGBEE_persist_save: no persistence data to save !");
    return false;
  }
  if (len > ST_PERSIST_MAX_ALLOC_SZ)
  {
    /* if persistence length to big to store */
    APP_DBG("APP_ZIGBEE_persist_save: persist size too large for storage (%d)", len);
    return false;
  }

  /* Store in cache the persistent data */
  len = ZbPersistGet(zigbee_app_info.zb, &cache_persistent_data.U8_data[ST_PERSIST_FLASH_DATA_OFFSET], len);

  /* Store in cache the persistent data length */
  cache_persistent_data.U32_data[0] = len;

  persistNumWrites++;
  APP_DBG("APP_ZIGBEE_persist_save: Persistence written in cache RAM (num writes = %d) len=%d",
          persistNumWrites, cache_persistent_data.U32_data[0]);

#ifdef CFG_NVM

  if (!APP_ZIGBEE_NVM_Write(cache_persistent_data.U32_data))
  {
    return false;
  }
  APP_DBG("APP_ZIGBEE_persist_save: Persistent data FLASHED");

  azcl_restore_nwk_addr();

#endif /* CFG_NVM */

  return true;
} /* APP_ZIGBEE_persist_save */

void APP_ZIGBEE_persist_delete(void)
{
  /* Clear RAM cache */
  memset(cache_persistent_data.U8_data, 0x00, ST_PERSIST_MAX_ALLOC_SZ);
  APP_DBG("Persistent Data RAM cache cleared");

#ifdef CFG_NVM

  APP_DBG("FLASH ERASED");
  APP_ZIGBEE_NVM_Erase();

#endif /* CFG_NVM */
} /* APP_ZIGBEE_persist_delete */

#ifdef CFG_REMOTE
void app_zigbee_cluster_client_callback(struct ZbZclCommandRspT *rsp, void *arg)
{
  APP_DBG("app_zigbee_cluster_client_callback %s", rsp->payload);

  APP_ZIGBEE_sleep_timer_enter(10);
  return;

  // first char is o if success
  if (rsp->payload[0] != 'o')
  {
    APP_ZIGBEE_sleep_timer_enter(10);
  } else {
  
  }

#else

void app_zigbee_cluster_device_tx_callback(struct ZbZclCommandRspT *rsp, void *arg)
{
  APP_DBG("app_zigbee_device_txclient_callback %s", rsp->payload);
}
enum ZclStatusCodeT device_tx_callback(
    struct ZbZclClusterT *clusterPtr,
    struct set_custom_ls_command_req_t *cmd_req,
    struct ZbZclAddrInfoT *src_info,
    void *arg)
    {
      uint16_t myaddr = azcl_getShortAddress();
      uint16_t recvaddr = strToU16(&cmd_req->string[2]);

      uint8_t cmd = cmd_req->string[0];

      printf("%4x\r\n", recvaddr);
      if (cmd == CMD_INDICATE && recvaddr == myaddr)
      {
        onIndicateNextPair();
      }
      else if (cmd == CMD_DISCONNECT && recvaddr == myaddr)
      {
        onKickoutCurrentPair();
      }
    }

enum ZclStatusCodeT server_cmd_callback(
    struct ZbZclClusterT *clusterPtr,
    struct set_custom_ls_command_req_t *cmd_req,
    struct ZbZclAddrInfoT *src_info,
    void *arg)
{
  APP_DBG("%d %d %d", cmd_req->string[0], cmd_req->string[1], cmd_req->string[2]);

  uint16_t myaddr = azcl_getShortAddress();
  uint16_t recvaddr = strToU16(&cmd_req->string[2]);

  // device should send response with their address after 'o'
  struct custom_ls_command_rsp_t _rsp = {0};
  sprintf(_rsp.one, "o%x\0", myaddr);

  zb_cluster_server_sendCommandRsp(clusterPtr, src_info, &_rsp);

  // command handler
  if ((cmd_req->string[0] == CMD_PERMITJOIN) && (cmd_req->string[1] == CMD_PERMITJOIN)){

	  APP_DBG("permit join requested from remote");
	  azcl_broadcastPermitJoinRequest(0x80);
  }
  else if (cmd_req->string[0] == CMD_INDICATE) {
    
    uint16_t addr = app_zigbee_serverList_next();

    if (addr != 0) 
    {
        // send tx command with CMD_INDICATE 
        // zb_cluster_client_command_broadcast(cluster_device_tx_id, addr, "IND\0", NULL);
        APP_ZIGBEE_Indicate_s2s(addr);
    } else {
        // if server list head is empty, device should indicate itself
        printf("this device is selected\r\n");
    }
  } else if (cmd_req->string[0] == CMD_DISCONNECT) {

    uint16_t addr = app_zigbee_serverList_cur();

    if (addr != 0) 
    {
        // send tx command with CMD_INDICATE 
        // zb_cluster_client_command_broadcast(cluster_device_tx_id, addr, "LVE\0", NULL);
        APP_ZIGBEE_Leave_s2s(addr);
    } else {
        // if server list head is empty, device should indicate itself
        printf("this device should leave\r\n");
    }
  }

  return ZCL_STATUS_SUCCESS;
#endif
}

void server_parent_timer_handler(void)
{
  // if (m_flag_parent == true)
  // {
    serverlist_addr* ptr;
    uint16_t size;
    size = app_zigbee_serverList_getTimeouts(ptr);

    app_zigbee_serverList_clearTimeouts();


  // }
}
void sleep_entry_timer_handler(void)
{

  //	HAL_PWR_DisableSleepOnExit();
  //	APP_ZIGBEE_enter_sleep();
  // when wake up, exit sleep
  UTIL_SEQ_SetTask(1U << user_task_zigbee_enter_stop, CFG_SCH_PRIO_0);
}
void APP_ZIGBEE_sleep_mode_entry(void)
{
#ifdef CFG_REMOTE
  HW_TS_Create(1, &TS_SLEEP_ID1, hw_ts_SingleShot, sleep_entry_timer_handler);

  APP_ZIGBEE_sleep_timer_enter();
#endif
}
void APP_ZIGBEE_sleep_timer_enter(int elapse_second)
{
#ifdef CFG_REMOTE
  HW_TS_Stop(TS_SLEEP_ID1);
  HW_TS_Start(TS_SLEEP_ID1, HW_TS_1S * elapse_second);
#endif
}
void APP_ZIGBEE_enter_sleep(void)
{
#ifdef CFG_REMOTE
  HAL_PWR_DisableSleepOnExit();

  UTIL_LPM_SetStopMode(1U << CFG_LPM_APP, UTIL_LPM_ENABLE);
  UTIL_LPM_SetOffMode(1U << CFG_LPM_APP, UTIL_LPM_DISABLE);
#endif
}
void APP_ZIGBEE_exit_sleep(void)
{
#ifdef CFG_REMOTE
  UTIL_LPM_SetOffMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
  UTIL_LPM_SetStopMode(1 << CFG_LPM_APP, UTIL_LPM_DISABLE);
#endif
}

void nnt_timeout_timer(void)
{
	APP_ZIGBEE_exit_sleep();
	uint8_t tmp[2] = {0xff, 0xff};
	APP_ZIGBEE_Broadcast2bytes(tmp);
}
void APP_ZIGBEE_prevent_nwk_nnt_timeout(void)
{
#ifdef CFG_REMOTE
  HW_TS_Create(2, &TS_NNT_TIMEOUT_ID1, hw_ts_Repeated, nnt_timeout_timer);
  HW_TS_Start(TS_NNT_TIMEOUT_ID1, HW_TS_1S * 90);
#endif
}

void APP_ZIGBEE_SetNwkTask(enum ZbStartType type){
  m_nwkJoinType = type;
  if (type == ZbStartTypeForm) {
    UTIL_SEQ_SetTask(1U << user_task_zigbee_nwk_form, CFG_SCH_PRIO_0);
  } else {
    UTIL_SEQ_SetTask(1U << user_task_zigbee_nwk_join, CFG_SCH_PRIO_0);
  }
  // UTIL_SEQ_SetTask(1U << CFG_TASK_ZIGBEE_NETWORK_START, CFG_SCH_PRIO_0);
}
void APP_ZIGBEE_NwkStartForm(void)
{
  printf(__func__);
  m_nwkJoinType = ZbStartTypeForm;
  APP_ZIGBEE_NwkStart();
}
void APP_ZIGBEE_NwkStartJoin(void)
{
  m_nwkJoinType = ZbStartTypeJoin;
  APP_ZIGBEE_NwkStart();
}
static void APP_ZIGBEE_NwkStart()
{
  if (zigbee_app_info.join_status == ZB_STATUS_SUCCESS)
  {
    return;
  }

  struct ZbStartupT config;
  enum ZbStatusCodeT status;

  /* Configure Zigbee Logging */
  ZbSetLogging(zigbee_app_info.zb, ZB_LOG_MASK_LEVEL_5, NULL);

  /* Attempt to join a zigbee network */
  ZbStartupConfigGetProDefaults(&config);

  /* Set the distributed network */
  APP_DBG("Network config startup: %x", m_nwkJoinType);
  config.startupControl = m_nwkJoinType;

  /* Set the TC address to be distributed. */
  config.security.trustCenterAddress = ZB_DISTRIBUTED_TC_ADDR;

  /* Using the Uncertified Distributed Global Key (d0:d1:d2:d3:d4:d5:d6:d7:d8:d9:da:db:dc:dd:de:df) */
  memcpy(config.security.distributedGlobalKey, sec_key_distrib_uncert, ZB_SEC_KEYSIZE);

  config.channelList.count = 1;
  config.channelList.list[0].page = 0;
  config.channelList.list[0].channelMask = 1 << CHANNEL; /*Channel in use */

#ifdef CFG_REMOTE
  /* For Distributed and Touchlink network in RFD application, End device configuration has to be set */
  config.capability &= ~(MCP_ASSOC_CAP_RXONIDLE | MCP_ASSOC_CAP_DEV_TYPE | MCP_ASSOC_CAP_ALT_COORD);
  config.endDeviceTimeout = ZED_SLEEP_TIME_30S;
#else
  config.capability &= ~(MCP_ASSOC_CAP_ALT_COORD);
  config.capability |= MCP_ASSOC_CAP_RXONIDLE | MCP_ASSOC_CAP_DEV_TYPE;
#endif

  /* Using ZbStartupWait (blocking) */
  status = ZbStartupWait(zigbee_app_info.zb, &config);

  APP_DBG("ZbStartup Callback (status = 0x%02x)", status);
  zigbee_app_info.join_status = status;

  if (status == ZB_STATUS_SUCCESS)
  {
    zigbee_app_info.join_delay = 0U;
    zigbee_app_info.init_after_join = true;
    APP_DBG("Startup done !\n");

    ZbPersistNotifyRegister(zigbee_app_info.zb, APP_ZIGBEE_persist_notify_cb, NULL);

    APP_ZIGBEE_persist_notify_cb(zigbee_app_info.zb, NULL);

    APP_ZIGBEE_sleep_mode_entry();
  }
  else
  {
    APP_DBG("Startup failed.");
  }

  /* If Network forming/joining was not successful reschedule the current task to retry the process */
  if (zigbee_app_info.join_status != ZB_STATUS_SUCCESS)
  {

  }
  else
  {
    APP_ZIGBEE_ConfigSettings();
    azcl_registerZdoMsg();
    
    if (m_nwkJoinType == ZbStartTypeForm){
      azcl_broadcastPermitJoinRequest(0x80);
    }
    /* USER CODE END NW_FORM */
  }
}
void APP_ZIGBEE_SendMultibytes(uint8_t*b, uint8_t size, uint16_t nwkAddr, enum ZbApsAddrModeT mode)
{
  uint8_t* buf = malloc(size + 1);
  memcpy(buf, b, size);
  buf[size] = '\0';

	struct ZbApsAddrT dst;
	memset(&dst, 0, sizeof(dst));

//	for (int i = 0; i < size; i++){
//		dst.mode = ZB_APSDE_ADDRMODE_SHORT;
//		dst.endpoint = 1;
//		dst.nwkAddr = azcl_addr_maps.maps[i].nwk_addr;
//		zb_cluster_client_command(&dst, tmp, NULL);
//	}

	dst.mode = mode;
	dst.endpoint = SW1_ENDPOINT;
	dst.nwkAddr = nwkAddr;
	zb_cluster_client_command(cluster_remote_cmd_id, &dst, buf, NULL);

  free(buf);
}
void APP_ZIGBEE_Send2bytes(uint8_t *b, uint16_t nwkAddr, enum ZbApsAddrModeT mode){
	char tmp[3];

	tmp[0] = b[0];
	tmp[1] = b[1];
	tmp[2] = '\0';

	struct ZbApsAddrT dst;
	memset(&dst, 0, sizeof(dst));
	int size = azcl_addr_maps.count;

//	for (int i = 0; i < size; i++){
//		dst.mode = ZB_APSDE_ADDRMODE_SHORT;
//		dst.endpoint = 1;
//		dst.nwkAddr = azcl_addr_maps.maps[i].nwk_addr;
//		zb_cluster_client_command(&dst, tmp, NULL);
//	}

	dst.mode = mode;
	dst.endpoint = SW1_ENDPOINT;
	dst.nwkAddr = nwkAddr;
	zb_cluster_client_command(cluster_remote_cmd_id, &dst, tmp, NULL);
}
void APP_ZIGBEE_Groupsend2bytes(uint8_t *b, uint8_t groupAddress) {
// for (int i = 0; i < m_groupAddressSize; i++) {
//
//	 // should not send consecutively, need some delay after sending one.
//  APP_ZIGBEE_Send2bytes(b, m_groupAddress[i], ZB_APSDE_ADDRMODE_GROUP);
// }

	 APP_ZIGBEE_Send2bytes(b, groupAddress, ZB_APSDE_ADDRMODE_GROUP);
}
void APP_ZIGBEE_Broadcast2bytes(uint8_t* b){

  APP_ZIGBEE_Send2bytes(b, 0xFFFD, ZB_APSDE_ADDRMODE_SHORT);
}

void APP_ZIGBEE_BroadcastExit(){

	char tmp[64];
  uint64_t exitAddr = 0x07e132;

	tmp[0] = 0xde;
	tmp[1] = 0xde;
	sprintf(&tmp[2], "%x", exitAddr);
  tmp[10] = '\0';

	struct ZbApsAddrT dst;
	memset(&dst, 0, sizeof(dst));
	int size = azcl_addr_maps.count;

//	for (int i = 0; i < size; i++){
//		dst.mode = ZB_APSDE_ADDRMODE_SHORT;
//		dst.endpoint = 1;
//		dst.nwkAddr = azcl_addr_maps.maps[i].nwk_addr;
//		zb_cluster_client_command(&dst, tmp, NULL);
//	}

	dst.mode = ZB_APSDE_ADDRMODE_SHORT;
	dst.endpoint = SW1_ENDPOINT;
	dst.nwkAddr = 0xfffd;
	zb_cluster_client_command(cluster_remote_cmd_id, &dst, tmp, NULL);
}
void APP_ZIGBEE_BroadcastPermitJoin(){

	char tmp[3];

	tmp[0] = 0xdd;
	tmp[1] = 0xdd;
	tmp[2] = '\0';

	struct ZbApsAddrT dst;
	memset(&dst, 0, sizeof(dst));
	int size = azcl_addr_maps.count;

//	for (int i = 0; i < size; i++){
//		dst.mode = ZB_APSDE_ADDRMODE_SHORT;
//		dst.endpoint = 1;
//		dst.nwkAddr = azcl_addr_maps.maps[i].nwk_addr;
//		zb_cluster_client_command(&dst, tmp, NULL);
//	}

	dst.mode = ZB_APSDE_ADDRMODE_SHORT;
	dst.endpoint = SW1_ENDPOINT;
	dst.nwkAddr = 0xfffd;
	zb_cluster_client_command(cluster_remote_cmd_id, &dst, tmp, NULL);
}

void APP_ZIGBEE_registerCallback(app_zigbee_logic_callbacks_t* callback){
  m_logicCallbacks = callback;
}

void APP_ZIGBEE_ConfigGA_Remote(void) {

	APP_DBG("Configure group address for remote");
  m_groupAddressSize = 2;

  m_groupAddress[0] = 1;
  m_groupAddress[1] = 2;

  for (int i = 0; i < m_groupAddressSize; i++) {
    azcl_addGroup(SW1_ENDPOINT, m_groupAddress[i]);
  }
}

// void APP_ZIGBEE_Indicate(uint16_t addr) 
// {
//   char tmp[10];
//   tmp[0] = CMD_INDICATE;
//   tmp[1] = CMD_INDICATE;

//   sprintf(&tmp[2], "%4x", addr);
//   APP_ZIGBEE_SendMultibytes(tmp, 6, m_parent, ZB_APSDE_ADDRMODE_SHORT);
// }

// void APP_ZIGBEE_Leave(uint16_t addr)
// {
//   leaveRequestFlag = true;
//   leaveRequestAddr = addr;

//   char tmp[10];
//   tmp[0] = CMD_DISCONNECT;
//   tmp[1] = CMD_DISCONNECT;

//   sprintf(&tmp[2], "%4x", addr);
//   APP_ZIGBEE_SendMultibytes(tmp, 6, m_parent, ZB_APSDE_ADDRMODE_SHORT);
// }

/* USER CODE END FD_LOCAL_FUNCTIONS */
