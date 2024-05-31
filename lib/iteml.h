#ifndef ITEML_H
#define ITEML_H

#include <stdio.h>
#include <string.h>
#ifdef NRF
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <tfm_ns_interface.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#include <zephyr/drivers/display.h>
#endif
#include <lvgl.h>
#include <time.h>

// Threads
#ifdef NRF
#include <zephyr/sys/__assert.h>
#define THREAD_STACK_SIZE 17408 // Same value as CONFIG_MAIN_STACK_SIZE
#define THREAD_PRIORITY 7
#endif

// Touch screen resolution
#define LV_HOR_RES_MAX 320
#define LV_VER_RES_MAX 240

// 
#define ITEML_OK 1
#define ITEML_ERROR -1

// Tab type
#define ITEML_CUSTOM 0
#define ITEML_TEXT 1
#define ITEML_LOG 2
#define ITEML_SIMPLE_LIST 3
#define ITEML_DATA_LIST 4
#define ITEML_BUTTON_MATRIX 5

typedef struct{
    char * key;
    char * value;
} iteml_pair;

typedef struct{
    uint16_t len;
    iteml_pair * pair;
} iteml_list;

typedef struct{
    char ** map;
    void (* cb)(int);
} iteml_btnm;

typedef struct{
    uint16_t type;
    char * name;
    union{
        char * content;
        iteml_list list;
        iteml_btnm btnm;
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
 * @brief Displays text on a temporary pop-up message box
 * 
 * @param title Title of pop-up message box
 * @param text Text of pop-up message box
*/
void iteml_display_popup(const char * title, const char * text);

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

/**
 * @brief Sets button map in the specified tab. A menu must have been previously created.
 * 
 * @param tab_id Id of the tab to modify
 * @param map New button matrix map
*/
int iteml_set_btnm_map(int tab_id, const char * map);

/**
 * @brief Returns an object that contains the objects of the selected tab.
 * 
 * @param tab_id Tab identifier
 * @param text Tab container
*/
void iteml_get_tab(int tab_id, lv_obj_t ** content);

#endif
