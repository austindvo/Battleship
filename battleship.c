/*-----------------------------------------------------------------------------
-					                 CPRE 185 Final Project
-             Developed for 185 Selim by Austin Dvorak
-	Name: Austin Dvorak
- 	Section: C
-	NetID: austindv
-	Date: 5/3/19
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
-	                            Includes
-----------------------------------------------------------------------------*/

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>
#include <time.h>

/*-----------------------------------------------------------------------------
-	                             Defines
-----------------------------------------------------------------------------*/

#define HIT 104
#define MISS 109

#define CARRIER 3
#define DESTROYER 2
#define PATROL 1
#define EMPTY 0

/*-----------------------------------------------------------------------------
-	                             Structs
-----------------------------------------------------------------------------*/

typedef struct{

        uint8_t dir;
        uint8_t dir_num;

        uint8_t row_1;
        uint8_t col_1;

        uint8_t row_2;
        uint8_t col_2;

        uint8_t row_3;
        uint8_t col_3;

        uint8_t row_4;
        uint8_t col_4;

} expand_hit;

typedef struct{

        uint8_t dir;
        uint8_t num_dir;

        uint8_t row_1;
        uint8_t col_1;

        uint8_t row_2;
        uint8_t col_2;

} linear_hit;

/*-----------------------------------------------------------------------------
-	                            Prototypes
-----------------------------------------------------------------------------*/

/* initialize the computer board */
void init_comp_board();

/* places a carrier at random on board */
void generate_carrier(uint8_t board);

/* places a destroyer at random on board */
void generate_destroyer(uint8_t board);

/* places a patrol boat at random on board */
void generate_patrol(uint8_t board);

/* initialize the user board */
void init_user_board();

/* manualy create map */
void manual_map();

/* set rows and columns */
void set_rc_cola(uint8_t *row, uint8_t *col, char *str);

/* sets the selected cell to hit or miss */
int hit_or_miss(uint8_t row, uint8_t col, uint8_t board, uint8_t *hits);

/*checks the hits to see if all boats are sunk */
void win_check(uint8_t player, uint8_t hits);

/* handles user turn */
void user_turn();

/* does computer turn */
void computer_turn(uint8_t *row, uint8_t *col, uint8_t *hits);

/* sets the hit_struct */
void set_hits(uint8_t row, uint8_t col);

/*draw the game to the screen */
void draw_screen();

/* updates the grid */
void update_screen();

/* draws the windows for the screen */
void draw_windows();

/* draw the stupidly extravagant title */
void draw_title();

/* creates a new window with a given height, width, start x pos, and start y pos */
WINDOW *create_newwin(int height, int width, int starty, int startx);

/*-----------------------------------------------------------------------------
-	                         Global Variables
-----------------------------------------------------------------------------*/

WINDOW *my_wins[7];

uint8_t BOARD_1[10][10];
uint8_t BOARD_2[10][10];

expand_hit hit;
expand_hit *first_hit = &hit;

linear_hit lin_hit;
linear_hit *sec_hit = &lin_hit;

uint8_t no_hits = 0;


/*-----------------------------------------------------------------------------
-				 Implementation
-----------------------------------------------------------------------------*/
int main(){
        time_t t;
        srand((unsigned) time(&t));

        uint8_t row, col;
        uint8_t user1_hits = 0, user2_hits = 0;

        char *str;

        first_hit->dir = 0;
        sec_hit->dir = 0;

	/* start ncurses */
    	initscr();
    	refresh();

        /* check if terminal supports colors */
	if (has_colors() == FALSE) {
	    endwin();
	    printf("Your terminal does not support color\n");
	    exit(1);
	}

	/* init colors */
	start_color();

        /* color for default background */
	init_pair(1, COLOR_BLACK, COLOR_BLUE);
	init_pair(2, COLOR_BLACK, COLOR_CYAN);
	init_pair(3, COLOR_BLACK, COLOR_GREEN);
	init_pair(4, COLOR_BLACK, COLOR_YELLOW);
	init_pair(5, COLOR_BLACK, COLOR_WHITE);
        init_pair(6, COLOR_BLACK, COLOR_RED);
        init_pair(7, COLOR_BLUE, COLOR_BLUE);

        /* initialize the computers board */
        init_comp_board();

        /* draws the base screen */
        draw_screen();

        /* initialize user board */
        init_user_board();

        /* move the cursor to correct posititon */
        move(26, 34);
        refresh();

        do{
                /* print the user turn string */
                user_turn();

                /* allocate user input string for 10 characters max */
                str = malloc(10);

                /* recieve user input */
                getstr(str);

                /* invalid col (not a letter between a-j and A-J*/
                while((str[0] < 65) || ((str[0] > 74) && (str[0] < 97)) || str[0] > 106){
                        /* clear the user input from the screen */
                        mvprintw(26, 35, "Invalid Col,     Try again:");
                        refresh();
                        mvprintw(26, 66, "    ");
                        move(26, 66);
                        getstr(str);
                }

                /* invalid row (not a number between 1 (ascii 48) and 11 */
                if(str[1] == ' '){
                        while(((str[2] == '1') && ((str[3] != 48) && (str[3] != '\0'))) ||
                                ((str[2] < 48) || (str[2] > 57))){
                                mvprintw(26, 35, "Invalid Row,     Try again:");
                                refresh();
                                mvprintw(26, 66, "    ");
                                move(26, 66);
                                getstr(str);
                        }
                }
                else {
                        while(((str[1] == '1') && ((str[2] != 48) && (str[2] != '\0'))) ||
                                ((str[1] < 48) || (str[1] > 57))){
                                mvprintw(26, 35, "Invalid Row,     Try again:");
                                refresh();
                                mvprintw(26, 66, "    ");
                                move(26, 66);
                                getstr(str);
                        }
                }

                /* adjusts the user input for use with array */
                set_rc_cola(&row, &col, str);

                /* check if coordinate was already set */
                while(BOARD_2[row][col] == HIT || BOARD_2[row][col] == MISS){
                        mvprintw(26, 35, "Already Tried,   Try again:");
                        refresh();
                        mvprintw(26, 66, "    ");
                        move(26, 66);
                        getstr(str);

                        set_rc_cola(&row, &col, str);
                }

                /* clear the user input from the screen */
                mvprintw(26, 66, "    ");
                refresh();

                /* sets the cell as hit or miss and
                   updates hit count */
                hit_or_miss(row, col, 2, &user1_hits);

                /* update the screen for current hits
                   and misses */
                update_screen();

                /* check if the user won */
                win_check(1, user1_hits);

                /* take the computer turn */
                computer_turn(&row, &col, &user2_hits);


                /* update the screen for current hits
                   and misses */
                update_screen();

                /* clear the input area */
                mvprintw(26, 66, "     ");
                refresh();

                /* check if the computer won */
                win_check(2, user2_hits);

                /* reset the input string */
                free(str);

        } while(1);

        endwin();

        return 0;
}


/* initialize the computer board */
void init_comp_board(){

        /* empty the board */
        for(int i = 0; i < 10; i++){
                for(int j = 0; j < 10; j++){
                        BOARD_2[i][j] = EMPTY;
                }
        }

        /* generate required ships */
        generate_carrier(2);

        generate_destroyer(2);
        generate_destroyer(2);
        generate_destroyer(2);

        generate_patrol(2);
        generate_patrol(2);
        generate_patrol(2);
        generate_patrol(2);
}

/* places a carrier at random on board
   this method selects a start position
   and picks a random direction to place
   the middle and end cells checking,
   checking for bounds */
