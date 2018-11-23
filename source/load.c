#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>                         // for alarms
#include <sys/time.h>                       // for time structs
#include <termios.h>                        // for TTY mode settings
#include <string.h>

// #include "minunit_3line.h"               // for unit testing
#include "minunit.h"                        // for slightly more convenient unit testing
#include "mainlib.h"                        // function definitions, struct definition

int level = 0;
int score = 0;

void load_your_level();

void main()
{
    load_your_level();
}

void load_your_level()
{
    FILE * open_fd;
    char * pathname = "../saves/your_save";
    int number;
    char temp[200];

    open_fd = fopen(pathname, "r");

    if(!open_fd)
    {
        printf("There is no file\n");
        return;
    }


    fscanf(open_fd, "%s %d", temp, &level);

    fscanf(open_fd, "%s %d", temp, &score);

    fclose(open_fd);

    return;
}