#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#include "iteml.h"
#include "lvgl.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

int last_button_pressed = -1;

char kb_buffer[1024] = "The user wrote nothing";

char * map[] = {"Write", "Pop-up", "Clean", ""};

void btn_pressed(int b){
	last_button_pressed = b;
}

void main(void){
	// Set up menu
	k_sleep(K_MSEC(1000));
	// Create menu
	iteml_t t1 = {ITEML_BUTTON_MATRIX, "Temporary functions", {.btnm = {.cb = btn_pressed, .map = map}}};
	iteml_create_menu(1, t1);
	
	while(1){
		k_sleep(K_MSEC(10));
		switch(last_button_pressed){
			case 0:
				printk("Button pressed: Write\n");
				k_sleep(K_SECONDS(1));
				iteml_get_text_kb(kb_buffer, "Write something");
				printk("User writes: %s\n", kb_buffer);

				last_button_pressed = -1;
				break;
			case 1:
				printk("Button pressed: Pop-up\n");
				iteml_display_popup("Text written by user", kb_buffer);
				last_button_pressed = -1;
				break;
			case 2:
				printk("Button pressed: Clean\n");
				k_sleep(K_SECONDS(1));
				iteml_clean();
				printk("Touchscreen cleaned\n");
				k_sleep(K_SECONDS(1));
				iteml_display_text("3");
				k_sleep(K_SECONDS(1));
				iteml_clean();
				iteml_display_text("2");
				k_sleep(K_SECONDS(1));
				iteml_clean();
				iteml_display_text("1");
				k_sleep(K_SECONDS(1));
				iteml_t t1 = {ITEML_BUTTON_MATRIX, "Temporary functions", {.btnm = {.cb = btn_pressed, .map = map}}};
				iteml_create_menu(1, t1);
				printk("New interface created\n");

				last_button_pressed = -1;
				break;
			default:
				break;
		}
	}
}