void generate_carrier(uint8_t board){

        /* set carrier position */
        uint8_t carrier_start_num = rand() % 100;

        uint8_t carrier_start_x = carrier_start_num / 10;
        uint8_t carrier_start_y = carrier_start_num % 10;

        /* check which board to place carrier on */
        if(board == 1){
                BOARD_1[carrier_start_y][carrier_start_x] = CARRIER;
        }

        else if(board == 2){
                BOARD_2[carrier_start_y][carrier_start_x] = CARRIER;
        }

        uint8_t carrier_num_dir;
        uint8_t carrier_mid_num;
        uint8_t carrier_end_num;

        uint8_t carrier_mid_x;
        uint8_t carrier_mid_y;

        uint8_t carrier_end_x;
        uint8_t carrier_end_y;

        /* check for corners and only expand in 2 directions */
        if(carrier_start_y == 0 && carrier_start_x == 0){
                carrier_num_dir = rand() % 2;

                switch(carrier_num_dir){
                        /* carrier is vertical */
                        case 0:
                                carrier_mid_num = carrier_start_num + 1;
                                carrier_end_num = carrier_start_num + 2;
                                break;
                        /* carrier is horizontal */
                        case 1:
                                carrier_mid_num = carrier_start_num + 10;
                                carrier_end_num = carrier_start_num + 20;
                                break;
                }

                /* set x and y positions for middle and end cell */
                carrier_mid_x = carrier_mid_num / 10;
                carrier_mid_y = carrier_mid_num % 10;

                carrier_end_x = carrier_end_num / 10;
                carrier_end_y = carrier_end_num % 10;

        }

        /* lower left corner */
        else if(carrier_start_y == 9 && carrier_start_x == 0){
                carrier_num_dir = rand() % 2;

                switch(carrier_num_dir){
                        case 0:
                                carrier_mid_num = carrier_start_num - 1;
                                carrier_end_num = carrier_start_num - 2;
                                break;
                        case 1:
                                carrier_mid_num = carrier_start_num + 10;
                                carrier_end_num = carrier_start_num + 20;
                                break;
                }

                /* set x and y positions for middle and end cell */
                carrier_mid_x = carrier_mid_num / 10;
                carrier_mid_y = carrier_mid_num % 10;

                carrier_end_x = carrier_end_num / 10;
                carrier_end_y = carrier_end_num % 10;
        }

        /* upper right corner */
        else if(carrier_start_y == 0 && carrier_start_x == 9){
                carrier_num_dir = rand() % 2;

                switch(carrier_num_dir){
                        case 0:
                                carrier_mid_num = carrier_start_num + 1;
                                carrier_end_num = carrier_start_num + 2;
                                break;
                        case 1:
                                carrier_mid_num = carrier_start_num - 10;
                                carrier_end_num = carrier_start_num - 20;
                                break;
                }

                /* set x and y positions for middle and end cell */
                carrier_mid_x = carrier_mid_num / 10;
                carrier_mid_y = carrier_mid_num % 10;

                carrier_end_x = carrier_end_num / 10;
                carrier_end_y = carrier_end_num % 10;
        }

        /* lower right corner */
        else if(carrier_start_y == 9 && carrier_start_x == 9){
                carrier_num_dir = rand() % 2;

                switch(carrier_num_dir){
                        case 0:
                                carrier_mid_num = carrier_start_num - 1;
                                carrier_end_num = carrier_start_num - 2;
                                break;

                        case 1:
                                carrier_mid_num = carrier_start_num - 10;
                                carrier_end_num = carrier_start_num - 20;
                                break;
                }


                /* set x and y positions for middle and end cell */
                carrier_mid_x = carrier_mid_num / 10;
                carrier_mid_y = carrier_mid_num % 10;

                carrier_end_x = carrier_end_num / 10;
                carrier_end_y = carrier_end_num % 10;
        }

        /* right side */
        else if(carrier_start_x == 9){
                carrier_num_dir = rand() % 3;

                /* carrier is on right, can't move right */
                switch(carrier_num_dir){
                        case 0:
                                carrier_mid_num = carrier_start_num - 1;

                                if(carrier_mid_num%10 > 0)
                                        carrier_end_num = carrier_start_num - 2;
                                else
                                        carrier_end_num = carrier_start_num + 1;
                                break;
                        case 1:
                                carrier_mid_num = carrier_start_num + 1;

                                if(carrier_mid_num%10 < 9)
                                        carrier_end_num = carrier_start_num + 2;
                                else
                                        carrier_end_num = carrier_start_num - 1;
                                break;
                        case 2:
                                carrier_mid_num = carrier_start_num - 10;
                                carrier_end_num = carrier_start_num - 20;
                                break;
                }

                /* set x and y positions for middle and end cell */
                carrier_mid_x = carrier_mid_num / 10;
                carrier_mid_y = carrier_mid_num % 10;

                carrier_end_x = carrier_end_num / 10;
                carrier_end_y = carrier_end_num % 10;
        }

        /* left side */
        else if(carrier_start_x == 0){
                carrier_num_dir = rand() % 3;

                /* carrier is on left side, can't go left */
                switch(carrier_num_dir){
                        case 0:
                                carrier_mid_num = carrier_start_num - 1;

                                if(carrier_mid_num%10 > 0)
                                        carrier_end_num = carrier_start_num - 2;
                                else
                                        carrier_end_num = carrier_start_num + 1;
                                break;
                        case 1:
                                carrier_mid_num = carrier_start_num + 1;

                                if(carrier_mid_num%10 < 9)
                                        carrier_end_num = carrier_start_num + 2;
                                else
                                        carrier_end_num = carrier_start_num - 1;
                                break;
                        case 2:
                                carrier_mid_num = carrier_start_num + 10;
                                carrier_end_num = carrier_start_num + 20;
                                break;
                }

                /* set x and y positions for middle and end cell */
                carrier_mid_x = carrier_mid_num / 10;
                carrier_mid_y = carrier_mid_num % 10;

                carrier_end_x = carrier_end_num / 10;
                carrier_end_y = carrier_end_num % 10;
        }

        /* bottom */
        else if(carrier_start_y == 9){
                carrier_num_dir = rand() % 3;

                /* carrier is on bottom row, can't go down */
                switch(carrier_num_dir){
                        case 0:
                                carrier_mid_num = carrier_start_num - 1;
                                carrier_end_num = carrier_start_num - 2;
                                break;
                        case 1:
                                carrier_mid_num = carrier_start_num + 10;

                                if(carrier_mid_num/10 < 9)
                                        carrier_end_num = carrier_start_num + 20;
                                else
                                        carrier_end_num = carrier_start_num - 10;
                                break;
                        case 2:
                                carrier_mid_num = carrier_start_num - 10;

                                if(carrier_mid_num/10 > 0)
                                        carrier_end_num = carrier_start_num - 20;
                                else
                                        carrier_end_num = carrier_start_num + 10;
                                break;
                }

                /* set x and y positions for middle and end cell */
                carrier_mid_x = carrier_mid_num / 10;
                carrier_mid_y = carrier_mid_num % 10;

                carrier_end_x = carrier_end_num / 10;
                carrier_end_y = carrier_end_num % 10;

        }

        /* top */
        else if(carrier_start_y == 0){
                carrier_num_dir = rand() % 3;

                /* carrier is on top row, can't go farther up */
                switch(carrier_num_dir){
                        case 0:
                                carrier_mid_num = carrier_start_num + 1;
                                carrier_end_num = carrier_start_num + 2;
                                break;
                        case 1:
                                carrier_mid_num = carrier_start_num - 10;

                                if(carrier_mid_num/10 > 0)
                                        carrier_end_num = carrier_start_num - 20;
                                else
                                        carrier_end_num = carrier_start_num + 10;
                                break;
                        case 2:
                                carrier_mid_num = carrier_start_num + 10;

                                if(carrier_mid_num/10 < 9)
                                        carrier_end_num = carrier_start_num + 20;
                                else
                                        carrier_end_num = carrier_start_num - 10;
                                break;
                }

                /* set x and y positions for middle and end cell */
                carrier_mid_x = carrier_mid_num / 10;
                carrier_mid_y = carrier_mid_num % 10;

                carrier_end_x = carrier_end_num / 10;
                carrier_end_y = carrier_end_num % 10;
        }

        /* otherwise space is in center */
        else {
                carrier_num_dir = rand() % 4;

                /* carrier is inside borders of board, pick a
                   direction and go if possible */
                switch(carrier_num_dir){
                        case 0:
                                carrier_mid_num = carrier_start_num - 1;

                                if(carrier_mid_num%10 > 0)
                                        carrier_end_num = carrier_start_num - 2;
                                else
                                        carrier_end_num = carrier_start_num + 1;
                                break;
                        case 1:
                                carrier_mid_num = carrier_start_num + 1;

                                if(carrier_mid_num%10 < 9)
                                        carrier_end_num = carrier_start_num + 2;
                                else
                                        carrier_end_num = carrier_start_num - 1;
                                break;
                        case 2:
                                carrier_mid_num = carrier_start_num - 10;

                                if(carrier_mid_num/10 > 0)
                                        carrier_end_num = carrier_start_num - 20;
                                else
                                        carrier_end_num = carrier_start_num + 10;
                                break;
                        case 3:
                                carrier_mid_num = carrier_start_num + 10;

                                if(carrier_mid_num/10 < 9)
                                        carrier_end_num = carrier_start_num + 20;
                                else
                                        carrier_end_num = carrier_start_num - 10;
                                break;
                }

                /* set x and y positions for middle and end cell */
                carrier_mid_x = carrier_mid_num / 10;
                carrier_mid_y = carrier_mid_num % 10;

                carrier_end_x = carrier_end_num / 10;
                carrier_end_y = carrier_end_num % 10;
        }

        /* set middle and end cells as carriers */
        if(board == 1){
                BOARD_1[carrier_mid_y][carrier_mid_x] = CARRIER;
                BOARD_1[carrier_end_y][carrier_end_x] = CARRIER;
        }

        else if(board == 2){
                BOARD_2[carrier_mid_y][carrier_mid_x] = CARRIER;
                BOARD_2[carrier_end_y][carrier_end_x] = CARRIER;
        }

}

/* places a destroyer at random on board
   this method selects a start position
   and picks a random direction to place
   the middle and end cells checking,
   checking for bounds. Loops check for
   occupied space, regenerating until
   all spaces unoccupied */
