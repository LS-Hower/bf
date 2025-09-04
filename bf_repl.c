/*
 * Copyright (C) 2024, LS Hower.
 * This project is licensed under the BSD 3-Clause License.
 * Project homepage: https://github.com/LS-Hower/bf
 */


/* 2023-01-04 bfpret.c */
/* 2024-12-18 bf_test.c */
/* 2025-09-04 bf_repl.c */


#if (defined _MSC_VER && !(defined _CRT_SECURE_NO_WARNINGS))
#define _CRT_SECURE_NO_WARNINGS
#endif


#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "bf.h"


#ifndef BF_TEST_INTERPRETER_COMMAND_SIZE
#define BF_TEST_INTERPRETER_COMMAND_SIZE  1000
#endif

#ifndef BF_TEST_INTERPRETER_BUFFER_SIZE
#define BF_TEST_INTERPRETER_BUFFER_SIZE  60
#endif

#ifndef BF_TEST_MEMORY_BUFFER_SIZE
#define BF_TEST_MEMORY_BUFFER_SIZE  300
#endif

#ifndef BF_TEST_MEMORY_INPUT
#define BF_TEST_MEMORY_INPUT  "helloworld\n"
#endif

#ifndef BF_TEST_MEMORY_INPUT_SIZE
#define BF_TEST_MEMORY_INPUT_SIZE  (sizeof BF_TEST_MEMORY_INPUT)
#endif


#if (CHAR_BIT != 8)
#error function bf_format_16() only works when a byte has only 8 bits.
#endif


int main(void);
static void bf_test_interpreter(void);
static void bf_test_memory(void);



/* Main function. Call bf_test_memory() and/or bf_test_interpreter() here. */
int
main(void)
{
    bf_test_memory();
    bf_test_interpreter();

    return 0;
}


/*
 * Run a BF program that converts lowercase letters to uppercase ones.
 * It should print "HELLOWORLD".
 */
static void
bf_test_memory(void)
{
    bf_s         game;
    bf_byte_t    buf[BF_TEST_MEMORY_BUFFER_SIZE];


    /* Clear buffer content. */
    memset(buf, 0x00, BF_TEST_MEMORY_BUFFER_SIZE);

    bf_init(&game, buf, buf + BF_TEST_MEMORY_BUFFER_SIZE);

    /* Convert lowercase letters to uppercase ones. */
    bf_run_memory(&game, ",----------[----------------------.,----------]",
                  (bf_byte_t *) BF_TEST_MEMORY_INPUT,
                  BF_TEST_MEMORY_INPUT_SIZE,
                  (bf_handler_t) &fputc, (void *) stdout);

    /* Print an LF. */
    bf_run_memory(&game, "[-]++++++++++.", (bf_byte_t *) BF_TEST_MEMORY_INPUT,
                  BF_TEST_MEMORY_INPUT_SIZE, (bf_handler_t) &fputc,
                  (void *) stdout);
}


/* Run an interactive BF interpreter. */
static void
bf_test_interpreter(void)
{
    bf_s  game;
    char  cmd[BF_TEST_INTERPRETER_COMMAND_SIZE];

    /* Only use u.s.buf. */
    union {
        struct {
            bf_byte_t dummy0;
            bf_byte_t dummy1;
            bf_byte_t dummy2;
            bf_byte_t buf[BF_TEST_INTERPRETER_BUFFER_SIZE];
            /*
             * So that &buf[0] mod 8 will be 3.
             * This boundary case tests bf_format_16().
             */
        } s;
        long long_dummy;      /* Force `s` to align. */
        double double_dummy;  /* Force `s` to align. */
    } u;

    /* Clear buffer content. */
    memset(u.s.buf, 0x00, BF_TEST_INTERPRETER_BUFFER_SIZE);

    bf_init(&game, u.s.buf, u.s.buf + BF_TEST_INTERPRETER_BUFFER_SIZE);

    printf("Size of buffer for command input: %d\n",
                                             BF_TEST_INTERPRETER_COMMAND_SIZE);
    printf("Enter BF commands. Simulate an EOF or enter \"exit\" to exit.\n");

    for ( ;; ) {
        int             err;

        putchar('\n');
        bf_show_nearby_memory(&game, 7, 7);

        /* Prompt. */
        printf("(BF) ");
        fgets(cmd, BF_TEST_INTERPRETER_COMMAND_SIZE, stdin);

        if (strcmp(cmd, "exit\n") == 0) {
            break;
        }

        if (feof(stdin)) {
            /* EOF to terminate. */
            break;
        }

        err = bf_run_file(&game, cmd, stdin, (bf_handler_t) &fputc,
                                                              (void *) stdout);

        switch (err) {
        case 0:
            /* Normal. */
            break;

        case -1:
            perror("failed to open file");
            break;

        case -2:
            fprintf(stderr, "Error: bracket nested incorrectly\n");
            break;

        default:
            fprintf(stderr, "Error: unrecognized error code: %d\n", err);
            break;
        }
    }

    return;
}
