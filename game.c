/* 
* CS 355 System Programming -- Snake Game Final Project
*
* Text-based version of snake game!
* Starts with a snake of length 3 moving in a random direction every time
* A trophy with a randomly generated value will be spawned once the game begins
* The trophy value determines how big the snake grows once it eats a trophy
* Trophies will respawn in a new area when the user eats the trophy or it has been 1-9 seconds since the trophy spawned
* Player wins if the snake grows to half the perimeter of the border
* Player loses if the snake crashes into itself or the wall at any point
*
* Programmers: Tyler Brown, Gjin Rexhaj
* Date: May 8th, 2025
*/

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <string.h>

// Function prototypes
int get_terminal_width();
int get_terminal_height();
void game_over();
void drop_trophy();
void randomize_direction();
void win();

#define SNAKE_CHAR "#"
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
    int value;
    int exp_time;
} Trophy;

SnakeSegment snake[100]; // initialize an array of size 100, each array position holds an x and y coordinate
int snake_length = 1; // start with a snake size of size 3
int starting_length = 5;
int snake_speed = 100000; // initialize snake speed
bool endless = false;

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

time_t trophy_time; // create timer var
Trophy trophy; // Create trophy at global scope

// Collaborative effort to make the main function
int main() {
    srand(time(NULL)); // seed the rng
    initscr(); // initialize the curses screen
    clear(); // clears screen
    noecho(); // Will capture keystrokes, nothing shown on the screen (unless a key corresponds to an addstr string)
    curs_set(0); // hides the cursor

    box(stdscr, 0, 0); // draw border

    keypad(stdscr, TRUE); // captures the entire screen
    nodelay(stdscr, TRUE); // makes getch() not interrupt the console and wait for an input
    randomize_direction(); // randomize the snake direction on spawn
    initialize_snake(snake_length); // initialize the snake length
    drop_trophy();
    refresh();

    // Infinite loop to log arrow keystrokes and move the snake
    while(1) {

        // grow the snake with initial size
        if (starting_length > 1) {
            snake_length++;
            starting_length--;
        }

        // trophy collision logic
        if (snake[0].x == trophy.x && snake[0].y == trophy.y) {
            for (int i = 0; i < trophy.value; i++) {
                snake_length++;
            }

            snake_speed -= trophy.value * 930; // snake speed grows proportionally w/ size per trophy eaten
            drop_trophy();
        }

        // Check if snake is out of bounds, using ioctl
        if (snake[0].x < 1 || snake[0].x > get_terminal_width() - 2 || snake[0].y < 1 || snake[0].y > get_terminal_height() - 2) {
            game_over(); // function to print game over to the screen when out of bounds occurs
            exit(0);
        }

        // drop new trophy if expired
        if (difftime(time(NULL), trophy_time) > trophy.exp_time) {
            move(trophy.y, trophy.x);
            addstr(" ");
            drop_trophy();
        }

        int ch = getch();
        switch(ch) {
            // change an x or y value by 1 in a specific direction to move the snake
            // FOR WASD CONTROLS: KEY_UP = 119, KEY_DOWN = 115, KEY_LEFT = 97, KEY_RIGHT = 100
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
        usleep(snake_speed); // sleep the console for a little bit

        // check for win condition
        if (!endless) {
            int perimeter = (LINES + COLS) * 2; // calculates perimeter 
            if (snake_length >= perimeter / 2) { // if the snake is greater than/equal to half the perimeter, player wins
                win();
            }
        }
    }
    endwin();
}

// Programmer: Gjin Rexhaj, trophy spawn fix by Tyler Brown
void drop_trophy() {
    srand(time(NULL));
    // randomize x and y coord
    bool not_snake = false;
    int min = 2;
    int c_max = COLS-2;
    int l_max = LINES-2;
    while (not_snake == false) { // checks to see if a trophy is being spawned on a snake
        trophy.x = (rand() % (c_max - min)) + min;
        trophy.y = (rand() % (l_max - min)) + min;
        not_snake = true; // set this to true first
        for (int i = 0; i < snake_length + 2; i++) {
            if (trophy.x == snake[i].x && trophy.y == snake[i].y) { // if the trophy spawns on a snake, generate new coords
                not_snake = false;
                break;
            } 
        }
    }
    // randomize value from 1 to 9
    trophy.value = rand() % (9-1 + 1) + 1; 
    // randomize exp_time in seconds from 1 to 9
    trophy.exp_time = rand() % (9-1 + 1) + 1;

    // print to screen
    char trophy_str[2];

    trophy_time = time(NULL);

    move(trophy.y, trophy.x);

    sprintf(trophy_str, "%d", trophy.value);
    addstr(trophy_str);
}


// Programmer: Gjin Rexhaj (both terminal width and height)
int get_terminal_width() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

int get_terminal_height() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_row;
}

// Programmer: Tyler Brown
void game_over() { // cleaner game over screen -- appears when snake runs into itself or hits a wall
    clear(); // clears screen
    refresh(); // refresh the screen
    // get back to the middle of the terminal
    int x = COLS / 2;
    int y = LINES / 2;
    move(y, x); // move cursor to the middle of the terminal
    addstr("** Game Over **"); // print game over
    refresh(); // refresh the screen to update it with the changes
    usleep(1000000); // sleep console, close program
    endwin();
    exit(0);
}

// Programmer: Gjin Rexhaj
void win() {
    clear(); // clears screen
    refresh(); // refresh the screen
    // get back to the middle of the terminal
    int x = COLS / 2;
    int y = LINES / 2;
    move(y, x); // move cursor to the middle of the terminal
    addstr("** You Win **"); // print win
    move(y+3, x);
    addstr("Congrats!"); // closes program after this print
    refresh();
    sleep(1);
    endwin();
    exit(0);
}

// Programmer: Tyler Brown
// randomizes direction when the game begins
void randomize_direction() {
    int direction = rand() % 4; // random value 0-3 decides initial direction
    switch (direction) {
    case 0:  // move up
        move_x = 0;
        move_y = -1;
        break;
    case 1: // down
        move_x = 0;
        move_y = 1;
        break;
    case 2: // left
        move_x = -1;
        move_y = 0;
        break;
    case 3: // right
        move_x = 1;
        move_y = 0;
        break;
    default:
        break;
    }
}