void generate_destroyer(uint8_t board){

        /* set destroyer start position */
        uint8_t destroyer_start_num = rand() % 100;

        uint8_t destroyer_start_x = destroyer_start_num / 10;
        uint8_t destroyer_start_y = destroyer_start_num % 10;

        /* check which board to place on */
        if(board == 1){
                /* loop till chosen spot is empty */
                while(BOARD_1[destroyer_start_y][destroyer_start_x] != EMPTY){
                        destroyer_start_num = rand() % 100;

                        destroyer_start_x = destroyer_start_num / 10;
                        destroyer_start_y = destroyer_start_num % 10;
                }

                BOARD_1[destroyer_start_y][destroyer_start_x] = DESTROYER;

                uint8_t destroyer_num_dir;
                uint8_t destroyer_end_num;

                uint8_t destroyer_end_x;
                uint8_t destroyer_end_y;

                /* check for corners and only expand in 2 directions */
                if(destroyer_start_y == 0 && destroyer_start_x == 0){
                        destroyer_num_dir = rand() % 2;

                        switch(destroyer_num_dir){
                                case 0:
                                        destroyer_end_num = destroyer_start_num + 1;
                                        break;
                                case 1:
                                        destroyer_end_num = destroyer_start_num + 10;
                                        break;
                        }

                        destroyer_end_x = destroyer_end_num / 10;
                        destroyer_end_y = destroyer_end_num % 10;

                        /* loop till chosen spot is empty */
                        while(BOARD_1[destroyer_end_y][destroyer_end_x] != EMPTY){
                                destroyer_num_dir = rand() % 2;

                                switch(destroyer_num_dir){
                                        case 0:
                                                destroyer_end_num = destroyer_start_num + 1;
                                                break;
                                        case 1:
                                                destroyer_end_num = destroyer_start_num + 10;
                                                break;
                                }

                                destroyer_end_x = destroyer_end_num / 10;
                                destroyer_end_y = destroyer_end_num % 10;
                        }

                }

                /* lower left corner */
                else if(destroyer_start_y == 9 && destroyer_start_x == 0){
                        destroyer_num_dir = rand() % 2;

                        switch(destroyer_num_dir){
                                case 0:
                                        destroyer_end_num = destroyer_start_num - 1;
                                        break;
                                case 1:
                                        destroyer_end_num = destroyer_start_num + 10;
                                        break;
                        }

                        destroyer_end_x = destroyer_end_num / 10;
                        destroyer_end_y = destroyer_end_num % 10;

                        /* loop till chosen spot is empty */
                        while(BOARD_1[destroyer_end_y][destroyer_end_x] != EMPTY){
                                destroyer_num_dir = rand() % 2;

                                switch(destroyer_num_dir){
                                        case 0:
                                                destroyer_end_num = destroyer_start_num - 1;
                                                break;
                                        case 1:
                                                destroyer_end_num = destroyer_start_num + 10;
                                                break;
                                }

                                destroyer_end_x = destroyer_end_num / 10;
                                destroyer_end_y = destroyer_end_num % 10;
                        }

                }

                /* upper right corner */
                else if(destroyer_start_y == 0 && destroyer_start_x == 9){
                        destroyer_num_dir = rand() % 2;

                        switch(destroyer_num_dir){
                                case 0:
                                        destroyer_end_num = destroyer_start_num + 1;
                                        break;
                                case 1:
                                        destroyer_end_num = destroyer_start_num - 10;
                                        break;
                        }

                        destroyer_end_x = destroyer_end_num / 10;
                        destroyer_end_y = destroyer_end_num % 10;

                        /* loop till chosen spot is empty */
                        while(BOARD_1[destroyer_end_y][destroyer_end_x] != EMPTY){
                                destroyer_num_dir = rand() % 2;

                                switch(destroyer_num_dir){
                                        case 0:
                                                destroyer_end_num = destroyer_start_num + 1;
                                                break;
                                        case 1:
                                                destroyer_end_num = destroyer_start_num - 10;
                                                break;
                                }

                                destroyer_end_x = destroyer_end_num / 10;
                                destroyer_end_y = destroyer_end_num % 10;
                        }

                }

                /* lower right corner */
                else if(destroyer_start_y == 9 && destroyer_start_x == 9){
                        destroyer_num_dir = rand() % 2;

                        switch(destroyer_num_dir){
                                case 0:
                                        destroyer_end_num = destroyer_start_num - 1;
                                        break;

                                case 1:
                                        destroyer_end_num = destroyer_start_num - 10;
                                        break;
                        }

                        destroyer_end_x = destroyer_end_num / 10;
                        destroyer_end_y = destroyer_end_num % 10;

                        /* loop till chosen spot is empty */
                        while(BOARD_1[destroyer_end_y][destroyer_end_x] != EMPTY){
                                destroyer_num_dir = rand() % 2;

                                switch(destroyer_num_dir){
                                        case 0:
                                                destroyer_end_num = destroyer_start_num - 1;
                                                break;

                                        case 1:
                                                destroyer_end_num = destroyer_start_num - 10;
                                                break;
                                }

                                destroyer_end_x = destroyer_end_num / 10;
                                destroyer_end_y = destroyer_end_num % 10;
                        }

                }

                /* right side */
                else if(destroyer_start_x == 9){
                        destroyer_num_dir = rand() % 3;

                        switch(destroyer_num_dir){
                                case 0:
                                        destroyer_end_num = destroyer_start_num - 1;
                                        break;
                                case 1:
                                        destroyer_end_num = destroyer_start_num + 1;
                                        break;
                                case 2:
                                        destroyer_end_num = destroyer_start_num - 10;
                                        break;
                        }

                        destroyer_end_x = destroyer_end_num / 10;
                        destroyer_end_y = destroyer_end_num % 10;

                        /* loop till chosen spot is empty */
                        while(BOARD_1[destroyer_end_y][destroyer_end_x] != EMPTY){
                                destroyer_num_dir = rand() % 3;

                                switch(destroyer_num_dir){
                                        case 0:
                                                destroyer_end_num = destroyer_start_num - 1;
                                                break;
                                        case 1:
                                                destroyer_end_num = destroyer_start_num + 1;
                                                break;
                                        case 2:
                                                destroyer_end_num = destroyer_start_num - 10;
                                                break;
                                }

                                destroyer_end_x = destroyer_end_num / 10;
                                destroyer_end_y = destroyer_end_num % 10;
                        }

                }

                /* left side */
                else if(destroyer_start_x == 0){
                        destroyer_num_dir = rand() % 3;

                        switch(destroyer_num_dir){
                                case 0:
                                        destroyer_end_num = destroyer_start_num + 1;
                                        break;
                                case 1:
                                        destroyer_end_num = destroyer_start_num - 1;
                                        break;
                                case 2:
                                        destroyer_end_num = destroyer_start_num + 10;
                                        break;
                        }

                        destroyer_end_x = destroyer_end_num / 10;
                        destroyer_end_y = destroyer_end_num % 10;

                        /* loop till chosen spot is empty */
                        while(BOARD_1[destroyer_end_y][destroyer_end_x] != EMPTY){
                                destroyer_num_dir = rand() % 3;

                                switch(destroyer_num_dir){
                                        case 0:
                                                destroyer_end_num = destroyer_start_num + 1;
                                                break;
                                        case 1:
                                                destroyer_end_num = destroyer_start_num - 1;
                                                break;
                                        case 2:
                                                destroyer_end_num = destroyer_start_num + 10;
                                                break;
                                }

                                destroyer_end_x = destroyer_end_num / 10;
                                destroyer_end_y = destroyer_end_num % 10;
                        }

                }

                /* bottom */
                else if(destroyer_start_y == 9){
                        destroyer_num_dir = rand() % 3;

                        switch(destroyer_num_dir){
                                case 0:
                                        destroyer_end_num = destroyer_start_num - 1;
                                        break;
                                case 1:
                                        destroyer_end_num = destroyer_start_num + 10;
                                        break;
                                case 2:
                                        destroyer_end_num = destroyer_start_num - 10;
                                        break;
                        }

                        destroyer_end_x = destroyer_end_num / 10;
                        destroyer_end_y = destroyer_end_num % 10;

                        /* loop till chosen spot is empty */
                        while(BOARD_1[destroyer_end_y][destroyer_end_x] != EMPTY){
                                destroyer_num_dir = rand() % 3;

                                switch(destroyer_num_dir){
                                        case 0:
                                                destroyer_end_num = destroyer_start_num - 1;
                                                break;
                                        case 1:
                                                destroyer_end_num = destroyer_start_num + 10;
                                                break;
                                        case 2:
                                                destroyer_end_num = destroyer_start_num - 10;
                                                break;
                                }

                                destroyer_end_x = destroyer_end_num / 10;
                                destroyer_end_y = destroyer_end_num % 10;
                        }

                }

                /* top */
                else if(destroyer_start_y == 0){
                        destroyer_num_dir = rand() % 3;

                        switch(destroyer_num_dir){
                                case 0:
                                        destroyer_end_num = destroyer_start_num + 1;
                                        break;
                                case 1:
                                        destroyer_end_num = destroyer_start_num - 10;
                                        break;
                                case 2:
                                        destroyer_end_num = destroyer_start_num + 10;
                                        break;
                        }

                        destroyer_end_x = destroyer_end_num / 10;
                        destroyer_end_y = destroyer_end_num % 10;


                        while(BOARD_1[destroyer_end_y][destroyer_end_x] != EMPTY){
                                destroyer_num_dir = rand() % 3;

                                switch(destroyer_num_dir){
                                        case 0:
                                                destroyer_end_num = destroyer_start_num + 1;
                                                break;
                                        case 1:
                                                destroyer_end_num = destroyer_start_num - 10;
                                                break;
                                        case 2:
                                                destroyer_end_num = destroyer_start_num + 10;
                                                break;
                                }

                                destroyer_end_x = destroyer_end_num / 10;
                                destroyer_end_y = destroyer_end_num % 10;
                        }

                }

                /* otherwise pick a spot and go, no edges */
                else {
                        destroyer_num_dir = rand() % 4;

                        switch(destroyer_num_dir){
                                case 0:
                                        destroyer_end_num = destroyer_start_num - 1;
                                        break;
                                case 1:
                                        destroyer_end_num = destroyer_start_num + 1;
                                        break;
                                case 2:
                                        destroyer_end_num = destroyer_start_num - 10;
                                        break;
                                case 3:
                                        destroyer_end_num = destroyer_start_num + 10;
                                        break;
                        }

                        destroyer_end_x = destroyer_end_num / 10;
                        destroyer_end_y = destroyer_end_num % 10;

                        /* loop till chosen spot is empty */
                        while(BOARD_1[destroyer_end_y][destroyer_end_x] != EMPTY){
                                destroyer_num_dir = rand() % 4;

                                switch(destroyer_num_dir){
                                        case 0:
                                                destroyer_end_num = destroyer_start_num - 1;
                                                break;
                                        case 1:
                                                destroyer_end_num = destroyer_start_num + 1;
                                                break;
                                        case 2:
                                                destroyer_end_num = destroyer_start_num - 10;
                                                break;
                                        case 3:
                                                destroyer_end_num = destroyer_start_num + 10;
                                                break;
                                }

                                destroyer_end_x = destroyer_end_num / 10;
                                destroyer_end_y = destroyer_end_num % 10;
                        }
                }


                BOARD_1[destroyer_end_y][destroyer_end_x] = DESTROYER;

        }

        /* generating computer board */
        if(board == 2){
                while(BOARD_2[destroyer_start_y][destroyer_start_x] != EMPTY){
                        destroyer_start_num = rand() % 100;

                        destroyer_start_x = destroyer_start_num / 10;
                        destroyer_start_y = destroyer_start_num % 10;
                }

                BOARD_2[destroyer_start_y][destroyer_start_x] = DESTROYER;

                uint8_t destroyer_num_dir;
                uint8_t destroyer_end_num;

                uint8_t destroyer_end_x;
                uint8_t destroyer_end_y;

                /* upper left corner */
                if(destroyer_start_y == 0 && destroyer_start_x == 0){
                        destroyer_num_dir = rand() % 2;

                        switch(destroyer_num_dir){
                                case 0:
                                        destroyer_end_num = destroyer_start_num + 1;
                                        break;
                                case 1:
                                        destroyer_end_num = destroyer_start_num + 10;
                                        break;
                        }

                        destroyer_end_x = destroyer_end_num / 10;
                        destroyer_end_y = destroyer_end_num % 10;

                        /* loop till chosen spot is empty */
                        while(BOARD_2[destroyer_end_y][destroyer_end_x] != EMPTY){
                                destroyer_num_dir = rand() % 2;

                                switch(destroyer_num_dir){
                                        case 0:
                                                destroyer_end_num = destroyer_start_num + 1;
                                                break;
                                        case 1:
                                                destroyer_end_num = destroyer_start_num + 10;
                                                break;
                                }

                                destroyer_end_x = destroyer_end_num / 10;
                                destroyer_end_y = destroyer_end_num % 10;
                        }

                }

                /* lower left corner */
                else if(destroyer_start_y == 9 && destroyer_start_x == 0){
                        destroyer_num_dir = rand() % 2;

                        switch(destroyer_num_dir){
                                case 0:
                                        destroyer_end_num = destroyer_start_num - 1;
                                        break;
                                case 1:
                                        destroyer_end_num = destroyer_start_num + 10;
                                        break;
                        }

                        destroyer_end_x = destroyer_end_num / 10;
                        destroyer_end_y = destroyer_end_num % 10;

                        /* loop till chosen spot is empty */
                        while(BOARD_2[destroyer_end_y][destroyer_end_x] != EMPTY){
                                destroyer_num_dir = rand() % 2;

                                switch(destroyer_num_dir){
                                        case 0:
                                                destroyer_end_num = destroyer_start_num - 1;
                                                break;
                                        case 1:
                                                destroyer_end_num = destroyer_start_num + 10;
                                                break;
                                }

                                destroyer_end_x = destroyer_end_num / 10;
                                destroyer_end_y = destroyer_end_num % 10;
                        }

                }

                /* upper right corner */
                else if(destroyer_start_y == 0 && destroyer_start_x == 9){
                        destroyer_num_dir = rand() % 2;

                        switch(destroyer_num_dir){
                                case 0:
                                        destroyer_end_num = destroyer_start_num + 1;
                                        break;
                                case 1:
                                        destroyer_end_num = destroyer_start_num - 10;
                                        break;
                        }

                        destroyer_end_x = destroyer_end_num / 10;
                        destroyer_end_y = destroyer_end_num % 10;


                        /* loop till chosen spot is empty */
                        while(BOARD_2[destroyer_end_y][destroyer_end_x] != EMPTY){
                                destroyer_num_dir = rand() % 2;

                                switch(destroyer_num_dir){
                                        case 0:
                                                destroyer_end_num = destroyer_start_num + 1;
                                                break;
                                        case 1:
                                                destroyer_end_num = destroyer_start_num - 10;
                                                break;
                                }

                                destroyer_end_x = destroyer_end_num / 10;
                                destroyer_end_y = destroyer_end_num % 10;
                        }

                }

                /* lower right corner */
                else if(destroyer_start_y == 9 && destroyer_start_x == 9){
                        destroyer_num_dir = rand() % 2;

                        switch(destroyer_num_dir){
                                case 0:
                                        destroyer_end_num = destroyer_start_num - 1;
                                        break;

                                case 1:
                                        destroyer_end_num = destroyer_start_num - 10;
                                        break;
                        }

                        destroyer_end_x = destroyer_end_num / 10;
                        destroyer_end_y = destroyer_end_num % 10;

                        /* loop till chosen spot is empty */
                        while(BOARD_2[destroyer_end_y][destroyer_end_x] != EMPTY){
                                destroyer_num_dir = rand() % 2;

                                switch(destroyer_num_dir){
                                        case 0:
                                                destroyer_end_num = destroyer_start_num - 1;
                                                break;

                                        case 1:
                                                destroyer_end_num = destroyer_start_num - 10;
                                                break;
                                }

                                destroyer_end_x = destroyer_end_num / 10;
                                destroyer_end_y = destroyer_end_num % 10;
                        }

                }

                /* right side */
                else if(destroyer_start_x == 9){
                        destroyer_num_dir = rand() % 3;

                        switch(destroyer_num_dir){
                                case 0:
                                        destroyer_end_num = destroyer_start_num - 1;
                                        break;
                                case 1:
                                        destroyer_end_num = destroyer_start_num + 1;
                                        break;
                                case 2:
                                        destroyer_end_num = destroyer_start_num - 10;
                                        break;
                        }

                        destroyer_end_x = destroyer_end_num / 10;
                        destroyer_end_y = destroyer_end_num % 10;

                        /* loop till chosen spot is empty */
                        while(BOARD_2[destroyer_end_y][destroyer_end_x] != EMPTY){
                                destroyer_num_dir = rand() % 3;

                                switch(destroyer_num_dir){
                                        case 0:
                                                destroyer_end_num = destroyer_start_num - 1;
                                                break;
                                        case 1:
                                                destroyer_end_num = destroyer_start_num + 1;
                                                break;
                                        case 2:
                                                destroyer_end_num = destroyer_start_num - 10;
                                                break;
                                }

                                destroyer_end_x = destroyer_end_num / 10;
                                destroyer_end_y = destroyer_end_num % 10;
                        }

                }

                /* left side */
                else if(destroyer_start_x == 0){
                        destroyer_num_dir = rand() % 3;

                        switch(destroyer_num_dir){
                                case 0:
                                        destroyer_end_num = destroyer_start_num + 1;
                                        break;
                                case 1:
                                        destroyer_end_num = destroyer_start_num - 1;
                                        break;
                                case 2:
                                        destroyer_end_num = destroyer_start_num + 10;
                                        break;
                        }

                        destroyer_end_x = destroyer_end_num / 10;
                        destroyer_end_y = destroyer_end_num % 10;

                        /* loop till chosen spot is empty */
                        while(BOARD_2[destroyer_end_y][destroyer_end_x] != EMPTY){
                                destroyer_num_dir = rand() % 3;

                                switch(destroyer_num_dir){
                                        case 0:
                                                destroyer_end_num = destroyer_start_num + 1;
                                                break;
                                        case 1:
                                                destroyer_end_num = destroyer_start_num - 1;
                                                break;
                                        case 2:
                                                destroyer_end_num = destroyer_start_num + 10;
                                                break;
                                }

                                destroyer_end_x = destroyer_end_num / 10;
                                destroyer_end_y = destroyer_end_num % 10;
                        }

                }

                /* bottom */
                else if(destroyer_start_y == 9){
                        destroyer_num_dir = rand() % 3;

                        switch(destroyer_num_dir){
                                case 0:
                                        destroyer_end_num = destroyer_start_num - 1;
                                        break;
                                case 1:
                                        destroyer_end_num = destroyer_start_num + 10;
                                        break;
                                case 2:
                                        destroyer_end_num = destroyer_start_num - 10;
                                        break;
                        }

                        destroyer_end_x = destroyer_end_num / 10;
                        destroyer_end_y = destroyer_end_num % 10;

                        /* loop till chosen spot is empty */
                        while(BOARD_2[destroyer_end_y][destroyer_end_x] != EMPTY){
                                destroyer_num_dir = rand() % 3;

                                switch(destroyer_num_dir){
                                        case 0:
                                                destroyer_end_num = destroyer_start_num - 1;
                                                break;
                                        case 1:
                                                destroyer_end_num = destroyer_start_num + 10;
                                                break;
                                        case 2:
                                                destroyer_end_num = destroyer_start_num - 10;
                                                break;
                                }

                                destroyer_end_x = destroyer_end_num / 10;
                                destroyer_end_y = destroyer_end_num % 10;
                        }

                }

                /* top */
                else if(destroyer_start_y == 0){
                        destroyer_num_dir = rand() % 3;

                        switch(destroyer_num_dir){
                                case 0:
                                        destroyer_end_num = destroyer_start_num + 1;
                                        break;
                                case 1:
                                        destroyer_end_num = destroyer_start_num - 10;
                                        break;
                                case 2:
                                        destroyer_end_num = destroyer_start_num + 10;
                                        break;
                        }

                        destroyer_end_x = destroyer_end_num / 10;
                        destroyer_end_y = destroyer_end_num % 10;

                        /* loop till chosen spot is empty */
                        while(BOARD_2[destroyer_end_y][destroyer_end_x] != EMPTY){
                                destroyer_num_dir = rand() % 3;

                                switch(destroyer_num_dir){
                                        case 0:
                                                destroyer_end_num = destroyer_start_num + 1;
                                                break;
                                        case 1:
                                                destroyer_end_num = destroyer_start_num - 10;
                                                break;
                                        case 2:
                                                destroyer_end_num = destroyer_start_num + 10;
                                                break;
                                }

                                destroyer_end_x = destroyer_end_num / 10;
                                destroyer_end_y = destroyer_end_num % 10;
                        }

                }

                /* otherwise pick a spot and go, no edges */
                else {
                        destroyer_num_dir = rand() % 4;

                        switch(destroyer_num_dir){
                                case 0:
                                        destroyer_end_num = destroyer_start_num - 1;
                                        break;
                                case 1:
                                        destroyer_end_num = destroyer_start_num + 1;
                                        break;
                                case 2:
                                        destroyer_end_num = destroyer_start_num - 10;
                                        break;
                                case 3:
                                        destroyer_end_num = destroyer_start_num + 10;
                                        break;
                        }

                        destroyer_end_x = destroyer_end_num / 10;
                        destroyer_end_y = destroyer_end_num % 10;

                        /* loop till chosen spot is empty */
                        while(BOARD_2[destroyer_end_y][destroyer_end_x] != EMPTY){
                                destroyer_num_dir = rand() % 4;

                                switch(destroyer_num_dir){
                                        case 0:
                                                destroyer_end_num = destroyer_start_num - 1;
                                                break;
                                        case 1:
                                                destroyer_end_num = destroyer_start_num + 1;
                                                break;
                                        case 2:
                                                destroyer_end_num = destroyer_start_num - 10;
                                                break;
                                        case 3:
                                                destroyer_end_num = destroyer_start_num + 10;
                                                break;
                                }

                                destroyer_end_x = destroyer_end_num / 10;
                                destroyer_end_y = destroyer_end_num % 10;
                        }
                }


                BOARD_2[destroyer_end_y][destroyer_end_x] = DESTROYER;
        }


}

