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

int level = 6;
int score = 6;

void save_your_level();

void main()
{
    save_your_level();
}

void save_your_level()
{
    int open_fd;
    char * pathname = "../saves/your_save";
    char * buf;
    char * number;

    open_fd = creat(pathname, 0644);
    buf = (char*)calloc(200, sizeof(char));
    number = (char*)calloc(20, sizeof(char));

    strcat(buf, "LEVEL: ");
    sprintf(number, "%d", level);
    strcat(buf, number);
    strcat(buf, "\nSCORE: ");
    sprintf(number, "%d", score);
    strcat(buf, number);
    strcat(buf, "\n");

    printf("%s", buf);

    write(open_fd, buf, 200);

    close(open_fd);

    return;
}