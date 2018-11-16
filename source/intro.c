#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include "mainlib.h"

void start_menu_screen();

void main()
{
    start_menu_screen();

    return;
}

void start_menu_screen()
{
    char c = 0;

    initscr();

    clear();

    //draw ui box
    for(int i = 0;i<=COLUMNS;i++)
    {
        move(1, i); //upper cover
        addch('=');
        move(ROWS+9, i); //lower cover
        addch('=');
    }
    for(int i = 2;i<=ROWS+8;i++)
    {
        move(i, 0); //left cover
        addch('|');
        move(i, COLUMNS); //right cover
        addch('|');

    }


    //draw MENU
    move(6, COLUMNS/2 - 9);
    printw("        __ ");
    move(7, COLUMNS/2 - 9);
    printw("|\\  /| |__ |\\ | |  |");
    move(8, COLUMNS/2 - 9);
    printw("| \\/ | |__ | \\| |__|");



    //draw input Section
    for(int i = 0;i<COLUMNS-5;i++)
    {
        move(ROWS + 6, i + 3); //lower cover
        addch('_');
        move(3, i + 3); //upper cover
        addch('_');
    }

    for(int i = 0;i<ROWS + 3;i++)
    {

        move(i + 4, 3); //left cover
        addch('|');
        move(i + 4, COLUMNS - 3); //right cover
        addch('|');
    }

    //draw Selects

    move(14, COLUMNS/2 - 5);
    printw("1. PLAY");

    move(17, COLUMNS/2 - 5);
    printw("2. OPTIONS");

    move(20, COLUMNS/2 - 5);
    printw("3. EXIT");

    switch(c = getch())
    {
        case 1:
            draw_ui();
            break;

        case 2 :

        case 3:
            endwin();
            break;
    }


    return;
}

void draw_ui()
{
    //draw ui box
    for(int i = 0;i<=COLUMNS;i++)
    {
        move(ROWS+1, i); //upper cover
        addch('=');
        move(ROWS+9, i); //lower cover
        addch('=');
    }
    for(int i = ROWS+2;i<=ROWS+8;i++)
    {
        move(i, 0); //left cover
        addch('|');
        move(i, COLUMNS); //right cover
        addch('|');

    }

    //draw LIVES and SCORE
    move(ROWS+8, COLUMNS/2-COLUMNS/5*2); //lives
    printw("LIVES : 3");
    move(ROWS+8, COLUMNS/2+COLUMNS/5*2-12); //score
    printw("SCORE : 100");

    //draw input Section
    for(int i = 0;i<MAX_WORD_LENGTH;i++)
    {
        move(ROWS+6, (COLUMNS-MAX_WORD_LENGTH)/2 + i); //lower cover
        addch('_');
        move(ROWS+3, (COLUMNS-MAX_WORD_LENGTH)/2 + i); //upper cover
        addch('_');
    }

    for(int i = 0;i<3;i++)
    {
        move(ROWS+4+i, (COLUMNS-MAX_WORD_LENGTH)/2-1); //left cover
        addch('|');
        move(ROWS+4+i, (COLUMNS-MAX_WORD_LENGTH)/2 + 30); //right cover
        addch('|');
    }



}