/* places a patrol boat at random on board */
void generate_patrol(uint8_t board){
        /*set patrol positition */
        uint8_t patrol_num = rand() % 100;
        uint8_t patrol_x = patrol_num / 10;
        uint8_t patrol_y = patrol_num % 10;

        /* check board */
        if(board == 1){
                /* loop till spot is empty */
                while(BOARD_1[patrol_y][patrol_x] != EMPTY){
                        patrol_num = rand() % 100;
                        patrol_x = patrol_num / 10;
                        patrol_y = patrol_num % 10;
                }

                BOARD_1[patrol_y][patrol_x] = PATROL;
        }

        /* check board */
        if(board == 2){
                /* loop till spot is empty */
                while(BOARD_2[patrol_y][patrol_x] != EMPTY){
                        patrol_num = rand() % 100;
                        patrol_x = patrol_num / 10;
                        patrol_y = patrol_num % 10;
                }

                BOARD_2[patrol_y][patrol_x] = PATROL;
        }
}

/* initialize the user board */
void init_user_board(){
        for(int i = 0; i < 10; i++){
                for(int j = 0; j < 10; j++){
                        BOARD_1[i][j] = EMPTY;
                }
        }
        /* prompt for auto generation */
        mvprintw(26, 35, "AUTO GENERATE MAP?   (Y/N):");
        refresh();

        /* move cursor to spot */
        move(26, 66);
        refresh();

        char *str = malloc(10);
        getstr(str);

        /* if yes, use generation methods to create grid */
        if(str[0] == 'y'){
                generate_carrier(1);

                generate_destroyer(1);
                generate_destroyer(1);
                generate_destroyer(1);

                generate_patrol(1);
                generate_patrol(1);
                generate_patrol(1);
                generate_patrol(1);
                update_screen();

        }
        /* otherwise call manual map generation method */
        else {

                mvprintw(26, 35, "                           ");
                refresh();

                mvprintw(26, 66, "  ");
                refresh();

                manual_map();



        }

        /* clear the area */
        mvprintw(26, 35, "                           ");
        refresh();

        mvprintw(26, 66, "    ");
        refresh();
}

