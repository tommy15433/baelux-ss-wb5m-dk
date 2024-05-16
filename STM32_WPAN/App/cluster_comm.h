#pragma once

#include <string.h>

#include "zigbee.h"
#include "zcl/zcl.h"
#include "app_zigbee_clusters.h"

struct ZbZclClusterT* ccServer_alloc( struct ZigBeeT *zb, uint8_t endpoint, struct zb_cluster_server_callback_t *callbacks, void *arg);

struct ZbZclClusterT* ccClient_alloc(struct ZigbeeT *zb, uint8_t endpoint);

enum ZclStatusCodeT ccClient_commnad( struct ZbApsAddrT *dst, char *string, void *arg);
void ccClient_setResponseCallback(void (*callback)(struct ZbZclCommandRspT* rsp, void* arg));

struct ZbZclClusterT *zb_cluster_client_alloc(
		struct ZigBeeT *zb,
		uint8_t endpoint);

void zb_cluster_client_setOnResponseCallback(
		void (*response_callback)(struct ZbZclCommandRspT *rsp, void *arg));

enum ZclStatusCodeT zb_cluster_client_command(
		struct ZbApsAddrT *dst,
		char *string,
		void *arg);

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

struct ZbZclClusterT * zb_cluster_server_alloc(
	struct ZigBeeT *zb,
	uint8_t endpoint,
	struct zb_cluster_server_callback_t *callbacks,
	void *arg);

enum ZclStatusCodeT zb_cluster_server_sendCommandRsp(
	struct ZbZclClusterT *clusterPtr,
	struct ZbZclAddrInfoT *dst_info,
    struct custom_ls_command_rsp_t *rsp);
