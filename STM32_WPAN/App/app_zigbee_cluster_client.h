#pragma once

#include <string.h>

#include "zigbee.h"
#include "zcl/zcl.h"
#include "app_zigbee_clusters.h"

clusterId_t zb_cluster_client_alloc(
		struct ZigBeeT *zb,
		uint8_t endpoint);

void zb_cluster_client_setOnResponseCallback(
	clusterId_t id, 
	void (*response_callback)(struct ZbZclCommandRspT *rsp, 
	void *arg));

enum ZclStatusCodeT zb_cluster_client_command(
	clusterId_t id,
	struct ZbApsAddrT *dst,
	char *string,
	void *arg);

enum ZclStatusCodeT zb_cluster_client_command_broadcast(
	clusterId_t id,
	uint16_t shortAddr,
	char* string,
	void* arg
);