/* manualy create map */
void manual_map(){
        uint8_t row, col;
        char *str = malloc(10);

        /* prompt patrol boat locations */
        for(int i = 0; i < 4; i++){
                mvprintw(26, 35, "Patrol Boat %d loc (rowcol):", i+1);
                refresh();

                move(26, 66);
                refresh();

                getstr(str);

                switch(str[0]){
                        case 'a':
                        case 'A':
                                col = 0;
                        break;

                        case 'b':
                        case 'B':
                                col = 1;
                        break;

                        case 'c':
                        case 'C':
                                col = 2;
                        break;

                        case 'd':
                        case 'D':
                                col = 3;
                        break;

                        case 'e':
                        case 'E':
                                col = 4;
                        break;

                        case 'f':
                        case 'F':
                                col = 5;
                        break;

                        case 'g':
                        case 'G':
                                col = 6;
                        break;

                        case 'h':
                        case 'H':
                                col = 7;
                        break;

                        case 'i':
                        case 'I':
                                col = 8;
                        break;

                        case 'j':
                        case 'J':
                                col = 9;
                        break;

                }

                if(str[1] == ' '){
                        switch(str[2]){
                                case '1':
                                        if(str[3] == '0'){
                                                row = 9;
                                        }
                                        else row = 0;
                                break;

                                case '2':
                                        row = 1;
                                break;

                                case '3':
                                        row = 2;
                                break;

                                case '4':
                                        row = 3;
                                break;

                                case '5':
                                        row = 4;
                                break;

                                case '6':
                                        row = 5;
                                break;

                                case '7':
                                        row = 6;
                                break;

                                case '8':
                                        row = 7;
                                break;

                                case '9':
                                        row = 8;
                                break;
                        }
                }

                else {
                        switch(str[1]){
                                case '1':
                                        if(str[2] == '0'){
                                                row = 9;
                                        }
                                        else row = 0;
                                break;

                                case '2':
                                        row = 1;
                                break;

                                case '3':
                                        row = 2;
                                break;

                                case '4':
                                        row = 3;
                                break;

                                case '5':
                                        row = 4;
                                break;

                                case '6':
                                        row = 5;
                                break;

                                case '7':
                                        row = 6;
                                break;

                                case '8':
                                        row = 7;
                                break;

                                case '9':
                                        row = 8;
                                break;
                        }
                }

                BOARD_1[row][col] = PATROL;

                update_screen();

                mvprintw(26, 35, "                           ");
                refresh();

                mvprintw(26, 66, "    ");
                refresh();
        }

        /* prompt destroyer locations */
        for(int i = 0; i < 3; i++){
                mvprintw(26, 35, "Destroyer %d start (rowcol):", i+1);
                refresh();

                move(26, 66);
                refresh();

                getstr(str);

                switch(str[0]){
                        case 'a':
                        case 'A':
                                col = 0;
                        break;

                        case 'b':
                        case 'B':
                                col = 1;
                        break;

                        case 'c':
                        case 'C':
                                col = 2;
                        break;

                        case 'd':
                        case 'D':
                                col = 3;
                        break;

                        case 'e':
                        case 'E':
                                col = 4;
                        break;

                        case 'f':
                        case 'F':
                                col = 5;
                        break;

                        case 'g':
                        case 'G':
                                col = 6;
                        break;

                        case 'h':
                        case 'H':
                                col = 7;
                        break;

                        case 'i':
                        case 'I':
                                col = 8;
                        break;

                        case 'j':
                        case 'J':
                                col = 9;
                        break;

                }

                if(str[1] == ' '){
                        switch(str[2]){
                                case '1':
                                        if(str[3] == '0'){
                                                row = 9;
                                        }
                                        else row = 0;
                                break;

                                case '2':
                                        row = 1;
                                break;

                                case '3':
                                        row = 2;
                                break;

                                case '4':
                                        row = 3;
                                break;

                                case '5':
                                        row = 4;
                                break;

                                case '6':
                                        row = 5;
                                break;

                                case '7':
                                        row = 6;
                                break;

                                case '8':
                                        row = 7;
                                break;

                                case '9':
                                        row = 8;
                                break;
                        }
                }

                else {
                        switch(str[1]){
                                case '1':
                                        if(str[2] == '0'){
                                                row = 9;
                                        }
                                        else row = 0;
                                break;

                                case '2':
                                        row = 1;
                                break;

                                case '3':
                                        row = 2;
                                break;

                                case '4':
                                        row = 3;
                                break;

                                case '5':
                                        row = 4;
                                break;

                                case '6':
                                        row = 5;
                                break;

                                case '7':
                                        row = 6;
                                break;

                                case '8':
                                        row = 7;
                                break;

                                case '9':
                                        row = 8;
                                break;
                        }
                }

                BOARD_1[row][col] = DESTROYER;

                //update_screen();

                mvprintw(26, 35, "                           ");
                refresh();

                mvprintw(26, 66, "   ");
                refresh();

                mvprintw(26, 35, "Destroyer %d dir (u/d/l/r):", i+1);
                refresh();

                move(26, 66);
                refresh();

                getstr(str);

                switch(str[0]){
                        case 'u':
                                BOARD_1[row-1][col] = DESTROYER;
                        break;

                        case 'd':
                                BOARD_1[row+1][col] = DESTROYER;
                        break;

                        case 'l':
                                BOARD_1[row][col-1] = DESTROYER;
                        break;

                        case 'r':
                                BOARD_1[row][col+1] = DESTROYER;
                        break;

                }

                update_screen();

                mvprintw(26, 35, "                           ");
                refresh();

                mvprintw(26, 66, "    ");
                refresh();
        }

        /* prompt carrier location */
        mvprintw(26, 35, "Carrier start     (rowcol):");
        refresh();

        move(26, 66);
        refresh();

        getstr(str);

        switch(str[0]){
                case 'a':
                case 'A':
                        col = 0;
                break;

                case 'b':
                case 'B':
                        col = 1;
                break;

                case 'c':
                case 'C':
                        col = 2;
                break;

                case 'd':
                case 'D':
                        col = 3;
                break;

                case 'e':
                case 'E':
                        col = 4;
                break;

                case 'f':
                case 'F':
                        col = 5;
                break;

                case 'g':
                case 'G':
                        col = 6;
                break;

                case 'h':
                case 'H':
                        col = 7;
                break;

                case 'i':
                case 'I':
                        col = 8;
                break;

                case 'j':
                case 'J':
                        col = 9;
                break;

        }

        if(str[1] == ' '){
                switch(str[2]){
                        case '1':
                                if(str[3] == '0'){
                                        row = 9;
                                }
                                else row = 0;
                        break;

                        case '2':
                                row = 1;
                        break;

                        case '3':
                                row = 2;
                        break;

                        case '4':
                                row = 3;
                        break;

                        case '5':
                                row = 4;
                        break;

                        case '6':
                                row = 5;
                        break;

                        case '7':
                                row = 6;
                        break;

                        case '8':
                                row = 7;
                        break;

                        case '9':
                                row = 8;
                        break;
                }
        }

        else {
                switch(str[1]){
                        case '1':
                                if(str[2] == '0'){
                                        row = 9;
                                }
                                else row = 0;
                        break;

                        case '2':
                                row = 1;
                        break;

                        case '3':
                                row = 2;
                        break;

                        case '4':
                                row = 3;
                        break;

                        case '5':
                                row = 4;
                        break;

                        case '6':
                                row = 5;
                        break;

                        case '7':
                                row = 6;
                        break;

                        case '8':
                                row = 7;
                        break;

                        case '9':
                                row = 8;
                        break;
                }
        }

        BOARD_1[row][col] = CARRIER;

        mvprintw(26, 35, "                           ");
        refresh();

        mvprintw(26, 66, "    ");
        refresh();

        mvprintw(26, 35, "Destroyer    dir (u/d/l/r):");
        refresh();

        move(26, 66);
        refresh();

        getstr(str);

        switch(str[0]){
                case 'u':
                        BOARD_1[row-1][col] = CARRIER;
                        BOARD_1[row-2][col] = CARRIER;
                break;

                case 'd':
                        BOARD_1[row+1][col] = CARRIER;
                        BOARD_1[row+2][col] = CARRIER;
                break;

                case 'l':
                        BOARD_1[row][col-1] = CARRIER;
                        BOARD_1[row][col-2] = CARRIER;
                break;

                case 'r':
                        BOARD_1[row][col+1] = CARRIER;
                        BOARD_1[row][col+2] = CARRIER;
                break;

        }

        update_screen();

        mvprintw(26, 35, "                           ");
        refresh();

        mvprintw(26, 66, "    ");
        refresh();

}

