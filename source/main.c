#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>                         // for alarms
#include <sys/time.h>                       // for time structs
#include <termios.h>                        // for TTY mode settings
#include <curses.h>                         // Curses
#include <stdbool.h>

// #include "minunit_3line.h"               // for unit testing
#include "minunit.h"                        // for slightly more convenient unit testing
#include "mainlib.h"                        // function definitions, struct definition


//---------Global Variables--------
falling_word *head = NULL;                 // head of falling_word linked list
/* used to communicate between function called by signal
 * and the rest of the program. REASON: functions called
 * by signals cant return values or take parameters.
 * example: signal(SIGLARM, handle_signal_50ms) */
int lives_lost = 0;                                     // can't update this when called by signal handler
bool level_clear_flag = false;
int level = 1;                                          // can't send this as parameter to signal handler
int score = 0;
char **word_list_global_ptr = NULL;                     // can't send this as parameter to signal handler


// Tests the functions related to manipulating the linked list
MU_TEST_SUITE(linked_list_tests) {
    MU_RUN_TEST(test_create_word);
    MU_RUN_TEST(test_add_word);
    MU_RUN_TEST(test_delete_word);
    MU_RUN_TEST(test_find_word);
    MU_RUN_TEST(test_empty_list);
}

MU_TEST_SUITE(graphics_tests) {
    MU_RUN_TEST(test_draw_falling_words);
}


int main() {
    //MU_RUN_SUITE(graphics_tests);
    MU_RUN_SUITE(linked_list_tests); // Run the linked_list_tests test suite
    if (DEBUG) printf("Head: %p\n", head);
    MU_REPORT(); // Report the results
    if (DEBUG) printf("Head: %p\n", head);

    initscr();

    char c;
    int running = 1;
    int level_clear = 0;

    draw_splash_screen();
    getch();


    while (running) {
        if (!level_clear) {
            setup_main_menu();
            switch (c = getch()) {
                case '1':
                    setup_gameplay_stage();
                    refresh();
                    level_clear = gameplay_loop();
                    break;
                case '2':
                    //load_game();
                case '3':
                    running = 0;
                    prepare_game_exit();
                    break;
            }
        }
        else if (level_clear){
            level_clear = 0;
            //TODO
            /*setup_level_clear_menu();
              switch (c = getch()) {
                case '1':
                    setup_gameplay_stage();
                    refresh();
                    level_clear = gameplay_loop();
                    break;
                case '2':
                    //save_game();
                case '3':
                    running = 0;
                    prepare_game_exit();
                    break;
            }*/
        }
    }


    return 0;
}

// Temporary version of the main loop
int gameplay_loop() {
    char *word_list[WORD_LIST_SIZE];
    load_words("../resources/words_5000", word_list, WORD_LIST_SIZE);
    int remaining_lives = LIVES_AT_START;

    char input_word[MAX_WORD_LENGTH];
    char input_letter;

    level_clear_flag = false;

    while (1) {
        pause();      // wait for a signal (SIGALRM)
        if (level_clear_flag) {
            level_finished(1);
            level_clear_flag = false;
            return 1;
        }

        fflush(stdin);
        // every 50 ms, check whether a letter was entered
        // Because we use non-blocking mode, it does not block until the user enters something.
        while ((input_letter = getch()) != ERR &&
               strchr("\n\033abcdefghijklmnopqrstuvwxyz", input_letter) == NULL);

        // handle input letter
        if (input_letter == ESC) {
            level_finished(0);
            return 0;
        } else if (input_letter != ERR) {
            handle_input_letter(input_word, input_letter);
        }

        // if the user pressed enter, process the word
        if (input_letter == ENTER) {
            if (DEBUG) printf("Detected ENTER\n");

            score += handle_input_word(input_word);
            refresh_score_clear_input_box(score);

            for (int i = 0; i < MAX_WORD_LENGTH; i++)
                input_word[i] = '\0';
        }

        if (lives_lost > 0) {
            remaining_lives -= lives_lost;
            refresh_lives(remaining_lives);
            lives_lost = 0;
            //printf("\n---------\nLives left: %d, Score: %d\n------", remaining_lives, score);
            if (remaining_lives <= 0) {
                level_finished(0);
                break;
            }
        }
    }

    nodelay(stdscr, 1);
}



