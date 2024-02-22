#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#include "iteml.h"
#include "lvgl.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

void main(void){
	// Set up menu
	k_sleep(K_MSEC(1000));
	
	lv_obj_t * custom_tab_1;
	lv_obj_t * custom_tab_2;
	
	// Create menu
	iteml_t t1 = {ITEML_CUSTOM, "Color", NULL};
	iteml_t t2 = {ITEML_CUSTOM, "Lines", NULL};
	iteml_create_menu(2, t1, t2);
	
	// Get first tab
	iteml_get_tab(0, &custom_tab_1);
	// Create color wheel
	lv_obj_t * cw = lv_colorwheel_create(custom_tab_1, true);
	lv_obj_set_size(cw, 150, 150);
	lv_obj_center(cw);
	
	// Get second tab
	iteml_get_tab(1, &custom_tab_2);
	// Create an array for the points of the line
    static lv_point_t line_points[] = { {5, 5}, {70, 70}, {120, 10}, {180, 60}, {240, 10} };
    // Create style
    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 8);
    lv_style_set_line_color(&style_line, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_line_rounded(&style_line, true);
    // Create a line and apply the new style
    lv_obj_t * line;
    line = lv_line_create(custom_tab_2);
    lv_line_set_points(line, line_points, 5);     /*Set the points*/
    lv_obj_add_style(line, &style_line, 0);
    lv_obj_center(line);

	while(1){
		k_sleep(K_MSEC(10000));
	}
}