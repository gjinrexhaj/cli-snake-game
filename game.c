#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <unistd.h>

#define SNAKE_CHAR "p"
int move_y;
int move_x;

typedef struct { // declare a struct that holds x and y coordinates, name it SnakeSegment
    int x;
    int y;
} SnakeSegment;

SnakeSegment snake[100]; // initialize an array of size 100, each array position holds an x and y coordinate
int snake_length = 5; // start with a snake size of size 5

void initialize_snake(int snake_length) {
    // starts at the middle of the screen
    int start_x = COLS / 2;
    int start_y = LINES / 2;

    for (int i = 0; i < snake_length; i++) {
       snake[i].y = start_y; // start at the middle of the console (LINES / 2)
       snake[i].x = start_x - i; // start_x - i to add characters to the back of the snake
       move(snake[i].y, snake[i].x); // move the snake to the coordinates specified in the struct
       addstr(SNAKE_CHAR); // add the string to the screen
    }
}

void move_snake(int move_x, int move_y) {
    move(snake[snake_length - 1].y, snake[snake_length - 1].x); // moves cursor to the end of the snake
    addstr(" "); // adds a space to the end of the snake, effectively removes the tail at the new cursor position
    for (int i = snake_length - 1; i > 0; i--) { // move the snake forward, excluding the head
        snake[i] = snake[i - 1];
    }
    // set the head of the snake to an individual value
    // the snake will trail the head with this logic
    snake[0].x += move_x; 
    snake[0].y += move_y; 

    move(snake[0].y, snake[0].x); // move the head in the specified direction
    addstr(SNAKE_CHAR); // add the string to the console
    refresh(); // refresh the string
}

int main() {
    initscr(); // initialize the curses screen
    clear(); // clears screen
    noecho(); // Will capture keystrokes, nothing shown on the screen (unless a key corresponds to an addstr string)
    curs_set(0); // hides the cursor

    box(stdscr, 0, 0); // draw border

    keypad(stdscr, TRUE); // captures the entire screen
    nodelay(stdscr, TRUE); // makes getch() not interrupt the console and wait for an input
    // snake always starts by moving right at the start
    move_x = 1, move_y = 0;
    initialize_snake(snake_length); // initialize the snake length
    refresh();

    // Infinite loop to log arrow keystrokes and move the snake
    while(1) {
        
        int ch = getch();
        switch(ch) {
            // change an x or y value by 1 in a specific direction to move the snake
            case KEY_UP:
                move_x = 0;
                move_y = -1;
                break;
            case KEY_DOWN:
                move_x = 0;
                move_y = 1;
                break;
            case KEY_LEFT:
                move_x = -1;
                move_y = 0;
                break;
            case KEY_RIGHT:
                move_x = 1;
                move_y = 0;
                break;
            case 'x':
                endwin();
                return 0;
        }
        move_snake(move_x, move_y); // move the snake 1 space in specified direction
        refresh(); // refresh to update the console window
        usleep(100000); // sleep the console for a little bit
    }
    endwin();
}