//-------------Stage setup/cleanup functions-----------

void setup_gameplay_stage() {
    cbreak();       // Disables line buffering
    nodelay(stdscr, 1); // cause getch() to be non-blocking
    noecho();          // cause getch() to be no-echo

    // clear virtual screen
    clear();

    // set_cr_noecho_mode();
    // set_nodelay_mode();

    set_50ms_timer(); // Every 50 ms, SIGALRM will be received
    signal(SIGALRM, handle_signal_50ms);

    srand(time(NULL));

    draw_game_hud();
}

void setup_main_menu() {
    char c = 0;

    clear();

    cbreak();       // Disables line buffering
    nodelay(stdscr, 0); // cause getch() to be blocking (wait for input)
    noecho();       // cause getch() to be no-echo
    curs_set(0);    // disable cursor

    //draw ui box
    for (int i = 0; i <= COLUMNS; i++) {
        move(1, i); //upper cover
        addch('=');
        move(ROWS - 1, i); //lower cover
        addch('=');
    }
    for (int i = 2; i <= ROWS - 2; i++) {
        move(i, 0); //left cover
        addch('|');
        move(i, COLUMNS); //right cover
        addch('|');

    }


    //draw MENU
    move(6, COLUMNS / 2 - 9);
    printw("        __ ");
    move(7, COLUMNS / 2 - 9);
    printw("|\\  /| |__ |\\ | |  |");
    move(8, COLUMNS / 2 - 9);
    printw("| \\/ | |__ | \\| |__|");



    //draw input Section
    for (int i = 0; i < COLUMNS - 5; i++) {
        move(ROWS - 4, i + 3); //lower cover
        addch('_');
        move(3, i + 3); //upper cover
        addch('_');
    }

    for (int i = 0; i < ROWS - 7; i++) {

        move(i + 4, 3); //left cover
        addch('|');
        move(i + 4, COLUMNS - 3); //right cover
        addch('|');
    }

    //draw Selects

    move(14, COLUMNS / 2 - 5);
    printw("1. NEW GAME");

    move(17, COLUMNS / 2 - 5);
    printw("2. CONTINUE");

    move(20, COLUMNS / 2 - 5);
    printw("3. EXIT");


    return;
}


void level_finished(int user_won) {
    if (user_won)
        level += 1;
    else
        level = 1;

    erase_all_falling_words();
    empty_linked_list(); // empty LL

    // stop 50ms timer
    struct itimerval itimer_stop;

    itimer_stop.it_interval.tv_sec = 0; // 0 s
    itimer_stop.it_interval.tv_usec = 0; // 0 ms
    itimer_stop.it_value.tv_sec = 0; // Start 0 secs after setting
    itimer_stop.it_value.tv_usec = 0;

    setitimer(ITIMER_REAL, &itimer_stop, NULL);
}

void prepare_game_exit() {
    echo(); // cause getch to echo
    endwin();
}

//-------------Alarm functions--------------

// handle the signal that comes every 50ms
void handle_signal_50ms(int signum) {
    static int updates_done = 0;
    updates_done++;

    // drop the words
    if (updates_done % (int) UPDATES_PER_SECOND * DROP_TIME == 0) {
        lives_lost = check_words_bottom();
        erase_all_falling_words();
        drop_words_position();
        draw_all_falling_words();
    }

    // every second refresh time displayed
    if (updates_done % (int) UPDATES_PER_SECOND == 0) {
        refresh_time(LEVEL_TIME - updates_done / UPDATES_PER_SECOND);
    }

    // spawn a new word
    if (updates_done % (int) UPDATES_PER_SECOND * SPAWN_TIME == 0) {
        spawn_word(word_list_global_ptr);
    }

    if (updates_done >= (int) UPDATES_PER_SECOND * LEVEL_TIME) {
        level_clear_flag = true;
		updates_done = 0;
    }

    return;
}

