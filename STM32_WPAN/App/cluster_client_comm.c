#include "cluster_comm.h"

static struct ZigbeeT* m_zigbee;
static uint8_t m_endpoint;
static struct ZbZclClusterT* m_cluster;
void (*response_callback)(struct ZbZclCommandRspT *rsp, void *arg);

//struct ZbZclClusterT* ccClient_alloc(struct ZigbeeT *zb, uint8_t endpoint)
//{
//	return NULL;
////    m_zigbee = zb;
////	m_endpoint = endpoint;
////
////	m_cluster = ZbZclClusterAlloc(zb, sizeof(struct ZbZclClusterT), ZCL_CLUSTER_CUSTOM_LS, endpoint, ZCL_DIRECTION_TO_CLIENT);
////	response_callback = NULL;
////
////    if (m_cluster == NULL) {
////        return NULL;
////    }
////
////    /* This is the flags for Fragmentation */
//////    m_client.cluster->txOptions |= (uint16_t)(ZB_APSDE_DATAREQ_TXOPTIONS_SECURITY | ZB_APSDE_DATAREQ_TXOPTIONS_ACK | ZB_APSDE_DATAREQ_TXOPTIONS_FRAG);
////    m_cluster->txOptions |= (uint16_t)(ZB_APSDE_DATAREQ_TXOPTIONS_SECURITY | ZB_APSDE_DATAREQ_TXOPTIONS_FRAG);
////    m_cluster->txOptions &= ~(uint16_t)(ZB_APSDE_DATAREQ_TXOPTIONS_ACK);
////
////    //ZbZclClusterSetTxOptions(&clusterPtr->cluster, ZB_APSDE_DATAREQ_TXOPTIONS_FRAG);
////    (void)ZbZclAttrIntegerWrite(m_cluster, ZCL_GLOBAL_ATTR_CLUSTER_REV, 1);
////
////    ZbZclClusterAttach(m_cluster);
////    return m_cluster;
//
//}

//struct ZbZclClusterT* ccClient_alloc(struct ZigbeeT *zb, uint8_t endpoint){
//	return NULL;
//}
enum ZclStatusCodeT ccClient_commnad( struct ZbApsAddrT *dst, char *string, void *arg)
{
    struct ZbZclClusterCommandReqT req;
    struct ZbZclClusterT *cluster_;
    uint8_t payload[2050];
    uint16_t len;

    cluster_ = m_cluster;

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

    ZbZclClusterCommandReq(cluster_, &req, response_callback, arg);
//    ZbZclClusterCommandReq(cluster_, &req, NULL, arg);

    return ZCL_STATUS_SUCCESS;
}

void ccClient_setResponseCallback(
    void (*callback)(struct ZbZclCommandRspT* rsp, void* arg)
) {
    response_callback = callback;
}
