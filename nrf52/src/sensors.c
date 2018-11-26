/*
 * ADC Code
 * Basic timer functionality added
 * Need to process data
*/

#include <zephyr.h>
#include <board.h>
#include <device.h>
#include <sensor.h>
#include <adc.h>
#include <hal/nrf_saadc.h>
#include <misc/printk.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "sensors.h"
#include "config.h"
#include "lights.h"


#define ADC_DEVICE_NAME		CONFIG_ADC_0_NAME
#define ADC_RESOLUTION		10
#define ADC_GAIN		ADC_GAIN_1_6
#define ADC_REFERENCE		ADC_REF_INTERNAL
#define ADC_ACQUISITION_TIME	ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 10)
#define ADC_1ST_CHANNEL_ID	0
#define ADC_1ST_CHANNEL_INPUT	NRF_SAADC_INPUT_AIN1
#define ADC_2ND_CHANNEL_ID	2
#define ADC_2ND_CHANNEL_INPUT	NRF_SAADC_INPUT_AIN2

#define BUFFER_SIZE  6
static s16_t m_sample_buffer[BUFFER_SIZE];

static const struct adc_channel_cfg m_1st_channel_cfg = {
	.gain             = ADC_GAIN,
	.reference        = ADC_REFERENCE,
	.acquisition_time = ADC_ACQUISITION_TIME,
	.channel_id       = ADC_1ST_CHANNEL_ID,
#if defined(CONFIG_ADC_CONFIGURABLE_INPUTS)
	.input_positive   = ADC_1ST_CHANNEL_INPUT,
#endif
};
#if defined(ADC_2ND_CHANNEL_ID)
static const struct adc_channel_cfg m_2nd_channel_cfg = {
	.gain             = ADC_GAIN,
	.reference        = ADC_REFERENCE,
	.acquisition_time = ADC_ACQUISITION_TIME,
	.channel_id       = ADC_2ND_CHANNEL_ID,
#if defined(CONFIG_ADC_CONFIGURABLE_INPUTS)
	.input_positive   = ADC_2ND_CHANNEL_INPUT,
#endif
};
#endif /* defined(ADC_2ND_CHANNEL_ID) */

static struct device *init_adc(void)
{
	int ret;
	struct device *adc_dev = device_get_binding(ADC_DEVICE_NAME);

	if (!adc_dev) {
		printf("Cannot get ADC device");
		return;
	}

	ret = adc_channel_setup(adc_dev, &m_1st_channel_cfg);
	if (ret) {
		printf("Setting up of the first channel failed with code %d", ret);
	}

#if defined(ADC_2ND_CHANNEL_ID)
	ret = adc_channel_setup(adc_dev, &m_2nd_channel_cfg);
	if (ret) {
		printf("Setting up of the second channel failed with code %d", ret);
	}
#endif /* defined(ADC_2ND_CHANNEL_ID) */

	(void)memset(m_sample_buffer, 0, sizeof(m_sample_buffer));

	return adc_dev;
}

int sample_sensor (int channel_id)
{
	int ret;

	const struct adc_sequence sequence = {
		.channels    = BIT(channel_id),
		.buffer      = m_sample_buffer,
		.buffer_size = sizeof(m_sample_buffer),
		.resolution  = ADC_RESOLUTION,
	};

	struct device *adc_dev = init_adc();

	if (!adc_dev) {
		printf("Failed to initialise ADC");
		return -1;
	}

	ret = adc_read(adc_dev, &sequence);

	if (ret) {
		printf("Failed to read ADC with code %d", ret);
	}

	return m_sample_buffer[0];
}

void sensors(struct device *dev)
{
	char json_buff[42];
	char dht_buff[20] = "";
	char force_buff[11] = "";
	char soil_buff[9] = "";

	// if(!dev){
	// 	printf("CAN'T ACCESS DHT11\n");
	// }
	// struct sensor_value temp, humidity;
	//
	// sensor_sample_fetch(dev);
	// sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
	// sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, &humidity);
	//
	// snprintf(dht_buff, 20, "\"T\":\"%d\",\"H\":\"%d\",", temp.val1,humidity.val1);

	//#if defined(CONFIG_ADC)
	//#if defined(ADC_1ST_CHANNEL_ID)
	uint16_t force_sample = sample_sensor(ADC_1ST_CHANNEL_ID);
	snprintf(force_buff, 11, "\"F\":\"%d\",", force_sample);
	//#endif
	//#if defined(ADC_2ND_CHANNEL_ID)
	uint16_t soil_sample = sample_sensor(ADC_2ND_CHANNEL_ID);
	uint16_t soil_moisture = (soil_sample*100)/1024;
	snprintf(soil_buff, 9, "\"M\":\"%d\"", soil_moisture);
	//#endif
	//#endif

	snprintf(json_buff, 42, "{%s%s%s}", dht_buff, force_buff, soil_buff);
	tb_publish_telemetry(json_buff);
}