// set a timer that goes off every 50 ms
void set_50ms_timer() {
    struct itimerval itimer_50ms;
    itimer_50ms.it_interval.tv_sec = 0; // 0 s
    itimer_50ms.it_interval.tv_usec = 50000; // 50 ms
    itimer_50ms.it_value.tv_sec = 1; // Start 1 secs after setting
    itimer_50ms.it_value.tv_usec = 0;

    setitimer(ITIMER_REAL, &itimer_50ms, NULL);
}

//open word list file and load to array
int load_words(char *file_name, char **word_list, int list_size) {
    FILE *word_file_fd = fopen(file_name, "r");

    if (!word_file_fd) {
        perror("no word list");
        exit(1);
    }

    //memory allocating + load words to array
    for (int i = 0; i < list_size; i++) {
        word_list[i] = (char *) calloc(MAX_WORD_LENGTH, sizeof(char));
        fscanf(word_file_fd, "%s", word_list[i]);
    }

    word_list_global_ptr = word_list;
}



// -------------Word handling functions--------

// Spawns a new word
void spawn_word(char *word_list[]) {
    char new_word[MAX_WORD_LENGTH];
    // Pick a random word from the list and copy it into new_word
    strncpy(new_word, word_list[rand() % WORD_LIST_SIZE], MAX_WORD_LENGTH);
    // Pick a random x coordinate
    int x = rand() % (COLUMNS - strlen(new_word) - 1);
    // Create and add the word
    falling_word *new_falling_word = create_falling_word(new_word, x, 0);
    add_falling_word(new_falling_word);
    draw_new_falling_word(new_falling_word);
    // if (DEBUG) printf("New word: %s\n", new_word);
}

void drop_words_position() {
    falling_word *temp = head;

    while (temp) {
        //if (DEBUG) printf("Temp: %p, Word: %s, Y: %d, Next: %p\n",temp, temp->word, temp->y, temp->next);
        temp->y += 1;
        temp = temp->next;
    }
}

int check_words_bottom() {
    falling_word *word = head;
    falling_word *next_word;
    int words_at_bottom = 0;

    while (word) {
        next_word = word->next; // do this first because we might delete the current word
        if (word->y == FIELD_BOTTOM) {
            words_at_bottom++;
            delete_falling_word(word);
        }
        word = next_word; //go to the next word
    }

    return words_at_bottom;
}

// -------------User Input Functions ------------
void handle_input_letter(char *input_word, char input_letter) {
    static int index = 0;

    //if (DEBUG && input_letter != EOF) printf("Char entered: %c, input_letter: %d\n", input_letter, input_letter);

    switch (input_letter) {
        case ENTER:
            // complete input_word by adding 'NULL' to array   +   index reset
            input_word[index++] = '\0';
            index = 0;
            // finding and deleting word -> called from gameplay_loop because of score
            break;

        default:
            //	with each one letter, add to 'input_word'
            if (index >= MAX_WORD_LENGTH - 2) { // IF comes to MAX LENGTH, no more input letter will be added
            } else if (index < MAX_WORD_LENGTH - 1) { // '\0' can be in the last address
                input_word[index++] = input_letter;
                input_word[index] = '\0';
                // INPUT WORD REFRESH
                move(ROWS - 6 + 1, (COLUMNS - MAX_WORD_LENGTH) / 2);
                printw("                              "); // 30 space reset
                move(ROWS - 6 + 1, (COLUMNS - MAX_WORD_LENGTH) / 2);
                addstr(input_word);
                refresh();
                //1118
            }
            break;
    }
}

