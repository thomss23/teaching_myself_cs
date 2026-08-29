#include <stdio.h>
#include <sys/ioctl.h>
#include <signal.h>



void redraw_handler(int signum) {
    struct winsize size;
    ioctl(0, TIOCGWINSZ, &size);

    int term_rows = size.ws_row;
    int term_cols = size.ws_col;

    printRectangle(term_cols, term_rows);

}

void printRectangle(int screenWidth, int screenHeight) {

    int rect_width = screenWidth / 2;
    int rect_height = screenHeight / 2;

 
    int paddingTop = (screenHeight / 2) - (rect_height / 2); 
    int paddingLeft = (screenWidth / 2) - (rect_width / 2);

    printf("\033[2J\033[H");

    for(int i = 0; i < paddingTop; i++) {
        printf("\n");
    }

    for(int i = 0; i < rect_height; i++) {

        for(int j = 0; j < paddingLeft; j++) {
            printf(" ");
        }
        
        for (int j = 0; j < rect_width; j++) {
            printf("*");
        }
        
        printf("\n");
    }

}


void main() {
    struct winsize size;

    ioctl(0, TIOCGWINSZ, &size);

    printf("\e[1;1H\e[2J");

    int term_rows = size.ws_row;
    int term_cols = size.ws_col;


    printRectangle(term_cols, term_rows);
    signal(SIGWINCH, redraw_handler);

    while (1) {
    
    }

}