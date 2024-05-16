/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    App/app_zigbee.h
  * @author  MCD Application Team
  * @brief   Header for Zigbee Application.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_ZIGBEE_H
#define APP_ZIGBEE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "tl.h"
#include "tl_zigbee_hci.h"

/* USER CODE BEGIN Includes */
#include "app_zigbee_config.h"
#include "app_zigbee_nvm.h"
#include "app_zigbee_clusters.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/

/* Application errors                 */
/*------------------------------------*/

/*
 *  List of all errors tracked by the Zigbee application
 *  running on M4. Some of these errors may be fatal
 *  or just warnings
 */
typedef enum
{
  ERR_ZIGBE_CMD_TO_M0,
/* USER CODE BEGIN ERROR_APPLI_ENUM */

/* USER CODE END ERROR_APPLI_ENUM */
  ERR_ZIGBEE_CHECK_WIRELESS
} ErrAppliIdEnum_t;
/* USER CODE BEGIN ET */
typedef struct {
  void (*onNetworkInitSuccess)(void);
} app_zigbee_logic_callbacks_t;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */
union cache
{
  uint8_t  U8_data[ST_PERSIST_MAX_ALLOC_SZ];     // in bytes
  uint32_t U32_data[ST_PERSIST_MAX_ALLOC_SZ/4U]; // in U32 words
};

extern union cache cache_persistent_data;

/* USER CODE END EV */

/* Exported macros ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define CMD_INDICATE  0xdb
#define CMD_DISCONNECT  0xdc
#define CMD_PERMITJOIN  0xdd
/* USER CODE END EM */

/* Exported functions ------------------------------------------------------- */
void APP_ZIGBEE_Init(void);
void APP_ZIGBEE_Error(uint32_t ErrId, uint32_t ErrCode);
void APP_ZIGBEE_RegisterCmdBuffer(TL_CmdPacket_t *p_buffer);
void APP_ZIGBEE_TL_INIT(void);
void Pre_ZigbeeCmdProcessing(void);

void APP_ZIGBEE_NwkStartForm(void);
void APP_ZIGBEE_NwkStartJoin(void);

/* USER CODE BEGIN EF */
void APP_ZIGBEE_registerCallback(app_zigbee_logic_callbacks_t* callback);
void APP_ZIGBEE_sleep_timer_enter();
void APP_ZIGBEE_SetNwkTask(enum ZbStartType type);
void APP_ZIGBEE_Broadcast2bytes(uint8_t* b);
void APP_ZIGBEE_BroadcastPermitJoin();
void APP_ZIGBEE_BroadcastExit();

void APP_ZIGBEE_Groupsend2bytes(uint8_t *b, uint8_t groupAddress);
void APP_ZIGBEE_Send2bytes(uint8_t *b, uint16_t groupAddr, enum ZbApsAddrModeT mode);
void APP_ZIGBEE_Broadcast2bytes(uint8_t* b);

void APP_ZIGBEE_enter_sleep(void);
bool APP_ZIGBEE_persist_save();
extern uint16_t m_parent;

extern clusterId_t cluster_remote_cmd_id;
extern clusterId_t cluster_device_cmd_id;
extern clusterId_t cluster_device_tx_id;
extern clusterId_t cluster_device_rx_id;

/* USER CODE END EF */
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* APP_ZIGBEE_H */
