/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// #define LOG_MODULE_NAME net_tb_device
// #define NET_LOG_LEVEL LOG_LEVEL_DBG

#include <zephyr.h>

#include <net/net_context.h>

#include <misc/printk.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#if defined(CONFIG_NET_L2_BT)
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#endif

#include "tb_pubsub.h"
#include "sensors.h"
#include "config.h"
#include "devConf.h"

void main(void){
	for( auto sensor : sensors )
		sensor->initialise();

	while( 1 ){
		k_sleep( 1000 );
		for( auto sensor : sensors )
			printf( "%s\n", sensor->requestPayload() );
		printf( "\n" );		
	}
}
