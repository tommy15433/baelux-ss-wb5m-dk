#include "app_zigbee_clusters.h"


static struct ZigbeeT* m_zigbee;
static uint8_t m_endpoint;
static struct ZbZclClusterT* m_cluster;
zcl_custom_ls_server_cluster_t* cluster_and_callback;

struct ZbZclClusterT cluster;
struct zb_cluster_server_callback_t callbacks;

static const struct ZbZclAttrT zcl_customls_server_attr_list[] = {
    {
        ZCL_CUSTOM_LS_ATTR, ZCL_DATATYPE_STRING_CHARACTER,
        ZCL_ATTR_FLAG_REPORTABLE|ZCL_ATTR_FLAG_PERSISTABLE, 3, NULL, {0, 0}, {0, 0}
    },
};

struct ZbZclClusterT * ccServer_alloc(
	struct ZigBeeT *zb,
	uint8_t endpoint,
	struct zb_cluster_server_callback_t *callbacks,
	void *arg)
{

	m_zigbee = zb;
	m_endpoint = endpoint;
	m_server.cluster_and_callback =
			ZbZclClusterAlloc(
				zb,
				sizeof(zcl_custom_ls_server_cluster_t),
				ZCL_CLUSTER_CUSTOM_LS,
				endpoint,
				ZCL_DIRECTION_TO_SERVER);

    if (m_server.cluster_and_callback == NULL) {
        return NULL;
    }

    m_server.cluster_and_callback->cluster.txOptions |= ZB_APSDE_DATAREQ_TXOPTIONS_SECURITY;

    memset(&m_server.cluster_and_callback->callbacks, 0, sizeof(m_server.cluster_and_callback->callbacks));
    if (callbacks != NULL) {
        memcpy(&m_server.cluster_and_callback->callbacks, callbacks, sizeof(m_server.cluster_and_callback->callbacks));
    }

    m_server.cluster_and_callback->cluster.command = custom_ls_command_handler;

    /* not used in our case */
#if 1
    if (ZbZclAttrAppendList(&m_server.cluster_and_callback->cluster, zcl_customls_server_attr_list,\
      ZCL_ATTR_LIST_LEN(zcl_customls_server_attr_list)) != ZCL_STATUS_SUCCESS) {
        ZbZclClusterFree(&m_server.cluster_and_callback->cluster);
        return NULL;
    }
#endif

    (void)ZbZclAttrIntegerWrite(&m_server.cluster_and_callback->cluster, ZCL_GLOBAL_ATTR_CLUSTER_REV, 1);

    ZbZclClusterSetCallbackArg(&m_server.cluster_and_callback->cluster, arg);

    ZbZclClusterAttach(&m_server.cluster_and_callback->cluster);
    return &m_server.cluster_and_callback->cluster;
}


static enum ZclStatusCodeT custom_ls_command_handler(
	struct ZbZclClusterT *clusterPtr,
	struct ZbZclHeaderT *zclHdrPtr,
	struct ZbApsdeDataIndT *dataIndPtr)
{
    zcl_custom_ls_server_cluster_t *custom_ls_cluster = (struct zcl_custom_ls_server_cluster_t *)clusterPtr;
    struct ZbZclAddrInfoT src_info;
    uint8_t cmd_id = zclHdrPtr->cmdId;
    enum ZclStatusCodeT return_status = ZCL_STATUS_SUCCESS_NO_DEFAULT_RESPONSE;
    char string[2049];
    uint16_t len;

    (void)memset(&src_info, 0, sizeof(src_info));
    src_info.addr = dataIndPtr->src;
    src_info.seqnum = zclHdrPtr->seqNum;
    //src_info.tx_options = dataIndPtr->securityStatus.;

    switch (cmd_id) {
        case ZCL_SET_CUSTOM_LS_COMMAND_REQ:
        {
            struct set_custom_ls_command_req_t req;
            memset(&req, 0, sizeof(req));

            if (custom_ls_cluster->callbacks.set_custom_ls_command == NULL) {
                return_status = ZCL_STATUS_UNSUPP_COMMAND;
                break;
            }

            len = pletoh16(&dataIndPtr->asdu[0]);
            memcpy(string, &dataIndPtr->asdu[2], len);
            string[len] = '\0';
            req.string = string;

            return_status = custom_ls_cluster->callbacks.set_custom_ls_command(clusterPtr, &req, &src_info, clusterPtr->app_cb_arg);
            break;
        }
        default:
            return_status = ZCL_STATUS_UNSUPP_COMMAND;
            break;
    }
    return return_status;
}

/*
 *
 * Command Functions
 *
 */


enum ZclStatusCodeT zb_cluster_server_sendCommandRsp(
	struct ZbZclClusterT *clusterPtr,
	struct ZbZclAddrInfoT *dst_info,
    struct custom_ls_command_rsp_t *rsp)
{
    uint8_t rsp_payload[2];
    unsigned int length = 0U;
    struct ZbApsBufT bufv[1];
    dst_info->tx_options = ZB_APSDE_DATAREQ_TXOPTIONS_SECURITY;
    /* Form the payload */
    putle16(&rsp_payload[length], rsp->one);
    length += 64;

    // bufv[0].data = rsp_payload;
    // bufv[0].len = length;

    bufv[0].data = rsp->one;
    bufv[0].len = 64;

    return ZbZclClusterCommandRsp(clusterPtr, dst_info, ZCL_CUSTOM_LS_COMMAND_RSP, bufv, 1U);
}