int handle_input_word(char *input_word) {
    if (DEBUG) printf("Input word: %s\n", input_word);

    int returnScore = 1; // score of a word -> ? just 1 ?
    falling_word *searched_pointer = NULL;
    searched_pointer = find_falling_word(input_word);    // search the word

    if (DEBUG) printf("Word %sfound\n", searched_pointer == NULL ? "not" : " ");

    // if the word is found, delete it and return points
    if (searched_pointer) {
        delete_falling_word(searched_pointer);
        return returnScore;
    }
        // if the word is not found, return 0
    else if (searched_pointer == NULL) {
        return 0;
    }
}


//---------- Setting TTY mode Functions ---------
void set_cr_noecho_mode() {
    struct termios ttystate;

    tcgetattr(0, &ttystate);
    ttystate.c_lflag &= ~ICANON;
    ttystate.c_lflag &= ~ECHO;
    ttystate.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &ttystate);
}

void set_nodelay_mode() {
    int termflags;
    termflags = fcntl(0, F_GETFL);
    termflags |= O_NDELAY;
    fcntl(0, F_SETFL, termflags);
}

void tty_mode(int how) {
    static struct termios original_mode;
    static int original_flags;
    static int stored = 0;
    if (how == 0) {
        tcgetattr(0, &original_mode);
        original_flags = fcntl(0, F_GETFL);
        stored = 1;
    } else if (stored) {
        tcsetattr(0, TCSANOW, &original_mode);
        fcntl(0, F_SETFL, original_flags);
    }
}


// -------------Linked List Functions-------------

// Delete a node from the linked list
int delete_falling_word(falling_word *word_to_delete) {
    erase_falling_word(word_to_delete);
    //if (DEBUG) printf("Deleting %s.\nPrev: %p.\nNext: %p.\n Head is %p.\n", word_to_delete->word, word_to_delete->prev, word_to_delete->next, head);


    // If a previous node exist, we need to tie that one to the next
    if (word_to_delete->prev != NULL) {
        word_to_delete->prev->next = word_to_delete->next;
    }
        // If not, that means it's the head, so the new head becomes the next
    else {
        head = word_to_delete->next;
        //if (DEBUG) printf("New head is at %p.\n", head);
    }

    // If a next node exists, we need to tie that one to the prev
    if (word_to_delete->next != NULL) {
        word_to_delete->next->prev = word_to_delete->prev;
    }

    free(word_to_delete);
    return 0;
}

// If the word is found, returns the node. If not, returns NULL.
falling_word *find_falling_word(char *word_to_search) {
    // iter_node = current node, iterates over linked list
    for (falling_word *iter_node = head; iter_node != NULL; iter_node = iter_node->next) {
        if (!strcmp(word_to_search, iter_node->word)) {
            return iter_node;
        }
    }

    return NULL;
}


// Add a node to the linked list. It puts the new node at the front of the list,
// so it becomes the new head.
int add_falling_word(falling_word *word_to_add) {
    if (head == NULL) {
        head = word_to_add;
        return 0;
    }

    head->prev = word_to_add;
    word_to_add->next = head;
    head = word_to_add;
    return 0;
}

// Create (allocate and initialize) a falling word node
falling_word *create_falling_word(char word[], int x, int y) {
    falling_word *new_falling_word = malloc(sizeof(struct falling_word));
    new_falling_word->next = new_falling_word->prev = NULL;
    strncpy(new_falling_word->word, word, MAX_WORD_LENGTH);
    new_falling_word->x = x;
    new_falling_word->y = y;
    return new_falling_word;
}

void empty_linked_list() {
    falling_word *current;
    for (falling_word *temp = head; temp != NULL; temp = current) {
        current = temp->next;
        free(temp);
    }
    head = NULL;
}

//-----------------Graphics functions------------

