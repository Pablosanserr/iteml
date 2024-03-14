#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#include "iteml.h"
#include "lvgl.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

char * map[] = {"    ", "    ", "    ", "\n",
				  "    ", "    ", "    ", "\n",
				  "    ", "    ", "    ", ""
				 };
char board[3][3] = {{' ',' ',' '},
					{' ',' ',' '},
					{' ',' ',' '}};
int player_turn = 1;

int check_three_in_a_row(){
    // Check rows and columns
    for(int i = 0; i < 3; ++i) {
        // Check rows
        if(board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][0] == board[i][2]) return 1;
        // Check colums
        if(board[0][i] != ' ' && board[0][i] == board[1][i] && board[0][i] == board[2][i]) return 1;
    }

    // Check diagonals
    if((board[0][0] != ' ' && board[0][0] == board[1][1] && board[0][0] == board[2][2]) ||
        (board[0][2] != ' ' && board[0][2] == board[1][1] && board[0][2] == board[2][0])) return 1;

	// If nobody wins
    return 0;
}

void clear_board(){
	// Reset board
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			board[i][j] = ' ';
		}
	}

	// Reset map
	for(int i = 0; i < 12; i++){
		if(i == 0 || (i+1)%4 != 0) map[i] = "    ";
	}
}

void print_board(){
	printk("-----\n");
	for(int i = 0 ; i < 3 ; i++){
		for(int j = 0 ; j < 3 ; j++) printk("%c ", board[i][j]);
		printk("\n");
	}
	printk("-----\n");
}

void btn_pressed(int b){
	int index;
	if(b < 3) index = b;
	else if(b < 6) index = b + 1;
	else index = b + 2;

	if(strcmp(map[index], "    ") == 0){
		if(player_turn == 1){
			map[index] = LV_SYMBOL_CLOSE;
			board[b/3][b%3] = 'X';
			print_board();
			if(check_three_in_a_row()){
				printk("Player 1 wins!\n");
				clear_board();
			}
			player_turn = 2;
		}else{
			map[index] = LV_SYMBOL_STOP;
			board[b/3][b%3] = 'O';
			print_board();
			if(check_three_in_a_row()){
				printk("Player 2 wins!\n");
				clear_board();
			}
			player_turn = 1;
		}
	}
}

void main(void){
	// Set up menu
	k_sleep(K_MSEC(1000));
	// Create menu	
	iteml_t t1 = {ITEML_BUTTON_MATRIX, "Tic tac toe", {.btnm = {.cb = btn_pressed, .map = map}}};
	iteml_create_menu(1, t1);
	
	while(1){
		k_sleep(K_MSEC(10000));
	}
}