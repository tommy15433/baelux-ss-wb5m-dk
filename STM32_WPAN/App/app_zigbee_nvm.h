#ifndef __APP_ZIGBEE_NVM_H_
#define __APP_ZIGBEE_NVM_H_

#include "app_common.h"
#include "dbg_trace.h"
#include "stm_logging.h"
#include "zcl/zcl.h"

#include "hw_flash.h"
#include "ee_override.h"


/*
 * Configurations
 */
// set to 1 to use NVM, otherwise uses cache
#define CFG_NVM	1


/*
 * parameters
 */

/*
	NVM PARAMETERES

	CFG_EE_BANK0_SIZE is the size allocated for the EE bank0 it should be
	the considered as the max Flash size for all computation and <= of the
	allocated size within the scatterfile in bytes

	CFG_NVM_BASE_ADDRESS : offset to add to the base flash address to get the
	beginning of the NVM (shall be within  allocation range of scatterfile)

	ST_PERSIST_MAX_ALLOC_SZ : max size of the RAM cache in bytes
	either an arbitrary choice or the CFG_NVM_MAX_SIZE

	ST_PERSIST_FLASH_DATA_OFFSET : offset in bytes of zigbee data
	(U8[4] for length  - 1st data[]...)
	CFG_NB_OF_PAGE : Number of page of flash to use

	CFG_NVM_MAX_SIZE : Max allocable size in byte for NVM
	Flash size/8 * (number of element by page in byte)
	ZIGBEE_DB_START_ADDR: beginning of zigbee NVM

	CFG_EE_AUTO_CLEAN : Clean the flash automatically when needed
*/

#define CFG_NB_OF_PAGE                          (16U)
#define CFG_EE_BANK0_SIZE                       (CFG_NB_OF_PAGE * HW_FLASH_PAGE_SIZE)
#define CFG_NVM_BASE_ADDRESS                    ( 0x70000U )
#define CFG_EE_BANK0_MAX_NB                     (1000U)                  // In U32 words
#define ST_PERSIST_MAX_ALLOC_SZ                 (4U*CFG_EE_BANK0_MAX_NB) // Max data in bytes
#define ST_PERSIST_FLASH_DATA_OFFSET            (4U)
#define ZIGBEE_DB_START_ADDR                    (0U)
#define CFG_EE_AUTO_CLEAN                       (1U)

/*
 * types
 */

void APP_ZIGBEE_NVM_Init(void);
bool APP_ZIGBEE_NVM_Read(uint32_t* ptr);
bool APP_ZIGBEE_NVM_Write(uint32_t* ptr);
void APP_ZIGBEE_NVM_Erase(void);

#endif
