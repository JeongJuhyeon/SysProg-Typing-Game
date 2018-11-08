#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>                     // for alarms
#include <sys/time.h>                     // for time structs
// #include "minunit_3line.h"                        // for unit testing
#include "minunit.h"                                 // for slightly more convenient unit testing
#include "mainlib.h"                                 // function definitions, struct definition
#include <termios.h>								// for TTY mode settings



//---------Global Variables--------
falling_word *head = NULL;                 // head of falling_word linked list
/* used to communicate between function called by signal
 * and the rest of the program. REASON: functions called
 * by signals cant return values or take parameters.
 * example: signal(SIGLARM, handle_signal_50ms) */
int lives_lost = 0;


// Tests the functions related to manipulating the linked list
MU_TEST_SUITE(linked_list_tests)
{
    MU_RUN_TEST(test_create_word);
    MU_RUN_TEST(test_add_word);
    MU_RUN_TEST(test_delete_word);
    MU_RUN_TEST(test_find_word);
}


int main()
{
    char * word_list[WORD_LIST_SIZE];

    MU_RUN_SUITE(linked_list_tests); // Run the linked_list_tests test suite
    MU_REPORT(); // Report the results

    return 0;
}

// Temporary version of the main loop
int main_loop_temp()
{
    int remaining_lives = LIVES_AT_START;
    int score = 0;

	char input_word[MAX_WORD_LENGTH];
	char letter; int index = 0;

	tty_mode(0);
	set_cr_noecho_mode();
	set_nodelay_mode();

    set_50ms_timer(); // Every 50 ms, SIGALRM will be received
    signal(SIGALRM, handle_signal_50ms);

    while (1)
    {
        pause();      // wait for a signal (SIGALRM)


		//fscanf(stdin," %c", &input_letter); 
		fflush(stdin);
		while ((input_letter = getchar()) != EOF && strchr("\n\033abcdefghijklmnopqrstuvwxyz", input_letter) == NULL);
						  // every 50 ms, check the BUFFER whether there is inputted letter

		user_word_input(input_word, &index, letter);	// act with one letter

		if ((int)letter == ENTER)		score += check_input_word(input_word); // act with one completed word
												// FIND_FALLING_WORD
												// DELETE_FALLING_WORD
												// modify  'score'
				
		// TO DO -> Print window refresh

        if (lives_lost > 0)
        {
            remaining_lives -= lives_lost;
            lives_lost = 0;
            if (remaining_lives <= 0)
            {
                level_finished(0);
            }
        }
    }

	tty_mode(1);
}


void handle_signal_50ms(int signum)
{
    static int updates_done = 0;
    // every 50 ms
    if (DEBUG) printf("50ms\n");
    updates_done++;

    // every 1 second
    if (updates_done == 20)
    {
        if (DEBUG) printf("\n1 second\n\n");
        drop_words_position();
        lives_lost = check_words_bottom();
        updates_done = 0;
    }

    return;
}


void set_50ms_timer()
{
    struct itimerval itimer_50ms;
    itimer_50ms.it_interval.tv_sec = 0; // 0 s
    itimer_50ms.it_interval.tv_usec = 50000; // 50 ms
    itimer_50ms.it_value.tv_sec = 2; // Start 2 secs after setting
    itimer_50ms.it_value.tv_usec = 0;

    setitimer(ITIMER_REAL, &itimer_50ms, NULL);
}

//open word list file and load to array
int load_words(char * file_name, char ** word_list, int list_size)
{
    FILE* word_file_fd = fopen(file_name, "r");

    if(!word_file_fd)
    {
        perror("no word list");
        exit(1);
    }

    //memory allocating + load words to array
    for(int i = 0;i<list_size;i++)
    {
        word_list[i] = (char*)calloc(MAX_WORD_LENGTH,sizeof(char));
        fscanf(word_file_fd, "%s", word_list[i]);
    }
}



// -------------TO-DO Functions---------------

void drop_words_position()
{ ; }

int check_words_bottom()
{ ; }

void level_finished(int user_won)
{ ; }

