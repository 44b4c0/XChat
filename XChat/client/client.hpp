#pragma once

#include <ncurses.h>
#include <signal.h>
#include <openssl/ssl.h>

#include <string.h>

/* Communication config */
#define MESSAGE_SIZE 1024
#define USERNAME_SIZE 32

char username_buffer[USERNAME_SIZE];
WINDOW* chat_window;
WINDOW* message_window;

int CreateScreen(){
    initscr();
    echo();
    keypad(stdscr, TRUE);

    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);

    int chat_window_height = screen_height - 1;

    chat_window = newwin(chat_window_height, screen_width, 0, 0);
    message_window = newwin(1, screen_width, screen_height - 2, 0);

    scrollok(chat_window, TRUE);
    wbkgd(message_window, COLOR_PAIR(1));

    return 0;
}

int DeleteScreen(){
    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);

    int chat_window_height = screen_height - 2;

    delwin(chat_window);
    delwin(message_window);

    endwin();

    return 0;
}

void HandleGlobalResize(int resize_signal){
    endwin();

    // clear();
    refresh();

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);

    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);

    int chat_window_height = screen_height - 1;
    chat_window = newwin(chat_window_height, screen_width, 0, 0);
    message_window = newwin(1, screen_width, screen_height - 2, 0);

    scrollok(chat_window, TRUE);
    wbkgd(message_window, COLOR_PAIR(1));
    refresh();
}

int ReadFromServer(WINDOW* chat_window, int client_socket){
    while(true == true){

    }

    return 0;
}