/* set rows and columns */
void set_rc_cola(uint8_t *row, uint8_t *col, char *str){
        /* convert letters to numbers */
        switch(str[0]){
                case 'a':
                case 'A':
                        *col = 0;
                break;

                case 'b':
                case 'B':
                        *col = 1;
                break;

                case 'c':
                case 'C':
                        *col = 2;
                break;

                case 'd':
                case 'D':
                        *col = 3;
                break;

                case 'e':
                case 'E':
                        *col = 4;
                break;

                case 'f':
                case 'F':
                        *col = 5;
                break;

                case 'g':
                case 'G':
                        *col = 6;
                break;

                case 'h':
                case 'H':
                        *col = 7;
                break;

                case 'i':
                case 'I':
                        *col = 8;
                break;

                case 'j':
                case 'J':
                        *col = 9;
                break;

        }

        /* convert user row to array row */
        if(str[1] == ' '){
                switch(str[2]){
                        case '1':
                                if(str[3] == '0'){
                                        *row = 9;
                                }
                                else *row = 0;
                        break;

                        case '2':
                                *row = 1;
                        break;

                        case '3':
                                *row = 2;
                        break;

                        case '4':
                                *row = 3;
                        break;

                        case '5':
                                *row = 4;
                        break;

                        case '6':
                                *row = 5;
                        break;

                        case '7':
                                *row = 6;
                        break;

                        case '8':
                                *row = 7;
                        break;

                        case '9':
                                *row = 8;
                        break;
                }
        }

        else {
                switch(str[1]){
                        case '1':
                                if(str[2] == '0'){
                                        *row = 9;
                                }
                                else *row = 0;
                        break;

                        case '2':
                                *row = 1;
                        break;

                        case '3':
                                *row = 2;
                        break;

                        case '4':
                                *row = 3;
                        break;

                        case '5':
                                *row = 4;
                        break;

                        case '6':
                                *row = 5;
                        break;

                        case '7':
                                *row = 6;
                        break;

                        case '8':
                                *row = 7;
                        break;

                        case '9':
                                *row = 8;
                        break;
                }
        }
}

/* sets the selected cell to hit or miss */
int hit_or_miss(uint8_t row, uint8_t col, uint8_t board, uint8_t *hits){

        /* if the method was called to check board 1 */
        if(board == 1){

                /* check the cell at [row][col] and
                   update appropriately */
                if(BOARD_1[row][col] == CARRIER ||
                        BOARD_1[row][col] == PATROL ||
                        BOARD_1[row][col] == DESTROYER ||
                        BOARD_1[row][col] == HIT){

                        BOARD_1[row][col] = HIT;

                        /* update kill count */
                        *hits += 1;

                        return 1;
                }

                else BOARD_1[row][col] = MISS;
                return 0;
        }

        /* if the method was called to check board 2 */
        else if(board == 2){

                /* check the cell at [row][col] and
                   update appropriately */
                if(BOARD_2[row][col] == CARRIER ||
                        BOARD_2[row][col] == PATROL ||
                        BOARD_2[row][col] == DESTROYER ||
                        BOARD_2[row][col] == HIT){

                        BOARD_2[row][col] = HIT;

                        /* update kill count */
                        *hits += 1;
                        return 1;
                }

                else BOARD_2[row][col] = MISS;
                return 0;
        }
        return 0;
}

/*checks the hits to see if all boats are sunk */
void win_check(uint8_t player, uint8_t hits){
                /*check for win cases */
                if(hits == 13){
                        endwin(); //?
                        if(player == 1)
                                printf("You won!\n");
                        else
                                printf("You Lose!\n");
                        exit(0);
                }
}

/* handles user turn */
void user_turn(){
        mvprintw(26, 35, "Your Move?       (row col):");
        move(26, 66);
        refresh();
}

/* does computer turn */
void computer_turn(uint8_t *row, uint8_t *col, uint8_t *hits){

        uint8_t hit_num;

        mvprintw(26, 35, "Opponent's Move  (row col):");
        move(26, 66);
        refresh();

        /* do enemy turn stuff */
        /* generate the random spot */
        if((first_hit->dir == 0) && (sec_hit->dir == 0)){

                *row = rand()%10;
                *col = rand()%10;

                while((BOARD_1[*row][*col] == MISS) || (BOARD_1[*row][*col] == HIT)){
                        *row = rand()%10;
                        *col = rand()%10;
                }

                /* sets the cell as hit or miss and
                   updates hit count, sets struct for
                   hit searching */
                if(hit_or_miss(*row, *col, 1, &hit_num)){
                        set_hits(*row, *col);
                }
        }

        /* if last turn was a hit */
        else if(sec_hit->dir == 0){

                /* go through the surrounding cells
                   and check before selecting another
                   random cell */
                if(first_hit->dir == 1){
                        if(first_hit->row_1 !=255){
                                *row = first_hit->row_1;
                                *col = first_hit->col_1;

                                if(hit_or_miss(*row, *col, 1, &hit_num)){
                                        //set_hits(*row, *col);
                                }
                        }
                        else first_hit->dir = 2;
                }

                if(first_hit->dir == 2){
                        if(first_hit->row_2 !=255){
                                *row = first_hit->row_2;
                                *col = first_hit->col_2;

                                if(hit_or_miss(*row, *col, 1, &hit_num)){
                                        //set_hits(*row, *col);
                                }
                        }
                        else first_hit->dir = 3;
                }

                if(first_hit->dir == 3){
                        if(first_hit->row_3 !=255){
                                *row = first_hit->row_3;
                                *col = first_hit->col_3;

                                if(hit_or_miss(*row, *col, 1, &hit_num)){
                                        //set_hits(*row, *col);
                                }
                        }
                        else first_hit->dir = 4;
                }

                if(first_hit->dir == 4){
                        if(first_hit->row_4 !=255){
                                *row = first_hit->row_4;
                                *col = first_hit->col_4;
                                first_hit->dir = 0;

                                if(hit_or_miss(*row, *col, 1, &hit_num)){
                                        //set_hits(*row, *col);
                                }
                        }
                        else no_hits = 1;
                }

                /* cell gets updated to 0 on last search */
                if(first_hit->dir != 0){
                        first_hit->dir++;
                }

                /* if at last cell and last cell isn't in bounds
                   select a new spot */
                if(no_hits){
                        *row = rand()%10;
                        *col = rand()%10;

                        while((BOARD_1[*row][*col] == MISS) || (BOARD_1[*row][*col] == HIT)){
                                *row = rand()%10;
                                *col = rand()%10;
                        }

                        if(hit_or_miss(*row, *col, 1, &hit_num)){
                                set_hits(*row, *col);
                        }
                }

        }

        *hits = hit_num;

        sleep(1);

        /* convert values for printing */
        switch(*col){
                case 0:
                        mvprintw(26, 66, "A");
                        refresh();
                break;

                case 1:
                        mvprintw(26, 66, "B");
                        refresh();
                break;

                case 2:
                        mvprintw(26, 66, "C");
                        refresh();
                break;

                case 3:
                        mvprintw(26, 66, "D");
                        refresh();
                break;

                case 4:
                        mvprintw(26, 66, "E");
                        refresh();
                break;

                case 5:
                        mvprintw(26, 66, "F");
                        refresh();
                break;

                case 6:
                        mvprintw(26, 66, "G");
                        refresh();
                break;

                case 7:
                        mvprintw(26, 66, "H");
                        refresh();
                break;

                case 8:
                        mvprintw(26, 66, "I");
                        refresh();
                break;

                case 9:
                        mvprintw(26, 66, "J");
                        refresh();
                break;

        }

        sleep(1);
        mvprintw(26, 67, "%d", (*row)+1);
        refresh();

        sleep(1);

        return;
}

/* sets the hit_struct */
void set_hits(uint8_t row, uint8_t col){
        first_hit->dir++;
        first_hit->dir_num = 4;

        if(row-1 > 0 && ((BOARD_1[row-1][col] != MISS) && (BOARD_1[row-1][col] != HIT))){
                first_hit->row_1 = row-1;
                first_hit->col_1 = col;
        } else {
                first_hit->row_1 = -1;
                first_hit->col_1 = -1;
                first_hit->dir_num--;
        }

        if(col+1 < 10 && ((BOARD_1[row][col+1] != MISS) && (BOARD_1[row][col+1] != HIT))){
                first_hit->row_2 = row;
                first_hit->col_2 = col+1;
        } else {
                first_hit->row_2 = -1;
                first_hit->col_2 = -1;
                first_hit->dir_num--;
        }

        if(row+1 < 10 && ((BOARD_1[row+1][col] != MISS) && (BOARD_1[row+1][col] != HIT))){
                first_hit->row_3 = row+1;
                first_hit->col_3 = col;
        } else {
                first_hit->row_3 = -1;
                first_hit->col_3 = -1;
                first_hit->dir_num--;
        }

        if(col-1 > 0 && ((BOARD_1[row][col-1] != MISS) && (BOARD_1[row][col-1] != HIT))){
                first_hit->row_4 = row;
                first_hit->col_4 = col-1;
        } else {
                first_hit->row_4 = -1;
                first_hit->col_4 = -1;
                first_hit->dir_num--;
        }
}

/*draw the game to the screen */
void draw_screen(){
        draw_windows();
        draw_title();

        /* print leading dashes */
        for(int i = 2; i < 20; i++){
                mvaddch(8, i, '-');
        }

        /* print board id */
        mvprintw(8, 20, "Your Board");
        refresh();

        /*print middle dashes */
        for(int i = 30; i < 71; i++){
                mvaddch(8, i, '-');
        }

        /* print board id */
        mvprintw(8, 71, "Opponent's Board");
        refresh();


        /* print end dashes */
        for(int i = 87; i < 103; i++){
                mvaddch(8, i, '-');
        }
        refresh();

        /* print left board row nums */
        for(int i = 1; i < 11; i++){
                char num[2];
                sprintf(num, "%d", i);
                if(i != 10){
                        mvprintw(i+11, 4, num);
                } else  mvprintw(i+11, 3, "10");
        }

        /* print left board */
        int j = 0;
        for(int i = 2; i < 40; i+=4){
                char num[4];
                sprintf(num, "%d", i);
                mvaddch(11, i+6, j+65);
                mvaddch(11, i+4, '|');
                mvaddch(11, i+8, '|');

                for(int k = 12; k < 22; k++){
                        mvaddch(k, i+4, '|');
                        mvaddch(k, i+8, '|');

	                attron(COLOR_PAIR(1));
                        mvprintw(k, i+5, "   ");
                        attroff(COLOR_PAIR(1));
                }
                refresh();
                j++;
        }
        refresh();

        /* print right board row nums */
        for(int i = 1; i < 11; i++){
                char num[2];
                sprintf(num, "%d", i);
                if(i != 10){
                        mvprintw(i+11, 58, num);
                } else mvprintw(i+11, 57, "10");
        }

        /* print right board */
        j = 0;
        for(int i = 2; i < 40; i+=4){
                char num[4];
                sprintf(num, "%d", i);
                mvaddch(11, i+60, j+65);
                mvaddch(11, i+58, '|');
                mvaddch(11, i+62, '|');

                for(int k = 12; k < 22; k++){
                        mvaddch(k, i+58, '|');
                        mvaddch(k, i+62, '|');


                        //if(BOARD_2[k-12][j] == EMPTY){
        	                attron(COLOR_PAIR(1));
                                mvprintw(k, i+59, "   ");
                                attroff(COLOR_PAIR(1));
                        //}
                }
                refresh();
                j++;
        }

        refresh();
}

