/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.
 */

#include <zephyr.h>
#include <device.h>
#include <sensor.h>
#include <stdio.h>

void main(void)
{
	struct device *dev = device_get_binding("DHT");

  if(!dev){
    printk("CAN'T ACCESS DEVICE\n");
    return;
  }
  else printk("dev %p name %s\n", dev, dev->config->name);

	while (1) {
		struct sensor_value temp, humidity;

		sensor_sample_fetch(dev);
		sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
		sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, &humidity);

		printk("temp: %d.%02d; humidity: %d.%02d\n",
		      temp.val1, temp.val2,
		      humidity.val1, humidity.val2);

		k_sleep(2000);
	}
}
