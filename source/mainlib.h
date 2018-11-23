//
// Created by jjh-L on 11/6/2018.
//

#ifndef SYSPROG_TYPING_GAME_MAINLIB_H
#define SYSPROG_TYPING_GAME_MAINLIB_H

#endif //SYSPROG_TYPING_GAME_MAINLIB_H

//--------#define numbers/settings----------

#define MAX_WORD_LENGTH 30
#define LIVES_AT_START    6
#define DEBUG            0
#define WORD_LIST_SIZE 5000
#define ROWS 25
#define FIELD_BOTTOM (ROWS - 10)
#define COLUMNS 50
#define UPDATES_PER_SECOND 20
#define SPAWN_TIME 2
#define DROP_TIME 1
#define ERASER "                                "
#define LEVEL_TIME 60

// for user input letter check
typedef enum { DEFAULT = -1, ENTER = 10, ESC = 27 } USR_INPUT_ASCII;

typedef struct falling_word {
    char word[MAX_WORD_LENGTH];
    struct falling_word *next;
    struct falling_word *prev;
    int x;
    int y;
} falling_word;

//---------function declarations------------

// linked list functions
falling_word *find_falling_word(char *word_to_search);
int delete_falling_word(falling_word *word_to_delete);
int add_falling_word(falling_word *word_to_add);
falling_word *create_falling_word(char word[], int x, int y);
void empty_linked_list();

// gameplay internal functions
void drop_words_position(void);
int check_words_bottom(void);
void level_finished(int user_won);
void spawn_word(char *word_list[]);
int gameplay_loop(void);

// stage change functions
void setup_gameplay_stage(void);
void setup_main_menu();
void draw_game_hud();
void prepare_game_exit();

// alarm, timer, update functions
void set_50ms_timer(void);
void handle_signal_50ms(int);

// User input functions
void handle_input_letter(char *, char);
int handle_input_word(char *);
void handle_esc();

// TTY mode settings
void set_cr_noecho_mode();
void set_nodelay_mode();
void tty_mode(int how);


// test functions
static void test_create_word();
static void test_add_word();
static void test_delete_word();
static void test_find_word();
static void test_empty_list();
static void test_draw_falling_words();

// graphics functions

void erase_all_falling_words();
void erase_falling_word(falling_word *word_to_erase);
void draw_all_falling_words();
void draw_new_falling_word(falling_word *new_word);
void draw_splash_screen();
void refresh_score_clear_input_box();
void refresh_lives();
void refresh_time(int seconds);

// file related functions
int load_words(char * file_name, char ** word_list, int list_size);
void load_your_level();