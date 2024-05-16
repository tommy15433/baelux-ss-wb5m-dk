#pragma once
#include <string.h>

#include "zigbee.h"
#include "zcl/zcl.h"
#include "app_zigbee_clusters.h"

struct zb_cluster_server_callback_t {
    enum ZclStatusCodeT (*set_custom_ls_command)(
		struct ZbZclClusterT *clusterPtr,
		struct set_custom_ls_command_req_t *cmd_req,
		struct ZbZclAddrInfoT *src_info,
		void *arg);
};

typedef struct zcl_custom_ls_server_cluster_t_ {
    struct ZbZclClusterT cluster;
    struct zb_cluster_server_callback_t callbacks;
}zcl_custom_ls_server_cluster_t;

clusterId_t zb_cluster_server_alloc(
	struct ZigBeeT *zb,
	uint8_t endpoint,
	struct zb_cluster_server_callback_t *callbacks,
	void *arg);

enum ZclStatusCodeT zb_cluster_server_sendCommandRsp(
	struct ZbZclClusterT* clusterPtr,
	struct ZbZclAddrInfoT *dst_info,
    struct custom_ls_command_rsp_t *rsp);
