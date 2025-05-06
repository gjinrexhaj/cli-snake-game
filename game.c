/* 
* CS 355 System Programming -- Snake Game Final Project
*
* Snake moves with a starting length of 5 moving right
* Snake moves in all directions
* Border is visible
*
* Programmers: Tyler Brown, Gjin Rexhaj
* Date: May 1st, 2025
*/

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include <sys/ioctl.h>


// Function prototypes
int get_terminal_width();
int get_terminal_height();
void game_over();

#define SNAKE_CHAR "p"
int move_y;
int move_x;

// Programmer: Gjin Rexhaj
typedef struct { // declare a struct that holds x and y coordinates, name it SnakeSegment
    int x;
    int y;
} SnakeSegment;

typedef struct { // define Trophy struct, which contains a given trophies' xy coord
    int x;
    int y;
} Trophy;

SnakeSegment snake[100]; // initialize an array of size 100, each array position holds an x and y coordinate
int snake_length = 3; // start with a snake size of size 3

// Programmer: Gjin Rexhaj
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

// Programmer: Tyler Brown
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
    for (int i = 1; i < snake_length; i++) { // kills the snake if the snake runs into itself (loops across all body parts excluding the head)
        if (snake[i].x == snake[0].x && snake[i].y == snake[0].y) {
            game_over();
        }
    }
    move(snake[0].y, snake[0].x); // move the head in the specified direction
    addstr(SNAKE_CHAR); // add the string to the console
    refresh(); // refresh the string
}

// Collaborative effort to make the main function
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

        // TODO: add trophy collision and snake growth logic (also drop new trophy if eat)

        // TODO: add snake colliding with itself logic (if self collide, then game over)

        // Check if snake is out of bounds, using ioctl
        if (snake[0].x < 1 || snake[0].x > get_terminal_width() - 2 || snake[0].y < 1 || snake[0].y > get_terminal_height() - 2) {
            // TODO: make game over screen prettier
            game_over(); // this isnt prettier but it provides a scaffolding!
            exit(0);
        }

        // TODO: check for trophy expiration (randomize a given trophies'
        //  expiration time, drop new trophy if expire)

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

        // TODO: add win condition (snake length > 1/2 size of terminal window
    }
    endwin();
}


// IMPLEMENT PROTOTYPES
int get_terminal_width() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

int get_terminal_height()
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_row;
}

void game_over() { // cleaner game over screen -- appears when snake runs into itself or hits a wall
    clear(); // clears screen
    refresh(); // refresh the screen
    // get back to the middle of the terminal
    int x = COLS / 2;
    int y = LINES / 2;
    move(y, x); // move cursor to the middle of the terminal
    addstr("**Game Over**"); // print game over
    refresh(); // refresh the screen to update it with the changes
    usleep(1000000); // sleep console, close program
    endwin();
    exit(0);
}