void draw_splash_screen() {
    clear();
    curs_set(0);    // disable cursor

    char line[80];
    int y = 5;

    FILE *fd = fopen("../resources/splash4", "r");

    if (!fd) {
        printf("FD Error.... \n");
        exit(1);
    }

    fgets(line, 80, fd);
    while (!feof(fd)) {
        move(y, 0);
        addstr(line);
        fgets(line, 80, fd);
        y++;
    }

    y += 3;
    move(y, 19);
    addstr("Press any key to continue");

    refresh();
}

void draw_game_hud() {
    //draw ui box
    for (int i = 0; i <= COLUMNS; i++) {
        move(ROWS - 9, i); //upper cover
        addch('=');
        move(ROWS - 1, i); //lower cover
        addch('=');
    }
    for (int i = ROWS - 8; i <= ROWS - 2; i++) {
        move(i, 0); //left cover
        addch('|');
        move(i, COLUMNS); //right cover
        addch('|');

    }

    //draw LIVES and SCORE
    move(ROWS - 2, COLUMNS / 2 - COLUMNS / 5 * 2); //lives
    printw("LIVES: %d", LIVES_AT_START);
    move(ROWS - 2, COLUMNS / 2 + COLUMNS / 5 * 2 - 8); //score
    printw("SCORE: %d", score);

    // draw time

    move(ROWS - 2, COLUMNS / 2 - 3); //lives
    printw("TIME: %d", LEVEL_TIME);

    //draw input Section
    for (int i = 0; i < MAX_WORD_LENGTH; i++) {
        move(ROWS - 4, (COLUMNS - MAX_WORD_LENGTH) / 2 + i); //lower cover
        addch('_');
        move(ROWS - 7, (COLUMNS - MAX_WORD_LENGTH) / 2 + i); //upper cover
        addch('_');
    }

    for (int i = 0; i < 3; i++) {
        move(ROWS - 6 + i, (COLUMNS - MAX_WORD_LENGTH) / 2 - 1); //left cover
        addch('|');
        move(ROWS - 6 + i, (COLUMNS - MAX_WORD_LENGTH) / 2 + 30); //right cover
        addch('|');
    }

}

void refresh_score_clear_input_box(int score) {
    // Clear input box
    move(ROWS - 6 + 1, (COLUMNS - MAX_WORD_LENGTH) / 2);
    printw("                              \0"); // 30 space reset

    // SCORE REFRESH
    move(ROWS - 2, COLUMNS / 2 + COLUMNS / 5 * 2 - 8); //score
    printw("            ");// 12 space reset
    move(ROWS - 2, COLUMNS / 2 + COLUMNS / 5 * 2 - 8); //score
    printw("SCORE: %d", score);
    refresh();
}

void refresh_lives(int remaining_lives) {
    //LIVES REFRESH
    move(ROWS - 2, COLUMNS / 2 - COLUMNS / 5 * 2);
    printw("            ");// 12 space reset
    move(ROWS - 2, COLUMNS / 2 - COLUMNS / 5 * 2);
    printw("LIVES: %d", remaining_lives);
    refresh();
}

void refresh_time(int seconds) {
    move(ROWS - 2, COLUMNS / 2 + 3); //lives
    printw("%2d", seconds);
    refresh();
}

void erase_all_falling_words() {
    char resized_eraser[MAX_WORD_LENGTH];
    for (falling_word *cur = head; cur != NULL; cur = cur->next) {
        move(cur->y, cur->x);
        strncpy(resized_eraser, ERASER, strlen(cur->word));
        resized_eraser[strlen(cur->word)] = '\0';
        addstr(resized_eraser);
    }

    refresh();
}

void erase_falling_word(falling_word *word_to_erase) {
    char resized_eraser[MAX_WORD_LENGTH];
    move(word_to_erase->y, word_to_erase->x);
    strncpy(resized_eraser, ERASER, strlen(word_to_erase->word));
    resized_eraser[strlen(word_to_erase->word)] = '\0';
    addstr(resized_eraser);

    refresh();
}

void draw_all_falling_words() {
    for (falling_word *cur = head; cur != NULL; cur = cur->next) {
        move(cur->y, cur->x);
        addstr(cur->word);
    }

    refresh();
}

