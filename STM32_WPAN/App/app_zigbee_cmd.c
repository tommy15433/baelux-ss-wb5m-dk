#include <stdint.h>

#include "app_zigbee_cmd.h"
#include "app_zigbee_cluster_client.h"
#include "app_zigbee_cluster_server.h"
#include "app_zigbee_customlib.h"

static uint8_t m_buf[1024];
static void __attribute__((unused)) cmd_bc_c2s(uint8_t* buf, uint8_t size)
{
    memcpy(m_buf, buf, size);
    m_buf[size] = '\0';
    if (zb_cluster_client_command_broadcast(cluster_remote_cmd_id, 0xfffd, (char*)m_buf, NULL) 
        != ZCL_STATUS_SUCCESS)
        {
            
        }
}
static void __attribute__((unused)) cmd_uc_c2p(uint8_t* buf, uint8_t size)
{

    uint16_t pa = azcl_getParentAddr();

    printf("c2p %4x\r\n", pa);
    memcpy(m_buf, buf, size);
    m_buf[size] = '\0';
    if (zb_cluster_client_command_broadcast(cluster_remote_cmd_id, pa, (char*)m_buf, NULL) 
        != ZCL_STATUS_SUCCESS)
        {
            
        }
}
static void __attribute__((unused)) cmd_uc_s2s(uint8_t* buf, uint8_t size, uint16_t addr)
{
    memcpy(m_buf, buf, size);
    m_buf[size] = '\0';
    if (zb_cluster_client_command_broadcast(cluster_device_tx_id, addr, (char*)m_buf, NULL) 
        != ZCL_STATUS_SUCCESS)
        {
            
        }
}
static void __attribute__((unused)) cmd_bc_s2s(uint8_t* buf, uint8_t size)
{
    memcpy(m_buf, buf, size);
    m_buf[size] = '\0';
    if (zb_cluster_client_command_broadcast(cluster_device_tx_id, 0xfffd, (char*)m_buf, NULL) 
        != ZCL_STATUS_SUCCESS)
        {
            
        }
}
/**
 * @brief 
 * 
 * @param addr 
 */
void APP_ZIGBEE_Indicate_s2s(uint16_t addr) 
{
    uint8_t tmp[10];
    tmp[0] = CMD_INDICATE;
    tmp[1] = CMD_INDICATE;

    sprintf((char*)&tmp[2], "%4x", addr);
    cmd_bc_s2s(tmp, 6);
}
/**
 * @brief 
 * 
 */
void APP_ZIGBEE_Indicate_c2s(void) 
{
  uint8_t tmp[10];
  tmp[0] = CMD_INDICATE;
  tmp[1] = CMD_INDICATE;

  cmd_uc_c2p(tmp, 2);
}
/**
 * @brief 
 * 
 * @param addr 
 */
void APP_ZIGBEE_Leave_c2s(void)
{
    uint8_t tmp[10];
    tmp[0] = CMD_DISCONNECT;
    tmp[1] = CMD_DISCONNECT;

    cmd_uc_c2p(tmp, 2);
}

void APP_ZIGBEE_Leave_s2s(uint16_t addr)
{
    uint8_t tmp[10];
    tmp[0] = CMD_DISCONNECT;
    tmp[1] = CMD_DISCONNECT;

    sprintf((char*)&tmp[2], "%4x", addr);
    cmd_bc_s2s(tmp, 6);
}

void APP_ZIGBEE_Broadcast2byte_c2s(uint8_t b1, uint8_t b2)
{
    uint8_t tmp[10];
    tmp[0] = b1;
    tmp[1] = b2;
    cmd_bc_c2s(tmp, 2);
}
