/*
 * Basic DHT11 Application
 *
 * Mark Anderson
 * Trinity College Dublin
 * 5/11/18
 */

#include <zephyr.h>
#include <device.h>
#include <sensor.h>
#include <misc/printk.h>

#define SAMPLE_TIME 2

void dht_work_handler(struct k_work *work)
{
	int temp = k_cycle_get_32() % 3 + 20;
	int humidity = k_cycle_get_32() % 5 + 60;
	printk("Temperature: %d\tHumidity: %d\n", temp, humidity);
}

K_WORK_DEFINE(dht_work, dht_work_handler);

void dht_timer_handler(struct k_timer *soil_timer)
{
	k_work_submit(&dht_work);
}

void main(void)
{
	struct device *dev = device_get_binding("DHT");
	struct k_timer dht_timer;

  if(!dev){
    printk("CAN'T ACCESS DEVICE\n");
    return;
  }
  else printk("dev %p name %s\n", dev, dev->config->name);

	k_timer_init(&dht_timer, dht_timer_handler, NULL);
	k_timer_start(&dht_timer, K_SECONDS(SAMPLE_TIME), K_SECONDS(SAMPLE_TIME));
	k_thread_suspend(k_current_get());
}
