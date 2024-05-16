#include "app_zigbee_cluster_client.h"



uint16_t m_curid = 0;
struct ZigBeeT* m_zigbee;

struct client_t{
	struct ZbZclClusterT* cluster;
	uint8_t endpoint;

	void (*response_callback)(struct ZbZclCommandRspT *rsp, void *arg);
} m_client[maxid];



/*
 *
 * Allocation Functions
 *
 */
uint16_t zb_cluster_client_alloc(
	struct ZigBeeT *zb,
	uint8_t endpoint)
{
    if (m_curid >= maxid) {
        return invalidid;
    } 
	m_zigbee = zb;

	m_client[m_curid].endpoint = endpoint;

	m_client[m_curid].cluster = ZbZclClusterAlloc(zb, sizeof(struct ZbZclClusterT), ZCL_CLUSTER_CUSTOM_LS, endpoint, ZCL_DIRECTION_TO_CLIENT);
	m_client[m_curid].response_callback = NULL;

    if (m_client[m_curid].cluster == NULL) {
        return NULL;
    }

    /* This is the flags for Fragmentation */
//    m_client[m_curid].cluster->txOptions |= (uint16_t)(ZB_APSDE_DATAREQ_TXOPTIONS_SECURITY | ZB_APSDE_DATAREQ_TXOPTIONS_ACK | ZB_APSDE_DATAREQ_TXOPTIONS_FRAG);
    m_client[m_curid].cluster->txOptions |= (uint16_t)(ZB_APSDE_DATAREQ_TXOPTIONS_SECURITY | ZB_APSDE_DATAREQ_TXOPTIONS_FRAG);
    m_client[m_curid].cluster->txOptions &= ~(uint16_t)(ZB_APSDE_DATAREQ_TXOPTIONS_ACK);

    //ZbZclClusterSetTxOptions(&clusterPtr->cluster, ZB_APSDE_DATAREQ_TXOPTIONS_FRAG);
    (void)ZbZclAttrIntegerWrite(m_client[m_curid].cluster, ZCL_GLOBAL_ATTR_CLUSTER_REV, 1);

    ZbZclClusterAttach(m_client[m_curid].cluster);

    return m_curid++;
}
/*
 *
 * Set Callback Functions
 *
 */
void zb_cluster_client_setOnResponseCallback(uint16_t id, void (*response_callback)(struct ZbZclCommandRspT *rsp, void *arg)){
	m_client[id].response_callback = response_callback;
}

/*
 *
 * Command Functions
 *
 */
enum ZclStatusCodeT zb_cluster_client_command(
    uint16_t id,
	struct ZbApsAddrT *dst,
	char *string,
	void *arg)
{
    struct ZbZclClusterCommandReqT req;
    struct ZbZclClusterT *cluster_;
    uint8_t payload[2050];
    uint16_t len;

    cluster_ = m_client[id].cluster;

    len = (uint16_t)strlen(string);
    if (len > 2048) {
      return ZCL_STATUS_INVALID_VALUE;
    }

    putle16(&payload[0], len);
    memcpy(&payload[2], string, len);

    (void)memset(&req, 0, sizeof(req));
    req.dst = *dst;
    req.cmdId = ZCL_SET_CUSTOM_LS_COMMAND_REQ;
    req.noDefaultResp = /*ZCL_NO_DEFAULT_RESPONSE_FALSE*/ZCL_NO_DEFAULT_RESPONSE_TRUE;
    req.payload = payload;
    req.length = len + 2;

    ZbZclClusterCommandReq(cluster_, &req, m_client[id].response_callback, arg);
//    ZbZclClusterCommandReq(cluster_, &req, NULL, arg);

    return ZCL_STATUS_SUCCESS;
}

enum ZclStatusCodeT zb_cluster_client_command_broadcast(
	clusterId_t id,
	uint16_t shortAddr,
	char* string,
	void* arg
)
{
    struct ZbZclClusterCommandReqT req;
    struct ZbZclClusterT *cluster_;
    uint8_t payload[2050];
    uint16_t len;

    struct ZbApsAddrT dst = {
        .endpoint = m_client[id].endpoint,
        .mode = ZB_APSDE_ADDRMODE_SHORT,
        .nwkAddr = shortAddr
    };

    cluster_ = m_client[id].cluster;

    len = (uint16_t)strlen(string);
    if (len > 2048) {
      return ZCL_STATUS_INVALID_VALUE;
    }

    putle16(&payload[0], len);
    memcpy(&payload[2], string, len);

    (void)memset(&req, 0, sizeof(req));
    req.dst = dst;
    req.cmdId = ZCL_SET_CUSTOM_LS_COMMAND_REQ;
    req.noDefaultResp = /*ZCL_NO_DEFAULT_RESPONSE_FALSE*/ZCL_NO_DEFAULT_RESPONSE_TRUE;
    req.payload = payload;
    req.length = len + 2;

    ZbZclClusterCommandReq(cluster_, &req, m_client[id].response_callback, arg);
//    ZbZclClusterCommandReq(cluster_, &req, NULL, arg);

    return ZCL_STATUS_SUCCESS;

}