// -------------User Input Functions ------------
void user_word_input(char input_word[], int* index, char input_letter)
{
	// void user_word_input
	switch ((int)input_letter)
	{
	case		ESC:
		// EXIT ( TO - DO -> 'pause' )
		handle_esc();
		break;

	case ENTER:
		// complete input_word by adding 'NULL' to array   +   index reset
		input_word[*index++] = '\0';
		*index = 0;

		// finding and deleting word -> in main_loop_temp
		break;

	default:
		//	with each one letter, add to 'input_word'
		if (*index >= MAX_WORD_LENGTH - 2)
			; // IF comes to MAX LENGTH, no more input letter will be added
		else if (*index < MAX_WORD_LENGTH - 1) // '\0' can be in the last address 
			input_word[*index++] = input_letter;
		break;
	}
}

void handle_esc()
{
	// just EXIT ( To do : 'pause' )
	exit(0);
}

int check_input_word(char input_word[])
{
	int returnScore = 1; // score of a word -> ? just 1 ?
	falling_word* searched_pointer = NULL;
	searched_pointer = find_falling_word(input_word);	// search

	if (searched_pointer)												// act
	{
		delete_falling_word(searched_pointer);
		return returnScore;
	}
	else if (searched_pointer == NULL) 
		return 0;		
}


//---------- Setting TTY mode Functions ---------
set_cr_noecho_mode()
{
	struct termios ttystate;

	tcgetattr(0, &ttystate);
	ttystate.c_lflag &= ~ICANON;
	ttystate.c_lflag &= ~ECHO;
	ttystate.c_cc[VMIN] = 1;
	tcsetattr(0, TCSANOW, &ttystate);
}

set_nodelay_mode()
{
	int termflags;
	termflags = fcntl(0, F_GETFL);
	termflags |= O_NDELAY;
	fcntl(0, F_SETFL, termflags);
}

tty_mode(int how)
{
	static struct termios original_mode;
	static int original_flags;
	static int stored = 0;
	if (how == 0) {
		tcgetattr(0, &original_mode);
		original_flags = fcntl(0, F_GETFL);
		stored = 1;
	}
	else if (stored)
	{
		tcsetattr(0, TCSANOW, &original_mode);
		fcntl(0, F_SETFL, original_flags);
	}
}


// -------------Linked List Functions-------------

// Delete a node from the linked list
int delete_falling_word(falling_word *word_to_delete)
{
    if (word_to_delete->prev != NULL)
    {
        word_to_delete->prev->next = word_to_delete->next;
    } else
    {          // deleting the head
        head = word_to_delete->next;
    }

    if (word_to_delete->next != NULL)
    {
        word_to_delete->next->prev = word_to_delete->prev;
    }
    // nothing special has to be done if it's the tail

    free(word_to_delete);
    return 0;
}

// If the word is found, returns the node. If not, returns NULL.
falling_word *find_falling_word(char *word_to_search)
{
    falling_word *iter_node = head;

    // iter_node = current node, iterates over linked list
    for (falling_word *iter_node = head; iter_node != NULL; iter_node = iter_node->next)
    {
        if (!strcmp(word_to_search, iter_node->word))
        {
            return iter_node;
        }
    }

    return NULL;
}


// Add a node to the linked list. It puts the new node at the front of the list,
// so it becomes the new head.
int add_falling_word(falling_word *word_to_add)
{
    if (head == NULL)
    {
        head = word_to_add;
        return 0;
    }

    head->prev = word_to_add;
    word_to_add->next = head;
    head = word_to_add;
    return 0;
}

// Create (allocate and initialize) a falling word node
falling_word *create_falling_word(char word[], int x, int y)
{
    falling_word *new_falling_word = malloc(sizeof(struct falling_word));
    new_falling_word->next = new_falling_word->prev = NULL;
    strncpy(new_falling_word->word, word, MAX_WORD_LENGTH);
    new_falling_word->x = x;
    new_falling_word->y = y;
    return new_falling_word;
}

//-------------------Test Functions----------------

// Unit tests for delete_falling_word()
MU_TEST(test_create_word)
{
    falling_word *new_word = create_falling_word("stock", 2, 5);
    mu_check(strcmp("stock", new_word->word) == 0);
    mu_check(new_word->x == 2);
    mu_check(new_word->y == 5);
}

// Unit tests for add_falling_word()
MU_TEST(test_add_word)
{
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
MU_TEST(test_delete_word)
{
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
MU_TEST(test_find_word)
{
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
    delete_falling_word(new_word);
    delete_falling_word(new_word2);
    delete_falling_word(new_word3);
}


