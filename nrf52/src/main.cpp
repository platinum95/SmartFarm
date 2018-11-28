/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// #define LOG_MODULE_NAME net_tb_device
// #define NET_LOG_LEVEL LOG_LEVEL_DBG

#include <zephyr.h>
#include <board.h>
#include <device.h>

#include <string.h>
#include <stdio.h>

#include "tb_pubsub.h"
#include "btHelper.h"
#include "config.h"
#include "devConf.h"
#include <type_traits>
#include <pwm.h>

// RPC callback for updating overall device settings.
// Common between all devices for easier remote configuration.
// Not yet implemented.
char * updateSettings( char* json, int jsonLen ){
    printf( "Updating settings...\n" );
    return 0;
}

RPCB( settingsRpc, updateSettings, updateSettings );

// Retrieve payloads from all configured sensors. Concatentate
// and send the data to the MQTT broker.
void sensorWorker() {
    static char jsonBuff[ 142 ];
    jsonBuff[ 0 ] = 0;
    snprintf( jsonBuff, 142, "{" );
    for( auto sensor : sensors ){
        if( const char * sensorPayload = sensor->requestPayload() ){
            snprintf( jsonBuff + strlen( jsonBuff ), 
                      142, "%s,", sensorPayload );
        }
    }
    if( strlen( jsonBuff ) < 2 ){
        return;    // Abort if some error has occurred
    }
    // Remove final separator
    jsonBuff[ strlen( jsonBuff ) - 1 ] = 0;
    snprintf( jsonBuff + strlen( jsonBuff ), 142, "}" );
    tb_publish_telemetry( jsonBuff );
    printf( "telemetry published: %s\n", jsonBuff );
}

// Send attribute update to MQTT/TB
void attributeWorker(){
    static char payload[ 128 ];

    /* Formulate the JSON payload for the attribute update */
    snprintf( payload, sizeof(payload), 
        "{\"firmware_version\":\"%s\", \"serial_number\":\"%s\", \"uptime\":\"%d\"}",
        "1.2.3",
        "andersm001",
        (uint32_t)k_uptime_get_32() / 1000 );

    tb_publish_attributes( payload );
    printf( "Attribs published: %s\n", payload );
}

// The work task that updates TB
void interWorker( struct k_work *work ){
    attributeWorker();
    sensorWorker();
}


K_WORK_DEFINE( interWork, interWorker );
// The timer event that queues the work task
void interEvent( struct k_timer *attribute_timer ){
    k_work_submit( &interWork );
}

void main(void){
    // Initialise all sensors and lights for this device
    lights_init();
    for( auto sensor : sensors )
        sensor->initialise();

    // Start BT connection
    int rc = networkSetup();
    printf( "Network setup - %i\n", rc );
    if (rc < 0) {
        return;
    }

    // Get number of RPC callbacks
    int rpcLen = std::extent< decltype( rpcList ) >::value;
    // Start the TB thread, pass along RPC callbacks
    tb_pubsub_start( rpcList, rpcLen );

    // Start the attrib/telemetry timer event
    struct k_timer interTimer;
    k_timer_init( &interTimer, interEvent, NULL );
    k_timer_start( &interTimer, K_SECONDS( 5 ), K_SECONDS( 5 ) );

    // Suspend the main thread
    k_thread_suspend( k_current_get() );

}


