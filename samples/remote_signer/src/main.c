#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#include "iteml.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

void build_info_text(char * buffer, const char * ip, int state, const char * last_request){
	if(state == 0) sprintf(buffer, "IP: %s\nState: Disconnected %s\nLast request: %s\nOperating time: 0m", ip, LV_SYMBOL_CLOSE, last_request);
	else sprintf(buffer, "IP: %s\nState: Connected %s\nLast request: %s\nOperating time: 0m", ip, LV_SYMBOL_OK, last_request);
}

void main(void)
{
	char info[200];
	char ip[] = "192.168.1.12";
	char connected_msg[] = "Connected: user with IP = 192.168.1.12";
	char keep_alive[] = "Keep alive message";
	

	// Set up menu
	k_sleep(K_MSEC(1000));
	
	build_info_text(info, "0.0.0.0", 0, "");
	iteml_t t1 = {ITEML_TEXT, "Info", info};
	/*iteml_t t2 = {ITEML_SIMPLE_LIST, "Keys", {.list = {.len = 10, .pair = (iteml_pair[]){{"ae24...eec2", NULL},
																				 {"eef0...12a3", NULL},
																				 {"29db...40c1", NULL},
																				 {"2744...f98a", NULL},
																				 {"b5bd...7e04", NULL},
																				 {"e2f1...7112", NULL},
																				 {"89bd...0a37", NULL},
																				 {"7a54...7e13", NULL},
																				 {"1256...d6b4", NULL},
																				 {"ae94...a085", NULL}
																				 }}}};*/
	iteml_t t2 = {ITEML_DATA_LIST, "Keys", {.list = {.len = 10, .pair = (iteml_pair[]){{"ae24...eec2", "3 times used"},
																				 {"eef0...12a3", "2 times used"},
																				 {"29db...40c1", "1 times used"},
																				 {"2744...f98a", "3 times used"},
																				 {"b5bd...7e04", "0 times used"},
																				 {"e2f1...7112", "7 times used"},
																				 {"89bd...0a37", "3 times used"},
																				 {"7a54...7e13", "4 times used"},
																				 {"1256...d6b4", "5 times used"},
																				 {"ae94...a085", "0 times used"}
																				 }}}};
	iteml_t t3 = {ITEML_LOG, "Log", NULL};
	iteml_t t4 = {ITEML_TEXT, "About", "Working prototype for an Eth 2.0 remote signer that follows EIP-3030/Web3Signer ETH2 Api for the ARM Cortex-M33 architecture.\n\n"
									   "Founded by the Eth2 staking community grants, from Etherum Fundation.\n\n"
									   "Developed by Decentralized Security\n"};
	iteml_create_menu(4, t1, t2, t3, t4);
	 
	// Change IP
	k_sleep(K_MSEC(5000));

	build_info_text(info, ip, 1, connected_msg);
	int r = iteml_set_text(0, info);
	if(r == ITEML_OK) printk("Text changed successfully\n");
	else printk("Error\n");
	iteml_log_write(connected_msg);

	// Keepalive msg every 10 seconds
	while(1){
		k_sleep(K_MSEC(10000));

		build_info_text(info, ip, 1, keep_alive);
		// Set info
		int r = iteml_set_text(0, info);
		if(r == ITEML_OK) printk("Text changed successfully\n");
		else printk("Error\n");
		// Add log
		iteml_log_write(keep_alive);
	}
}