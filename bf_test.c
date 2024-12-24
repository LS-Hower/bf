/*
 * Copyright (C) 2024, LS Hower.
 * This project is licensed under the BSD 3-Clause License.
 * Project homepage: https://github.com/LS-Hower/bf
 */


/* 2023-01-04 bfpret.c */
/* 2024-12-18 bf_test.c */


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

#define BF_FORMAT_16_RESULT_TEMPLATE \
    " -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- -- "
#define BF_FORMAT_16_RESULT_SIZE  (sizeof BF_FORMAT_16_RESULT_TEMPLATE)
#define BF_FORMAT_BYTE_BEGIN_INDICES_INITIALIZER {  1,  4,  7, 10, \
                                                   14, 17, 20, 23, \
                                                   27, 30, 33, 36, \
                                                   40, 43, 46, 49 }

#if (CHAR_BIT != 8)
#error function bf_format_16() only works when a byte has only 8 bits.
#endif


int main(void);
static void bf_test_interpreter(void);
static void bf_test_memory(void);
static const char *bf_format_16(const bf_nearby_16_s *nearby);


static char       bf_format_16_result[BF_FORMAT_16_RESULT_SIZE] = {0};
static const int  bf_format_byte_begin_indices[16] =
                                      BF_FORMAT_BYTE_BEGIN_INDICES_INITIALIZER;


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
    printf("Enter BF commands. Simulate an EOF to quit.\n");

    for ( ;; ) {
        int             err;
        bf_nearby_16_s  nearby;

        bf_nearby_16(&game, &nearby);

        printf("%p: %s\n", nearby.base_ptr, bf_format_16(&nearby));

        /* Prompt. */
        printf("(BF) ");
        fgets(cmd, BF_TEST_INTERPRETER_COMMAND_SIZE, stdin);


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



/*
 * Format a static string in the form of BF_FORMAT_16_RESULT_TEMPLATE
 * (" -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- -- ").
 * Return the string. Caller shouldn't change its content.
 */
static const char *
bf_format_16(const bf_nearby_16_s *nearby)
{
    int  i, valid_bytes_num;

    /* Copy the template string to result buffer. */
    memcpy(bf_format_16_result, BF_FORMAT_16_RESULT_TEMPLATE,
                                                     BF_FORMAT_16_RESULT_SIZE);

    valid_bytes_num = nearby->end_offset - nearby->begin_offset;

    /* Format bytes. */
    for (i = 0; i < valid_bytes_num; ++i) {
        int  ind_in_str;

        ind_in_str = bf_format_byte_begin_indices[nearby->begin_offset + i];
        sprintf(&bf_format_16_result[ind_in_str], "%02X", nearby->base_ptr[i]);
    }

    /* The byte pointed by data pointed is wrapped around by '[]'.  */
    bf_format_16_result[
               bf_format_byte_begin_indices[nearby->current_offset] - 1] = '[';
    bf_format_16_result[
               bf_format_byte_begin_indices[nearby->current_offset] + 2] = ']';

    /* Fill in the holes ('\0') made by sprintf as spaces (' ') */
    for (i = 0; i < BF_FORMAT_16_RESULT_SIZE - 1; ++i) {

        if (bf_format_16_result[i] == '\0') {
            bf_format_16_result[i] = ' ';
        }
    }

    return bf_format_16_result;
}