/* updates the grid */
void update_screen(){
        int j = 0;

        /* update board 1 checking for contents */
        for(int i = 2; i < 40; i+=4){
                char num[4];
                sprintf(num, "%d", i);
                mvaddch(11, i+6, j+65);
                mvaddch(11, i+4, '|');
                mvaddch(11, i+8, '|');

                for(int k = 12; k < 22; k++){
                        mvaddch(k, i+4, '|');
                        mvaddch(k, i+8, '|');

                        /* change color for each definition */
                        if(BOARD_1[k-12][j] == EMPTY){
        	                attron(COLOR_PAIR(1));
                                mvprintw(k, i+5, "   ");
                                attroff(COLOR_PAIR(1));
                        }

                        /* is carrier */
                        if(BOARD_1[k-12][j] == CARRIER){
        	                attron(COLOR_PAIR(2));
                                mvprintw(k, i+5, " 0 ");
                                attroff(COLOR_PAIR(2));
                        }

                        /* is destroyer */
                        if(BOARD_1[k-12][j] == DESTROYER){
        	                attron(COLOR_PAIR(3));
                                mvprintw(k, i+5, " 0 ");
                                attroff(COLOR_PAIR(3));
                        }

                        /* is patrol */
                        if(BOARD_1[k-12][j] == PATROL){
        	                attron(COLOR_PAIR(4));
                                mvprintw(k, i+5, " 0 ");
                                attroff(COLOR_PAIR(4));
                        }

                        /* is miss */
                        if(BOARD_1[k-12][j] == MISS){
        	                attron(COLOR_PAIR(1));
                                mvprintw(k, i+5, " M ");
                                attroff(COLOR_PAIR(1));
                        }

                        /* is hit */
                        if(BOARD_1[k-12][j] == HIT){
        	                attron(COLOR_PAIR(6));
                                mvprintw(k, i+5, " X ");
                                attroff(COLOR_PAIR(6));
                        }
                }
                refresh();
                j++;
        }
        refresh();

        /* board 2 regeneration */
        j = 0;
        for(int i = 2; i < 40; i+=4){
                char num[4];
                sprintf(num, "%d", i);
                mvaddch(11, i+60, j+65);
                mvaddch(11, i+58, '|');
                mvaddch(11, i+62, '|');

                for(int k = 12; k < 22; k++){
                        mvaddch(k, i+58, '|');
                        mvaddch(k, i+62, '|');

                        /* DEBUG: remove conditional for gameplay */
                        //if(BOARD_2[k-12][j] == EMPTY){
        	                attron(COLOR_PAIR(1));
                                mvprintw(k, i+59, "   ");
                                attroff(COLOR_PAIR(1));
                        //}

                        /* is miss */
                        if(BOARD_2[k-12][j] == MISS){
        	                attron(COLOR_PAIR(1));
                                mvprintw(k, i+59, " M ");
                                attroff(COLOR_PAIR(1));
                        }

                        /* is hit */
                        if(BOARD_2[k-12][j] == HIT){
        	                attron(COLOR_PAIR(6));
                                mvprintw(k, i+59, " X ");
                                attroff(COLOR_PAIR(6));
                        }
                }
                refresh();
                j++;
        }
}

/* draws the windows for the screen */
void draw_windows(){

        /* create outer window */
	my_wins[0] = create_newwin(30, 105, 0, 0);

        /* create opponent window */
	my_wins[1] = create_newwin(13, 49, 10, 1);

        /* create user window */
	my_wins[2] = create_newwin(13, 49, 10, 55);

        /* create user input window */
        my_wins[3] = create_newwin(5, 41, 24, 32);

        /* create prompt window */
        my_wins[4] = create_newwin(3, 30, 25, 33);

        /* create input window */
        my_wins[5] = create_newwin(3, 9, 25, 63);

        /* create title window */
        my_wins[6] = create_newwin(6, 103, 1, 1);
}

