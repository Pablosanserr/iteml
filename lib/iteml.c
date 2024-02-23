#include "iteml.h"

struct k_sem sem;

char buff[100];

const struct device *display_dev;

lv_obj_t * main_tabview;
lv_obj_t * log;

static lv_obj_t * btn_list;
static lv_obj_t * currentButton = NULL;

/**
 * @brief Initialize touch screen
 * 
 * @param text (Optional) Text displayed on the touch screen
*/
void iteml_init(const char * text);
/**
 * @brief Text area event callback
*/
static void iteml_ta_event_cb(lv_event_t * e);
/**
 * @brief Get text written by user via touch screen keyboard
 * 
 * @param buffer Text entered by user
 * @param displayed_text (Optional) Text displayed on the top. If NULL, no text is displayed
*/
void iteml_request_text_kb(const char * displayed_text);
/**
 * @brief Displays text on the touch screen. It can be re-colored by commands
 * 
 * @param buffer Text displayed
*/
void iteml_createLabel(const char * text);
/**
 * @brief Thread that manages touch screen behavior
*/
void iteml_display_manager();

void iteml_init(const char * text){
	k_sem_init(&sem, 0, 1);

	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		printk("[iteml_init] Device not ready, aborting test\n");
		return;
	}

	if(text) iteml_createLabel(text);
	printk("[iteml_init] Touchscreen is ready\n");
}

static void iteml_ta_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);
	
	if(code == LV_EVENT_VALUE_CHANGED){
		// printk("Value changed\n");
	}

	if(code == LV_EVENT_READY){
		const char * msg = lv_textarea_get_text(ta);
		strcpy(buff, msg);

		k_sem_give(&sem);
	}

    if(code == LV_EVENT_FOCUSED) {
        lv_keyboard_set_textarea(kb, ta);
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }

    if(code == LV_EVENT_DEFOCUSED) {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

void iteml_request_text_kb(const char * displayed_text){
	iteml_clean();

	// Create a keyboard to use it with the text area
    lv_obj_t * kb = lv_keyboard_create(lv_scr_act());

    // Create label
    lv_obj_t *label = lv_label_create(lv_scr_act());
    if(displayed_text) lv_label_set_text(label, displayed_text);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 10, 10);

    // Create a text area. The keyboard will write here
    lv_obj_t * ta;
    ta = lv_textarea_create(lv_scr_act());
    lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_add_event_cb(ta, iteml_ta_event_cb, LV_EVENT_ALL, kb);
    lv_textarea_set_placeholder_text(ta, "...");
    lv_obj_set_size(ta, 280, 80);

    lv_keyboard_set_textarea(kb, ta);

	// Display on screen
	lv_task_handler();
	display_blanking_off(display_dev);

	while (k_sem_take(&sem, K_MSEC(10))) {
		lv_task_handler();
	}
}

void iteml_createLabel(const char * text){
	iteml_clean();

    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP); //Break the long lines
    lv_label_set_recolor(label, true); //Enable re-coloring by commands in the text
    lv_label_set_text(label, text);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    // Display on screen
	lv_task_handler();
	display_blanking_off(display_dev);
}

void iteml_display_manager(){
	iteml_init(NULL);

	lv_task_handler();
	display_blanking_off(display_dev);
	while (k_sem_take(&sem, K_MSEC(10))) {
		lv_task_handler();
	}
}

K_THREAD_DEFINE(thread_id, THREAD_STACK_SIZE, iteml_display_manager, NULL, NULL, NULL, THREAD_PRIORITY, 0, 0);

void iteml_clean(){
	lv_obj_clean(lv_scr_act());
}

void iteml_display_text(const char * text){
	iteml_createLabel(text);
}

void iteml_get_text_kb(char * buffer, const char * displayed_text){
	iteml_request_text_kb(displayed_text);
	strcpy(buffer, buff);
	iteml_createLabel("Intuitive Touchscreen Embedded Menu Library"); // TODO
}

static void iteml_list_btn_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_t * info_label = lv_event_get_user_data(e);
    //printk(" ---> %s", lv_label_get_text(info2));
    if(code == LV_EVENT_CLICKED) {
        printk("Clicked: %s\n", lv_list_get_btn_text(btn_list, obj));

        if(currentButton == obj) {
            currentButton = NULL;
            lv_label_set_text(info_label, "Click on a element to view more info\n");
        }
        else {
            currentButton = obj;
        }
        lv_obj_t * parent = lv_obj_get_parent(obj);
        uint32_t i;
        for(i = 0; i < lv_obj_get_child_cnt(parent); i++) {
            lv_obj_t * child = lv_obj_get_child(parent, i);
            if(child == currentButton) {
                lv_obj_add_state(child, LV_STATE_CHECKED);
                lv_label_set_text(info_label, lv_obj_get_user_data(child));
            }
            else {
                lv_obj_clear_state(child, LV_STATE_CHECKED);
            }
        }
    }
}