void draw_new_falling_word(falling_word *new_word) {
    move(new_word->y, new_word->x);
    addstr(new_word->word);

    refresh();
}


//-------------------Test Functions----------------

// Unit tests for delete_falling_word()
MU_TEST(test_create_word) {
    falling_word *new_word = create_falling_word("stock", 2, 5);
    mu_check(strcmp("stock", new_word->word) == 0);
    mu_check(new_word->x == 2);
    mu_check(new_word->y == 5);
}

// Unit tests for add_falling_word()
MU_TEST(test_add_word) {
    mu_check(head == NULL);
    falling_word *new_word = create_falling_word("stock", 2, 5);
    add_falling_word(new_word);
    mu_check(head == new_word);
    falling_word *new_word2 = create_falling_word("ab", 3, 1);
    add_falling_word(new_word2);
    mu_check(head == new_word2);
    mu_check(head->next == new_word);
    mu_check(head->prev == NULL);
    mu_check(head->next->next == NULL);
    mu_check(head->next->prev == head);
}

// Unit tests for test_delete_word()
MU_TEST(test_delete_word) {
    // Empty the LL
    while (head != NULL)
        delete_falling_word(head);
    mu_check(head == NULL);

    // LL is now empty. Add word
    falling_word *new_word = create_falling_word("stock", 2, 5);
    add_falling_word(new_word);
    mu_check(head == new_word);
    // Delete word
    delete_falling_word(new_word);
    mu_check(head == NULL);
    // Add word
    falling_word *new_word2 = create_falling_word("ab", 3, 1);
    add_falling_word(new_word2);
    mu_check(head == new_word2);
    mu_check(head->next == NULL);
    mu_check(head->prev == NULL);
    // Add word
    falling_word *new_word3 = create_falling_word("cc", 4, 1);
    add_falling_word(new_word3);
    // Now it should be "new_word3 -> new_word2"
    mu_check(head->next == new_word2);
    delete_falling_word(new_word3);
    mu_check(head->next == NULL);
    delete_falling_word(new_word2);
}

// Unit tests for test_delete_word()
MU_TEST(test_find_word) {
    falling_word *new_word = create_falling_word("stock", 2, 5);
    add_falling_word(new_word);
    mu_check(head == new_word);
    mu_check(find_falling_word("stack") == NULL);
    mu_check(find_falling_word("stock") == head);
    delete_falling_word(new_word);
    mu_check(find_falling_word("stock") == NULL);
    falling_word *new_word2 = create_falling_word("ab", 3, 1);
    add_falling_word(new_word2);
    mu_check(find_falling_word("ab") == head);
    falling_word *new_word3 = create_falling_word("cc", 4, 1);
    add_falling_word(new_word3);
    // "cc" -> "ab"
    mu_check(find_falling_word("ab") == head->next);
    mu_check(find_falling_word("cc") == head);
    mu_check(find_falling_word("abc") == NULL);
}

MU_TEST(test_empty_list) {
    empty_linked_list();
    mu_check(head == NULL);
}

MU_TEST(test_draw_falling_words) {
    // to spawn words
    char *word_list[WORD_LIST_SIZE];
    load_words("../resources/words_5000", word_list, WORD_LIST_SIZE);

    // add some words
    spawn_word(word_list_global_ptr);
    falling_word *new_word = create_falling_word("stock", 2, 2);
    add_falling_word(new_word);
    falling_word *new_word3 = create_falling_word("cc", 4, 1);
    add_falling_word(new_word3);

    // setup stage
    initscr();
    clear();

    printf("After initscr()");

    draw_all_falling_words();
    sleep(2);
    erase_all_falling_words();
    drop_words_position();
    draw_all_falling_words();

    sleep(2);

    // clear the LL
    while (head != NULL)
        delete_falling_word(head);

    // exit curses screen
    clear();
    endwin();
}

