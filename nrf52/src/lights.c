#include <zephyr.h>
#include <board.h>
#include <device.h>
#include <gpio.h>
#include "lights.h"
#include <json.h>
#include <pwm.h>
#include <nrfx_pwm.h>

#define PWM_DEV CONFIG_PWM_NRF5_SW_0_DEV_NAME

/*
 * Unlike pulse width, period is not a critical parameter for
 * motor control. 20ms is commonly used.
 */
#define PERIOD 25000//(USEC_PER_SEC / 50) * 100

/* all in micro second */
#define STEPSIZE 100
#define MINPULSEWIDTH 700
#define MAXPULSEWIDTH 2300

#define SERVO_PIN 13

#define LED_PORT LED0_GPIO_CONTROLLER
#define LED_COUNT 4

const u32_t led_arr[LED_COUNT] = {
    LED0_GPIO_PIN,
    LED1_GPIO_PIN,
    LED2_GPIO_PIN,
    LED3_GPIO_PIN
};

struct rpc_putLights {
	const char* method;
	struct rpc_putLights_params {
		int ledno;
		bool value;
	} params;
};

struct rpc_setServo {
	const char* method;
	struct rpc_setServo_params {
		int value;
	} params;
};

struct device *led_dev;

void putLights(u32_t ledno, bool state)
{
    gpio_pin_write(led_dev, led_arr[ledno], state ? 0 : 1);
}
uint8_t ledState[4] = { 0, 0, 0 };

char * setServo( char * json, int jsonLen ){
    /* Refer to zephyr/include/json.h !!! */
    static char * payload[ 50 ];
    struct device *pwm_dev = device_get_binding( PWM_DEV );
	if (!pwm_dev) {
		printf("Cannot find PWM device!\n");
		return NULL;
	}

	/* JSON RPC params for putLights */
	static const struct json_obj_descr rpc_descr_params[] = {
		JSON_OBJ_DESCR_PRIM(struct rpc_setServo_params, value, JSON_TOK_NUMBER),
	};

	/* JSON generic thingsboard.io RPC */
	static const struct json_obj_descr rpc_descr[] = {
		JSON_OBJ_DESCR_PRIM(struct rpc_setServo, method, JSON_TOK_STRING),
		JSON_OBJ_DESCR_OBJECT(struct rpc_setServo, params, rpc_descr_params)
	};

	struct rpc_setServo rx_rpc={};
    
    json_obj_parse(json, jsonLen, rpc_descr, ARRAY_SIZE(rpc_descr), &rx_rpc);

    int pulseWidth = rx_rpc.params.value;
    printf("Got value %i\n", pulseWidth );
    if ( pulseWidth < MINPULSEWIDTH )
        pulseWidth = MINPULSEWIDTH;
    else if ( pulseWidth > MAXPULSEWIDTH )
        pulseWidth = MAXPULSEWIDTH;
    printf("Setting PWM to %i %i\n", PERIOD, pulseWidth );
    if ( pwm_pin_set_usec( pwm_dev, 13, PERIOD, pulseWidth ) ) {
			printf( "Failed to set PWM\n" );
	}

	return NULL;
}

char * putLights2( char * json, int jsonLen ){
    /* Refer to zephyr/include/json.h !!! */
    static char * payload[ 50 ];

	/* JSON RPC params for putLights */
	static const struct json_obj_descr rpc_descr_params[] = {
		JSON_OBJ_DESCR_PRIM(struct rpc_putLights_params, ledno, JSON_TOK_NUMBER),
		JSON_OBJ_DESCR_PRIM(struct rpc_putLights_params, value, JSON_TOK_TRUE),
	};

	/* JSON generic thingsboard.io RPC */
	static const struct json_obj_descr rpc_descr[] = {
		JSON_OBJ_DESCR_PRIM(struct rpc_putLights, method, JSON_TOK_STRING),
		JSON_OBJ_DESCR_OBJECT(struct rpc_putLights, params, rpc_descr_params)
	};

	struct rpc_putLights rx_rpc={};
    
    json_obj_parse(json, jsonLen, rpc_descr, ARRAY_SIZE(rpc_descr), &rx_rpc);
    ledState[ rx_rpc.params.ledno -1  ] = rx_rpc.params.value;

    gpio_pin_write(led_dev, led_arr[rx_rpc.params.ledno], rx_rpc.params.value ? 0 : 1);

    
	snprintf( payload, 50, "{\"1\":%s,\"2\":%s,\"3\":%s}", ledState[0]?"true":"false",ledState[1]?"true":"false",ledState[2]?"true":"false");
	return payload;
}

bool getLights(u32_t ledno)
{
    u32_t value;

    gpio_pin_read(led_dev, led_arr[ledno], &value);

    return value ? false : true;
}

void lights_init()
{
    led_dev = device_get_binding(LED_PORT);

    for (u32_t i = 0; i < LED_COUNT; i++) {
        gpio_pin_configure(led_dev, led_arr[i], GPIO_DIR_OUT);
        gpio_pin_write(led_dev, led_arr[i], 1);
    }
}
