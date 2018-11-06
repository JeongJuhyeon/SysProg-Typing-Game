//
// Created by jjh-L on 11/6/2018.
//

#ifndef SYSPROG_TYPING_GAME_MINUNIT_H
#define SYSPROG_TYPING_GAME_MINUNIT_H

#endif //SYSPROG_TYPING_GAME_MINUNIT_H

/* file: minunit.h */
#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; \
                                if (message) return message; } while (0)
extern int tests_run;