void iteml_create_menu(int argc, ...){
    va_list args;
    va_start(args, argc);

    // Create a Tab view object
    main_tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 38);

	lv_obj_t * tab_btns = lv_tabview_get_tab_btns(main_tabview);
    lv_obj_set_style_bg_color(tab_btns, lv_palette_darken(LV_PALETTE_GREY, 3), 0);
    lv_obj_set_style_text_color(tab_btns, lv_palette_lighten(LV_PALETTE_GREY, 5), 0);
    lv_obj_set_style_border_side(tab_btns, LV_BORDER_SIDE_BOTTOM, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_clear_flag(lv_tabview_get_content(main_tabview), LV_OBJ_FLAG_SCROLLABLE);

    for(int i = 0; i < argc; i++){
        iteml_t t = va_arg(args, iteml_t);

        // Add tab
        lv_obj_t * tab = lv_tabview_add_tab(main_tabview, t.name);
        switch(t.type){
            case ITEML_CUSTOM:
                // Nothing is created. The user can get the tab via iteml_get_tab and modify it as required, using LVGL.
                break;
            case ITEML_TEXT:
                lv_obj_t * label = lv_label_create(tab);
                lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP); //Break the long lines
                lv_obj_set_size(label, lv_pct(100), lv_pct(100));
                lv_label_set_text(label, t.content);
                break;
            case ITEML_LOG:
                if(log){
                    // TODO It should not be possible to create more than 1 log
                    printk("Error: Log is already created\n");
                }
                log = lv_textarea_create(tab);
                lv_textarea_set_cursor_click_pos(log, false);
                lv_obj_set_size(log, lv_pct(100), lv_pct(100));
                if(t.content) lv_textarea_set_text(log, t.content);
                break;
            case ITEML_SIMPLE_LIST:
                iteml_pair * sl = (iteml_pair *) t.list.pair;

                lv_obj_t * simple_list = lv_list_create(tab);
                lv_obj_set_size(simple_list, lv_pct(100), lv_pct(100));

                for(int i = 0; i < t.list.len; i++) {
                    char buf[32];
                    //lv_list_add_btn(simple_list, LV_SYMBOL_LIST, sl[i].key);
                    lv_list_add_btn(simple_list, NULL, sl[i].key);
                }
                break;
            case ITEML_DATA_LIST:
                iteml_pair * l = (iteml_pair *) t.list.pair;

                // Create info label
                lv_obj_t * info_label = lv_label_create(tab);
                lv_obj_set_size(info_label, lv_pct(40), lv_pct(100));
                lv_obj_align(info_label, LV_ALIGN_TOP_RIGHT, 0, 0);
                lv_obj_set_flex_flow(info_label, LV_FLEX_FLOW_COLUMN);
                lv_label_set_text(info_label, "Click on a element to view more info");

                // Create a list
                lv_obj_t * btn_list = lv_list_create(tab);
                lv_obj_set_size(btn_list, lv_pct(60), lv_pct(100));
                lv_obj_set_style_pad_row(btn_list, 5, 0);

                // Add buttons to the list
                lv_obj_t * btn;
                for(int j = 0; j < t.list.len; j++) {
                    btn = lv_btn_create(btn_list);
                    lv_obj_set_width(btn, lv_pct(50));
                    lv_obj_set_width(btn, lv_pct(100));
                    lv_obj_add_event_cb(btn, iteml_list_btn_handler, LV_EVENT_CLICKED, (void *) info_label);
                    lv_obj_set_user_data(btn, (void *) l[j].value);

                    lv_obj_t * lab = lv_label_create(btn);
                    lv_label_set_text_fmt(lab, "%s", l[j].key);
                }

                // Select the first button by default
                currentButton = lv_obj_get_child(btn_list, 0);
                lv_obj_add_state(currentButton, LV_STATE_CHECKED);
                break;
        }
    }

    va_end(args);
}

void iteml_log_write(const char * buffer){
    char entry[strlen("29/01/23-12:31:54") + strlen(buffer) + strlen(" \n\0")]; // TODO Get time
    sprintf(entry, "%s %s\n", "[29/01/23-12:31:54]", buffer);
    //lv_textarea_add_text(log, "[29/01/23-12:31:54] "); // TODO Get time
	//lv_textarea_add_text(log, buffer);
	//lv_textarea_add_text(log, "\n");
    lv_textarea_add_text(log, entry);
}

int iteml_set_text(int tab_id, const char * text){    
    lv_obj_t * content = lv_tabview_get_content(main_tabview);
    lv_obj_t * tab = lv_obj_get_child(content, tab_id);
    lv_obj_t * label = lv_obj_get_child(tab, NULL);

    if (!lv_obj_check_type(label, &lv_label_class)) {
        return ITEML_ERROR;
    }

    lv_label_set_text(label, text);
    return ITEML_OK;
}

void iteml_get_tab(int tab_id, lv_obj_t ** content){ // TODO: If tab_id is too large, return error
    lv_obj_t * tabs_container = lv_tabview_get_content(main_tabview); // Object containing tabs
    *content = lv_obj_get_child(tabs_container, tab_id); // Desired tab
}