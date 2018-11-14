/*
 * ADC Code
 * Basic timer functionality added
 * Need to process data
*/

#include <zephyr.h>
#include <board.h>
#include <device.h>
#include <adc.h>
#include <hal/nrf_saadc.h>
#include <misc/printk.h>


#define ADC_DEVICE_NAME		CONFIG_ADC_0_NAME
#define ADC_RESOLUTION		10
#define ADC_GAIN		ADC_GAIN_1_6
#define ADC_REFERENCE		ADC_REF_INTERNAL
#define ADC_ACQUISITION_TIME	ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 10)
#define ADC_1ST_CHANNEL_ID	0
#define ADC_1ST_CHANNEL_INPUT	NRF_SAADC_INPUT_AIN1
#define ADC_2ND_CHANNEL_ID	2
#define ADC_2ND_CHANNEL_INPUT	NRF_SAADC_INPUT_AIN2

#define SAMPLE_TIME 60

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
		printk("Cannot get ADC device");
		return;
	}

	ret = adc_channel_setup(adc_dev, &m_1st_channel_cfg);
	if (ret) {
		printk("Setting up of the first channel failed with code %d", ret);
	}

#if defined(ADC_2ND_CHANNEL_ID)
	ret = adc_channel_setup(adc_dev, &m_2nd_channel_cfg);
	if (ret) {
		printk("Setting up of the second channel failed with code %d", ret);
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
		printk("Failed to initialise ADC");
		return -1;
	}

	ret = adc_read(adc_dev, &sequence);

	if (ret) {
		printk("Failed to read ADC with code %d", ret);
	}

	return m_sample_buffer[0];
}

void dht_work_handler(struct k_work *work)
{
	int temp = k_cycle_get_32() % 3 + 20;
	int humidity = k_cycle_get_32() % 5 + 60;
	printk("\nTemperature: %d\nHumidity: %d\n", temp, humidity);
}

K_WORK_DEFINE(dht_work, dht_work_handler);

void dht_timer_handler(struct k_timer *soil_timer)
{
	k_work_submit(&dht_work);
}

void force_work_handler(struct k_work *work)
{
	int force_sample = sample_sensor(ADC_1ST_CHANNEL_ID);
	printk("Force Sample: %d\n", force_sample);
}

K_WORK_DEFINE(force_work, force_work_handler);

void force_timer_handler(struct k_timer *force_timer)
{
	k_work_submit(&force_work);
}

void soil_work_handler(struct k_work *work)
{
	int soil_sample = sample_sensor(ADC_2ND_CHANNEL_ID);
	printk("Soil Sample: %d\n", soil_sample);
}

K_WORK_DEFINE(soil_work, soil_work_handler);

void soil_timer_handler(struct k_timer *soil_timer)
{
	k_work_submit(&soil_work);
}

void main(void)
{
	struct device *dev = device_get_binding("DHT");
	struct k_timer dht_timer, force_timer, soil_timer;

	#if defined(DHT11)
	if(!dev){
		printk("CAN'T ACCESS DEVICE\n");
		return;
	}
	else printk("dev %p name %s\n", dev, dev->config->name);
	#endif

	k_timer_init(&dht_timer, dht_timer_handler, NULL);
	k_timer_init(&force_timer, force_timer_handler, NULL);
	k_timer_init(&soil_timer, soil_timer_handler, NULL);

	k_timer_start(&dht_timer, K_SECONDS(SAMPLE_TIME), K_SECONDS(SAMPLE_TIME));
	k_timer_start(&force_timer, K_SECONDS(SAMPLE_TIME), K_SECONDS(SAMPLE_TIME));
	k_timer_start(&soil_timer, K_SECONDS(SAMPLE_TIME), K_SECONDS(SAMPLE_TIME));

	k_thread_suspend(k_current_get());
}
