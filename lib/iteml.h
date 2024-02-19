#ifndef ITEML_H
#define ITEML_H

#include <stdio.h>
#include <string.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <tfm_ns_interface.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#include <zephyr/drivers/display.h>
#include <lvgl.h>
#include <time.h>

// Threads
#include <zephyr/sys/__assert.h>
#define THREAD_STACK_SIZE 17408 // Same value as CONFIG_MAIN_STACK_SIZE
#define THREAD_PRIORITY 7

// Touch screen resolution
#define LV_HOR_RES_MAX 320
#define LV_VER_RES_MAX 240

// 
#define ITEML_OK 1
#define ITEML_ERROR -1

// Tab type
#define ITEML_TEXT 1
#define ITEML_LOG 2
#define ITEML_LIST 3

typedef struct{
    char * key;
    char * value;
}iteml_pair;

typedef struct{
    uint16_t len;
    iteml_pair * pair;
} iteml_list;

typedef struct{
    uint16_t type;
    char * name;
    union{
        char * content;
        iteml_list list;
    };
} iteml_t;

/**
 * @brief Removes touchscreen content
*/
void iteml_clean();

/**
 * @brief Displays text on the touchscreen
 * 
 * @param text Text displayed
*/
void iteml_display_text(const char * text);

/**
 * @brief Get text written by user via touchscreen keyboard
 * 
 * @param buffer Text entered by user
 * @param displayed_text (Optional) Text displayed on the top. If NULL, no text is displayed
*/
void iteml_get_text_kb(char * buffer, const char * displayed_text);

/**
 * @brief Creates a custom tabview menu
 * 
 * @param argc Number of tabs
 * @param ... iteml_t types for each tab configuration
*/
void iteml_create_menu(int argc, ...);

/**
 * @brief Logs a message
 * 
 * @param buffer Text logged
*/
void iteml_log_write(const char * buffer);

/**
 * @brief Displays text in the specified tab. A menu must have been previously created.
 * 
 * @param tab_id Id of the tab to write in
 * @param text Text displayed
*/
int iteml_set_text(int tab_id, const char * text);

#endif
