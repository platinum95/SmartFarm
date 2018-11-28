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


char * updateSettings( char* json, int jsonLen ){
    printf( "Updating settings...\n" );
    return 0;
}

RpCallback settingsRpc = {
    callback    : &updateSettings,
    name        : "updateSettings"
};

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


void interWorker( struct k_work *work ){
    attributeWorker();
    sensorWorker();
}


K_WORK_DEFINE( interWork, interWorker );

void interEvent( struct k_timer *attribute_timer ){
    k_work_submit( &interWork );
}

#if defined(CONFIG_SOC_QUARK_SE_C1000) || defined(CONFIG_SOC_QUARK_D2000)
#define PWM_DEV CONFIG_PWM_QMSI_DEV_NAME
#elif defined(CONFIG_PWM_NRF5_SW)
#define PWM_DEV CONFIG_PWM_NRF5_SW_0_DEV_NAME
#else
//#error "Choose supported board or add new board for the application"
#endif
/*
 * Unlike pulse width, period is not a critical parameter for
 * motor control. 20ms is commonly used.
 */
#define PERIOD 25000//(USEC_PER_SEC / 50) * 100

/* all in micro second */
#define STEPSIZE 100
#define MINPULSEWIDTH 700
#define MAXPULSEWIDTH 2300


void main(void){
    lights_init();
    // Initialise all sensors for this device
    for( auto sensor : sensors )
        sensor->initialise();

    int rc = networkSetup();
    printf( "Network setup - %i\n", rc );
    if (rc < 0) {
        return;
    }

    int rpcLen = std::extent< decltype( rpcList ) >::value;
    printf("Got size %i\n", rpcLen );
    tb_pubsub_start( rpcList, rpcLen );

    struct k_timer interTimer;
    k_timer_init( &interTimer, interEvent, NULL );
    k_timer_start( &interTimer, K_SECONDS( 5 ), K_SECONDS( 5 ) );

    k_thread_suspend( k_current_get() );
	


    // while( 1 ){
    //     k_sleep( 1000 );
    //     for( auto sensor : sensors )
    //         printf( "%s\n", sensor->requestPayload() );
    //     printf( "\n" );        
    // }
}


