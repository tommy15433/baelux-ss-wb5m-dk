#include <app_zigbee_nvm.h>

static int persistNumWrites = 0;

/*
 * NVM Functions
 */
/* NVM variables */

/* cache in uninit RAM to store/retrieve persistent data */


/* timer to delay reading attribute back from persistence */
//static uint8_t TS_ID1;
static uint8_t TS_ID2;

/*
 * NVM FUNCTIONS
 */

/**
 * @brief  Init the NVM
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_NVM_Init(void)
{
  int eeprom_init_status;

  APP_DBG("Flash starting address = %x",HW_FLASH_ADDRESS  + CFG_NVM_BASE_ADDRESS);
  eeprom_init_status = EEE_Init( 0 , HW_FLASH_ADDRESS + CFG_NVM_BASE_ADDRESS );

  if(eeprom_init_status != EEE_OK)
  {
    /* format NVM since init failed */
    APP_DBG("init failed. format start");
    eeprom_init_status= EEE_Init( 1, HW_FLASH_ADDRESS + CFG_NVM_BASE_ADDRESS );
  }
  APP_DBG("EE_init status = %d",eeprom_init_status);

} /* APP_ZIGBEE_NVM_Init */

/**
*@brief  Read the persistent data from NVM
* @param  None
* @retval true if success , false if failed
*/
bool APP_ZIGBEE_NVM_Read(uint32_t* ptr)
{
    uint16_t num_words = 0;
    bool status = true;
    int ee_status = 0;
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGSERR | FLASH_FLAG_WRPERR | FLASH_FLAG_OPTVERR);

    /* Read the data length from cache */
    ee_status = EEE_Read(0, ZIGBEE_DB_START_ADDR, ptr);
    if (ee_status != EEE_OK)
    {
        APP_DBG("Read -> persistent data length not found ERASE to be done - Read Stopped");
        status = false;
    }
      /* Check length is not too big nor zero */
    else if((ptr[0] == 0) ||
            (ptr[0]> ST_PERSIST_MAX_ALLOC_SZ))
    {
            APP_DBG("No data or too large length : %d", ptr[0]);
            status = false;
    }
        /* Length is within range */
    else
    {
           /* Adjust the length to be U32 aligned */
            num_words = (uint16_t) (ptr[0]/4) ;
            if (ptr[0] % 4 != 0)
            {
                num_words++;
            }

            /* copy the read data from Flash to cache including length */
            for (uint16_t local_length = 1; local_length <= num_words; local_length++)
            {
                /* read data from first data in U32 unit */
                ee_status = EEE_Read(0, local_length + ZIGBEE_DB_START_ADDR, &ptr[local_length] );
                if (ee_status != EEE_OK)
                {
                    APP_DBG("Read not found leaving");
                    status = false;
                    break;
                }
            }
    }

    HAL_FLASH_Lock();
    if(status)
    {
        APP_DBG("READ PERSISTENT DATA LEN = %d", ptr[0]);
    }
    return status;
} /* APP_ZIGBEE_NVM_Read */

/**
 * @brief  Write the persistent data in NVM
 * @param  None
 * @retval None
 */
bool APP_ZIGBEE_NVM_Write(uint32_t* ptr)
{
    int ee_status = 0;

    uint16_t num_words;
    uint16_t local_current_size;


    num_words = 1U; /* 1 words for the length */
    num_words += (uint16_t) (ptr[0]/4);


    /* Adjust the length to be U32 aligned */
    if (ptr[0] % 4 != 0)
    {
        num_words++;
    }

    //save data in flash
    for (local_current_size = 0; local_current_size < num_words; local_current_size++)
    {
        ee_status = EEE_Write(0, (uint16_t)local_current_size + ZIGBEE_DB_START_ADDR, ptr[local_current_size]);
        if (ee_status != EEE_OK)
        {
           if(ee_status == EEE_CLEAN_NEEDED) /* Shall not be there if CFG_EE_AUTO_CLEAN = 1*/
           {
              APP_DBG("CLEAN NEEDED, CLEANING");
              EEE_Clean(0,0);
           }
           else
           {
              /* Failed to write , an Erase shall be done */
              APP_DBG("APP_ZIGBEE_NVM_Write failed @ %d status %d", local_current_size,ee_status);
              break;
           }
        }
    }


    if(ee_status != EEE_OK)
    {
       APP_DBG("WRITE STOPPED, need a FLASH ERASE");
       return false;
    }

    APP_DBG("WRITTEN PERSISTENT DATA LEN = %d",ptr[0]);
    return true;

} /* APP_ZIGBEE_NVM_Write */

/**
 * @brief  Erase the NVM
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_NVM_Erase(void)
{
   EEE_Init(1, HW_FLASH_ADDRESS + CFG_NVM_BASE_ADDRESS); /* Erase Flash */
} /* APP_ZIGBEE_NVM_Erase */

/**
 * @brief  read on off attribute after a startup form persistence
 * @param  clusterPtr: pointer to cluster
 *
 * @retval stack status code
 */


