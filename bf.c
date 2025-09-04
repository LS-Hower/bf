/*
 * Copyright (C) 2024, LS Hower.
 * This project is licensed under the BSD 3-Clause License.
 * Project homepage: https://github.com/LS-Hower/bf
 */


/* 2022-12-05 bfsimu.c */
/* 2024-12-18 bf.c */


#if (defined _MSC_VER && !(defined _CRT_SECURE_NO_WARNINGS))
#define _CRT_SECURE_NO_WARNINGS
#endif


#include <stdio.h>
#include <string.h>

#include "bf.h"


#ifndef BF_MAX_BRACKET_DEPTH
#define BF_MAX_BRACKET_DEPTH  300
#endif


/* Used by bf_run_memory() to keep track of byte getting state. */
typedef struct {
    const bf_byte_t  *ptr;
    size_t            num_left;
} bf_run_memory_ctx;


static const char *bf_find_right_square_bracket(const char *in_op);
static int bf_is_bracket_nested_incorrectly(const char *s);
static int bf_reader_memory(void *stream);


/*
 * Find the corresponding right square bracket.
 * (*pc) should be '['.
 * Assume that the corresponding right square bracket really exists.
 */
static const char *
bf_find_right_square_bracket(const char *pc)
{
    /* program counter? pointer to char! */
    int  depth;

    depth = 0;

    /*
     * The code below was written by me 2 years ago.
     * Maybe not a good coding style.
     */

    while (*++pc != ']' || depth != 0) {

        if (*pc == '[') {
            ++depth;

        } else if (*pc == ']') {
            --depth;
        }
    }

    return pc;
}


/*
 * Return 1 if the brackets are nested incorrectly.
 * Otherwise, return 0.
 */
static int
bf_is_bracket_nested_incorrectly(const char *cmd)
{
    int  depth;

    for (depth = 0; *cmd; ++cmd) {

        if ((*cmd == '[' && ++depth > BF_MAX_BRACKET_DEPTH)
            || (*cmd == ']' && --depth < 0))
        {
            return 1;
        }
    }

    return (depth != 0);
}


/* See doc in header file. */
int
bf_init(bf_s *ths, bf_byte_t *begin, bf_byte_t *end)
{
    if (!(begin < end)) {
        return -1;
    }

    ths->begin   = begin;
    ths->end     = end;
    ths->current = begin;

    return 0;
}


/* See doc in header file. */
int
bf_run_stream(bf_s *ths, const char *command, bf_reader_t reader,
    void *in_stream, bf_handler_t handler, void *out_stream)
{
    bf_byte_t        *pcurrent;
    const char       *program_counter, **stack_top;
    const bf_byte_t  *pbegin, *pend;
    const char       *left_bracket_position_stack[BF_MAX_BRACKET_DEPTH + 1];

    pbegin   = ths->begin;
    pend     = ths->end;
    pcurrent = ths->current;

    stack_top = left_bracket_position_stack;

    if (bf_is_bracket_nested_incorrectly(command)) {
        /* Bracket nested incorrectly. */
        return -2;
    }

    /* Interpret the BF code. This is the core part of the code. */
    for (program_counter = command;
         *program_counter != '\0';
         ++program_counter)
    {
        switch (*program_counter) {
        case '+':
            ++*pcurrent;
            break;

        case '-':
            --*pcurrent;
            break;

        case ',':
            *pcurrent = (*reader)(in_stream);
            break;

        case '.':
            (*handler)(*pcurrent, out_stream);
            break;

        case '>':
            if (pcurrent == pend - 1) {
                pcurrent = (bf_byte_t *) pbegin;

            } else {
                ++pcurrent;
            }
            break;

        case '<':
            if (pcurrent == pbegin) {
                pcurrent = (bf_byte_t *) pend - 1;

            } else {
                --pcurrent;
            }
            break;

        case '[':
            if (*pcurrent) {
                *++stack_top = program_counter;

            } else {
                program_counter = bf_find_right_square_bracket(
                                                              program_counter);
            }
            break;

        case ']':
            if (*pcurrent) {
                program_counter = *stack_top;

            } else {
                --stack_top;
            }
            break;

        default:
            break;
        }
    }

    ths->current = pcurrent;

    return 0;
}


/* See doc in header file. */
int
bf_run_file(bf_s *ths, const char *command, FILE *in_file,
    bf_handler_t handler, void *out_stream)
{
    return bf_run_stream(ths, command, (bf_reader_t) &fgetc, (void *) in_file,
                                                          handler, out_stream);
}


/*
 * A bf_reader_t function to read the next byte from a buffer in memory.
 * This is the fgetc() equivalent used by bf_run_memory().
 */
static int
bf_reader_memory(void *stream)
{
    bf_run_memory_ctx  *pctx;

    pctx = (bf_run_memory_ctx *) stream;

    if (pctx->num_left == 0) {
        return EOF;

    } else {
        --(pctx->num_left);

        return *(pctx->ptr++);
    }
}


/* See doc in header file. */
int
bf_run_memory(bf_s *ths, const char *command, const bf_byte_t *mem,
    size_t mem_size, bf_handler_t handler, void *out_stream)
{
    bf_run_memory_ctx  ctx;

    ctx.ptr = mem;
    ctx.num_left = mem_size;

    return bf_run_stream(ths, command, &bf_reader_memory, (void *) &ctx,
                                                          handler, out_stream);
}


/* See doc in header file. */
int
bf_run_filename(bf_s *ths, const char *command, const char *filename,
    bf_handler_t handler, void *out_stream)
{
    int    error_code;
    FILE  *file;

    file = fopen(filename, "rb");

    /* Cannot open file. */
    if (file == NULL) {
        return -1;
    }

    error_code = bf_run_file(ths, command, file, handler, out_stream);

    fclose(file);

    return error_code;
}


/* See doc in header file. */
int
bf_nearby_16(const bf_s *ths, bf_nearby_16_s *out)
{
    const bf_byte_t  *ptr;

    ptr = ths->current;
    out->current_offset = (int) ptr & 0xf;

    for (ptr = ths->current;
         ths->begin < ptr && (((int) ptr & 0xf) != 0);
         --ptr)
    {
        /* void */
    }
    out->base_ptr = ptr;
    out->begin_offset = (int) ptr & 0xf;

    for (ptr = ths->current;
         ptr < (ths->end - 1) && (((int) ptr & 0xf) != 0xf);
         ++ptr)
    {
        /* void */
    }
    out->end_offset = (int) ptr & 0xf;

    ++out->end_offset;

    return 0;
}