/* draw the stupidly extravagant title */
/* for  your own sanity don't look in here */
void draw_title(){
        int offset=1;
        int y_off = 1;
        /* char B */

        mvaddch(1+y_off, 2+offset, '/');
        mvaddch(2+y_off, 2+offset, '\\');
        mvaddch(2+y_off, 4+offset, '\\');
        mvaddch(3+y_off, 5+offset, '\\');

        mvaddch(1+y_off, 3+offset, '\\');
        mvaddch(2+y_off, 4+offset, '\\');
        mvaddch(3+y_off, 3+offset, '\\');
        mvaddch(4+y_off, 4+offset, '\\');
        mvaddch(4+y_off, 5+offset, '/');

        mvprintw(1+y_off, 4+offset, "````");
        mvaddch(1+y_off, 8+offset, '\\');
        mvaddch(2+y_off, 8+offset, '<');
        attron(A_UNDERLINE);
        mvprintw(1+y_off, 6+offset, "`");
        attroff(A_UNDERLINE);
        mvaddch(3+y_off, 10+offset, '\\');
        mvprintw(3+y_off, 7+offset, "``");
        mvprintw(3+y_off, 6+offset, " `` ");
        attron(A_UNDERLINE);
        mvprintw(4+y_off, 6+offset, "`````");
        attroff(A_UNDERLINE);
        mvaddch(4+y_off, 10+offset, '/');


        /* char A */

        mvaddch(1+y_off, 12+offset, '/');
        mvaddch(2+y_off, 12+offset, '\\');
        mvaddch(2+y_off, 14+offset, '\\');
        mvaddch(3+y_off, 15+offset, '\\');

        mvaddch(1+y_off, 13+offset, '\\');
        mvaddch(2+y_off, 14+offset, '\\');
        mvaddch(3+y_off, 13+offset, '\\');
        mvaddch(4+y_off, 14+offset, '\\');

        mvaddch(4+y_off, 15+offset, '/');
        attron(A_UNDERLINE);
        mvaddch(4+y_off, 16+offset, ' ');
        attroff(A_UNDERLINE);

        mvprintw(1+y_off, 14+offset, "`````");
        //attron(A_UNDERLINE);
        mvprintw(2+y_off, 16+offset, "```");
        //attroff(A_UNDERLINE);
        mvaddch(1+y_off, 19+offset, '\\');
        mvaddch(2+y_off, 20+offset, '\\');
        mvaddch(3+y_off, 21+offset, '\\');

        mvaddch(3+y_off, 17+offset, '\\');
        attron(A_UNDERLINE);
        mvaddch(3+y_off, 16+offset, ' ');
        attroff(A_UNDERLINE);


        mvaddch(3+y_off, 19+offset, '\\');

        mvaddch(3+y_off, 21+offset, '\\');
        attron(A_UNDERLINE);
        mvaddch(3+y_off, 20+offset, ' ');
        attroff(A_UNDERLINE);

        mvaddch(4+y_off, 17+offset, '/');

        mvaddch(4+y_off, 21+offset, '/');

        mvaddch(4+y_off, 19+offset, '/');
        attron(A_UNDERLINE);
        mvaddch(4+y_off, 20+offset, ' ');
        attroff(A_UNDERLINE);

        mvaddch(4+y_off, 18+offset, '\\');

        mvprintw(3+y_off, 18+offset, "^");

        /* character T */

        mvaddch(1+y_off, 22+offset, '/');
        mvaddch(2+y_off, 22+offset, '\\');
        mvaddch(2+y_off, 26+offset, '\\');
        mvaddch(3+y_off, 27+offset, '\\');

        attron(A_UNDERLINE);
        mvaddch(1+y_off, 24+offset, '`');
        attroff(A_UNDERLINE);

        mvprintw(1+y_off, 25+offset, "`````");
        attron(A_UNDERLINE);
        mvaddch(1+y_off, 29+offset, '`');
        attroff(A_UNDERLINE);
        mvaddch(1+y_off, 30+offset, '\\');

        mvaddch(2+y_off, 28+offset, '\\');
        mvaddch(3+y_off, 29+offset, '\\');
        mvaddch(2+y_off, 30+offset, '/');

        attron(A_UNDERLINE);
        mvaddch(2+y_off, 29+offset, ' ');
        attroff(A_UNDERLINE);


        mvaddch(1+y_off, 23+offset, '\\');
        attron(A_UNDERLINE);
        mvaddch(2+y_off, 24+offset, ' ');
        attroff(A_UNDERLINE);
        mvaddch(3+y_off, 25+offset, '\\');
        mvaddch(4+y_off, 26+offset, '\\');
        mvaddch(4+y_off, 27+offset, '/');

        mvaddch(4+y_off, 29+offset, '/');

        attron(A_UNDERLINE);
        mvaddch(3+y_off, 28+offset, ' ');
        attroff(A_UNDERLINE);

        attron(A_UNDERLINE);
        mvaddch(4+y_off, 28+offset, ' ');
        attroff(A_UNDERLINE);


        /* character T */

        mvaddch(1+y_off, 33+offset, '/');
        mvaddch(2+y_off, 33+offset, '\\');
        mvaddch(2+y_off, 37+offset, '\\');
        mvaddch(3+y_off, 38+offset, '\\');

        attron(A_UNDERLINE);
        mvaddch(1+y_off, 35+offset, '`');
        attroff(A_UNDERLINE);

        mvprintw(1+y_off, 36+offset, "`````");
        attron(A_UNDERLINE);
        mvaddch(1+y_off, 40+offset, '`');
        attroff(A_UNDERLINE);
        mvaddch(1+y_off, 41+offset, '\\');

        mvaddch(2+y_off, 39+offset, '\\');
        mvaddch(3+y_off, 40+offset, '\\');
        mvaddch(2+y_off, 41+offset, '/');

        attron(A_UNDERLINE);
        mvaddch(2+y_off, 40+offset, ' ');
        attroff(A_UNDERLINE);


        mvaddch(1+y_off, 34+offset, '\\');
        attron(A_UNDERLINE);
        mvaddch(2+y_off, 35+offset, ' ');
        attroff(A_UNDERLINE);
        mvaddch(3+y_off, 36+offset, '\\');
        mvaddch(4+y_off, 37+offset, '\\');
        mvaddch(4+y_off, 38+offset, '/');

        mvaddch(4+y_off, 40+offset, '/');

        attron(A_UNDERLINE);
        mvaddch(3+y_off, 39+offset, ' ');
        attroff(A_UNDERLINE);

        attron(A_UNDERLINE);
        mvaddch(4+y_off, 39+offset, ' ');
        attroff(A_UNDERLINE);

        /* character L */

        mvaddch(1+y_off, 44+offset, '/');
        mvaddch(2+y_off, 44+offset, '\\');
        mvaddch(2+y_off, 46+offset, '\\');
        mvaddch(3+y_off, 47+offset, '\\');

        mvaddch(1+y_off, 45+offset, '\\');
        mvaddch(2+y_off, 46+offset, '\\');
        mvaddch(3+y_off, 45+offset, '\\');
        mvaddch(4+y_off, 46+offset, '\\');
        mvaddch(4+y_off, 47+offset, '/');

        mvprintw(1+y_off, 46+offset, "`");


        mvaddch(1+y_off, 47+offset, '\\');
        mvaddch(2+y_off, 48+offset, '\\');
        mvaddch(3+y_off, 49+offset, '\\');
        mvaddch(4+y_off, 50+offset, '\\');


        attron(A_UNDERLINE);
        mvprintw(4+y_off, 48+offset, "    ");
        attroff(A_UNDERLINE);

        attron(A_UNDERLINE);
        mvprintw(2+y_off, 49+offset, "  ");
        attroff(A_UNDERLINE);

        mvprintw(3+y_off, 51+offset, "\\");

        mvaddch(4+y_off, 51+offset, '/');

        attron(A_UNDERLINE);
        mvprintw(3+y_off, 48+offset, "   ");
        attroff(A_UNDERLINE);

        /* character E */

        mvaddch(1+y_off, 53+offset, '/');
        mvaddch(2+y_off, 53+offset, '\\');
        mvaddch(2+y_off, 55+offset, '\\');
        mvaddch(3+y_off, 56+offset, '\\');

        mvaddch(1+y_off, 54+offset, '\\');
        mvaddch(2+y_off, 55+offset, '\\');
        mvaddch(3+y_off, 54+offset, '\\');
        mvaddch(4+y_off, 55+offset, '\\');
        mvaddch(4+y_off, 56+offset, '/');

        mvprintw(1+y_off, 55+offset, "````");
        mvaddch(1+y_off, 59+offset, '\\');
        mvaddch(2+y_off, 59+offset, '\\');
        attron(A_UNDERLINE);
        mvprintw(1+y_off, 57+offset, "`");
        attroff(A_UNDERLINE);

        mvprintw(2+y_off, 57+offset, "`");

        attron(A_UNDERLINE);
        mvprintw(2+y_off, 58+offset, "`");
        attroff(A_UNDERLINE);

        mvaddch(3+y_off, 61+offset, '\\');

        mvprintw(3+y_off, 58+offset, "``");
        mvprintw(3+y_off, 57+offset, " `` ");

        attron(A_UNDERLINE);
        mvprintw(4+y_off, 57+offset, "`````");
        attroff(A_UNDERLINE);
        mvaddch(4+y_off, 61+offset, '/');


        /* character S */
        mvaddch(1+y_off, 63+offset, '/');
        mvaddch(1+y_off, 64+offset, '\\');
        mvaddch(2+y_off, 63+offset, '\\');
        mvaddch(3+y_off, 64+offset, '\\');
        mvaddch(2+y_off, 65+offset, '\\');
        mvaddch(3+y_off, 65+offset, '/');
        mvaddch(3+y_off, 66+offset, '\\');
        mvaddch(4+y_off, 65+offset, '\\');
        mvaddch(4+y_off, 66+offset, '/');

        mvprintw(1+y_off, 65+offset, "``");
        attron(A_UNDERLINE);
        mvprintw(1+y_off, 67+offset, "``");
        attroff(A_UNDERLINE);
        mvaddch(1+y_off, 69+offset, '\\');
        mvaddch(2+y_off, 67+offset, '\\');

        attron(A_UNDERLINE);
        mvprintw(2+y_off, 68+offset, "  ");
        attroff(A_UNDERLINE);
        mvaddch(3+y_off, 70+offset, '\\');

        attron(A_UNDERLINE);
        mvprintw(3+y_off, 67+offset, "`` ");
        attroff(A_UNDERLINE);

        mvaddch(4+y_off, 70+offset, '/');

        attron(A_UNDERLINE);
        mvprintw(4+y_off, 67+offset, "   ");
        attroff(A_UNDERLINE);

        /* character H */
        mvaddch(1+y_off, 72+offset, '/');
        mvaddch(2+y_off, 72+offset, '\\');
        mvaddch(2+y_off, 74+offset, '\\');
        mvaddch(3+y_off, 75+offset, '\\');

        mvaddch(1+y_off, 73+offset, '\\');
        mvaddch(2+y_off, 74+offset, '\\');
        mvaddch(3+y_off, 73+offset, '\\');
        mvaddch(4+y_off, 74+offset, '\\');
        mvaddch(4+y_off, 75+offset, '/');

        mvprintw(1+y_off, 74+offset, "`");
        mvaddch(1+y_off, 75+offset, '\\');
        mvaddch(2+y_off, 76+offset, '\\');
        mvaddch(3+y_off, 77+offset, '\\');

        attron(A_UNDERLINE);
        mvprintw(3+y_off, 76+offset, " ");
        mvprintw(4+y_off, 76+offset, " ");
        attroff(A_UNDERLINE);

        mvaddch(4+y_off, 77+offset, '/');

        attron(A_UNDERLINE);
        mvprintw(2+y_off, 77+offset, "`");
        attroff(A_UNDERLINE);

        mvaddch(1+y_off, 77+offset, '\\');
        mvaddch(2+y_off, 78+offset, '\\');
        mvaddch(3+y_off, 79+offset, '\\');
        mvaddch(4+y_off, 79+offset, '/');

        mvaddch(1+y_off, 79+offset, '\\');
        mvaddch(2+y_off, 80+offset, '\\');
        mvaddch(3+y_off, 81+offset, '\\');
        mvaddch(4+y_off, 81+offset, '/');

        attron(A_UNDERLINE);
        mvprintw(3+y_off, 80+offset, " ");
        mvprintw(4+y_off, 80+offset, " ");
        attroff(A_UNDERLINE);

        mvprintw(1+y_off, 78+offset, "`");

        /* character I */

        offset++;

        mvaddch(1+y_off, 82+offset, '/');
        mvaddch(2+y_off, 82+offset, '\\');
        mvaddch(2+y_off, 84+offset, '\\');
        mvaddch(3+y_off, 85+offset, '\\');

        mvaddch(1+y_off, 83+offset, '\\');
        mvaddch(2+y_off, 84+offset, '\\');
        mvaddch(3+y_off, 83+offset, '\\');
        mvaddch(4+y_off, 84+offset, '\\');
        mvaddch(4+y_off, 85+offset, '/');

        mvaddch(1+y_off, 83+offset, '\\');
        mvaddch(1+y_off, 84+offset, '`');
        mvaddch(2+y_off, 84+offset, '\\');
        mvaddch(2+y_off, 86+offset, '\\');
        mvaddch(3+y_off, 87+offset, '\\');

        mvaddch(1+y_off, 85+offset, '\\');
        mvaddch(3+y_off, 85+offset, '\\');
        mvaddch(4+y_off, 87+offset, '/');

        attron(A_UNDERLINE);
        mvprintw(3+y_off, 86+offset, " ");
        mvprintw(4+y_off, 86+offset, " ");
        attroff(A_UNDERLINE);

        /* character P */

        offset++;

        mvaddch(1+y_off, 88+offset, '/');
        mvaddch(2+y_off, 88+offset, '\\');
        mvaddch(3+y_off, 89+offset, '\\');

        mvaddch(1+y_off, 89+offset, '\\');
        mvaddch(2+y_off, 90+offset, '\\');
        mvaddch(3+y_off, 91+offset, '\\');
        mvaddch(4+y_off, 90+offset, '\\');
        mvaddch(4+y_off, 91+offset, '/');

         mvaddch(1+y_off, 91+offset, '`');
        mvaddch(3+y_off, 93+offset, '\\');
        mvaddch(4+y_off, 93+offset, '/');
        mvaddch(3+y_off, 96+offset, '/');

        attron(A_UNDERLINE);
        mvprintw(1+y_off, 92+offset, "`");
        mvprintw(3+y_off, 94+offset, "  ");
        mvprintw(4+y_off, 92+offset, " ");
        attroff(A_UNDERLINE);

        mvaddch(1+y_off, 90+offset, '`');
        attron(A_UNDERLINE);
        mvprintw(2+y_off, 93+offset, "  ");
        attroff(A_UNDERLINE);

        mvaddch(1+y_off, 93+offset, '`');
        mvaddch(1+y_off, 94+offset, '`');

        mvaddch(1+y_off, 95+offset, '\\');
        mvaddch(2+y_off, 96+offset, '\\');

        attron(A_UNDERLINE);
        mvprintw(2+y_off, 95+offset, " ");
        attroff(A_UNDERLINE);
}

/* creates a new window with a given height, width, start x pos, and start y pos */
WINDOW *create_newwin(int height, int width, int starty, int startx){	WINDOW *local_win;
	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters
					 * for the vertical and horizontal
					 * lines			*/
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}