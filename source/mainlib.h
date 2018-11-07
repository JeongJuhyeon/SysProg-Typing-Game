//
// Created by jjh-L on 11/6/2018.
//

#ifndef SYSPROG_TYPING_GAME_MAINLIB_H
#define SYSPROG_TYPING_GAME_MAINLIB_H

#endif //SYSPROG_TYPING_GAME_MAINLIB_H

#define MAX_WORD_LENGTH 30

typedef struct falling_word {
    char word[MAX_WORD_LENGTH];
    struct falling_word *next;
    struct falling_word *prev;
    int x;
    int y;
} falling_word;


// linked list functions
falling_word *find_falling_word(char *word_to_search);
int delete_falling_word(falling_word *word_to_delete);
int add_falling_word(falling_word *word_to_add);
falling_word *create_falling_word(char word[], int x, int y);

// gameplay internal functions
void drop_words_position(void);
int check_words_bottom(void);
void level_finished(int user_won);

// alarm, timer, update functions
void set_50ms_timer(void);
void handle_signal_50ms(int);


// test functions
static void test_create_word();
static void test_add_word();
static void test_delete_word();
static void test_find_word();

int main_loop_temp(void);
