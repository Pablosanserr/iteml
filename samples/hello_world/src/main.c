#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#include "iteml.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

void main(void)
{
	printk("Hello world\n");
	k_sleep(K_MSEC(1000));
	iteml_display_text("Hello world!");
	 
	while(1);
}